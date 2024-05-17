//! @file view.hpp
//! @author ryftchen
//! @brief The declarations (view) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "config.hpp"

#ifndef __PRECOMPILED_HEADER
#include <map>
#include <vector>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/fsm.hpp"
#include "utility/include/socket.hpp"

//! @brief Get the view instance.
#define VIEW_GET_INSTANCE application::view::View::getInstance()
//! @brief All viewer options.
#define VIEW_OPTIONS VIEW_GET_INSTANCE.viewerOptions()
//! @brief TCP host address of the viewer.
#define VIEW_TCP_HOST VIEW_GET_INSTANCE.viewerTCPHost()
//! @brief TCP port number of the viewer.
#define VIEW_TCP_PORT VIEW_GET_INSTANCE.viewerTCPPort()
//! @brief UDP host address of the viewer.
#define VIEW_UDP_HOST VIEW_GET_INSTANCE.viewerUDPHost()
//! @brief UDP port number of the viewer.
#define VIEW_UDP_PORT VIEW_GET_INSTANCE.viewerUDPPort()
//! @brief TLV packet through viewer parsing.
#define VIEW_TLV_PACKET(buf, len) VIEW_GET_INSTANCE.parseTLVPacket(buf, len)
//! @brief Get the view instance if enabled.
#define VIEW_GET_INSTANCE_IF_ENABLED       \
    if (CONFIG_ACTIVATE_HELPER) [[likely]] \
    application::view::View::getInstance()
//! @brief Wait for the viewer output to complete.
#define VIEW_AWAIT VIEW_GET_INSTANCE_IF_ENABLED.outputAwait()
//! @brief Wake due to the viewer output completed.
#define VIEW_AWAKEN VIEW_GET_INSTANCE_IF_ENABLED.outputAwaken()

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief View-server-related functions in the application module.
namespace view
{
//! @brief Invalid Shm id.
constexpr int invalidShmId = -1;

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
    //! @brief Bash.
    bash,
    //! @brief Journal.
    journal,
    //! @brief Monitor.
    monitor
};

//! @brief Value in TLV.
struct TLVValue
{
    //! @brief Flag for stopping the connection.
    bool stopTag{false};
    //! @brief Shm id of the bash outputs.
    int bashShmId{invalidShmId};
    //! @brief Shm id of the log contents.
    int logShmId{invalidShmId};
    //! @brief Shm id of the status information.
    int statusShmId{invalidShmId};
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
    bool write(const void* const dst, const std::uint32_t offset);
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
    bool read(void* const dst, const std::uint32_t offset);

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
    //! @brief The operator (=) overloading of View class.
    //! @return reference of the View object
    View& operator=(const View&) = delete;

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
    void runViewer();
    //! @brief Wait for the viewer to start. Interface controller for external use.
    void waitForStart();
    //! @brief Wait for the viewer to stop. Interface controller for external use.
    void waitForStop();
    //! @brief Request to reset the viewer. Interface controller for external use.
    void requestToReset();

    //! @brief Alias for the option name.
    using Option = std::string;
    //! @brief Alias for the option help message.
    using HelpMessage = std::string;
    //! @brief Alias for the functor to build the TLV packet.
    typedef int (*BuildFunctor)(const std::vector<std::string>&, char*);
    //! @brief Alias for the attribute of Option.
    struct OptionAttr
    {
        //! @brief Help message.
        HelpMessage message;
        //! @brief Build functor.
        BuildFunctor functor;
    };
    //! @brief Alias for the map of Option and OptionTuple.
    using OptionMap = std::map<Option, OptionAttr>;
    //! @brief Get the viewer options.
    //! @return viewer options
    const OptionMap& viewerOptions() const;
    //! @brief Get the TCP server host address.
    //! @return TCP server host address
    std::string viewerTCPHost() const;
    //! @brief Get the TCP server port number.
    //! @return TCP server port number
    std::uint16_t viewerTCPPort() const;
    //! @brief Get the UDP server host address.
    //! @return UDP server host address
    std::string viewerUDPHost() const;
    //! @brief Get the UDP server port number.
    //! @return UDP server port number
    std::uint16_t viewerUDPPort() const;
    //! @brief Parse the TLV packet.
    //! @param buffer - TLV packet buffer
    //! @param length - buffer length
    //! @return value of TLV after parsing
    tlv::TLVValue parseTLVPacket(char* buffer, const int length) const;
    //! @brief Await depending on the output state.
    void outputAwait();
    //! @brief Awaken depending on the output state.
    void outputAwaken();

