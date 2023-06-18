//! @file observe.hpp
//! @author ryftchen
//! @brief The declarations (observe) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <map>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "utility/include/fsm.hpp"
#include "utility/include/socket.hpp"

//! @brief Start to observe.
#define OBSERVE_TO_START application::observe::Observe::getInstance().interfaceToStart()
//! @brief Stop to observe.
#define OBSERVE_TO_STOP application::observe::Observe::getInstance().interfaceToStop()
//! @brief Get all observer options.
#define OBSERVE_OPTIONS application::observe::Observe::getInstance().getObserverOptions()

//! @brief Observe-server-related functions in the application module.
namespace application::observe
{
//! @brief Maximum number of times to wait for the observer to change to the target state.
constexpr std::uint16_t maxTimesOfWaitObserver = 10;
//! @brief Time interval (ms) to wait for the observer to change to the target state.
constexpr std::uint16_t intervalOfWaitObserver = 10;
//! @brief Maximum length of the message.
constexpr std::uint32_t maxMsgLength = 8192;
//! @brief Invalid Shm id.
constexpr int invalidShmId = -1;

//! @brief Type-length-value scheme.
namespace tlv
{
//! @brief Enumerate the types in TLV.
enum TLVType : int
{
    header = 0x125e591,
    stop = 0,
    log,
    stat
};

//! @brief Value in TLV.
struct TLVValue
{
    //! @brief Flag for stopping the connection.
    bool stopFlag{false};
    //! @brief Shm id of the log contents.
    int logShmId{invalidShmId};
    //! @brief Shm id of the stat information.
    int statShmId{invalidShmId};
};

//! @brief TLV packet.
class Packet
{
public:
    //! @brief Construct a new Packet object.
    //! @param pBuf - TVL packet buffer
    //! @param len - buffer length
    Packet(char* pBuf, const std::uint32_t len) :
        pData(pBuf), length(len), pTail(pData + len), pWrite(pData), pRead(pData)
    {
    }
    //! @brief Destroy the Packet object.
    virtual ~Packet() = default;

    //! @brief Write data to the packet buffer.
    //! @tparam T type of data to be written
    //! @param data - original data
    //! @return whether it is continuously writable
    template <typename T>
    bool write(T data);
    //! @brief Write data to the packet buffer.
    //! @param pDst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously writable
    bool write(const void* pDst, const std::uint32_t offset);
    //! @brief Read data to the packet buffer.
    //! @tparam T type of data to be read
    //! @param data - original data
    //! @return whether it is continuously readable
    template <typename T>
    bool read(T* data);
    //! @brief Read data to the packet buffer.
    //! @param pDst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously readable
    bool read(void* pDst, const std::uint32_t offset);

private:
    //! @brief TLV packet buffer pointer.
    char* pData{nullptr};
    //! @brief Buffer length.
    const std::uint32_t length{0};
    //! @brief Pointer to the end of the buffer.
    char* pTail{nullptr};
    //! @brief Pointer to the current writing location.
    char* pWrite{nullptr};
    //! @brief Pointer to the current reading location.
    char* pRead{nullptr};
};

//! @brief Encode the TLV packet.
//! @param pBuf - TLV packet buffer
//! @param len -  buffer length
//! @param val - value of TLV after encoding
//! @return the value is 0 if successful, otherwise -1
int tlvEncode(char* pBuf, int& len, const TLVValue& val);
//! @brief Decode the TLV packet.
//! @param pBuf - TLV packet buffer
//! @param len -  buffer length
//! @param val - value of TLV after decoding
//! @return the value is 0 if successful, otherwise -1
int tlvDecode(char* pBuf, const int len, TLVValue& val);
} // namespace tlv

//! @brief Observer.
class Observe final : public utility::fsm::FSM<Observe>
{
public:
    //! @brief Destroy the Observe object.
    virtual ~Observe() = default;
    //! @brief Construct a new Observe object.
    Observe(const Observe&) = delete;
    //! @brief The operator (=) overloading of Observe class.
    //! @return reference of Observe object
    Observe& operator=(const Observe&) = delete;

    friend class FSM<Observe>;
    //! @brief Enumerate specific states for FSM.
    enum State : std::uint8_t
    {
        init,
        idle,
        work,
        done
    };

    //! @brief Get the Observe instance.
    //! @return reference of Observe object
    static Observe& getInstance();
    //! @brief Interface for running observer.
    void runObserver();
    //! @brief Wait until the observer starts. External use.
    void interfaceToStart();
    //! @brief Wait until the observer stops. External use.
    void interfaceToStop();

