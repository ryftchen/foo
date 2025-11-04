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
    static std::shared_ptr<View> getInstance();
    //! @brief Service for running.
    void service();

    //! @brief Enumerate specific states for FSM.
    enum State : std::uint8_t
    {
        //! @brief Initial.
        initial,
        //! @brief Active.
        active,
        //! @brief Established.
        established,
        //! @brief Inactive.
        inactive,
        //! @brief Idle.
        idle
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
        //! @return need to continue parsing or not
        bool onParsing(char* const buffer, const std::size_t length) const;
        //! @brief Get the supported options.
        //! @return supported options
        [[nodiscard]] auto getSupportedOptions() const noexcept { return inst->supportedOptions; }
        //! @brief Get the TCP server host address.
        //! @return TCP server host address
        [[nodiscard]] std::string getTCPHost() const noexcept { return inst->tcpHost; }
        //! @brief Get the TCP server port number.
        //! @return TCP server port number
        [[nodiscard]] std::uint16_t getTCPPort() const noexcept { return inst->tcpPort; }
        //! @brief Get the UDP server host address.
        //! @return UDP server host address
        [[nodiscard]] std::string getUDPHost() const noexcept { return inst->udpHost; }
        //! @brief Get the UDP server port number.
        //! @return UDP server port number
        [[nodiscard]] std::uint16_t getUDPPort() const noexcept { return inst->udpPort; }

    private:
        //! @brief Instance to be accessed.
        const std::shared_ptr<View> inst{getInstance()};

        //! @brief Wait until the viewer reaches the target state.
        //! @param state - target state
        //! @param handling - handling if unexpected state
        void waitOr(const State state, const std::function<void()>& handling) const;
        //! @brief Notify the viewer to change the state.
        //! @param action - action to be executed
        void notifyVia(const std::function<void()>& action) const;
        //! @brief Keep countdown if the viewer does not meet the condition in time.
        //! @param condition - condition of countdown
        //! @param handling - handling if timeout
        void countdownIf(const std::function<bool()>& condition, const std::function<void()>& handling) const;
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
        const std::shared_ptr<View> inst{getInstance()};
    };

private:
    //! @brief Construct a new View object.
    View();

    //! @brief TCP server host address.
    const std::string tcpHost{configure::detail::tcpHost4Viewer()};
    //! @brief TCP server port number.
    const std::uint16_t tcpPort{static_cast<std::uint16_t>(configure::detail::tcpPort4Viewer())};
    //! @brief UDP server host address.
    const std::string udpHost{configure::detail::udpHost4Viewer()};
    //! @brief UDP server port number.
    const std::uint16_t udpPort{static_cast<std::uint16_t>(configure::detail::udpPort4Viewer())};
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
    //! @brief Mapping table of all viewer options.
    const OptionMap supportedOptions{
        {OptDepend::name, OptDepend::description},
        {OptExecute::name, OptExecute::description},
        {OptJournal::name, OptJournal::description},
        {OptMonitor::name, OptMonitor::description},
        {OptProfile::name, OptProfile::description}};
    //! @brief Maximum size of the shared memory buffer.
    static constexpr std::size_t shmLimitSize{static_cast<std::size_t>(65536 * 10)};
    //! @brief Memory that can be accessed by multiple programs simultaneously.
    struct ShrMemBlock
    {
        //! @brief Data of the shared memory buffer.
        alignas(64) char data[shmLimitSize];
        //! @brief Size of the shared memory buffer.
        alignas(64) std::size_t size;
        //! @brief Flag for operable.
        alignas(64) std::atomic_bool signal;
    };

    //! @brief Build the response message.
    //! @param reqPlaintext - plaintext of the request
    //! @param respBuffer - buffer to store the response
    //! @return length of the response message
    static std::size_t buildResponse(const std::string& reqPlaintext, char* const respBuffer);
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
    static std::size_t buildAckTLVPacket(char* const buf);
    //! @brief Build the TLV packet of the response message to stop connection.
    //! @param buf - TLV packet buffer
    //! @return buffer length
    static std::size_t buildFinTLVPacket(char* const buf);
    //! @brief Build the TLV packet of the response message in a customized way.
    //! @tparam Opt - type of option attribute
    //! @param args - container of arguments
    //! @param buf - TLV packet buffer
    //! @return buffer length
    template <typename Opt>
    requires std::derived_from<Opt, OptBase>
    static std::size_t buildCustomTLVPacket(const Args& args, char* const buf);
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
    std::atomic_bool isOngoing{false};
    //! @brief Flag for rollback request.
    std::atomic_bool inResetting{false};
    //! @brief Mutex for controlling output.
    mutable std::mutex outputMtx;
    //! @brief The synchronization condition for output. Use with outputMtx.
    std::condition_variable outputCond;
    //! @brief Flag to indicate whether the output is complete.
    std::atomic_bool outputCompleted{true};
    //! @brief Renew the server.
    //! @tparam Sock - type of server
    template <typename Sock>
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
        // +------ Source ------+---- Event ----+------ Target ------+--------- Action ---------+- Guard -+
        // +--------------------+---------------+--------------------+--------------------------+---------+
        Row< State::initial     , CreateServer  , State::active      , &View::createViewServer            >,
        Row< State::active      , GoViewing     , State::established , &View::startViewing                >,
        Row< State::established , DestroyServer , State::active      , &View::destroyViewServer           >,
        Row< State::active      , NoViewing     , State::inactive    , &View::stopViewing                 >,
        Row< State::initial     , Standby       , State::idle        , &View::doToggle                    >,
        Row< State::active      , Standby       , State::idle        , &View::doToggle                    >,
        Row< State::established , Standby       , State::idle        , &View::doToggle                    >,
        Row< State::established , Relaunch      , State::initial     , &View::doRollback                  >,
        Row< State::inactive    , Relaunch      , State::initial     , &View::doRollback                  >,
        Row< State::idle        , Relaunch      , State::initial     , &View::doRollback                  >
        // +--------------------+---------------+--------------------+--------------------------+---------+
    >;
    // clang-format on
    //! @brief The notification loop.
    void notificationLoop();
    //! @brief Await notification to proceed.
    void awaitNotification2Proceed();
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
inline auto currentSupportedOptions()
{
    return View::Access().getSupportedOptions();
}
//! @brief Get the current TCP host address being used for viewing.
//! @return current TCP host address being used for viewing
inline std::string currentTCPHost()
{
    return View::Access().getTCPHost();
}
//! @brief Get the current TCP port number being used for viewing.
//! @return current TCP port number being used for viewing
inline std::uint16_t currentTCPPort()
{
    return View::Access().getTCPPort();
}
//! @brief Get the current UDP host address being used for viewing.
//! @return current UDP host address being used for viewing
inline std::string currentUDPHost()
{
    return View::Access().getUDPHost();
}
//! @brief Get the current UDP port number being used for viewing.
//! @return current UDP port number being used for viewing
inline std::uint16_t currentUDPPort()
{
    return View::Access().getUDPPort();
}
} // namespace info
} // namespace view
} // namespace application
