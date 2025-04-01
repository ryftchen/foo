//! @file view.hpp
//! @author ryftchen
//! @brief The declarations (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "configure.hpp"

#include "utility/include/common.hpp"
#include "utility/include/fsm.hpp"
#include "utility/include/socket.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief View-server-related functions in the application module.
namespace view
{
//! @brief Minimum port number.
inline constexpr std::uint16_t minPortNumber = 1024;
//! @brief Maximum port number.
inline constexpr std::uint16_t maxPortNumber = 65535;
//! @brief The internal symbol for exiting.
inline constexpr std::string_view exitSymbol = "stop";

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
    //! @brief Instance name.
    static constexpr std::string_view name{configure::field::viewer};
    //! @brief Service for running.
    void service();

    //! @brief Access for the instance.
    class Access
    {
    public:
        //! @brief Construct a new Access object.
        Access() : inst{getInstance()} {}
        //! @brief Destroy the Access object.
        virtual ~Access() = default;

        //! @brief Wait for the viewer to start. Interface controller for external use.
        void startup() const;
        //! @brief Wait for the viewer to stop. Interface controller for external use.
        void shutdown() const;
        //! @brief Request to reset the viewer. Interface controller for external use.
        void reload() const;

        //! @brief Parse the TLV packet.
        //! @param buffer - TLV packet buffer
        //! @param length - buffer length
        //! @return need to stop the connection or not
        bool onParsing(char* buffer, const int length) const;
        //! @brief Await depending on the output state.
        void enableWait() const;
        //! @brief Awaken depending on the output state.
        void disableWait() const;

        //! @brief Get the supported options.
        //! @return supported options
        [[nodiscard]] inline auto getSupportedOptions() const noexcept { return inst.supportedOptions; }
        //! @brief Get the TCP server host address.
        //! @return TCP server host address
        [[nodiscard]] inline std::string getTCPHost() const noexcept { return inst.tcpHost; }
        //! @brief Get the TCP server port number.
        //! @return TCP server port number
        [[nodiscard]] inline std::uint16_t getTCPPort() const noexcept { return inst.tcpPort; }
        //! @brief Get the UDP server host address.
        //! @return UDP server host address
        [[nodiscard]] inline std::string getUDPHost() const noexcept { return inst.udpHost; }
        //! @brief Get the UDP server port number.
        //! @return UDP server port number
        [[nodiscard]] inline std::uint16_t getUDPPort() const noexcept { return inst.udpPort; }

    private:
        //! @brief Instance to be accessed.
        View& inst;
    };

private:
    //! @brief Construct a new View object.
    //! @param initState - initialization value of state
    explicit View(const StateType initState = State::init) :
        FSM(initState),
        tcpHost{configure::detail::tcpHost4Viewer()},
        tcpPort{static_cast<std::uint16_t>(configure::detail::tcpPort4Viewer())},
        udpHost{configure::detail::udpHost4Viewer()},
        udpPort{static_cast<std::uint16_t>(configure::detail::udpPort4Viewer())}
    {
    }

    //! @brief Timeout period (ms) to waiting for the viewer to change to the target state.
    const std::uint32_t timeoutPeriod{static_cast<std::uint32_t>(configure::detail::helperTimeout())};
    //! @brief Option attribute.
    struct OptBase
    {
        //! @brief The option arguments.
        const std::vector<std::string> args{};
    };
    //! @brief Option attribute for the depend option.
    struct OptDepend : public OptBase
    {
        //! @brief The option name.
        static constexpr std::string_view name{"depend"};
        //! @brief The option prompt.
        static constexpr std::string_view prompt{"list all associated libraries"};
    };
    //! @brief Option attribute for the execute option.
    struct OptExecute : public OptBase
    {
        //! @brief The option name.
        static constexpr std::string_view name{"execute"};
        //! @brief The option prompt.
        static constexpr std::string_view prompt{"enter bash commands in quotes [inputs: 'CMD']"};
    };
    //! @brief Option attribute for the journal option.
    struct OptJournal : public OptBase
    {
        //! @brief The option name.
        static constexpr std::string_view name{"journal"};
        //! @brief The option prompt.
        static constexpr std::string_view prompt{"view the log with highlights"};
    };
    //! @brief Option attribute for the monitor option.
    struct OptMonitor : public OptBase
    {
        //! @brief The option name.
        static constexpr std::string_view name{"monitor"};
        //! @brief The option prompt.
        static constexpr std::string_view prompt{"query process status and stacks [inputs: NUM]"};
    };
    //! @brief Option attribute for the profile option.
    struct OptProfile : public OptBase
    {
        //! @brief The option name.
        static constexpr std::string_view name{"profile"};
        //! @brief The option prompt.
        static constexpr std::string_view prompt{"display current configuration"};
    };
    //! @brief Option type object's helper type for the visitor.
    //! @tparam Ts - type of visitors
    template <typename... Ts>
    struct OptionVisitor : Ts...
    {
        using Ts::operator()...;
    };
    //! @brief Explicit deduction guide for OptionVisitor.
    //! @tparam Ts - type of visitors
    template <typename... Ts>
    OptionVisitor(Ts...) -> OptionVisitor<Ts...>;
    //! @brief Alias for the option type.
    using OptionType = std::variant<OptBase, OptDepend, OptExecute, OptJournal, OptMonitor, OptProfile>;
    //! @brief Alias for the map of option name and OptionAttr.
    using OptionMap = std::map<std::string_view, std::string_view>;
    // clang-format off
    //! @brief Mapping table of all viewer options.
    const OptionMap supportedOptions
    {
        // ---- Option ----+-------- Help --------
        // ----------------+----------------------
        { OptDepend::name  , OptDepend::prompt  },
        { OptExecute::name , OptExecute::prompt },
        { OptJournal::name , OptJournal::prompt },
        { OptMonitor::name , OptMonitor::prompt },
        { OptProfile::name , OptProfile::prompt }
        // ----------------+----------------------
    };
    // clang-format on
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

