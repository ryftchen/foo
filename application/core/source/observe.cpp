//! @file observe.cpp
//! @author ryftchen
//! @brief The definitions (observe) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "observe.hpp"
#include "log.hpp"
#ifndef __PRECOMPILED_HEADER
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <cstring>
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/file.hpp"
#include "utility/include/time.hpp"

namespace application::observe
{
namespace tlv
{
template <typename T>
bool Packet::write(T data)
{
    T temp = 0;
    if constexpr (sizeof(T) == sizeof(std::uint32_t))
    {
        temp = htonl(data);
    }
    else if constexpr (sizeof(T) == sizeof(std::uint16_t))
    {
        temp = htons(data);
    }
    else
    {
        temp = data;
    }
    return write(&temp, sizeof(T));
}

bool Packet::write(const void* pDst, const std::uint32_t offset)
{
    std::memcpy(pWrite, pDst, offset);
    pWrite += offset;
    return (pWrite < pTail) ? true : false;
}

template <typename T>
bool Packet::read(T* data)
{
    const bool isEnd = read(data, sizeof(T));
    if constexpr (sizeof(T) == sizeof(std::uint32_t))
    {
        *data = ntohl(*data);
    }
    else if constexpr (sizeof(T) == sizeof(std::uint16_t))
    {
        *data = ntohs(*data);
    }
    return isEnd;
}

bool Packet::read(void* pDst, const std::uint32_t offset)
{
    std::memcpy(pDst, pRead, offset);
    pRead += offset;
    return (pRead < pTail) ? true : false;
}

int tlvEncode(char* pBuf, int& len, const TLVValue& val)
{
    if (!pBuf)
    {
        return -1;
    }

    constexpr int offset = sizeof(int) + sizeof(int);
    int sum = 0;

    Packet enc(pBuf, len);
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

    *reinterpret_cast<int*>(pBuf + sizeof(int)) = htonl(sum);
    len = offset + sum;

    return 0;
}

int tlvDecode(char* pBuf, const int len, TLVValue& val)
{
    if (!pBuf)
    {
        return -1;
    }

    Packet dec(pBuf, len);
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

Observe& Observe::getInstance()
{
    static Observe observer;
    return observer;
}

void Observe::runObserver()
{
    State expectedState = State::init;
    auto checkIfExceptedFSMState = [&](const State state) -> void
    {
        expectedState = state;
        if (currentState() != expectedState)
        {
            throw std::logic_error("<OBSERVE> Abnormal observer state.");
        }
    };

    try
    {
        checkIfExceptedFSMState(State::init);
        processEvent(CreateServer());

        checkIfExceptedFSMState(State::idle);
        processEvent(GoObserving());

        checkIfExceptedFSMState(State::work);
        while (isObserving.load())
        {
            if (std::unique_lock<std::mutex> lock(mtx); true)
            {
                cv.wait(
                    lock,
                    [this]() -> decltype(auto)
                    {
                        return !isObserving.load();
                    });
            }
        }

        processEvent(DestroyServer());

        checkIfExceptedFSMState(State::idle);
        processEvent(NoObserving());

        checkIfExceptedFSMState(State::done);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << " Expected state: " << expectedState
                  << ", current state: " << State(currentState()) << '.' << std::endl;
        stopObserving();
    }
}

void Observe::interfaceToStart()
{
    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [&]()
        {
            if ((State::work == currentState()) || (maxTimesOfWaitObserver == waitCount))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
#ifndef NDEBUG
                std::cout << "<OBSERVE> Wait for the observer to start... (" << waitCount << ')' << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitObserver);
    expiryTimer.reset();
}

void Observe::interfaceToStop()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isObserving.store(false);

        lock.unlock();
        cv.notify_one();
        utility::time::millisecondLevelSleep(1);
        lock.lock();
    }

    utility::time::BlockingTimer expiryTimer;
    std::uint16_t waitCount = 0;
    expiryTimer.set(
        [&]()
        {
            if ((State::done == currentState()) || (maxTimesOfWaitObserver == waitCount))
            {
                expiryTimer.reset();
            }
            else
            {
                ++waitCount;
#ifndef NDEBUG
                std::cout << "<OBSERVE> Wait for the observer to stop... (" << waitCount << ')' << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitObserver);
    expiryTimer.reset();
}

tlv::TLVValue Observe::parseTLVPacket(char* buffer, const int length)
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

int Observe::buildStopPacket(char* buffer)
{
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.stopFlag = true}) < 0)
    {
        throw std::runtime_error("<OBSERVE> Failed to build packet to stop");
    }
    return length;
}

int Observe::buildLogPacket(char* buffer)
{
    const int shmId = fillSharedMemory(getLogContents());
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        throw std::runtime_error("<OBSERVE> Failed to build packet for the log option.");
    }
    return length;
}

