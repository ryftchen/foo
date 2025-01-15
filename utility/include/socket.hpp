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
    //! @brief Get the transport ip address.
    //! @return transport ip address
    [[nodiscard]] std::string transportAddress() const;
    //! @brief Get the transport port number.
    //! @return transport port number
    [[nodiscard]] int transportPort() const;
    //! @brief Close the socket.
    void toClose();
    //! @brief Set the flag to indicate that an asynchronous exit has been requested.
    void asyncExit();
    //! @brief Check whether an asynchronous exit has been requested.
    //! @return has been requested or not
    [[nodiscard]] bool shouldExit() const;
    //! @brief When using asynchronous, wait for the non-detached thread to exit.
    void waitIfAlive();
    //! @brief Acquire the spin lock to ensure mutual exclusion.
    void spinLock();
    //! @brief Release the spin lock to allow other threads to acquire it.
    void spinUnlock();

    //! @brief Transport information.
    ::sockaddr_in sockAddr{};
    //! @brief Bytes buffer size.
    static constexpr std::uint16_t bufferSize{0xFFFFU};
    //! @brief Enumerate specific socket types.
    enum Type
    {
        //! @brief TCP.
        tcp = ::SOCK_STREAM,
        //! @brief UDP.
        udp = ::SOCK_DGRAM
    };

private:
    //! @brief Flag for asynchronous exit.
    std::atomic<bool> exitReady{false};
    //! @brief Spin lock to synchronize access to the socket.
    ::pthread_spinlock_t sockLock{};

protected:
    //! @brief Construct a new Socket object.
    //! @param socketType - socket type
    //! @param socketId - socket id
    explicit Socket(const Type socketType = Type::tcp, const int socketId = -1);
    //! @brief Destroy the Socket object.
    virtual ~Socket();

    //! @brief Get the ip address from transport information.
    //! @param addr - transport information
    //! @return ip address string
    static std::string ipString(const ::sockaddr_in& addr);

    //! @brief File descriptor.
    int sock{-1};
    //! @brief Result of asynchronous operations for the non-detached thread.
    std::future<void> task{};
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
    //! @param onConnected - handling on connected
    void toConnect(
        const std::string_view ip, const std::uint16_t port, const std::function<void()>& onConnected = []() {});
    //! @brief Create a thread to receive.
    //! @param toDetach - whether to detach
    void toReceive(const bool toDetach = false);
    //! @brief Handling on message received.
    std::function<void(const std::string_view)> onMessageReceived{};
    //! @brief Handling on raw message received.
    std::function<void(char*, const int)> onRawMessageReceived{};
    //! @brief Handling on socket closed.
    std::function<void(const int)> onSocketClosed{};

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
    void toBind(const std::string_view ip, const std::uint16_t port);
    //! @brief Bind to transport port number with default ip address.
    //! @param port - port number
    void toBind(const std::uint16_t port);
    //! @brief Listen on a port number. Wait for the connection to be established.
    void toListen();
    //! @brief Create a thread to accept the connection from the client.
    //! @param toDetach - whether to detach
    void toAccept(const bool toDetach = false);
    //! @brief Handling on new connection.
    std::function<void(const std::shared_ptr<TCPSocket>)> onNewConnection{};

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
    int toSendTo(
        const char* const bytes, const std::size_t length, const std::string_view ip, const std::uint16_t port);
    //! @brief Send the message string on socket FD to peer.
    //! @param message - message string
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    int toSendTo(const std::string_view message, const std::string_view ip, const std::uint16_t port);
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
    void toConnect(const std::string_view ip, const std::uint16_t port);
    //! @brief Create a thread to receive.
    //! @param toDetach - whether to detach
    void toReceive(const bool toDetach = false);
    //! @brief Create a thread to receive from peer.
    //! @param toDetach - whether to detach
    void toReceiveFrom(const bool toDetach = false);
    //! @brief Handling on message received.
    std::function<void(const std::string_view, const std::string_view, const std::uint16_t)> onMessageReceived{};
    //! @brief Handling on raw message received.
    std::function<void(char*, const int, const std::string_view, const std::uint16_t)> onRawMessageReceived{};

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
    void toBind(const std::string_view ip, const std::uint16_t port);
    //! @brief Bind to transport port number with default ip address.
    //! @param port - port number
    void toBind(const std::uint16_t port);
};
} // namespace socket
} // namespace utility