    //! @brief Maximum size of the shared memory.
    static constexpr std::uint64_t maxShmSize{65536 * 10};
    //! @brief Memory that can be accessed by multiple programs simultaneously.
    struct alignas(64) SharedMemory
    {
        //! @brief Flag for operable.
        std::atomic<bool> signal{false};
        //! @brief Shared memory buffer.
        char buffer[maxShmSize]{'\0'};
    };

private:
    //! @brief Construct a new View object.
    //! @param initState - initialization value of state
    explicit View(const StateType initState = State::init) noexcept :
        FSM(initState),
        tcpHost(CONFIG_HELPER_VIEWER_TCP_HOST),
        tcpPort(CONFIG_HELPER_VIEWER_TCP_PORT),
        udpHost(CONFIG_HELPER_VIEWER_UDP_HOST),
        udpPort(CONFIG_HELPER_VIEWER_UDP_PORT)
    {
    }

    //! @brief Timeout period (ms) to waiting for the viewer to change to the target state.
    const std::uint64_t timeoutPeriod{static_cast<std::uint64_t>(CONFIG_HELPER_TIMEOUT)};
    // clang-format off
    //! @brief Mapping table of all viewer options.
    const OptionMap optionDispatcher{
        // - Option -+--------------- Help ---------------+------- Build Packet -------
        // ----------+------------------------------------+----------------------------
        { "bash"     , { "execute bash commands in quotes" , &buildTLVPacket2Bash    }},
        { "journal"  , { "view the log with highlights"    , &buildTLVPacket2Journal }},
        { "monitor"  , { "show the status of the process"  , &buildTLVPacket2Monitor }}
        // ----------+------------------------------------+----------------------------
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
    //! @brief Build the TLV packet to stop connection.
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket2Stop(char* buffer);
    //! @brief Build the TLV packet to get bash outputs.
    //! @param args - container of arguments
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket2Bash(const std::vector<std::string>& args, char* buffer);
    //! @brief Build the TLV packet to view log contents.
    //! @param args - container of arguments
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket2Journal(const std::vector<std::string>& args, char* buffer);
    //! @brief Build the TLV packet to show status information.
    //! @param args - container of arguments
    //! @param buffer - TLV packet buffer
    //! @return buffer length
    static int buildTLVPacket2Monitor(const std::vector<std::string>& args, char* buffer);
    //! @brief Encrypt the message with AES-128-CFB-128.
    //! @param buf - message buffer
    //! @param len - buffer length
    static void encryptMessage(char* buf, const int len);
    //! @brief Decrypt the message with AES-128-CFB-128.
    //! @param buf - message buffer
    //! @param len - buffer length
    static void decryptMessage(char* buf, const int len);
    //! @brief Fill the shared memory.
    //! @param contents - contents to be filled
    //! @return shm id
    static int fillSharedMemory(const std::string& contents);
    //! @brief Print the shared memory.
    //! @param shmId - shm id
    //! @param withoutPaging - whether output without paging
    static void printSharedMemory(const int shmId, const bool withoutPaging = true);
    //! @brief Segmented output.
    //! @param buffer - output buffer
    static void segmentedOutput(const char* const buffer);
    //! @brief Get the log contents.
    //! @return log contents
    static std::string getLogContents();
    //! @brief Get the status information.
    //! @return status information
    static std::string getStatusInformation();

    //! @brief TCP server.
    std::shared_ptr<utility::socket::TCPServer> tcpServer;
    //! @brief UDP server.
    std::shared_ptr<utility::socket::UDPServer> udpServer;
    //! @brief Mutex for controlling server.
    mutable std::mutex mtx{};
    //! @brief The synchronization condition for server. Use with mtx.
    std::condition_variable cv{};
    //! @brief Flag to indicate whether it is viewing.
    std::atomic<bool> ongoing{false};
    //! @brief Flag for rollback request.
    std::atomic<bool> toReset{false};
    //! @brief Maximum length of the message.
    static constexpr std::uint32_t maxMsgLength{1024};
    //! @brief Mutex for controlling output.
    mutable std::mutex outputMtx{};
    //! @brief The synchronization condition for output. Use with outputMtx.
    std::condition_variable outputCv{};

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
    using TransitionMap = Map<
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
    //! @brief Await notification for rollback.
    //! @return whether rollback is required or not
    bool awaitNotification4Rollback();

protected:
    friend std::ostream& operator<<(std::ostream& os, const State state);
};
} // namespace view
} // namespace application
