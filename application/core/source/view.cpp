//! @file view.cpp
//! @author ryftchen
//! @brief The definitions (view) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "view.hpp"
#include "log.hpp"
#ifndef __PRECOMPILED_HEADER
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/file.hpp"
#include "utility/include/time.hpp"

namespace application::view
{
namespace tlv
{
template <typename T>
bool Packet::write(T data)
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

bool Packet::write(const void* dst, const std::uint32_t offset)
{
    std::memcpy(writer, dst, offset);
    writer += offset;
    return (writer < tail) ? true : false;
}

template <typename T>
bool Packet::read(T* data)
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

bool Packet::read(void* dst, const std::uint32_t offset)
{
    std::memcpy(dst, reader, offset);
    reader += offset;
    return (reader < tail) ? true : false;
}

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
    enc.write<bool>(val.stopFlag);
    sum += (offset + sizeof(bool));

    if (invalidShmId != val.logShmId)
    {
        enc.write<int>(TLVType::log);
        enc.write<int>(sizeof(int));
        enc.write<int>(val.logShmId);
        sum += (offset + sizeof(int));
    }
    else if (invalidShmId != val.statShmId)
    {
        enc.write<int>(TLVType::stat);
        enc.write<int>(sizeof(int));
        enc.write<int>(val.statShmId);
        sum += (offset + sizeof(int));
    }

    *reinterpret_cast<int*>(buf + sizeof(int)) = ::htonl(sum);
    len = offset + sum;

    return 0;
}

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
                dec.read<bool>(&val.stopFlag);
                sum -= (offset + sizeof(bool));
                break;
            case TLVType::log:
                dec.read<int>(&val.logShmId);
                sum -= (offset + sizeof(int));
                break;
            case TLVType::stat:
                dec.read<int>(&val.statShmId);
                sum -= (offset + sizeof(int));
                break;
            default:
                break;
        }
    }

    return 0;
}
} // namespace tlv

View& View::getInstance()
{
    static View viewer{};
    return viewer;
}

void View::runViewer()
{
    State expectedState = State::init;
    const auto checkIfExceptedFSMState = [this, &expectedState](const State state)
    {
        expectedState = state;
        if (currentState() != expectedState)
        {
            throw std::logic_error("Abnormal viewer state.");
        }
    };

    for (;;)
    {
        try
        {
            checkIfExceptedFSMState(State::init);
            processEvent(CreateServer());

            checkIfExceptedFSMState(State::idle);
            processEvent(GoViewing());

            checkIfExceptedFSMState(State::work);
            while (isViewing.load())
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(
                    lock,
                    [this]()
                    {
                        return (!isViewing.load() || restartRequest.load());
                    });

                if (restartRequest.load())
                {
                    break;
                }
            }

            if (restartRequest.load())
            {
                processEvent(Relaunch());
                continue;
            }
            processEvent(DestroyServer());

            checkIfExceptedFSMState(State::idle);
            processEvent(NoViewing());

            checkIfExceptedFSMState(State::done);
        }
        catch (const std::exception& error)
        {
            LOG_ERR << error.what() << " Expected viewer state: " << expectedState
                    << ", current viewer state: " << State(currentState()) << '.';
            if (std::unique_lock<std::mutex> lock(mtx); true)
            {
                cv.wait(lock);
            }

            if (restartRequest.load())
            {
                processEvent(Relaunch());
                if (currentState() == State::init)
                {
                    continue;
                }
                else
                {
                    LOG_ERR << "Failed to restart viewer.";
                }
            }
        }
        break;
    }
}

void View::waitToStart()
{
    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCount]()
        {
            if (State::work == currentState())
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
    expiryTimer.reset();
}

void View::waitToStop()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isViewing.store(false);
        restartRequest.store(false);

        lock.unlock();
        cv.notify_one();
        utility::time::millisecondLevelSleep(1);
        lock.lock();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [this, &expiryTimer, &waitCount]()
        {
            if (State::done == currentState())
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
    expiryTimer.reset();
}

void View::requestToRestart()
{
    std::unique_lock<std::mutex> lock(mtx);
    restartRequest.store(true);
    lock.unlock();
    cv.notify_one();
    utility::time::millisecondLevelSleep(1);
    lock.lock();
}

tlv::TLVValue View::parseTLVPacket(char* buffer, const int length)
{
    tlv::TLVValue value;
    tlv::tlvDecode(buffer, length, value);

    if (invalidShmId != value.logShmId)
    {
        printSharedMemory(value.logShmId);
    }
    else if (invalidShmId != value.statShmId)
    {
        printSharedMemory(value.statShmId);
    }

    return value;
}

int View::buildStopPacket(char* buffer)
{
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.stopFlag = true}) < 0)
    {
        throw std::runtime_error("Failed to build packet to stop");
    }
    return length;
}

int View::buildLogPacket(char* buffer)
{
    const int shmId = fillSharedMemory(getLogContents());
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the log option.");
    }
    return length;
}

