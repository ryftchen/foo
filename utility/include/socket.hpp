//! @file socket.hpp
//! @author ryftchen
//! @brief The declarations (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <arpa/inet.h>
#include <future>
#include <memory>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Network-socket-related functions in the utility module.
namespace socket
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_SOCKET";
}
extern const char* version() noexcept;

//! @brief Network socket.
class Socket
{
public:
    //! @brief Construct a new Socket object.
    Socket(const Socket&) = delete;
    //! @brief Construct a new Socket object.
    Socket(Socket&&) = delete;
    //! @brief The operator (=) overloading of Socket class.
    //! @return reference of the Socket object
    Socket& operator=(const Socket&) = delete;
    //! @brief The operator (=) overloading of Socket class.
    //! @return reference of the Socket object
    Socket& operator=(Socket&&) = delete;

    //! @brief Close the socket.
    void close();
    //! @brief Wait for the task to be done and then exit.
    void join();
    //! @brief Set the flag to indicate that a stop has been requested.
    void requestStop();
    //! @brief Check whether a stop has been requested.
    //! @return has been requested or not
    bool stopRequested() const;
    //! @brief Get the transport ip address.
    //! @return transport ip address
    std::string transportAddress() const;
    //! @brief Get the transport port number.
    //! @return transport port number
    std::uint16_t transportPort() const;

    //! @brief Transport information.
    ::sockaddr_in sockAddr{};
    //! @brief Bytes buffer size.
    static constexpr std::uint16_t bufferSize{0xFFFFU};
    //! @brief Enumerate specific socket types.
    enum class Type : std::uint8_t
    {
        //! @brief TCP.
        tcp = ::SOCK_STREAM,
        //! @brief UDP.
        udp = ::SOCK_DGRAM
    };

private:
    //! @brief Flag for request of stop.
    std::atomic_bool exitReady{false};
    //! @brief Result of asynchronous operations for the non-detached thread.
    std::future<void> ownedTask;
    //! @brief Spin lock to synchronize access to the socket.
    mutable ::pthread_spinlock_t sockLock{};

    //! @brief Acquire the spin lock to ensure mutual exclusion.
    void spinLock() const;
    //! @brief Release the spin lock to allow other threads to acquire it.
    void spinUnlock() const;

protected:
    //! @brief Construct a new Socket object.
    //! @param sockType - socket type
    //! @param sockId - socket id
    explicit Socket(const Type sockType = Type::tcp, const int sockId = -1);
    //! @brief Destroy the Socket object.
    ~Socket();

    //! @brief Start the detached operations.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of function arguments
    //! @param func - callable function
    //! @param args - function arguments
    template <typename Func, typename... Args>
    static void spawnDetached(Func&& func, Args&&... args);
    //! @brief Start the joinable operations.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of function arguments
    //! @param func - callable function
    //! @param args - function arguments
    template <typename Func, typename... Args>
    void spawnJoinable(Func&& func, Args&&... args);
    //! @brief Guard for the spin lock to ensure mutual exclusion.
    class Guard
    {
    public:
        //! @brief Construct a new Guard object.
        //! @param socket - target socket
        explicit Guard(const Socket& socket) : socket{socket} { socket.spinLock(); }
        //! @brief Destroy the Guard object.
        virtual ~Guard() { socket.spinUnlock(); }
        //! @brief Construct a new Guard object.
        Guard(const Guard&) = default;
        //! @brief Construct a new Guard object.
        Guard(Guard&&) noexcept = default;
        //! @brief The operator (=) overloading of Guard class.
        //! @return reference of the Guard object
        Guard& operator=(const Guard&) = delete;
        //! @brief The operator (=) overloading of Guard class.
        //! @return reference of the Guard object
        Guard& operator=(Guard&&) = delete;

    private:
        //! @brief Socket to be mutually exclusive.
        const Socket& socket;
    };

    //! @brief File descriptor.
    int sock{-1};
};

//! @brief TCP socket.
class TCPSocket : public Socket, public std::enable_shared_from_this<TCPSocket>
{
public:
    //! @brief Construct a new TCPSocket object.
    //! @param sockId - socket id
    explicit TCPSocket(const int sockId = -1) : Socket(Type::tcp, sockId) {}

    //! @brief Send bytes from the buffer to socket FD.
    //! @param bytes - bytes buffer
    //! @param size - length of buffer
    //! @return sent size
    ::ssize_t send(const char* const bytes, const std::size_t size);
    //! @brief Send the message string to socket FD.
    //! @param message - message string
    //! @return sent size
    ::ssize_t send(const std::string_view message);
    //! @brief Open a connection on socket FD to peer.
    //! @param ip - peer ip address
    //! @param port - peer port number
    void connect(const std::string& ip, const std::uint16_t port);
    //! @brief Create the thread to receive.
    //! @param detached - whether to detach
    void receive(const bool detached = false);

    //! @brief Alias for the handling on message received.
    using MessageCallback = std::function<void(const std::string_view)>;
    //! @brief Alias for the handling on raw message received.
    using RawMessageCallback = std::function<void(char* const, const std::size_t)>;
    //! @brief Bind the callback to handle the received message.
    //! @param callback - callback on received message
    void subscribeMessage(MessageCallback callback);
    //! @brief Bind the callback to handle the received raw message.
    //! @param callback - callback on received raw message
    void subscribeRawMessage(RawMessageCallback callback);

private:
    //! @brief Handling on message received.
    std::atomic<std::shared_ptr<MessageCallback>> msgCb;
    //! @brief Handling on raw message received.
    std::atomic<std::shared_ptr<RawMessageCallback>> rawMsgCb;

