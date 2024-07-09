//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "view.hpp"
#include "log.hpp"

#ifndef __PRECOMPILED_HEADER
#include <openssl/evp.h>
#include <readline/readline.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#if defined(__has_include) && __has_include(<gmp.h>)
#include <gmp.h>
#endif // defined(__has_include) && __has_include(<gmp.h>)
#include <mpfr.h>
#if defined(__has_include) && __has_include(<ncurses.h>)
#include <ncurses.h>
#endif // defined(__has_include) && __has_include(<ncurses.h>)
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

    int valLen = std::strlen(val.libInfo);
    enc.write<int>(TLVType::depend);
    enc.write<int>(valLen);
    enc.write(val.libInfo, valLen);
    sum += (offset + valLen);

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
    tryToFillShmId(TLVType::execute, &TLVValue::bashShmId);
    tryToFillShmId(TLVType::journal, &TLVValue::logShmId);
    tryToFillShmId(TLVType::monitor, &TLVValue::statusShmId);

    valLen = std::strlen(val.configDetail);
    enc.write<int>(TLVType::profile);
    enc.write<int>(valLen);
    enc.write(val.configDetail, valLen);
    sum += (offset + valLen);

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
            case TLVType::depend:
                dec.read(&val.libInfo, length);
                sum -= (offset + length);
                break;
            case TLVType::execute:
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
            case TLVType::profile:
                dec.read(&val.configDetail, length);
                sum -= (offset + length);
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
    T temp;
    if constexpr (sizeof(T) == sizeof(int))
    {
        temp = ::htonl(data);
    }
    else if constexpr (sizeof(T) == sizeof(short))
    {
        temp = ::htons(data);
    }
    else
    {
        temp = data;
    }
    return write(&temp, sizeof(T));
}

bool Packet::write(const void* const dst, const int offset)
{
    std::memcpy(writer, dst, offset);
    writer += offset;
    return (writer < tail) ? true : false;
}

template <typename T>
bool Packet::read(T* const data)
{
    const bool isEnd = read(data, sizeof(T));
    if constexpr (sizeof(T) == sizeof(int))
    {
        *data = ::ntohl(*data);
    }
    else if constexpr (sizeof(T) == sizeof(short))
    {
        *data = ::ntohs(*data);
    }
    return isEnd;
}

bool Packet::read(void* const dst, const int offset)
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

void View::stateController()
{
retry:
    try
    {
        assert(currentState() == State::init);
        processEvent(CreateServer());

        assert(currentState() == State::idle);
        processEvent(GoViewing());

        assert(currentState() == State::work);
        while (ongoing.load())
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(
                lock,
                [this]()
                {
                    return !ongoing.load() || toReset.load();
                });

            if (toReset.load())
            {
                break;
            }
        }

        if (toReset.load())
        {
            processEvent(Relaunch());
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
        processEvent(DestroyServer());

        assert(currentState() == State::idle);
        processEvent(NoViewing());

        assert(currentState() == State::done);
    }
    catch (const std::exception& err)
    {
        LOG_ERR << err.what() << " Current viewer state: " << State(currentState()) << '.';
        processEvent(Standby());

        if (awaitNotification4Rollback())
        {
            goto retry; // NOLINT (hicpp-avoid-goto)
        }
    }
}

void View::waitForStart()
{
    while (!isInUninterruptedState(State::idle))
    {
        if (isInUninterruptedState(State::hold))
        {
            LOG_ERR << "The viewer did not initialize successfully ...";
            return;
        }
        utility::time::millisecondLevelSleep(1);
    }

    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        ongoing.store(true);

        lock.unlock();
        cv.notify_one();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint32_t waitCounter = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCounter]()
        {
            if (isInUninterruptedState(State::work))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCounter;
            }

            if (timeoutPeriod == waitCounter)
            {
                LOG_ERR << "The viewer did not start properly in " << timeoutPeriod << "ms ...";
                expiryTimer.reset();
            }
        },
        1);
}

void View::waitForStop()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        ongoing.store(false);

        lock.unlock();
        cv.notify_one();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint32_t waitCounter = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCounter]()
        {
            if (isInUninterruptedState(State::done))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCounter;
            }

            if (timeoutPeriod == waitCounter)
            {
                LOG_ERR << "The viewer did not stop properly in " << timeoutPeriod << "ms ...";
                expiryTimer.reset();
            }
        },
        1);
}

void View::requestToReset()
{
    std::unique_lock<std::mutex> lock(mtx);
    toReset.store(true);
    lock.unlock();
    cv.notify_one();
}

