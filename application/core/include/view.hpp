//! @file view.hpp
//! @author ryftchen
//! @brief The declarations (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "config.hpp"

#ifndef __PRECOMPILED_HEADER
#include <vector>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/socket.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief View-server-related functions in the application module.
namespace view
{
//! @brief Invalid shared memory id in TLV value.
constexpr int invalidShmId = -1;
//! @brief Default information size in TLV value.
constexpr std::uint16_t defaultInfoSize = 256;

//! @brief Type-length-value scheme.
namespace tlv
{
//! @brief Enumerate the types in TLV.
enum TLVType : int
{
    //! @brief Header.
    header = 0x3b9aca07,
    //! @brief Stop.
    stop = 0,
    //! @brief Depend.
    depend,
    //! @brief Execute.
    execute,
    //! @brief Journal.
    journal,
    //! @brief Monitor.
    monitor,
    //! @brief Profile.
    profile
};

//! @brief Value in TLV.
struct TLVValue
{
    //! @brief Flag for stopping the connection.
    bool stopTag{false};
    //! @brief Information about the runtime library.
    char libInfo[defaultInfoSize]{'\0'};
    //! @brief Shared memory id of the bash outputs.
    int bashShmId{invalidShmId};
    //! @brief Shared memory id of the log contents.
    int logShmId{invalidShmId};
    //! @brief Shared memory id of the status reports.
    int statusShmId{invalidShmId};
    //! @brief Information about the current configuration.
    char configInfo[defaultInfoSize * 2]{'\0'};
};

//! @brief TLV packet.
class Packet
{
public:
    //! @brief Construct a new Packet object.
    //! @param buf - TVL packet buffer
    //! @param len - buffer length
    Packet(char* buf, const std::uint32_t len) : buffer(buf), tail(buffer + len), writer(buffer), reader(buffer) {}
    //! @brief Destroy the Packet object.
    virtual ~Packet() = default;

    //! @brief Write data to the packet buffer.
    //! @tparam T - type of data to be written
    //! @param data - original data
    //! @return whether it is continuously writable
    template <typename T>
    bool write(const T data);
    //! @brief Write data to the packet buffer.
    //! @param dst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously writable
    bool write(const void* const dst, const int offset);
    //! @brief Read data to the packet buffer.
    //! @tparam T - type of data to be read
    //! @param data - original data
    //! @return whether it is continuously readable
    template <typename T>
    bool read(T* const data);
    //! @brief Read data to the packet buffer.
    //! @param dst - data after conversion
    //! @param offset - data offset
    //! @return whether it is continuously readable
    bool read(void* const dst, const int offset);

private:
    //! @brief TLV packet buffer pointer.
    char* buffer{nullptr};
    //! @brief Pointer to the end of the buffer.
    const char* const tail{nullptr};
    //! @brief Pointer to the current writing location.
    char* writer{nullptr};
    //! @brief Pointer to the current reading location.
    const char* reader{nullptr};
};
} // namespace tlv

//! @brief Viewer.
class View final : public utility::fsm::FSM<View>
{
public:
    //! @brief Destroy the View object.
    virtual ~View() = default;
    //! @brief Construct a new View object.
    View(const View&) = delete;
    //! @brief Construct a new View object.
    View(View&&) = delete;
    //! @brief The operator (=) overloading of View class.
    //! @return reference of the View object
    View& operator=(const View&) = delete;
    //! @brief The operator (=) overloading of View class.
    //! @return reference of the View object
    View& operator=(View&&) = delete;

    friend class FSM<View>;
    //! @brief Enumerate specific states for FSM.
    enum State : std::uint8_t
    {
        //! @brief Init.
        init,
        //! @brief Idle.
        idle,
        //! @brief Work.
        work,
        //! @brief Done.
        done,
        //! @brief Hold.
        hold
    };

    //! @brief Get the View instance.
    //! @return reference of the View object
    static View& getInstance();
    //! @brief State controller for running viewer.
    void stateController();
    //! @brief Wait for the viewer to start. Interface controller for external use.
    void waitForStart();
    //! @brief Wait for the viewer to stop. Interface controller for external use.
    void waitForStop();
    //! @brief Request to reset the viewer. Interface controller for external use.
    void requestToReset();