    //! @brief Receive bytes from socket FD.
    //! @param socket - target socket
    static void doReceive(const std::shared_ptr<TCPSocket> socket);

    //! @brief Emit the received message.
    //! @param message - received message
    void onMessage(const std::string_view message) const;
    //! @brief Emit the received raw message.
    //! @param bytes - received bytes buffer
    //! @param size - length of buffer
    void onRawMessage(char* const bytes, const std::size_t size) const;
};

//! @brief TCP server.
class TCPServer : public Socket, public std::enable_shared_from_this<TCPServer>
{
public:
    //! @brief Construct a new TCPServer object.
    explicit TCPServer();

    //! @brief Bind to transport ip address and port number.
    //! @param ip - ip address
    //! @param port - port number
    void bind(const std::string& ip, const std::uint16_t port);
    //! @brief Bind to transport port number with default ip address.
    //! @param port - port number
    void bind(const std::uint16_t port);
    //! @brief Listen on a port number. Wait for the connection to be established.
    void listen();
    //! @brief Create the thread to accept the connection from the client.
    //! @param detached - whether to detach
    void accept(const bool detached = false);

    //! @brief Alias for the handling on new connection.
    using ConnectionCallback = std::function<void(const std::shared_ptr<TCPSocket>)>;
    //! @brief Bind the callback to handle the new connection.
    //! @param callback - callback on new connection
    void subscribeConnection(ConnectionCallback callback);

private:
    //! @brief Handling on new connection.
    std::atomic<std::shared_ptr<ConnectionCallback>> connCb;

    //! @brief Accept the connection on socket FD.
    //! @param server - target server
    static void accept(const std::shared_ptr<TCPServer> server);

    //! @brief Emit the new connection.
    //! @param client - new connected client
    void onConnection(const std::shared_ptr<TCPSocket> client) const;
};

//! @brief UDP socket.
class UDPSocket : public Socket, public std::enable_shared_from_this<UDPSocket>
{
public:
    //! @brief Construct a new UDPSocket object.
    //! @param sockId - socket id
    explicit UDPSocket(const int sockId = -1) : Socket(Type::udp, sockId) {}

    //! @brief Send bytes from the buffer on socket FD to peer.
    //! @param bytes - bytes buffer
    //! @param size - length of buffer
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    ::ssize_t sendTo(const char* const bytes, const std::size_t size, const std::string& ip, const std::uint16_t port);
    //! @brief Send the message string on socket FD to peer.
    //! @param message - message string
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    ::ssize_t sendTo(const std::string_view message, const std::string& ip, const std::uint16_t port);
    //! @brief Send bytes from the buffer to socket FD.
    //! @param bytes - bytes buffer
    //! @param size - length of buffer
    //! @return sent size
    ::ssize_t send(const char* const bytes, const std::size_t size);
    //! @brief Send the message string to socket FD.
    //! @param message - message string
    //! @return sent size
    ::ssize_t send(const std::string_view message);
    //! @brief Open a connection on socket FD to peer.
    //! @param ip - peer ip address
    //! @param port - peer port number
    void connect(const std::string& ip, const std::uint16_t port);
    //! @brief Create the thread to receive.
    //! @param detached - whether to detach
    void receive(const bool detached = false);
    //! @brief Create the thread to receive from peer.
    //! @param detached - whether to detach
    void receiveFrom(const bool detached = false);

    //! @brief Alias for the handling on message received.
    using MessageCallback = std::function<void(const std::string_view, const std::string&, const std::uint16_t)>;
    //! @brief Alias for the handling on raw message received.
    using RawMessageCallback =
        std::function<void(char* const, const std::size_t, const std::string&, const std::uint16_t)>;
    //! @brief Bind the callback to handle the received message.
    //! @param callback - callback on received message
    void subscribeMessage(MessageCallback callback);
    //! @brief Bind the callback to handle the received raw message.
    //! @param callback - callback on received raw message
    void subscribeRawMessage(RawMessageCallback callback);

private:
    //! @brief Handling on message received.
    std::atomic<std::shared_ptr<MessageCallback>> msgCb;
    //! @brief Handling on raw message received.
    std::atomic<std::shared_ptr<RawMessageCallback>> rawMsgCb;

    //! @brief Receive bytes from socket FD.
    //! @param socket - target socket
    static void doReceive(const std::shared_ptr<UDPSocket> socket);
    //! @brief Receive bytes through socket FD.
    //! @param socket - target socket
    static void doReceiveFrom(const std::shared_ptr<UDPSocket> socket);

    //! @brief Emit the received message.
    //! @param message - received message
    //! @param ip - source ip address
    //! @param port - source port number
    void onMessage(const std::string_view message, const std::string& ip, const std::uint16_t port) const;
    //! @brief Emit the received raw message.
    //! @param bytes - received bytes buffer
    //! @param size - length of buffer
    //! @param ip - source ip address
    //! @param port - source port number
    void onRawMessage(char* const bytes, const std::size_t size, const std::string& ip, const std::uint16_t port) const;
};

//! @brief UDP server.
class UDPServer : public UDPSocket
{
public:
    //! @brief Bind to transport ip address and port number.
    //! @param ip - ip address
    //! @param port - port number
    void bind(const std::string& ip, const std::uint16_t port);
    //! @brief Bind to transport port number with default ip address.
    //! @param port - port number
    void bind(const std::uint16_t port);
};
} // namespace socket
} // namespace utility
