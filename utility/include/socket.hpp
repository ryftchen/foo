//! @file socket.hpp
//! @author ryftchen
//! @brief The declarations (socket) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <arpa/inet.h>
#include <future>

//! @brief Network-socket-related functions in the utility module.
namespace utility::socket
{
//! @brief Network socket.
class Socket
{
public:
    //! @brief Close the socket.
    void toClose() const;
    //! @brief Get the transport ip address.
    //! @return transport ip address
    [[nodiscard]] std::string getTransportAddress() const;
    //! @brief Get the transport port number.
    //! @return transport port number
    [[nodiscard]] int getTransportPort() const;
    //! @brief Get the file descriptor.
    //! @return file descriptor
    [[nodiscard]] int getFileDescriptor() const;
    //! @brief When using asynchronous, wait for the receiving thread to exit.
    void waitIfAlive();
    //! @brief Set the blocking mode.
    void setBlocking() const;
    //! @brief Set the non-blocking mode.
    void setNonBlocking() const;

    //! @brief Bytes buffer size.
    static constexpr std::uint16_t bufferSize{0xFFFF};
    //! @brief Transport information.
    ::sockaddr_in sockAddr{};

    //! @brief Enumerate specific socket types.
    enum Type
    {
        //! @brief TCP.
        tcp = ::SOCK_STREAM,
        //! @brief UDP.
        udp = ::SOCK_DGRAM
    };

protected:
    //! @brief Construct a new Socket object.
    //! @param socketType - socket type
    //! @param socketId -  socket id
    explicit Socket(const Type socketType = tcp, const int socketId = -1);
    //! @brief Destroy the Socket object.
    virtual ~Socket();

    //! @brief Get the ip address from transport information.
    //! @param addr - transport information
    //! @return ip address string
    static std::string ipString(const ::sockaddr_in& addr);
    //! @brief Set the timeout period.
    //! @param microseconds - timeout period
    void setTimeout(const int microseconds) const;

    //! @brief File descriptor.
    int sock{0};
    //! @brief Result of asynchronous operations for the receiving thread.
    std::future<void> fut{};
};

//! @brief TCP socket.
class TCPSocket : public Socket
{
public:
    //! @brief Construct a new TCPSocket object.
    //! @param socketId - socket id
    explicit TCPSocket(const int socketId = -1) : Socket(tcp, socketId){};

    //! @brief Set the transport information.
    //! @param addr - transport information
    void setAddress(const ::sockaddr_in& addr);
    //! @brief Get the transport information.
    //! @return transport information
    [[nodiscard]] ::sockaddr_in getAddress() const;
    //! @brief Send bytes from the buffer to socket FD.
    //! @param bytes - bytes buffer
    //! @param length - length of buffer
    //! @return sent size
    int toSend(const char* bytes, const std::size_t length);
    //! @brief Send the message string to socket FD.
    //! @param message - message string
    //! @return sent size
    int toSend(const std::string& message);
    //! @brief Open a connection on socket FD to peer.
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @param onConnected - handling on connected
    void toConnect(
        const std::string& ip,
        const std::uint16_t port,
        const std::function<void()> onConnected =
            []()
        {
        });
    //! @brief Create a thread to receive.
    //! @param detach - whether to detach
    void toReceive(const bool detach = false);
    //! @brief Handling on message received.
    std::function<void(const std::string&)> onMessageReceived{};
    //! @brief Handling on raw message received.
    std::function<void(char*, const int)> onRawMessageReceived{};
    //! @brief Handling on socket closed.
    std::function<void(const int)> onSocketClosed{};
    //! @brief Flag for automatic release.
    std::atomic<bool> autoRelease{false};

private:
    //! @brief Receive bytes from socket FD.
    //! @param socket - target socket
    static void toRecv(TCPSocket* socket);
};

//! @brief TCP server.
class TCPServer : public Socket
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
    //! @brief Create a thread to accept the connection from the client.
    //! @param detach - whether to detach
    void toAccept(const bool detach = false);
    //! @brief Handling on new connection.
    std::function<void(TCPSocket*)> onNewConnection{};

private:
    //! @brief Accept the connection on socket FD.
    //! @param server - target server
    static void toAccept(TCPServer* server);
    //! @brief Retry times for listening.
    static constexpr int retryTimes{10};
};

//! @brief UDP socket.
class UDPSocket : public Socket
{
public:
    //! @brief Construct a new UDPSocket object.
    //! @param socketId - socket id
    explicit UDPSocket(const int socketId = -1) : Socket(Type::udp, socketId){};

    //! @brief Send bytes from the buffer on socket FD to peer.
    //! @param bytes - bytes buffer
    //! @param length - length of buffer
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    int toSendTo(const char* bytes, const std::size_t length, const std::string& ip, const std::uint16_t port);
    //! @brief Send the message string on socket FD to peer.
    //! @param message - message string
    //! @param ip - peer ip address
    //! @param port - peer port number
    //! @return sent size
    int toSendTo(const std::string& message, const std::string& ip, const std::uint16_t port);
    //! @brief Send bytes from the buffer to socket FD.
    //! @param bytes - bytes buffer
    //! @param length - length of buffer
    //! @return sent size
    int toSend(const char* bytes, const std::size_t length);
    //! @brief Send the message string to socket FD.
    //! @param message - message string
    //! @return sent size
    int toSend(const std::string& message);
    //! @brief Open a connection on socket FD to peer.
    //! @param ip - peer ip address
    //! @param port - peer port number
    void toConnect(const std::string& ip, const std::uint16_t port);
    //! @brief Create a thread to receive.
    //! @param detach - whether to detach
    void toReceive(const bool detach = false);
    //! @brief Create a thread to receive from peer.
    //! @param detach - whether to detach
    void toReceiveFrom(const bool detach = false);
    //! @brief Handling on message received.
    std::function<void(const std::string&, const std::string&, const std::uint16_t)> onMessageReceived{};
    //! @brief Handling on raw message received.
    std::function<void(char*, const int, const std::string&, const std::uint16_t)> onRawMessageReceived{};

private:
    //! @brief Receive bytes from socket FD.
    //! @param socket - target socket
    static void toRecv(UDPSocket* socket);
    //! @brief Receive bytes through socket FD.
    //! @param socket - target socket
    static void toRecvFrom(UDPSocket* socket);
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
    //! @brief Set the broadcast mode.
    void setBroadcast();
};
} // namespace utility::socket