    //! @brief Alias for the option name.
    using OptionName = std::string;
    //! @brief Alias for the help prompt of the option.
    using HelpPrompt = std::string;
    //! @brief Alias for the functor to build the TLV packet.
    typedef int (*BuildFunctor)(const std::vector<std::string>&, char*);
    //! @brief Alias for the attribute of the target option.
    struct OptionAttr
    {
        //! @brief Help prompt.
        HelpPrompt prompt{};
        //! @brief Build functor.
        BuildFunctor functor{};
    };
    //! @brief Alias for the map of OptionName and OptionAttr.
    using OptionMap = std::map<OptionName, OptionAttr>;
    //! @brief Get the viewer options.
    //! @return viewer options
    const OptionMap& getAllOptions() const;
    //! @brief Get the TCP server host address.
    //! @return TCP server host address
    std::string getTCPHost() const;
    //! @brief Get the TCP server port number.
    //! @return TCP server port number
    std::uint16_t getTCPPort() const;
    //! @brief Get the UDP server host address.
    //! @return UDP server host address
    std::string getUDPHost() const;
    //! @brief Get the UDP server port number.
    //! @return UDP server port number
    std::uint16_t getUDPPort() const;
    //! @brief Parse the TLV packet.
    //! @param buffer - TLV packet buffer
    //! @param length - buffer length
    //! @return value of TLV after parsing
    tlv::TLVValue parseTLVPacket(char* buffer, const int length) const;
    //! @brief Await depending on the output state.
    void awaitDueToOutput();
    //! @brief Awaken depending on the output state.
    void awakenDueToOutput();

    //! @brief Maximum size of the shared memory.
    static constexpr std::uint64_t maxShmSize{65536 * 10};
    //! @brief Memory that can be accessed by multiple programs simultaneously.
    struct alignas(64) SharedMemory
    {
        //! @brief Shared memory buffer.
        char buffer[sizeof(int) + maxShmSize]{'\0'};
        //! @brief Flag for operable.
        std::atomic<bool> signal{false};
    };

private:
    //! @brief Construct a new View object.
    //! @param initState - initialization value of state
    explicit View(const StateType initState = State::init) noexcept :
        FSM(initState),
        tcpHost(config::detail::tcpHost4Viewer()),
        tcpPort(config::detail::tcpPort4Viewer()),
        udpHost(config::detail::udpHost4Viewer()),
        udpPort(config::detail::udpPort4Viewer())
    {
    }

    //! @brief Alias for the lock mode.
    using LockMode = utility::common::ReadWriteLock::LockMode;
    //! @brief Timeout period (ms) to waiting for the viewer to change to the target state.
    const std::uint32_t timeoutPeriod{static_cast<std::uint32_t>(config::detail::helperTimeout())};
    // clang-format off
    //! @brief Mapping table of all viewer options.
    const OptionMap optionDispatcher
    {
        // - Option -+---------------------- Help ----------------------+------- Build Packet -------
        // ----------+--------------------------------------------------+----------------------------
        { "depend"   , { "list all associated libraries"                 , &buildTLVPacket4Depend  }},
        { "execute"  , { "enter bash commands in quotes [inputs: 'CMD']" , &buildTLVPacket4Execute }},
        { "journal"  , { "view the log with highlights"                  , &buildTLVPacket4Journal }},
        { "monitor"  , { "query process status and stacks [inputs: NUM]" , &buildTLVPacket4Monitor }},
        { "profile"  , { "display current configuration"                 , &buildTLVPacket4Profile }}
        // ----------+--------------------------------------------------+----------------------------
    };
    // clang-format on

    //! @brief TCP server host address.
    const std::string tcpHost{"localhost"};
    //! @brief TCP server port number.
    const std::uint16_t tcpPort{61501};
    //! @brief UDP server host address.
    const std::string udpHost{"localhost"};
    //! @brief UDP server port number.
    const std::uint16_t udpPort{61502};
    //! @brief Split string by space.
    //! @param str - target string
    //! @return strings after split
    static std::vector<std::string> splitString(const std::string_view str);
    //! @brief Build the null TLV packet.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildNullTLVPacket(char* buf);
    //! @brief Build the TLV packet to stop connection.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Stop(char* buf);
    //! @brief Build the TLV packet to get library information.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Depend(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet to get bash outputs.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Execute(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet to get log contents.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Journal(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet to get status reports.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Monitor(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet to get current configuration.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Profile(const std::vector<std::string>& args, char* buf);
    //! @brief Encrypt the message with AES-128-CFB-128.
    //! @param buffer - message buffer
    //! @param length - buffer length
    static void encryptMessage(char* buffer, const int length);
    //! @brief Decrypt the message with AES-128-CFB-128.
    //! @param buffer - message buffer
    //! @param length - buffer length
    static void decryptMessage(char* buffer, const int length);
    //! @brief Compress the data with LZ4.
    //! @param cache - data cache
    static void compressData(std::vector<char>& cache);
    //! @brief Decompress the data with LZ4.
    //! @param cache - data cache
    static void decompressData(std::vector<char>& cache);
    //! @brief Fill the shared memory.
    //! @param contents - contents to be filled
    //! @return shm id
    static int fillSharedMemory(const std::string_view contents);
    //! @brief Fetch the shared memory.
    //! @param shmId - shm id
    //! @param contents - contents to be fetched
    static void fetchSharedMemory(const int shmId, std::string& contents);
    //! @brief Print the shared memory.
    //! @param shmId - shm id
    //! @param withoutPaging - whether output without paging
    static void printSharedMemory(const int shmId, const bool withoutPaging = true);
    //! @brief Segmented output.
    //! @param buffer - output buffer
    static void segmentedOutput(const std::string_view buffer);
    //! @brief Get the log contents.
    //! @return log contents
    static std::string getLogContents();
    //! @brief Get the status reports.
    //! @param frame - maximum frame
    //! @return status reports
    static std::string getStatusReports(const std::uint16_t frame);

