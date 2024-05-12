//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "view.hpp"
#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <openssl/evp.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::view
{
namespace tlv
{
//! @brief Encode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len -  buffer length
//! @param val - value of TLV after encoding
//! @return the value is 0 if successful, otherwise -1
int tlvEncode(char* buf, int& len, const TLVValue& val)
{
    if (nullptr == buf)
    {
        return -1;
    }

    constexpr int offset = sizeof(int) + sizeof(int);
    int sum = 0;

    Packet enc(buf, len);
    enc.write<int>(TLVType::header);
    enc.write<int>(sum);

    enc.write<int>(TLVType::stop);
    enc.write<int>(sizeof(bool));
    enc.write<bool>(val.stopTag);
    sum += (offset + sizeof(bool));

    const auto tryToFillShmId = [&](const TLVType type, int TLVValue::*shmId)
    {
        if (invalidShmId != val.*shmId)
        {
            enc.write<int>(type);
            enc.write<int>(sizeof(int));
            enc.write<int>(val.*shmId);
            sum += (offset + sizeof(int));
        }
    };
    tryToFillShmId(TLVType::bash, &TLVValue::bashShmId);
    tryToFillShmId(TLVType::journal, &TLVValue::logShmId);
    tryToFillShmId(TLVType::monitor, &TLVValue::statusShmId);

    *reinterpret_cast<int*>(buf + sizeof(int)) = ::htonl(sum);
    len = offset + sum;

    return 0;
}

//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len -  buffer length
//! @param val - value of TLV after decoding
//! @return the value is 0 if successful, otherwise -1
int tlvDecode(char* buf, const int len, TLVValue& val)
{
    if (nullptr == buf)
    {
        return -1;
    }

    Packet dec(buf, len);
    int type = 0, length = 0, sum = 0;

    dec.read<int>(&type);
    if (TLVType::header != type)
    {
        return -1;
    }
    dec.read<int>(&sum);

    constexpr int offset = sizeof(int) + sizeof(int);
    while (sum > 0)
    {
        dec.read<int>(&type);
        dec.read<int>(&length);
        switch (type)
        {
            case TLVType::stop:
                dec.read<bool>(&val.stopTag);
                sum -= (offset + sizeof(bool));
                break;
            case TLVType::bash:
                dec.read<int>(&val.bashShmId);
                sum -= (offset + sizeof(int));
                break;
            case TLVType::journal:
                dec.read<int>(&val.logShmId);
                sum -= (offset + sizeof(int));
                break;
            case TLVType::monitor:
                dec.read<int>(&val.statusShmId);
                sum -= (offset + sizeof(int));
                break;
            default:
                break;
        }
    }

    return 0;
}

template <typename T>
bool Packet::write(const T data)
{
    T temp = 0;
    if constexpr (sizeof(T) == sizeof(std::uint32_t))
    {
        temp = ::htonl(data);
    }
    else if constexpr (sizeof(T) == sizeof(std::uint16_t))
    {
        temp = ::htons(data);
    }
    else
    {
        temp = data;
    }
    return write(&temp, sizeof(T));
}

bool Packet::write(const void* const dst, const std::uint32_t offset)
{
    std::memcpy(writer, dst, offset);
    writer += offset;
    return (writer < tail) ? true : false;
}

template <typename T>
bool Packet::read(T* const data)
{
    const bool isEnd = read(data, sizeof(T));
    if constexpr (sizeof(T) == sizeof(std::uint32_t))
    {
        *data = ::ntohl(*data);
    }
    else if constexpr (sizeof(T) == sizeof(std::uint16_t))
    {
        *data = ::ntohs(*data);
    }
    return isEnd;
}

bool Packet::read(void* const dst, const std::uint32_t offset)
{
    std::memcpy(dst, reader, offset);
    reader += offset;
    return (reader < tail) ? true : false;
}
} // namespace tlv

View& View::getInstance()
{
    static View viewer{};
    return viewer;
}

View& View::getExistingInstance()
{
    if (!CONFIG_ACTIVATE_HELPER) [[unlikely]]
    {
        throw std::logic_error("The viewer is disabled.");
    }

    return getInstance();
}

void View::runViewer()
{
retry:
    try
    {
        assert(currentState() == State::init);
        processEvent(CreateServer());

        assert(currentState() == State::idle);
        processEvent(GoViewing());

        assert(currentState() == State::work);
        while (isViewing.load())
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(
                lock,
                [this]()
                {
                    return (!isViewing.load() || rollbackRequest.load());
                });

            if (rollbackRequest.load())
            {
                break;
            }
        }

        if (rollbackRequest.load())
        {
            processEvent(Relaunch());
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
        processEvent(DestroyServer());

        assert(currentState() == State::idle);
        processEvent(NoViewing());

        assert(currentState() == State::done);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << error.what() << " Current viewer state: " << State(currentState()) << '.';
        processEvent(Standby());

        if (awaitNotification4Rollback())
        {
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
    }
}

void View::waitToStart()
{
    while (!((currentState() == State::idle) && !rollbackRequest.load()))
    {
        if ((currentState() == State::hold) && !rollbackRequest.load())
        {
            LOG_ERR << "The viewer did not initialize successfully...";
            return;
        }
        utility::time::millisecondLevelSleep(intervalOfWaitViewer);
    }

    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isViewing.store(true);

        lock.unlock();
        cv.notify_one();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCount]()
        {
            if ((currentState() == State::work) && !rollbackRequest.load())
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
            }

            if (maxTimesOfWaitViewer == waitCount)
            {
                LOG_ERR << "The viewer did not start properly...";
                expiryTimer.reset();
            }
        },
        intervalOfWaitViewer);
}

