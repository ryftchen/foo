//! @file view.hpp
//! @author ryftchen
//! @brief The declarations (view) in the application module.
//! @version 1.0
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

//! @brief Get the existing viewer instance.
#define VIEW_GET_EXISTING_INSTANCE application::view::View::getExistingInstance()
//! @brief Get the TCP host address of the viewer.
#define VIEW_TCP_HOST VIEW_GET_EXISTING_INSTANCE.getViewerTCPHost()
//! @brief Get the TCP port number of the viewer.
#define VIEW_TCP_PORT VIEW_GET_EXISTING_INSTANCE.getViewerTCPPort()
//! @brief Get the UDP host address of the viewer.
#define VIEW_UDP_HOST VIEW_GET_EXISTING_INSTANCE.getViewerUDPHost()
//! @brief Get the UDP port number of the viewer.
#define VIEW_UDP_PORT VIEW_GET_EXISTING_INSTANCE.getViewerUDPPort()
//! @brief Get all viewer options.
#define VIEW_OPTIONS VIEW_GET_EXISTING_INSTANCE.getViewerOptions()

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
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
    Packet(char* buf, const std::uint32_t len) : buffer(buf), tail(buffer + len), writer(buffer), reader(buffer){};
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
    //! @brief Get the existing View instance.
    //! @return reference of the View object
    static View& getExistingInstance();
    //! @brief Interface for running viewer.
    void runViewer();
    //! @brief Wait for the viewer to start. External use.
    void waitToStart();
    //! @brief Wait for the viewer to stop. External use.
    void waitToStop();
    //! @brief Request to rollback the viewer. External use.
    void requestToRollback();

    //! @brief Alias for the functor to build the TLV packet.
    typedef int (*BuildFunctor)(const std::vector<std::string>&, char*);
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
    OptionMap getViewerOptions() const;
    //! @brief Get a member of OptionTuple.
    //! @tparam T - type of member to be got
    //! @param tuple - a tuple containing the member types to be got
    //! @return member corresponding to the specific type
    template <typename T>
    static const T& get(const OptionTuple& tuple);
    //! @brief Get the TCP server host address.
    //! @return TCP server host address
    std::string getViewerTCPHost() const;
    //! @brief Get the TCP server port number.
    //! @return TCP server port number
    std::uint16_t getViewerTCPPort() const;
    //! @brief Get the UDP server host address.
    //! @return UDP server host address
    std::string getViewerUDPHost() const;
    //! @brief Get the UDP server port number.
    //! @return UDP server port number
    std::uint16_t getViewerUDPPort() const;
    //! @brief Parse the TLV packet.
    //! @param buffer - TLV packet buffer
    //! @param length - buffer length
    //! @return value of TLV after parsing
    static tlv::TLVValue parseTLVPacket(char* buffer, const int length);

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
    //! @brief Construct a new View object.
    //! @param initState - initialization value of state
    explicit View(const StateType initState = State::init) noexcept :
        FSM(initState),
        tcpHost(CONFIG_HELPER_VIEWER_TCP_HOST),
        tcpPort(CONFIG_HELPER_VIEWER_TCP_PORT),
        udpHost(CONFIG_HELPER_VIEWER_UDP_HOST),
        udpPort(CONFIG_HELPER_VIEWER_UDP_PORT){};

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
    static void printSharedMemory(const int shmId);
    //! @brief Get the log contents.
    //! @return log contents
    static std::string getLogContents();
    //! @brief Get the status information.
    //! @return status information
    static std::string getStatusInformation();
    //! @brief Maximum number of lines to view log contents.
    static constexpr std::uint32_t maxViewNumOfLines{20};

    //! @brief Maximum number of times to wait for the viewer to change to the target state.
    static constexpr std::uint16_t maxTimesOfWaitViewer{20};
    //! @brief Time interval (ms) to wait for the viewer to change to the target state.
    static constexpr std::uint16_t intervalOfWaitViewer{10};
    //! @brief Maximum length of the message.
    static constexpr std::uint32_t maxMsgLength{1024};
    //! @brief TCP server.
    std::shared_ptr<utility::socket::TCPServer> tcpServer;
    //! @brief UDP server.
    std::shared_ptr<utility::socket::UDPServer> udpServer;
    //! @brief Mutex for controlling server.
    mutable std::mutex mtx{};
    //! @brief The synchronization condition for server. Use with mtx.
    std::condition_variable cv{};
    //! @brief Flag to indicate whether it is viewing.
    std::atomic<bool> isViewing{false};
    //! @brief Flag for rollback request.
    std::atomic<bool> rollbackRequest{false};

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

template <typename T>
const T& View::get(const OptionTuple& tuple)
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
} // namespace view
} // namespace application