const View::OptionMap& View::viewerOptions() const
{
    return optionDispatcher;
}

std::string View::viewerTCPHost() const
{
    return tcpHost;
}

std::uint16_t View::viewerTCPPort() const
{
    return tcpPort;
}

std::string View::viewerUDPHost() const
{
    return udpHost;
}

std::uint16_t View::viewerUDPPort() const
{
    return udpPort;
}

tlv::TLVValue View::parseTLVPacket(char* buffer, const int length) const
{
    decryptMessage(buffer, length);

    tlv::TLVValue value{};
    if (tlv::tlvDecode(buffer, length, value) < 0)
    {
        throw std::runtime_error("Invalid message content.");
    }

    if (std::strlen(value.libInfo) != 0)
    {
        std::cout << value.libInfo << std::endl;
    }
    if (invalidShmId != value.bashShmId)
    {
        printSharedMemory(value.bashShmId, true);
    }
    if (invalidShmId != value.logShmId)
    {
        printSharedMemory(value.logShmId, !isInUninterruptedState(State::work));
    }
    if (invalidShmId != value.statusShmId)
    {
        printSharedMemory(value.statusShmId, true);
    }
    if (std::strlen(value.configDetail) != 0)
    {
        std::cout << utility::json::JSON::load(value.configDetail) << std::endl;
    }

    return value;
}

void View::outputAwait()
{
    if (isInUninterruptedState(State::work))
    {
        std::unique_lock<std::mutex> outputLock(outputMtx);
        outputCv.wait(
            outputLock,
            [this]()
            {
                return outputCompleted.load();
            });
        outputCompleted.store(false);
    }
    else
    {
        constexpr std::uint16_t maxLatency = 500;
        utility::time::millisecondLevelSleep(maxLatency);
    }
}

void View::outputAwaken()
{
    std::unique_lock<std::mutex> outputLock(outputMtx);
    outputCompleted.store(true);
    outputLock.unlock();
    outputCv.notify_one();
}

std::vector<std::string> View::splitString(const std::string& str)
{
    std::vector<std::string> split;
    std::istringstream is(str);
    std::string token;
    while (is >> token)
    {
        split.emplace_back(token);
    }

    return split;
}