void View::waitToStop()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isViewing.store(false);

        lock.unlock();
        cv.notify_one();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCount]()
        {
            if ((currentState() == State::done) && !rollbackRequest.load())
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
            }

            if (maxTimesOfWaitViewer == waitCount)
            {
                LOG_ERR << "The viewer did not stop properly...";
                expiryTimer.reset();
            }
        },
        intervalOfWaitViewer);
}

void View::requestToRollback()
{
    std::unique_lock<std::mutex> lock(mtx);
    rollbackRequest.store(true);
    lock.unlock();
    cv.notify_one();
}

View::OptionMap View::getViewerOptions() const
{
    return optionDispatcher;
}

std::string View::getViewerTCPHost() const
{
    return tcpHost;
}

std::uint16_t View::getViewerTCPPort() const
{
    return tcpPort;
}

std::string View::getViewerUDPHost() const
{
    return udpHost;
}

std::uint16_t View::getViewerUDPPort() const
{
    return udpPort;
}

tlv::TLVValue View::parseTLVPacket(char* buffer, const int length)
{
    decryptMessage(buffer, length);

    tlv::TLVValue value{};
    tlv::tlvDecode(buffer, length, value);

    if (invalidShmId != value.bashShmId)
    {
        printSharedMemory(value.bashShmId);
    }
    else if (invalidShmId != value.logShmId)
    {
        printSharedMemory(value.logShmId);
    }
    else if (invalidShmId != value.statusShmId)
    {
        printSharedMemory(value.statusShmId);
    }

    return value;
}

int View::buildTLVPacket2Stop(char* buffer)
{
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.stopTag = true}) < 0)
    {
        throw std::runtime_error("Failed to build packet to stop");
    }
    encryptMessage(buffer, length);
    return length;
}

int View::buildTLVPacket2Bash(const std::vector<std::string>& args, char* buffer)
{
    std::string cmds;
    for (const auto& arg : args)
    {
        cmds += arg + ' ';
    }
    if (!args.empty())
    {
        cmds.pop_back();
    }
    const int shmId = fillSharedMemory(utility::common::executeCommand("/bin/bash -c " + cmds, 5000));
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.bashShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the bash option.");
    }
    encryptMessage(buffer, length);
    return length;
}

int View::buildTLVPacket2Journal(const std::vector<std::string>& /*args*/, char* buffer)
{
    const int shmId = fillSharedMemory(getLogContents());
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the journal option.");
    }
    encryptMessage(buffer, length);
    return length;
}

int View::buildTLVPacket2Monitor(const std::vector<std::string>& /*args*/, char* buffer)
{
    const int shmId = fillSharedMemory(getStatusInformation());
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.statusShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the monitor option.");
    }
    encryptMessage(buffer, length);
    return length;
}

void View::encryptMessage(char* buf, const int len)
{
    constexpr unsigned char
        key[16] = {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
        iv[16] = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
    ::EVP_CIPHER_CTX* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        int outLen = 0, tempLen = 0;
        if (!::EVP_EncryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key, iv))
        {
            break;
        }
        if (!::EVP_EncryptUpdate(
                ctx, reinterpret_cast<unsigned char*>(buf), &outLen, reinterpret_cast<unsigned char*>(buf), len))
        {
            break;
        }
        if (!::EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buf) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}