    //! @brief TCP server.
    std::shared_ptr<utility::socket::TCPServer> tcpServer{};
    //! @brief UDP server.
    std::shared_ptr<utility::socket::UDPServer> udpServer{};
    //! @brief Mutex for controlling daemon.
    mutable std::mutex daemonMtx{};
    //! @brief The synchronization condition for daemon. Use with daemonMtx.
    std::condition_variable daemonCond{};
    //! @brief Flag to indicate whether it is viewing.
    std::atomic<bool> ongoing{false};
    //! @brief Flag for rollback request.
    std::atomic<bool> toReset{false};
    //! @brief Mutex for controlling output.
    mutable std::mutex outputMtx{};
    //! @brief The synchronization condition for output. Use with outputMtx.
    std::condition_variable outputCond{};
    //! @brief Flag to indicate whether the output is complete.
    std::atomic<bool> outputCompleted{false};
    //! @brief Spin lock for controlling state.
    mutable utility::common::SpinLock stateLock{};

    //! @brief Safely retrieve the current state.
    //! @return current state
    State safeCurrentState() const;
    //! @brief Safely process an event.
    //! @tparam T - type of target event
    //! @param event - target event
    template <class T>
    void safeProcessEvent(const T& event);
    //! @brief Check whether it is in the uninterrupted target state.
    //! @param state - target state
    //! @return in the uninterrupted target state or not
    bool isInUninterruptedState(const State state) const;
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
    //! @brief FSM event. Standby.
    struct Standby
    {
    };
    //! @brief FSM event. Relaunch.
    struct Relaunch
    {
    };
    //! @brief Create the view server.
    void createViewServer();
    //! @brief Destroy the view server.
    void destroyViewServer();
    //! @brief Start viewing.
    void startViewing();
    //! @brief Stop viewing.
    void stopViewing();
    //! @brief Do toggle.
    void doToggle();
    //! @brief Do rollback.
    void doRollback();
    // clang-format off
    //! @brief Alias for the transition map of the viewer.
    using TransitionMap = Map
    <
        // --- Source ---+---- Event ----+--- Target ---+--------- Action ---------+--- Guard (Optional) ---
        // --------------+---------------+--------------+--------------------------+------------------------
        Row< State::init , CreateServer  , State::idle  , &View::createViewServer                         >,
        Row< State::idle , GoViewing     , State::work  , &View::startViewing                             >,
        Row< State::work , DestroyServer , State::idle  , &View::destroyViewServer                        >,
        Row< State::idle , NoViewing     , State::done  , &View::stopViewing                              >,
        Row< State::init , Standby       , State::hold  , &View::doToggle                                 >,
        Row< State::idle , Standby       , State::hold  , &View::doToggle                                 >,
        Row< State::work , Standby       , State::hold  , &View::doToggle                                 >,
        Row< State::done , Standby       , State::hold  , &View::doToggle                                 >,
        Row< State::work , Relaunch      , State::init  , &View::doRollback                               >,
        Row< State::hold , Relaunch      , State::init  , &View::doRollback                               >
        // --------------+---------------+--------------+--------------------------+------------------------
    >;
    // clang-format on
    //! @brief Await notification to ongoing.
    void awaitNotification2Ongoing();
    //! @brief Await notification to view.
    void awaitNotification2View();
    //! @brief Await notification to retry.
    //! @return whether retry is required or not
    bool awaitNotification2Retry();

protected:
    friend std::ostream& operator<<(std::ostream& os, const State state);
};

//! @brief Instance information, if enabled.
namespace info
{
//! @brief Get the current supported viewer options.
//! @return current supported viewer options
inline const View::OptionMap& getAllOptions()
{
    return View::getInstance().getAllOptions();
}
//! @brief Get the current TCP host address being used for viewing.
//! @return current TCP host address being used for viewing
inline std::string viewerTCPHost()
{
    return View::getInstance().getTCPHost();
}
//! @brief Get the current TCP port number being used for viewing.
//! @return current TCP port number being used for viewing
inline std::uint16_t viewerTCPPort()
{
    return View::getInstance().getTCPPort();
}
//! @brief Get the current UDP host address being used for viewing.
//! @return current UDP host address being used for viewing
inline std::string viewerUDPHost()
{
    return View::getInstance().getUDPHost();
}
//! @brief Get the current UDP port number being used for viewing.
//! @return current UDP port number being used for viewing
inline std::uint16_t viewerUDPPort()
{
    return View::getInstance().getUDPPort();
}
} // namespace info
} // namespace view
} // namespace application
