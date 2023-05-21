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

bool Packet::write(const void* pDst, const uint32_t offset)
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

bool Packet::read(void* pDst, const uint32_t offset)
{
    std::memcpy(pDst, pRead, offset);
    pRead += offset;
    return (pRead < pEndData) ? true : false;
}

int tlvDecode(char* pBuf, int len, TLVValue* pVal)
{
    if (!pVal || !pBuf)
    {
        return -1;
    }

    Packet dec(pBuf, len);
    int type = 0, length = 0, sum = 0;

    dec.read<int>(&type);
    if (TLVType::root != type)
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
            case TLVType::quit:
                dec.read<bool>(&pVal->quitFlag);
                sum -= (offset + sizeof(bool));
                break;
            case TLVType::log:
                dec.read<int>(&pVal->logShmId);
                sum -= (offset + sizeof(int));
                break;
            default:
                break;
        }
    }

    return 0;
}

int tlvEncode(char* pBuf, int& len, TLVValue* pVal)
{
    if (!pVal || !pBuf)
    {
        return -1;
    }

    constexpr int offset = sizeof(int) + sizeof(int);
    constexpr int sum = (offset + sizeof(bool)) + (offset + sizeof(int));
    Packet enc(pBuf, len);
    enc.write<int>(TLVType::root);
    enc.write<int>(sum);

    enc.write<int>(TLVType::quit);
    enc.write<int>(sizeof(bool));
    enc.write<bool>(pVal->quitFlag);

    enc.write<int>(TLVType::log);
    enc.write<int>(sizeof(int));
    enc.write<int>(pVal->logShmId);

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
            throw std::logic_error("");
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
    catch (const std::exception&)
    {
        std::cerr << "Abnormal observer state, expected state: " << expectedState
                  << ", current state: " << State(currentState()) << "." << std::endl;
        stopObserving();
    }
}

void Observe::interfaceToStart()
{
    utility::time::BlockingTimer timer;
    uint16_t waitCount = 0;
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
                std::cout << "Wait for the observer to start... (" << waitCount << ")" << std::endl;
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
    uint16_t waitCount = 0;
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
                std::cout << "Wait for the observer to stop... (" << waitCount << ")" << std::endl;
#endif // NDEBUG
            }
        },
        intervalOfWaitObserver);
    timer.reset();
}

void Observe::enableAlternate()
{
    udpServer.toBind(udpPort);
    udpServer.toReceiveFrom();
}

void Observe::createObserveServer()
{
    using utility::socket::Socket;
    using utility::common::operator""_bkdrHash;

    tcpServer.onNewConnection = [](utility::socket::TCPSocket* newClient)
    {
        newClient->onMessageReceived = [newClient](const std::string& message)
        {
            char buffer[bufferSize] = {'\0'};
            switch (utility::common::bkdrHash(message.data()))
            {
                case "quit"_bkdrHash:
                    if (buildPacketForQuit(buffer) > 0)
                    {
                        newClient->toSend(buffer, sizeof(buffer));
                        newClient->cancelWait();
                    }
                    break;
                case "log"_bkdrHash:
                    if (buildPacketForLog(buffer) > 0)
                    {
                        newClient->toSend(buffer, sizeof(buffer));
                    }
                    break;
                default:
                    LOG_WRN("<OBSERVE> Unknown TCP message type.");
                    break;
            }
        };
    };

    udpServer.onMessageReceived = [&](const std::string& message, const std::string& ipv4, const uint16_t port)
    {
        char buffer[bufferSize] = {'\0'};
        switch (utility::common::bkdrHash(message.data()))
        {
            case "quit"_bkdrHash:
                if (buildPacketForQuit(buffer) > 0)
                {
                    udpServer.toSendTo(buffer, sizeof(buffer), ipv4, port);
                    udpServer.cancelWait();
                }
                break;
            case "log"_bkdrHash:
                if (buildPacketForLog(buffer) > 0)
                {
                    udpServer.toSendTo(buffer, sizeof(buffer), ipv4, port);
                }
                break;
            default:
                LOG_WRN("<OBSERVE> Unknown UDP message type.");
                break;
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
    }
}

tlv::TLVValue Observe::parseTLVPacket(char* buffer, const int length)
{
    using utility::socket::Socket;
    tlv::TLVValue value;
    std::memset(&value, 0, sizeof(tlv::TLVValue));
    tlv::tlvDecode(buffer, length, &value);

    if (value.quitFlag)
    {
        return value;
    }

    int shmId = 0;
    if ((shmId = value.logShmId) > 0)
    {
        void* shm = shmat(shmId, nullptr, 0);
        if (nullptr == shm)
        {
            throw std::logic_error("<OBSERVE> Failed to attach shared memory.");
        }
        application::observe::Observe::ShareMemory* shareMemory =
            reinterpret_cast< // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                application::observe::Observe::ShareMemory*>(shm);
        shareMemory->signal.store(true);
        while (true)
        {
            if (shareMemory->signal.load())
            {
                std::cout << "\r\n" << shareMemory->buffer << std::endl;
                shareMemory->signal.store(false);
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
        throw std::logic_error("<OBSERVE> Failed to create shared memory.");
    }
    void* shm = shmat(shmId, nullptr, 0);
    if (nullptr == shm)
    {
        throw std::logic_error("<OBSERVE> Failed to attach shared memory.");
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

    tlv::TLVValue value;
    std::memset(&value, 0, sizeof(tlv::TLVValue));
    value.logShmId = shmId;

    int length = 0;
    if (tlv::tlvEncode(buffer, length, &value))
    {
        length = 0;
    }
    return length;
}

int Observe::buildPacketForQuit(char* buffer)
{
    tlv::TLVValue value;
    std::memset(&value, 0, sizeof(tlv::TLVValue));
    value.quitFlag = true;

    int length = 0;
    if (tlv::tlvEncode(buffer, length, &value))
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