int View::buildNullTLVPacket(char* buf)
{
    int len = 0;
    if (tlv::tlvEncode(buf, len, tlv::TLVValue{}) < 0)
    {
        throw std::runtime_error("Failed to build null packet.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket2Stop(char* buf)
{
    int len = 0;
    if (tlv::tlvEncode(buf, len, tlv::TLVValue{.stopTag = true}) < 0)
    {
        throw std::runtime_error("Failed to build packet to stop");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket2Depend(const std::vector<std::string>& /*args*/, char* buf)
{
    int len = 0;
    tlv::TLVValue val{};
    std::string libNames;
    libNames += "GNU C Library " COMMON_TO_STRING(__GLIBC__) "." COMMON_TO_STRING(__GLIBC_MINOR__) "\n";
    libNames += "GNU C++ Standard Library " COMMON_TO_STRING(_GLIBCXX_RELEASE) " (" COMMON_TO_STRING(__GLIBCXX__) ")\n";
    libNames += "OpenSSL Library " OPENSSL_VERSION_STR "\n";
    libNames += "GNU MPFR Library " MPFR_VERSION_STRING "\n";
#if defined(__has_include) && __has_include(<gmp.h>)
    libNames += "GNU MP Library " COMMON_TO_STRING(__GNU_MP_VERSION) "." COMMON_TO_STRING(
        __GNU_MP_VERSION_MINOR) "." COMMON_TO_STRING(__GNU_MP_VERSION_PATCHLEVEL) "\n";
#endif // defined(__has_include) && __has_include(<gmp.h>)
    libNames += "GNU Readline Library " COMMON_TO_STRING(RL_VERSION_MAJOR) "." COMMON_TO_STRING(RL_VERSION_MINOR) "\n";
#if defined(__has_include) && __has_include(<ncurses.h>)
    libNames += "Ncurses Library " NCURSES_VERSION "";
#endif // defined(__has_include) && __has_include(<ncurses.h>)
    std::strncpy(val.libInfo, libNames.data(), sizeof(val.libInfo) - 1);
    val.libInfo[sizeof(val.libInfo) - 1] = '\0';
    if (tlv::tlvEncode(buf, len, val) < 0)
    {
        throw std::runtime_error("Failed to build packet for the depend option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket2Execute(const std::vector<std::string>& args, char* buf)
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

    int len = 0;
    const int shmId = fillSharedMemory(utility::common::executeCommand("/bin/bash -c " + cmds, 5000));
    if (tlv::tlvEncode(buf, len, tlv::TLVValue{.bashShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the execute option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket2Journal(const std::vector<std::string>& /*args*/, char* buf)
{
    int len = 0;
    const int shmId = fillSharedMemory(getLogContents());
    if (tlv::tlvEncode(buf, len, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the journal option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket2Monitor(const std::vector<std::string>& /*args*/, char* buf)
{
    int len = 0;
    const int shmId = fillSharedMemory(getStatusInformation());
    if (tlv::tlvEncode(buf, len, tlv::TLVValue{.statusShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the monitor option.");
    }
    encryptMessage(buf, len);
    return len;
}

int View::buildTLVPacket2Profile(const std::vector<std::string>& /*args*/, char* buf)
{
    int len = 0;
    tlv::TLVValue val{};
    const std::string currConfig = config::queryConfiguration().toUnescapedString();
    std::strncpy(val.configDetail, currConfig.c_str(), sizeof(val.configDetail) - 1);
    val.configDetail[sizeof(val.configDetail) - 1] = '\0';
    if (tlv::tlvEncode(buf, len, val) < 0)
    {
        throw std::runtime_error("Failed to build packet for the profile option.");
    }
    encryptMessage(buf, len);
    return len;
}

void View::encryptMessage(char* buffer, const int length)
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
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                length))
        {
            break;
        }
        if (!::EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen))
        {
            break;
        }
    }
    while (0);
    ::EVP_CIPHER_CTX_free(ctx);
}

void View::decryptMessage(char* buffer, const int length)
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
                ctx,
                reinterpret_cast<unsigned char*>(buffer),
                &outLen,
                reinterpret_cast<unsigned char*>(buffer),
                length))
        {
            break;
        }
        if (!::EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(buffer) + outLen, &tempLen))
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
            encryptMessage(shrMem->buffer, sizeof(shrMem->buffer));

            shrMem->signal.store(true);
            break;
        }
        utility::time::millisecondLevelSleep(1);
    }
    ::shmdt(shm);

    return shmId;
}

void View::printSharedMemory(const int shmId, const bool withoutPaging)
{
    void* const shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("Failed to attach shared memory.");
    }

    std::string output;
    auto* const shrMem = reinterpret_cast<SharedMemory*>(shm);
    shrMem->signal.store(true);
    for (;;)
    {
        if (shrMem->signal.load())
        {
            decryptMessage(shrMem->buffer, sizeof(shrMem->buffer));
            output = shrMem->buffer;
            shrMem->signal.store(false);
            break;
        }
        utility::time::millisecondLevelSleep(1);
    }
    ::shmdt(shm);
    ::shmctl(shmId, IPC_RMID, nullptr);

    if (withoutPaging)
    {
        std::cout << '\n' << output << std::endl;
    }
    else
    {
        std::cout << '\n';
        segmentedOutput(output);
        std::cout << std::endl;
    }
}

void View::segmentedOutput(const std::string& buffer)
{
    constexpr std::uint8_t terminalRows = 24;
    constexpr std::string_view hint = "----- Type <CR> for more, c to continue without paging, q to quit -----: ",
                               clearEscape = "\x1b[1A\x1b[2K\r";
    std::uint64_t counter = 0;
    bool withoutPaging = false;
    std::istringstream is(buffer);
    std::string line;
    while (std::getline(is, line))
    {
        std::cout << line << '\n';
        ++counter;
        if ((0 == (counter % terminalRows)) && !withoutPaging)
        {
            std::cout << hint;
            std::string input;
            for (;;)
            {
                std::getline(std::cin, input);
                std::cout << clearEscape;
                if (input.empty())
                {
                    --counter;
                    break;
                }
                else if ("c" == input)
                {
                    withoutPaging = true;
                    break;
                }
                else if ("q" == input)
                {
                    return;
                }
                else
                {
                    std::cout << hint;
                }
            }
        }
    }
}

std::string View::getLogContents()
{
    utility::file::ReadWriteGuard guard(LOG_FILE_LOCK, utility::file::LockMode::read);
    auto contents = utility::file::getFileContents(LOG_FILE_PATH, true);
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
    constexpr std::uint16_t totalLen = 512;
    char cmd[totalLen] = {'\0'};
    std::snprintf(cmd, totalLen, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);
    const std::string queryResult = utility::common::executeCommand(cmd);

    std::vector<std::string> cmdCntr;
    std::size_t pos = 0, prev = 0;
    const int currTid = ::gettid();
    const bool showStack = (::system("which eu-stack >/dev/null 2>&1") == EXIT_SUCCESS);
    while (std::string::npos != (pos = queryResult.find('\n', prev)))
    {
        const int tid = std::stoi(queryResult.substr(prev, pos - prev));
        char cmd[totalLen] = {'\0'};
        const int usedLen = std::snprintf(
            cmd,
            totalLen,
            "if [ -f /proc/%d/task/%d/status ] ; then head -n 10 /proc/%d/task/%d/status ",
            pid,
            tid,
            pid,
            tid);
        if (showStack)
        {
            if (currTid != tid)
            {
                std::snprintf(
                    cmd + usedLen,
                    totalLen - usedLen,
                    "&& echo 'Stack:' && (eu-stack -1v -n 3 -p %d 2>&1 | grep '#' || exit 0) ; fi",
                    tid);
            }
            else
            {
                std::strncpy(cmd + usedLen, "&& echo 'Stack:' && echo 'N/A' ; fi", totalLen - usedLen);
            }
        }
        else
        {
            std::strncpy(cmd + usedLen, "; fi", totalLen - usedLen);
        }
        cmdCntr.emplace_back(cmd);
        prev = pos + 1;
    }
    cmd[totalLen - 1] = '\0';

    std::string statInfo;
    std::for_each(
        cmdCntr.cbegin(),
        cmdCntr.cend(),
        [&statInfo](const auto& cmd)
        {
            statInfo += utility::common::executeCommand(cmd) + '\n';
        });
    if (!statInfo.empty())
    {
        statInfo.pop_back();
    }

    return statInfo;
}

bool View::isInUninterruptedState(const State state) const
{
    return (currentState() == state) && !toReset.load();
}

void View::createViewServer()
{
    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->onNewConnection = [this](utility::socket::TCPSocket* const newSocket)
    {
        newSocket->onMessageReceived = [this, newSocket](const std::string& message)
        {
            if (message.length() == 0)
            {
                return;
            }

            char buffer[maxMsgLen] = {'\0'};
            try
            {
                const auto msg = utility::common::base64Decode(message);
                if ("stop" == msg)
                {
                    buildTLVPacket2Stop(buffer);
                    newSocket->toSend(buffer, sizeof(buffer));
                    newSocket->setNonBlocking();
                    return;
                }

                auto args = splitString(msg);
                const auto optionIter = optionDispatcher.find(args.at(0));
                if (optionDispatcher.cend() == optionIter)
                {
                    throw std::logic_error("Unknown TCP message.");
                }
                args.erase(args.begin());
                (optionIter->second.functor)(args, buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
            catch (const std::exception& err)
            {
                LOG_WRN << err.what();
                buildNullTLVPacket(buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
        };
    };

    udpServer = std::make_shared<utility::socket::UDPServer>();
    udpServer->onMessageReceived = [this](const std::string& message, const std::string& ip, const std::uint16_t port)
    {
        if (message.length() == 0)
        {
            return;
        }

        char buffer[maxMsgLen] = {'\0'};
        try
        {
            const auto msg = utility::common::base64Decode(message);
            if ("stop" == msg)
            {
                buildTLVPacket2Stop(buffer);
                udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
                return;
            }

            auto args = splitString(msg);
            const auto optionIter = optionDispatcher.find(args.at(0));
            if (optionDispatcher.cend() == optionIter)
            {
                throw std::logic_error("Unknown UDP message.");
            }
            args.erase(args.begin());
            (optionIter->second.functor)(args, buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
        }
        catch (const std::exception& err)
        {
            LOG_WRN << err.what();
            buildNullTLVPacket(buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
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
            return ongoing.load();
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
    ongoing.store(false);
    toReset.store(false);

    std::unique_lock<std::mutex> outputLock(outputMtx);
    outputCompleted.store(false);
}

void View::doToggle()
{
}

void View::doRollback()
{
    std::unique_lock<std::mutex> lock(mtx);
    ongoing.store(false);
    toReset.store(false);
    if (tcpServer)
    {
        tcpServer.reset();
    }
    if (udpServer)
    {
        udpServer.reset();
    }

    std::unique_lock<std::mutex> outputLock(outputMtx);
    outputCompleted.store(false);
}

bool View::awaitNotification4Rollback()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(lock);
    }

    if (toReset.load())
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
