//! @file socket.hpp
//! @author ryftchen
//! @brief The declarations (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <arpa/inet.h>
#include <future>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Network-socket-related functions in the utility module.
namespace socket
{
extern const char* version() noexcept;

//! @brief Network socket.
class Socket
{
public:
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
    class SockGuard
    {
    public:
        //! @brief Construct a new SockGuard object.
        //! @param socket - target socket
        explicit SockGuard(const Socket& socket) : socket{socket} { socket.spinLock(); }
        //! @brief Destroy the SockGuard object.
        virtual ~SockGuard() { socket.spinUnlock(); }

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
    //! @brief Handling on message received.
    std::function<void(const std::string_view)> onMessageReceived;
    //! @brief Handling on raw message received.
    std::function<void(char*, const int)> onRawMessageReceived;

private:
    //! @brief Receive bytes from socket FD.
    //! @param socket - target socket
    static void toRecv(const std::shared_ptr<TCPSocket> socket);
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
    //! @brief Handling on new connection.
    std::function<void(const std::shared_ptr<TCPSocket>)> onNewConnection;

private:
    //! @brief Accept the connection on socket FD.
    //! @param server - target server
    static void toAccept(const std::shared_ptr<TCPServer> server);
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
    //! @brief Handling on message received.
    std::function<void(const std::string_view, const std::string&, const std::uint16_t)> onMessageReceived;
    //! @brief Handling on raw message received.
    std::function<void(char*, const int, const std::string&, const std::uint16_t)> onRawMessageReceived;

private:
    //! @brief Receive bytes from socket FD.
    //! @param socket - target socket
    static void toRecv(const std::shared_ptr<UDPSocket> socket);
    //! @brief Receive bytes through socket FD.
    //! @param socket - target socket
    static void toRecvFrom(const std::shared_ptr<UDPSocket> socket);
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