int Observe::buildStatPacket(char* buffer)
{
    const int shmId = fillSharedMemory(getStatInformation());
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.statShmId = shmId}) < 0)
    {
        throw std::runtime_error("<OBSERVE> Failed to build packet for the stat option.");
    }
    return length;
}

int Observe::fillSharedMemory(const std::string& contents)
{
    int shmId = shmget(
        static_cast<key_t>(0),
        sizeof(SharedMemory),
        IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (-1 == shmId)
    {
        throw std::runtime_error("<OBSERVE> Failed to create shared memory.");
    }
    void* shm = shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("<OBSERVE> Failed to attach shared memory.");
    }
    SharedMemory* shrMem = reinterpret_cast<SharedMemory*>(shm);

    shrMem->signal.store(false);
    while (true)
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
    shmdt(shm);

    return shmId;
}

void Observe::printSharedMemory(const int shmId)
{
    void* shm = shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::runtime_error("<OBSERVE> Failed to attach shared memory.");
    }
    SharedMemory* shrMem = reinterpret_cast<SharedMemory*>(shm);

    shrMem->signal.store(true);
    while (true)
    {
        if (shrMem->signal.load())
        {
            std::cout << "\r\n" << shrMem->buffer << std::endl;
            shrMem->signal.store(false);
            break;
        }
        utility::time::millisecondLevelSleep(1);
    }
    shmdt(shm);
    shmctl(shmId, IPC_RMID, nullptr);
}

std::string Observe::getLogContents()
{
    utility::file::ReadWriteGuard guard(utility::file::LockMode::read, LOG_FILE_LOCK);
    auto contents = utility::file::getFileContents(LOG_PATHNAME, true, maxViewNumOfLines);
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

std::string Observe::getStatInformation()
{
    const std::string showStat =
        R"(tail -n +1 /proc/$(ps -aux | grep foo | head -n 1 | awk '{split($0, a, " "); print a[2]}')/task/*/status)";
    return utility::common::executeCommand(showStat);
}

void Observe::createObserveServer()
{
    using utility::socket::Socket;
    using utility::common::operator""_bkdrHash;

    tcpServer.onNewConnection = [this](utility::socket::TCPSocket* newSocket)
    {
        newSocket->onMessageReceived = [this, newSocket](const std::string& message)
        {
            try
            {
                char buffer[maxMsgLength] = {'\0'};
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
                    throw std::logic_error("<OBSERVE> Unknown TCP message.");
                }
                (*get<BuildFunctor>(optionIter->second))(buffer);
                newSocket->toSend(buffer, sizeof(buffer));
            }
            catch (std::exception& error)
            {
                LOG_WRN(error.what());
            }
        };
    };

    udpServer.onMessageReceived = [this](const std::string& message, const std::string& ip, const std::uint16_t port)
    {
        try
        {
            char buffer[maxMsgLength] = {'\0'};
            if ("stop" == message)
            {
                buildStopPacket(buffer);
                udpServer.toSendTo(buffer, sizeof(buffer), ip, port);
                udpServer.setNonBlocking();
                return;
            }

            const auto optionIter = optionDispatcher.find(message);
            if (optionDispatcher.end() == optionIter)
            {
                throw std::logic_error("<OBSERVE> Unknown UDP message.");
            }
            (*get<BuildFunctor>(optionIter->second))(buffer);
            udpServer.toSendTo(buffer, sizeof(buffer), ip, port);
        }
        catch (std::exception& error)
        {
            LOG_WRN(error.what());
        }
    };
}

void Observe::startObserving()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isObserving.store(true);
        tcpServer.toBind(tcpPort);
        tcpServer.toListen();
        tcpServer.toAccept();
        udpServer.toBind(udpPort);
        udpServer.toReceiveFrom();
    }
}

void Observe::destroyObserveServer()
{
    tcpServer.toClose();
    udpServer.toClose();
}

void Observe::stopObserving()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        isObserving.store(false);
        tcpServer.waitIfAlive();
        udpServer.waitIfAlive();
    }
}

//! @brief The operator (<<) overloading of the State enum.
//! @param os - output stream object
//! @param state - the specific value of State enum
//! @return reference of output stream object
std::ostream& operator<<(std::ostream& os, const Observe::State& state)
{
    switch (state)
    {
        case Observe::State::init:
            os << "INIT";
            break;
        case Observe::State::idle:
            os << "IDLE";
            break;
        case Observe::State::work:
            os << "WORK";
            break;
        case Observe::State::done:
            os << "DONE";
            break;
        default:
            os << "UNKNOWN: " << static_cast<std::underlying_type_t<Observe::State>>(state);
    }

    return os;
}
} // namespace application::observe
