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
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/common.hpp"
#include "utility/include/file.hpp"
#include "utility/include/time.hpp"

namespace application::observe
{
namespace tlv
{
template <typename T>
bool Packet::write(T data)
{
    T temp = htonl(data);
    return write(&temp, sizeof(T));
}

bool Packet::write(const void* pDst, const std::uint32_t offset)
{
    std::memcpy(pWrite, pDst, offset);
    pWrite += offset;
    return (pWrite < pEndData) ? true : false;
}

template <typename T>
bool Packet::read(T* data)
{
    read(data, sizeof(T));
    *data = ntohl(*data);
    return true;
}

bool Packet::read(void* pDst, const std::uint32_t offset)
{
    std::memcpy(pDst, pRead, offset);
    pRead += offset;
    return (pRead < pEndData) ? true : false;
}

int tlvDecode(char* pbuf, const int len, TLVValue& val)
{
    if (!pbuf)
    {
        return -1;
    }

    Packet dec(pbuf, len);
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
            default:
                break;
        }
    }

    return 0;
}

int tlvEncode(char* pbuf, int& len, const TLVValue& val)
{
    if (!pbuf)
    {
        return -1;
    }

    constexpr int offset = sizeof(int) + sizeof(int);
    constexpr int sum = (offset + sizeof(bool)) + (offset + sizeof(int));
    Packet enc(pbuf, len);
    enc.write<int>(TLVType::header);
    enc.write<int>(sum);

    enc.write<int>(TLVType::stop);
    enc.write<int>(sizeof(bool));
    enc.write<bool>(val.stopFlag);

    enc.write<int>(TLVType::log);
    enc.write<int>(sizeof(int));
    enc.write<int>(val.logShmId);

    len = offset + sum;

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
                  << ", current state: " << State(currentState()) << "." << std::endl;
        stopObserving();
    }
}

void Observe::interfaceToStart()
{
    utility::time::BlockingTimer timer;
    std::uint16_t waitCount = 0;
    timer.set(
        [&]()
        {
            if ((State::work == currentState()) || (maxTimesOfWaitObserver == waitCount))
            {
                timer.reset();
            }
            else
            {
                ++waitCount;
#ifndef NDEBUG
                std::cout << "<LOG> Wait for the observer to start... (" << waitCount << ")" << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitObserver);
    timer.reset();
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

    utility::time::BlockingTimer timer;
    std::uint16_t waitCount = 0;
    timer.set(
        [&]()
        {
            if ((State::done == currentState()) || (maxTimesOfWaitObserver == waitCount))
            {
                timer.reset();
            }
            else
            {
                ++waitCount;
#ifndef NDEBUG
                std::cout << "<LOG> Wait for the observer to stop... (" << waitCount << ")" << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitObserver);
    timer.reset();
}

void Observe::createObserveServer()
{
    using utility::socket::Socket;
    using utility::common::operator""_bkdrHash;

    tcpServer.onNewConnection = [](utility::socket::TCPSocket* newSocket)
    {
        newSocket->onMessageReceived = [newSocket](const std::string& message)
        {
            try
            {
                char buffer[bufferSize] = {'\0'};
                switch (utility::common::bkdrHash(message.data()))
                {
                    case "stop"_bkdrHash:
                        if (buildPacketForStop(buffer) > 0)
                        {
                            newSocket->toSend(buffer, sizeof(buffer));
                            newSocket->setNonBlocking();
                        }
                        break;
                    case "log"_bkdrHash:
                        if (buildPacketForLog(buffer) > 0)
                        {
                            newSocket->toSend(buffer, sizeof(buffer));
                        }
                        break;
                    default:
                        throw std::logic_error("<OBSERVE> Unknown TCP message.");
                }
            }
            catch (std::exception& error)
            {
                LOG_WRN(error.what());
            }
        };
    };

    udpServer.onMessageReceived = [&](const std::string& message, const std::string& host, const std::uint16_t port)
    {
        try
        {
            char buffer[bufferSize] = {'\0'};
            switch (utility::common::bkdrHash(message.data()))
            {
                case "stop"_bkdrHash:
                    if (buildPacketForStop(buffer) > 0)
                    {
                        udpServer.toSendTo(buffer, sizeof(buffer), host, port);
                        udpServer.setNonBlocking();
                    }
                    break;
                case "log"_bkdrHash:
                    if (buildPacketForLog(buffer) > 0)
                    {
                        udpServer.toSendTo(buffer, sizeof(buffer), host, port);
                    }
                    break;
                default:
                    throw std::logic_error("<OBSERVE> Unknown UDP message.");
            }
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

tlv::TLVValue Observe::parseTLVPacket(char* buffer, const int length)
{
    using utility::socket::Socket;
    tlv::TLVValue value;
    tlv::tlvDecode(buffer, length, value);

    if (value.stopFlag)
    {
        return value;
    }

    int shmId = -1;
    if ((shmId = value.logShmId) >= 0)
    {
        void* shm = shmat(shmId, nullptr, 0);
        if (nullptr == shm)
        {
            throw std::runtime_error("<OBSERVE> Failed to attach shared memory.");
        }
        application::observe::Observe::ShareMemory* shareMem =
            reinterpret_cast< // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                application::observe::Observe::ShareMemory*>(shm);
        shareMem->signal.store(true);
        while (true)
        {
            if (shareMem->signal.load())
            {
                std::cout << "\r\n" << shareMem->buffer << std::endl;
                shareMem->signal.store(false);
                break;
            }
            utility::time::millisecondLevelSleep(1);
        }
        shmdt(shm);
        shmctl(shmId, IPC_RMID, nullptr);
    }

    return value;
}

int Observe::buildPacketForLog(char* buffer)
{
    int shmId = shmget(
        (key_t)0,
        sizeof(ShareMemory),
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
    ShareMemory* shareMemory =
        reinterpret_cast<ShareMemory*>(shm); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    shareMemory->signal.store(false);
    while (true)
    {
        if (!shareMemory->signal.load())
        {
            std::memset(shareMemory->buffer, 0, sizeof(shareMemory->buffer));
            const std::string content = utility::file::displayFileContents(
                utility::file::FileProperty{LOG_PATHNAME, LOG_FILE_LOCK},
                utility::file::DisplaySetting{true, maxViewNumOfLines, &log::changeToLogStyle});
            std::memcpy(shareMemory->buffer, content.c_str(), content.length());
            shareMemory->signal.store(true);
            break;
        }
        utility::time::millisecondLevelSleep(1);
    }
    shmdt(shm);

    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.logShmId = shmId}) < 0)
    {
        length = 0;
    }
    return length;
}

int Observe::buildPacketForStop(char* buffer)
{
    int length = 0;
    if (tlv::tlvEncode(buffer, length, tlv::TLVValue{.stopFlag = true}) < 0)
    {
        length = 0;
    }
    return length;
}

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