    //! @brief TCP server host address.
    const std::string tcpHost{"localhost"};
    //! @brief TCP server port number.
    const std::uint16_t tcpPort{61501};
    //! @brief UDP server host address.
    const std::string udpHost{"localhost"};
    //! @brief UDP server port number.
    const std::uint16_t udpPort{61502};
    //! @brief Alias for the lock mode.
    using LockMode = utility::common::ReadWriteLock::LockMode;
    //! @brief Build the response message.
    //! @param reqPlaintext - plaintext of the request
    //! @param respBuffer - buffer to store the response
    static void buildResponse(const std::string_view reqPlaintext, char* respBuffer);
    //! @brief Extract the option from the request.
    //! @param reqPlaintext - plaintext of the request
    //! @return option type
    static OptionType extractOption(const std::string_view reqPlaintext);
    //! @brief Split string by space.
    //! @param str - target string
    //! @return strings after split
    static std::vector<std::string> splitString(const std::string_view str);
    //! @brief Build the TLV packet of the response message to acknowledge only.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildAckTLVPacket(char* buf);
    //! @brief Build the TLV packet of the response message to stop connection.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Stop(char* buf);
    //! @brief Build the TLV packet of the response message to get library information.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Depend(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet of the response message to get bash outputs.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Execute(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet of the response message to get log contents.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Journal(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet of the response message to get status reports.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Monitor(const std::vector<std::string>& args, char* buf);
    //! @brief Build the TLV packet of the response message to get current configuration.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Profile(const std::vector<std::string>& args, char* buf);
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
    //! @brief Log contents preview.
    //! @return log contents
    static std::string logContentsPreview();
    //! @brief Status reports preview.
    //! @param frame - maximum frame
    //! @return status reports
    static std::string statusReportsPreview(const std::uint16_t frame);

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
    //! @brief Renew the server.
    //! @tparam T - type of server
    template <typename T>
    void renewServer();

    //! @brief Safely retrieve the current state.
    //! @return current state
    State safeCurrentState() const;
    //! @brief Safely process an event.
    //! @tparam T - type of target event
    //! @param event - target event
    template <typename T>
    void safeProcessEvent(const T& event);
    //! @brief Check whether it is in the uninterrupted serving state.
    //! @param state - target state
    //! @return in the uninterrupted serving state or not
    bool isInServingState(const State state) const;
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
    //! @brief Alias for the transition table of the viewer.
    using TransitionTable = Table
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
inline auto viewerSupportedOptions()
{
    return View::Access().getSupportedOptions();
}
//! @brief Get the current TCP host address being used for viewing.
//! @return current TCP host address being used for viewing
inline std::string viewerTCPHost()
{
    return View::Access().getTCPHost();
}
//! @brief Get the current TCP port number being used for viewing.
//! @return current TCP port number being used for viewing
inline std::uint16_t viewerTCPPort()
{
    return View::Access().getTCPPort();
}
//! @brief Get the current UDP host address being used for viewing.
//! @return current UDP host address being used for viewing
inline std::string viewerUDPHost()
{
    return View::Access().getUDPHost();
}
//! @brief Get the current UDP port number being used for viewing.
//! @return current UDP port number being used for viewing
inline std::uint16_t viewerUDPPort()
{
    return View::Access().getUDPPort();
}
} // namespace info
} // namespace view
} // namespace application
