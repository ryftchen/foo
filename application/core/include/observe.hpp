//! @file observe.hpp
//! @author ryftchen
//! @brief The declarations (observe) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifndef __PRECOMPILED_HEADER
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/fsm.hpp"
#include "utility/include/socket.hpp"

#define OBSERVE_TO_START application::observe::Observe::getInstance().interfaceToStart()
#define OBSERVE_TO_STOP application::observe::Observe::getInstance().interfaceToStop()

namespace application::observe
{
constexpr std::uint16_t maxTimesOfWaitObserver = 10;
constexpr std::uint16_t intervalOfWaitObserver = 10;
constexpr std::uint32_t bufferSize = 8192;
constexpr std::uint32_t maxBufferSize = bufferSize * 10;

namespace tlv
{
enum TLVType : int
{
    header = 0x125e591,
    stop = 0,
    log
};

struct TLVValue
{
    bool stopFlag{false};
    int logShmId{-1};
};

class Packet
{
public:
    Packet(char* pBuf, const std::uint32_t len) :
        pData(pBuf), length(len), pEnd(pData + len), pWrite(pData), pRead(pData)
    {
    }
    virtual ~Packet() = default;

    template <typename T>
    bool write(T data);
    bool write(const void* pDst, const std::uint32_t offset);
    template <typename T>
    bool read(T* data);
    bool read(void* pDst, const std::uint32_t offset);

private:
    char* pData{nullptr};
    const std::uint32_t length{0};
    char* pEnd{nullptr};
    char* pWrite{nullptr};
    char* pRead{nullptr};
};

int tlvDecode(char* pbuf, const int len, TLVValue& val);
int tlvEncode(char* pbuf, int& len, const TLVValue& val);
} // namespace tlv

class Observe final : public utility::fsm::FSM<Observe>
{
public:
    virtual ~Observe() = default;
    Observe(const Observe&) = delete;
    Observe& operator=(const Observe&) = delete;

    friend class FSM<Observe>;
    enum State : std::uint8_t
    {
        init,
        idle,
        work,
        done
    };

    static Observe& getInstance();
    void runObserver();
    void interfaceToStart();
    void interfaceToStop();
    static tlv::TLVValue parseTLVPacket(char* buffer, const int length);
    static constexpr std::string_view tcpHost{"localhost"};
    static constexpr std::uint16_t tcpPort{61501};
    static constexpr std::string_view udpHost{"localhost"};
    static constexpr std::uint16_t udpPort{61502};

    struct alignas(64) SharedMemory
    {
        std::atomic<bool> signal{false};
        char buffer[maxBufferSize]{'\0'};
    };

private:
    explicit Observe(const StateType initState = State::init) noexcept : FSM(initState){};

    utility::socket::TCPServer tcpServer;
    utility::socket::UDPServer udpServer;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> isObserving{false};

    struct CreateServer
    {
    };
    struct DestroyServer
    {
    };
    struct GoObserving
    {
    };
    struct NoObserving
    {
    };

    void createObserveServer();
    void startObserving();
    void destroyObserveServer();
    void stopObserving();

    // clang-format off
    using TransitionMap = Map<
        // --- Source ---+----- Event -----+--- Target ---+------------ Action ------------+--- Guard(Optional) ---
        // --------------+-----------------+--------------+--------------------------------+-----------------------
        Row< State::init ,  CreateServer   , State::idle  , &Observe::createObserveServer                         >,
        Row< State::idle ,  GoObserving    , State::work  , &Observe::startObserving                              >,
        Row< State::work ,  DestroyServer  , State::idle  , &Observe::destroyObserveServer                        >,
        Row< State::idle ,  NoObserving    , State::done  , &Observe::stopObserving                               >
        // --------------+-----------------+--------------+--------------------------------+-----------------------
        >;
    // clang-format on

    static int buildPacketForStop(char* buffer);
    static int buildPacketForLog(char* buffer);
    static constexpr std::uint32_t maxViewNumOfLines{50};

protected:
    friend std::ostream& operator<<(std::ostream& os, const Observe::State& state);
};
} // namespace application::observe
