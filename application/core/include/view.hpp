//! @file view.hpp
//! @author ryftchen
//! @brief The declarations (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "configure.hpp"

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
    friend class FSM<View>;
    //! @brief Destroy the View object.
    ~View() = default;
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

    //! @brief Instance name.
    static constexpr std::string name{configure::field::viewer};
    //! @brief Get the View instance.
    //! @return reference of the View object
    static View& getInstance();
    //! @brief Service for running.
    void service();

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
    //! @brief Access for the instance.
    class Access
    {
    public:
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
        //! @brief Get the supported options.
        //! @return supported options
        [[nodiscard]] auto getSupportedOptions() const noexcept { return inst.supportedOptions; }
        //! @brief Get the TCP server host address.
        //! @return TCP server host address
        [[nodiscard]] std::string getTCPHost() const noexcept { return inst.tcpHost; }
        //! @brief Get the TCP server port number.
        //! @return TCP server port number
        [[nodiscard]] std::uint16_t getTCPPort() const noexcept { return inst.tcpPort; }
        //! @brief Get the UDP server host address.
        //! @return UDP server host address
        [[nodiscard]] std::string getUDPHost() const noexcept { return inst.udpHost; }
        //! @brief Get the UDP server port number.
        //! @return UDP server port number
        [[nodiscard]] std::uint16_t getUDPPort() const noexcept { return inst.udpPort; }

    private:
        //! @brief Instance to be accessed.
        View& inst{getInstance()};

        //! @brief Wait until the viewer reaches the target state.
        //! @param state - target state
        //! @param handling - handling if unexpected state
        void waitOr(const State state, const std::function<void()>& handling) const;
        //! @brief Notify the viewer daemon to change the state.
        //! @param action - action to be executed
        void notifyVia(const std::function<void()>& action) const;
        //! @brief Start the reset timer.
        void startResetTimer() const;
    };
    //! @brief Synchronization for the instance.
    class Sync
    {
    public:
        //! @brief Block the caller until the output task is marked as done.
        void waitTaskDone() const;
        //! @brief Notify that the output task has been completed and unblock any waiters.
        void notifyTaskDone() const;

    private:
        //! @brief Instance to be synchronized.
        View& inst{getInstance()};
    };