void View::decryptMessage(char* buf, const int len)
{
    constexpr unsigned char
        key[16] = {0x37, 0x47, 0x10, 0x33, 0x6F, 0x18, 0xC8, 0x9A, 0x4B, 0xC1, 0x2B, 0x97, 0x92, 0x19, 0x25, 0x6D},
        iv[16] = {0x9F, 0x7B, 0x0E, 0x68, 0x2D, 0x2F, 0x4E, 0x7F, 0x1A, 0xFA, 0x61, 0xD3, 0xC6, 0x18, 0xF4, 0xC1};
    ::EVP_CIPHER_CTX* const ctx = ::EVP_CIPHER_CTX_new();
    do
    {
        int outLen = 0, tempLen = 0;
        if (!::EVP_DecryptInit_ex(ctx, ::EVP_aes_128_cfb128(), nullptr, key, iv))
        {
            break;
        }
        if (!::EVP_DecryptUpdate(
                ctx, reinterpret_cast<unsigned char*>(buf), &outLen, reinterpret_cast<unsigned char*>(buf), len))
        {
            break;
        }
        if (!::EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buf) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}

int View::fillSharedMemory(const std::string& contents)
{
    const int shmId = ::shmget(
        static_cast<::key_t>(0),
        sizeof(SharedMemory),
        IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (-1 == shmId)
    {
        throw std::runtime_error("Failed to create shared memory.");
    }
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("Failed to attach shared memory.");
    }

    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    shrMem->signal.store(false);
    for (;;)
    {
        if (!shrMem->signal.load())
        {
            std::memset(shrMem->buffer, 0, sizeof(shrMem->buffer));
            std::strncpy(shrMem->buffer, contents.c_str(), sizeof(shrMem->buffer) - 1);
            shrMem->buffer[sizeof(shrMem->buffer) - 1] = '\0';
            shrMem->signal.store(true);
            break;
        }
        utility::time::millisecondLevelSleep(1);
    }
    ::shmdt(shm);

    return shmId;
}

void View::printSharedMemory(const int shmId)
{
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("Failed to attach shared memory.");
    }

    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    shrMem->signal.store(true);
    for (;;)
    {
        if (shrMem->signal.load())
        {
            std::cout << "\r\n" << shrMem->buffer << std::endl;
            shrMem->signal.store(false);
            break;
        }
        utility::time::millisecondLevelSleep(1);
    }
    ::shmdt(shm);
    ::shmctl(shmId, IPC_RMID, nullptr);
}

std::string View::getLogContents()
{
    utility::file::ReadWriteGuard guard(utility::file::LockMode::read, LOG_FILE_LOCK);
    auto contents = utility::file::getFileContents(LOG_FILE_PATH, true, maxViewNumOfLines);
    std::for_each(
        contents.begin(),
        contents.end(),
        [](auto& line)
        {
            return log::changeToLogStyle(line);
        });
    std::ostringstream os;
    std::copy(contents.cbegin(), contents.cend(), std::ostream_iterator<std::string>(os, "\n"));
    return std::move(os).str();
}

std::string View::getStatusInformation()
{
    const int pid = ::getpid();
    constexpr std::uint16_t cmdLen = 512;
    char cmd[cmdLen] = {'\0'};
    std::snprintf(cmd, cmdLen, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);
    const std::string queryResult = utility::common::executeCommand(cmd);

    std::vector<std::string> cmdCntr;
    std::size_t pos = 0, prev = 0;
    const int currTid = ::gettid();
    const bool foundStrace = ::system("which strace >/dev/null 2>&1") == EXIT_SUCCESS;
    while ((pos = queryResult.find('\n', prev)) != std::string::npos)
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev));
        char cmd[cmdLen] = {'\0'};
        const int usedLen = std::snprintf(
            cmd,
            cmdLen,
            "if [ -f /proc/%d/task/%d/status ] ; then head -n 10 /proc/%d/task/%d/status ",
            pid,
            tid,
            pid,
            tid);
        if (foundStrace)
        {
            if (currTid != tid)
            {
                std::snprintf(
                    cmd + usedLen,
                    cmdLen - usedLen,
                    "&& echo 'Strace:' "
                    "&& (timeout --preserve-status --signal=2 0.02 strace -qq -ttT -vyy -s 96 -p %d 2>&1 || exit 0) "
                    "; fi",
                    tid);
            }
            else
            {
                std::strncpy(cmd + usedLen, "&& echo 'Strace:' && echo 'N/A' ; fi", cmdLen - usedLen);
            }
        }
        else
        {
            std::strncpy(cmd + usedLen, "; fi", cmdLen - usedLen);
        }
        cmdCntr.emplace_back(cmd);
        prev = pos + 1;
    }
    cmd[cmdLen - 1] = '\0';

    std::string statInfo;
    std::for_each(
        cmdCntr.cbegin(),
        cmdCntr.cend(),
        [&statInfo](const auto& cmd)
        {
            statInfo += utility::common::executeCommand(cmd) + '\n';
        });

    return statInfo;
}