int View::buildStatPacket(char* buffer)
{
    const int shmId = fillSharedMemory(getStatInformation());
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.statShmId = shmId}) < 0)
    {
        throw std::runtime_error("Failed to build packet for the stat option.");
    }
    return length;
}

int View::fillSharedMemory(const std::string& contents)
{
    int shmId = ::shmget(
        static_cast<::key_t>(0),
        sizeof(SharedMemory),
        IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (-1 == shmId)
    {
        throw std::runtime_error("Failed to create shared memory.");
    }
    void* shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("Failed to attach shared memory.");
    }
    SharedMemory* shrMem = reinterpret_cast<SharedMemory*>(shm);

    shrMem->signal.store(false);
    for (;;)
    {
        if (!shrMem->signal.load())
        {
            std::memset(shrMem->buffer, 0, sizeof(shrMem->buffer));
            std::memcpy(shrMem->buffer, contents.c_str(), contents.length());
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
    void* shm = ::shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("Failed to attach shared memory.");
    }
    SharedMemory* shrMem = reinterpret_cast<SharedMemory*>(shm);

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
    return os.str();
}

std::string View::getStatInformation()
{
    const int pid = ::getpid();
    constexpr std::uint16_t len = 256;
    char cmd[len + 1] = {'\0'};
    std::snprintf(cmd, len + 1, "ps -T -p %d | awk 'NR>1 {split($0, a, \" \"); print a[2]}'", pid);
    const std::string queryRes = utility::common::executeCommand(cmd);

    std::vector<int> tids;
    std::size_t pos = 0, prev = 0;
    while ((pos = queryRes.find('\n', prev)) != std::string::npos)
    {
        tids.emplace_back(std::stoi(queryRes.substr(prev, pos - prev)));
        prev = pos + 1;
    }

    std::string statList;
    for (const auto& tid : tids)
    {
        char cmd[len + 1] = {'\0'};
        if (::gettid() != tid)
        {
            std::snprintf(
                cmd,
                len + 1,
                "head -n 10 /proc/%d/task/%d/status "
                "&& echo 'Stack:' && (timeout --signal=2 0.02 strace -qq -ttT -vyy -s 96 -p %d 2>&1 || exit 0)",
                pid,
                tid,
                tid);
        }
        else
        {
            std::snprintf(cmd, len + 1, "head -n 10 /proc/%d/task/%d/status && echo 'Stack:' && echo 'N/A'", pid, tid);
        }
        statList += utility::common::executeCommand(cmd) + '\n';
    }

    return statList;
}

void View::createViewServer()
{
    using utility::socket::Socket;
    using utility::common::operator""_bkdrHash;

    tcpServer = std::make_shared<utility::socket::TCPServer>();
    tcpServer->onNewConnection = [this](utility::socket::TCPSocket* newSocket)
    {
        newSocket->onMessageReceived = [this, newSocket](const std::string& message)
        {
            try
            {
                if (0 == message.length())
                {
                    return;
                }

                char buffer[maxMsgLength + 1] = {'\0'};
                if ("stop" == message)
                {
                    buildStopPacket(buffer);
                    newSocket->toSend(buffer, sizeof(buffer));
                    newSocket->setNonBlocking();
                    return;
                }

                const auto optionIter = optionDispatcher.find(message);
                if (optionDispatcher.end() == optionIter)
                {
                    throw std::logic_error("Unknown TCP message.");
                }
                (*get<BuildFunctor>(optionIter->second))(buffer);
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

            char buffer[maxMsgLength + 1] = {'\0'};
            if ("stop" == message)
            {
                buildStopPacket(buffer);
                udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
                return;
            }

            const auto optionIter = optionDispatcher.find(message);
            if (optionDispatcher.end() == optionIter)
            {
                throw std::logic_error("Unknown UDP message.");
            }
            (*get<BuildFunctor>(optionIter->second))(buffer);
            udpServer->toSendTo(buffer, sizeof(buffer), ip, port);
        }
        catch (std::exception& error)
        {
            LOG_WRN << error.what();
        }
    };
}

void View::startViewing()
{
    std::unique_lock<std::mutex> lock(mtx);
    isViewing.store(true);
    tcpServer->toBind(tcpPort);
    tcpServer->toListen();
    tcpServer->toAccept();
    udpServer->toBind(udpPort);
    udpServer->toReceiveFrom();
}

void View::destroyViewServer()
{
    tcpServer.reset();
    udpServer.reset();
}

void View::stopViewing()
{
    std::unique_lock<std::mutex> lock(mtx);
    isViewing.store(false);
    restartRequest.store(false);
}

void View::rollBack()
{
    if (tcpServer)
    {
        tcpServer.reset();
    }
    if (udpServer)
    {
        udpServer.reset();
    }

    std::unique_lock<std::mutex> lock(mtx);
    isViewing.store(false);
    restartRequest.store(false);
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of output stream object
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
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<View::State>>(state);
    }

    return os;
}
} // namespace application::view
