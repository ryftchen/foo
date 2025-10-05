//! @file socket.hpp
//! @author ryftchen
//! @brief The declarations (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

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
    Socket(Socket&&) noexcept = delete;
    //! @brief The operator (=) overloading of Socket class.
    //! @return reference of the Socket object
    Socket& operator=(const Socket&) = delete;
    //! @brief The operator (=) overloading of Socket class.
    //! @return reference of the Socket object
    Socket& operator=(Socket&&) noexcept = delete;

    //! @brief Close the socket.
    void toClose();
    //! @brief Wait for the task to be done and then exit.
    void toJoin();
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
    int transportPort() const;

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
    std::future<void> asyncTask;
    //! @brief Spin lock to synchronize access to the socket.
    mutable ::pthread_spinlock_t sockLock{};

    //! @brief Acquire the spin lock to ensure mutual exclusion.
    void spinLock() const;
    //! @brief Release the spin lock to allow other threads to acquire it.
    void spinUnlock() const;

protected:
    //! @brief Construct a new Socket object.
    //! @param socketType - socket type
    //! @param socketId - socket id
    explicit Socket(const Type socketType = Type::tcp, const int socketId = -1);
    //! @brief Destroy the Socket object.
    ~Socket();

    //! @brief Launch the asynchronous operations.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of function arguments
    //! @param func - callable function
    //! @param args - function arguments
    template <typename Func, typename... Args>
    void launchAsyncTask(Func&& func, Args&&... args);
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
        Guard(const Guard&) = delete;
        //! @brief Construct a new Guard object.
        Guard(Guard&&) noexcept = delete;
        //! @brief The operator (=) overloading of Guard class.
        //! @return reference of the Guard object
        Guard& operator=(const Guard&) = delete;
        //! @brief The operator (=) overloading of Guard class.
        //! @return reference of the Guard object
        Guard& operator=(Guard&&) noexcept = delete;

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
    //! @param socketId - socket id
    explicit TCPSocket(const int socketId = -1) : Socket(Type::tcp, socketId) {}

    //! @brief Send bytes from the buffer to socket FD.
    //! @param bytes - bytes buffer
    //! @param length - length of buffer
    //! @return sent size
    int toSend(const char* const bytes, const std::size_t length);
    //! @brief Send the message string to socket FD.
    //! @param message - message string
    //! @return sent size
    int toSend(const std::string_view message);
    //! @brief Open a connection on socket FD to peer.
    //! @param ip - peer ip address
    //! @param port - peer port number
    void toConnect(const std::string& ip, const std::uint16_t port);
    //! @brief Create the thread to receive.
    //! @param toDetach - whether to detach
    void toReceive(const bool toDetach = false);

    //! @brief Alias for the handling on message received.
    using MessageCallback = std::function<void(const std::string_view)>;
    //! @brief Alias for the handling on raw message received.
    using RawMessageCallback = std::function<void(char*, const int)>;
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
    static void toRecv(const std::shared_ptr<TCPSocket> socket);

    //! @brief Emit the received message.
    //! @param message - received message
    void onMessage(const std::string_view message) const;
    //! @brief Emit the received raw message.
    //! @param buffer - received bytes buffer
    //! @param length - length of buffer
    void onRawMessage(char* buffer, const int length) const;
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
    void toBind(const std::string& ip, const std::uint16_t port);
    //! @brief Bind to transport port number with default ip address.
    //! @param port - port number
    void toBind(const std::uint16_t port);
    //! @brief Listen on a port number. Wait for the connection to be established.
    void toListen();
    //! @brief Create the thread to accept the connection from the client.
    //! @param toDetach - whether to detach
    void toAccept(const bool toDetach = false);

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
    static void toAccept(const std::shared_ptr<TCPServer> server);

    //! @brief Emit the new connection.
    //! @param client - new connected client
    void onConnection(const std::shared_ptr<TCPSocket> client) const;
};

//! @brief UDP socket.
class UDPSocket : public Socket, public std::enable_shared_from_this<UDPSocket>
{
public:
    //! @brief Construct a new UDPSocket object.
    //! @param socketId - socket id
    explicit UDPSocket(const int socketId = -1) : Socket(Type::udp, socketId) {}

    //! @brief Send bytes from the buffer on socket FD to peer.
    //! @param bytes - bytes buffer
    //! @param length - length of buffer
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    int toSendTo(const char* const bytes, const std::size_t length, const std::string& ip, const std::uint16_t port);
    //! @brief Send the message string on socket FD to peer.
    //! @param message - message string
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    int toSendTo(const std::string_view message, const std::string& ip, const std::uint16_t port);
    //! @brief Send bytes from the buffer to socket FD.
    //! @param bytes - bytes buffer
    //! @param length - length of buffer
    //! @return sent size
    int toSend(const char* const bytes, const std::size_t length);
    //! @brief Send the message string to socket FD.
    //! @param message - message string
    //! @return sent size
    int toSend(const std::string_view message);
    //! @brief Open a connection on socket FD to peer.
    //! @param ip - peer ip address
    //! @param port - peer port number
    void toConnect(const std::string& ip, const std::uint16_t port);
    //! @brief Create the thread to receive.
    //! @param toDetach - whether to detach
    void toReceive(const bool toDetach = false);
    //! @brief Create the thread to receive from peer.
    //! @param toDetach - whether to detach
    void toReceiveFrom(const bool toDetach = false);

    //! @brief Alias for the handling on message received.
    using MessageCallback = std::function<void(const std::string_view, const std::string&, const std::uint16_t)>;
    //! @brief Alias for the handling on raw message received.
    using RawMessageCallback = std::function<void(char*, const int, const std::string&, const std::uint16_t)>;
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
    static void toRecv(const std::shared_ptr<UDPSocket> socket);
    //! @brief Receive bytes through socket FD.
    //! @param socket - target socket
    static void toRecvFrom(const std::shared_ptr<UDPSocket> socket);

    //! @brief Emit the received message.
    //! @param message - received message
    //! @param ip - source ip address
    //! @param port - source port number
    void onMessage(const std::string_view message, const std::string& ip, const std::uint16_t port) const;
    //! @brief Emit the received raw message.
    //! @param buffer - received bytes buffer
    //! @param length - length of buffer
    //! @param ip - source ip address
    //! @param port - source port number
    void onRawMessage(char* buffer, const int length, const std::string& ip, const std::uint16_t port) const;
};

//! @brief UDP server.
class UDPServer : public UDPSocket
{
public:
    //! @brief Bind to transport ip address and port number.
    //! @param ip - ip address
    //! @param port - port number
    void toBind(const std::string& ip, const std::uint16_t port);
    //! @brief Bind to transport port number with default ip address.
    //! @param port - port number
    void toBind(const std::uint16_t port);
};
} // namespace socket
} // namespace utility
