//! @file view.hpp
//! @author ryftchen
//! @brief The declarations (view) in the application module.
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

//! @brief Start to view.
#define VIEW_TO_START application::view::View::getInstance().interfaceToStart()
//! @brief Stop to view.
#define VIEW_TO_STOP application::view::View::getInstance().interfaceToStop()
//! @brief Get all viewer options.
#define VIEW_OPTIONS application::view::View::getInstance().getViewerOptions()

//! @brief View-server-related functions in the application module.
namespace application::view
{
//! @brief Maximum number of times to wait for the viewer to change to the target state.
constexpr std::uint16_t maxTimesOfWaitViewer = 10;
//! @brief Time interval (ms) to wait for the viewer to change to the target state.
constexpr std::uint16_t intervalOfWaitViewer = 10;
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
    header = 0x0125e591,
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
    //! @param buf - TVL packet buffer
    //! @param len - buffer length
    Packet(char* buf, const std::uint32_t len) :
        buffer(buf), length(len), tail(buffer + len), writer(buffer), reader(buffer)
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
    //! @param dst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously writable
    bool write(const void* dst, const std::uint32_t offset);
    //! @brief Read data to the packet buffer.
    //! @tparam T type of data to be read
    //! @param data - original data
    //! @return whether it is continuously readable
    template <typename T>
    bool read(T* data);
    //! @brief Read data to the packet buffer.
    //! @param dst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously readable
    bool read(void* dst, const std::uint32_t offset);

private:
    //! @brief TLV packet buffer pointer.
    char* buffer{nullptr};
    //! @brief Buffer length.
    const std::uint32_t length{0};
    //! @brief Pointer to the end of the buffer.
    char* tail{nullptr};
    //! @brief Pointer to the current writing location.
    char* writer{nullptr};
    //! @brief Pointer to the current reading location.
    char* reader{nullptr};
};

//! @brief Encode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len -  buffer length
//! @param val - value of TLV after encoding
//! @return the value is 0 if successful, otherwise -1
int tlvEncode(char* buf, int& len, const TLVValue& val);
//! @brief Decode the TLV packet.
//! @param buf - TLV packet buffer
//! @param len -  buffer length
//! @param val - value of TLV after decoding
//! @return the value is 0 if successful, otherwise -1
int tlvDecode(char* buf, const int len, TLVValue& val);
} // namespace tlv

//! @brief Viewer.
class View final : public utility::fsm::FSM<View>
{
public:
    //! @brief Destroy the View object.
    virtual ~View() = default;
    //! @brief Construct a new View object.
    View(const View&) = delete;
    //! @brief The operator (=) overloading of View class.
    //! @return reference of View object
    View& operator=(const View&) = delete;

    friend class FSM<View>;
    //! @brief Enumerate specific states for FSM.
    enum State : std::uint8_t
    {
        init,
        idle,
        work,
        done
    };

    //! @brief Get the View instance.
    //! @return reference of View object
    static View& getInstance();
    //! @brief Interface for running viewer.
    void runViewer();
    //! @brief Wait until the viewer starts. External use.
    void interfaceToStart();
    //! @brief Wait until the viewer stops. External use.
    void interfaceToStop();

    //! @brief Alias for the functor to build the TLV packet.
    typedef int (*BuildFunctor)(char*);
    //! @brief Alias for the option name.
    using Option = std::string;
    //! @brief Alias for the option help message.
    using HelpMessage = std::string;
    //! @brief Alias for the tuple of HelpMessage and BuildFunctor.
    using OptionTuple = std::tuple<HelpMessage, BuildFunctor>;
    //! @brief Alias for the map of Option and OptionTuple.
    using OptionMap = std::map<Option, OptionTuple>;
    //! @brief Get the viewer options.
    //! @return viewer options
    inline OptionMap getViewerOptions() const;
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
        char buffer[maxShmSize + 1]{'\0'};
    };

private:
    //! @brief Construct a new View object.
    //! @param initState - initialization value of state
    explicit View(const StateType initState = State::init) noexcept : FSM(initState){};

    // clang-format off
    //! @brief Mapping table of all viewer options.
    const OptionMap optionDispatcher{
        // - Option -+------------- Help -------------+--- Build Packet ---
        // ----------+--------------------------------+--------------------
        { "log"      , { "view the log with highlights", &buildLogPacket  }},
        { "stat"     , { "show the stat of the process", &buildStatPacket }}
        // ----------+--------------------------------+--------------------
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
    //! @brief Flag to indicate whether it is viewing.
    std::atomic<bool> isViewing{false};

    //! @brief FSM event. Create server.
    struct CreateServer
    {
    };
    //! @brief FSM event. Destroy server.
    struct DestroyServer
    {
    };
    //! @brief FSM event. Go viewing.
    struct GoViewing
    {
    };
    //! @brief FSM event. NO viewing.
    struct NoViewing
    {
    };

    //! @brief Create the view server.
    void createViewServer();
    //! @brief Start viewing.
    void startViewing();
    //! @brief Destroy the view server.
    void destroyViewServer();
    //! @brief Stop viewing.
    void stopViewing();

    // clang-format off
    //! @brief Alias for the transition map of the viewer.
    using TransitionMap = Map<
        // --- Source ---+---- Event ----+--- Target ---+--------- Action ---------+--- Guard(Optional) ---
        // --------------+---------------+--------------+--------------------------+-----------------------
        Row< State::init , CreateServer  , State::idle  , &View::createViewServer                         >,
        Row< State::idle , GoViewing     , State::work  , &View::startViewing                             >,
        Row< State::work , DestroyServer , State::idle  , &View::destroyViewServer                        >,
        Row< State::idle , NoViewing     , State::done  , &View::stopViewing                              >
        // --------------+---------------+--------------+--------------------------+-----------------------
        >;
    // clang-format on

protected:
    friend std::ostream& operator<<(std::ostream& os, const View::State state);
};

inline View::OptionMap View::getViewerOptions() const
{
    return optionDispatcher;
}

template <typename T>
auto View::get(const OptionTuple& tuple)
{
    if constexpr (std::is_same_v<T, HelpMessage>)
    {
        return std::get<0>(tuple);
    }
    else if constexpr (std::is_same_v<T, BuildFunctor>)
    {
        return std::get<1>(tuple);
    }
}
} // namespace application::view