    //! @brief Alias for the functor to build the TLV packet.
    typedef int (*BuildFunctor)(char*);
    //! @brief Alias for the option name.
    using Option = std::string;
    //! @brief Alias for the option help information.
    using HelpInfo = std::string;
    //! @brief Alias for the tuple of OptionHelp and BuildFunctor.
    using OptionTuple = std::tuple<HelpInfo, BuildFunctor>;
    //! @brief Alias for the map of Option and OptionTuple.
    using OptionMap = std::map<Option, OptionTuple>;
    //! @brief Get the observer options.
    //! @return observer options
    inline OptionMap getObserverOptions() const;
    //! @brief Get a member of OptionTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    static auto get(const OptionTuple& tuple);
    //! @brief Parse the TLV packet.
    //! @param buffer - TLV packet buffer
    //! @param length - buffer length
    //! @return value of TLV after parsing
    static tlv::TLVValue parseTLVPacket(char* buffer, const int length);
    //! @brief TCP server host address.
    static constexpr std::string_view tcpHost{"localhost"};
    //! @brief TCP server port number.
    static constexpr std::uint16_t tcpPort{61501};
    //! @brief UDP server host address.
    static constexpr std::string_view udpHost{"localhost"};
    //! @brief UDP server port number.
    static constexpr std::uint16_t udpPort{61502};

    //! @brief Maximum size of the shared memory.
    static constexpr std::uint32_t maxShmSize{8192 * 10};
    //! @brief Memory that can be accessed by multiple programs simultaneously.
    struct alignas(64) SharedMemory
    {
        //! @brief Flag for operable.
        std::atomic<bool> signal{false};
        //! @brief Shared memory buffer.
        char buffer[maxShmSize]{'\0'};
    };

private:
    //! @brief Construct a new Observe object.
    //! @param initState - initialization value of state
    explicit Observe(const StateType initState = State::init) noexcept : FSM(initState){};

    // clang-format off
    //! @brief Mapping table of all observer options.
    const OptionMap optionDispatcher{
        // - Option -+------------- Help -------------+-- Build Packet --
        // ----------+--------------------------------+------------------
        { "log"      , { "view the log with highlights", &buildLogPacket  }},
        { "stat"     , { "show the stat of the process", &buildStatPacket }}
        // ----------+--------------------------------+------------------
    };
    // clang-format on
    //! @brief Build the TLV packet to stop connection.
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildStopPacket(char* buffer);
    //! @brief Build the TLV packet to view log contents.
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildLogPacket(char* buffer);
    //! @brief Build the TLV packet to show stat information.
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildStatPacket(char* buffer);
    //! @brief Fill the shared memory.
    //! @param contents - contents to be filled
    //! @return shm id
    static int fillSharedMemory(const std::string& contents);
    //! @brief Print the shared memory.
    //! @param shmId - shm id
    static void printSharedMemory(const int shmId);
    //! @brief Get the log Contents.
    //! @return log contents
    static std::string getLogContents();
    //! @brief Get the stat information.
    //! @return stat information
    static std::string getStatInformation();
    //! @brief Maximum number of lines to view log contents.
    static constexpr std::uint32_t maxViewNumOfLines{20};

    //! @brief TCP server.
    utility::socket::TCPServer tcpServer;
    //! @brief UDP server.
    utility::socket::UDPServer udpServer;
    //! @brief Mutex for controlling server.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for server. Use with mtx.
    std::condition_variable cv;
    //! @brief Flag to indicate whether it is observing.
    std::atomic<bool> isObserving{false};

    //! @brief FSM event. Create server.
    struct CreateServer
    {
    };
    //! @brief FSM event. Destroy server.
    struct DestroyServer
    {
    };
    //! @brief FSM event. Go observing.
    struct GoObserving
    {
    };
    //! @brief FSM event. NO observing.
    struct NoObserving
    {
    };

    //! @brief Create the observe server.
    void createObserveServer();
    //! @brief Start observing.
    void startObserving();
    //! @brief Destroy the observe server.
    void destroyObserveServer();
    //! @brief Stop observing.
    void stopObserving();

    // clang-format off
    //! @brief Alias for the transition map of the observer.
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

protected:
    friend std::ostream& operator<<(std::ostream& os, const Observe::State& state);
};

inline Observe::OptionMap Observe::getObserverOptions() const
{
    return optionDispatcher;
}

template <typename T>
auto Observe::get(const OptionTuple& tuple)
{
    if constexpr (std::is_same_v<T, HelpInfo>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, BuildFunctor>)
    {
        return std::get<1>(tuple);
    }
}
} // namespace application::observe