private:
    //! @brief Construct a new View object.
    //! @param tcpHost - tcp server host address
    //! @param tcpPort - tcp server port number
    //! @param udpHost - udp server host address
    //! @param udpPort - udp server port number
    explicit View(
        const std::string_view tcpHost = configure::detail::tcpHost4Viewer(),
        const std::uint16_t tcpPort = configure::detail::tcpPort4Viewer(),
        const std::string_view udpHost = configure::detail::udpHost4Viewer(),
        const std::uint16_t udpPort = configure::detail::udpPort4Viewer()) :
        FSM(State::init), tcpHost{tcpHost}, tcpPort{tcpPort}, udpHost{udpHost}, udpPort{udpPort}
    {
    }

    //! @brief TCP server host address.
    const std::string tcpHost{"localhost"};
    //! @brief TCP server port number.
    const std::uint16_t tcpPort{61501};
    //! @brief UDP server host address.
    const std::string udpHost{"localhost"};
    //! @brief UDP server port number.
    const std::uint16_t udpPort{61502};
    //! @brief Timeout period (ms) to waiting for the viewer to change to the target state.
    const std::uint32_t timeoutPeriod{static_cast<std::uint32_t>(configure::detail::helperTimeout())};

    //! @brief Alias for the option arguments.
    using Args = std::vector<std::string>;
    //! @brief Option attribute.
    struct OptBase
    {
        //! @brief The option arguments.
        const Args args;
    };
    //! @brief Option attribute for the depend option.
    struct OptDepend : public OptBase
    {
        //! @brief The option name.
        static constexpr const char* const name{"depend"};
        //! @brief The option description.
        static constexpr const char* const description{"list all associated libraries"};
    };
    //! @brief Option attribute for the execute option.
    struct OptExecute : public OptBase
    {
        //! @brief The option name.
        static constexpr const char* const name{"execute"};
        //! @brief The option description.
        static constexpr const char* const description{"enter bash commands in quotes [inputs: 'CMD']"};
    };
    //! @brief Option attribute for the journal option.
    struct OptJournal : public OptBase
    {
        //! @brief The option name.
        static constexpr const char* const name{"journal"};
        //! @brief The option description.
        static constexpr const char* const description{"view the log with highlights"};
    };
    //! @brief Option attribute for the monitor option.
    struct OptMonitor : public OptBase
    {
        //! @brief The option name.
        static constexpr const char* const name{"monitor"};
        //! @brief The option description.
        static constexpr const char* const description{"query process status and stacks [inputs: NUM]"};
    };
    //! @brief Option attribute for the profile option.
    struct OptProfile : public OptBase
    {
        //! @brief The option name.
        static constexpr const char* const name{"profile"};
        //! @brief The option description.
        static constexpr const char* const description{"display current configuration"};
    };
    //! @brief Option type object's helper type for the visitor.
    //! @tparam Ts - type of visitors
    template <typename... Ts>
    struct OptionVisitor : public Ts...
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
    using OptionMap = std::map<std::string, std::string>;
    // clang-format off
    //! @brief Mapping table of all viewer options.
    const OptionMap supportedOptions
    {
        // ---- Option ----+---------- Help ----------
        // ----------------+--------------------------
        { OptDepend::name  , OptDepend::description  },
        { OptExecute::name , OptExecute::description },
        { OptJournal::name , OptJournal::description },
        { OptMonitor::name , OptMonitor::description },
        { OptProfile::name , OptProfile::description }
        // ----------------+--------------------------
    };
    // clang-format on
    //! @brief Maximum size of the shared memory.
    static constexpr std::uint64_t maxShmSize{static_cast<std::uint64_t>(65536) * 10};
    //! @brief Memory that can be accessed by multiple programs simultaneously.
    struct ShrMemBlock
    {
        //! @brief Shared memory buffer.
        alignas(64) char buffer[sizeof(int) + maxShmSize];
        //! @brief Flag for operable.
        alignas(64) std::atomic_bool signal;
    };

    //! @brief Build the response message.
    //! @param reqPlaintext - plaintext of the request
    //! @param respBuffer - buffer to store the response
    //! @return length of the response message
    static int buildResponse(const std::string& reqPlaintext, char* respBuffer);
    //! @brief Extract the option from the request.
    //! @param reqPlaintext - plaintext of the request
    //! @return option type
    static OptionType extractOption(const std::string& reqPlaintext);
    //! @brief Split string by space.
    //! @param str - target string
    //! @return strings after split
    static std::vector<std::string> splitString(const std::string& str);
    //! @brief Build the TLV packet of the response message to acknowledge only.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildAckTLVPacket(char* buf);
    //! @brief Build the TLV packet of the response message to stop connection.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildFinTLVPacket(char* buf);
    //! @brief Build the TLV packet of the response message to get library information.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Depend(const Args& args, char* buf);
    //! @brief Build the TLV packet of the response message to get bash outputs.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Execute(const Args& args, char* buf);
    //! @brief Build the TLV packet of the response message to get log contents.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Journal(const Args& args, char* buf);
    //! @brief Build the TLV packet of the response message to get status reports.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Monitor(const Args& args, char* buf);
    //! @brief Build the TLV packet of the response message to get current configuration.
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket4Profile(const Args& args, char* buf);
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
    static void segmentedOutput(const std::string& buffer);
    //! @brief Log contents preview.
    //! @return log contents
    static std::string logContentsPreview();
    //! @brief Status reports preview.
    //! @param frame - maximum frame
    //! @return status reports
    static std::string statusReportsPreview(const std::uint16_t frame);

    //! @brief TCP server.
    std::shared_ptr<utility::socket::TCPServer> tcpServer;
    //! @brief UDP server.
    std::shared_ptr<utility::socket::UDPServer> udpServer;
    //! @brief Mutex for controlling daemon.
    mutable std::mutex daemonMtx;
    //! @brief The synchronization condition for daemon. Use with daemonMtx.
    std::condition_variable daemonCond;
    //! @brief Flag to indicate whether it is viewing.
    std::atomic_bool ongoing{false};
    //! @brief Flag for rollback request.
    std::atomic_bool toReset{false};
    //! @brief Mutex for controlling output.
    mutable std::mutex outputMtx;
    //! @brief The synchronization condition for output. Use with outputMtx.
    std::condition_variable outputCond;
    //! @brief Flag to indicate whether the output is complete.
    std::atomic_bool outputCompleted{false};
    //! @brief Renew the server.
    //! @tparam T - type of server
    template <typename T>
    void renewServer();

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