void View::createViewServer()
{
    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->onNewConnection = [this](utility::socket::TCPSocket* const newSocket)
    {
        newSocket->onMessageReceived = [this, newSocket](const std::string& message)
        {
            try
            {
                if (0 == message.length())
                {
                    return;
                }

                char buffer[maxMsgLength] = {'\0'};
                const auto msg = utility::common::base64Decode(message);
                if ("stop" == msg)
                {
                    buildTLVPacket2Stop(buffer);
                    newSocket->toSend(buffer, sizeof(buffer));
                    newSocket->setNonBlocking();
                    return;
                }

                std::vector<std::string> args;
                std::istringstream is(msg);
                std::string token;
                while (is >> token)
                {
                    args.emplace_back(token);
                }

                const auto optionIter = optionDispatcher.find(args.at(0));
                if (optionDispatcher.end() == optionIter)
                {
                    throw std::logic_error("Unknown TCP message.");
                }
                args.erase(args.begin());
                (optionIter->second.functor)(args, buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
            catch (std::exception& error)
            {
                LOG_WRN << error.what();
            }
        };
    };

    udpServer = std::make_shared<utility::socket::UDPServer>();
    udpServer->onMessageReceived = [this](const std::string& message, const std::string& ip, const std::uint16_t port)
    {
        try
        {
            if (0 == message.length())
            {
                return;
            }

            char buffer[maxMsgLength] = {'\0'};
            const auto msg = utility::common::base64Decode(message);
            if ("stop" == msg)
            {
                buildTLVPacket2Stop(buffer);
                udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
                return;
            }

            std::vector<std::string> args;
            std::istringstream is(msg);
            std::string token;
            while (is >> token)
            {
                args.emplace_back(token);
            }

            const auto optionIter = optionDispatcher.find(args.at(0));
            if (optionDispatcher.end() == optionIter)
            {
                throw std::logic_error("Unknown UDP message.");
            }
            args.erase(args.begin());
            (optionIter->second.functor)(args, buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
        }
        catch (std::exception& error)
        {
            LOG_WRN << error.what();
        }
    };
}

void View::destroyViewServer()
{
    tcpServer.reset();
    udpServer.reset();
}

void View::startViewing()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(
        lock,
        [this]()
        {
            return isViewing.load();
        });

    tcpServer->toBind(tcpPort);
    tcpServer->toListen();
    tcpServer->toAccept();
    udpServer->toBind(udpPort);
    udpServer->toReceiveFrom();
}

void View::stopViewing()
{
    std::unique_lock<std::mutex> lock(mtx);
    isViewing.store(false);
    rollbackRequest.store(false);
}

void View::doToggle()
{
}

void View::doRollback()
{
    std::unique_lock<std::mutex> lock(mtx);
    isViewing.store(false);
    rollbackRequest.store(false);

    if (tcpServer)
    {
        tcpServer.reset();
    }
    if (udpServer)
    {
        udpServer.reset();
    }
}

bool View::awaitNotification4Rollback()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(lock);
    }

    if (rollbackRequest.load())
    {
        processEvent(Relaunch());
        if (currentState() == State::init)
        {
            return true;
        }
        LOG_ERR << "Failed to rollback viewer.";
    }

    return false;
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of the output stream object
std::ostream& operator<<(std::ostream& os, const View::State state)
{
    switch (state)
    {
        case View::State::init:
            os << "INIT";
            break;
        case View::State::idle:
            os << "IDLE";
            break;
        case View::State::work:
            os << "WORK";
            break;
        case View::State::done:
            os << "DONE";
            break;
        case View::State::hold:
            os << "HOLD";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<View::State>>(state);
    }

    return os;
}
} // namespace application::view
