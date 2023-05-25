//! @file socket.hpp
//! @author ryftchen
//! @brief The declarations (socket) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

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
    void toClose() const;
    void waitIfAlive();
    [[nodiscard]] std::string getRemoteAddress() const;
    [[nodiscard]] int getRemotePort() const;
    [[nodiscard]] int getFileDescriptor() const;
    inline void setBlocking() const;
    inline void setNonBlocking() const;

    static constexpr uint16_t bufferSize{0xFFFF};
    sockaddr_in address{};
    enum Type
    {
        tcp = SOCK_STREAM,
        udp = SOCK_DGRAM
    };

protected:
    explicit Socket(const Type sockType = tcp, const int socketId = -1);
    virtual ~Socket() = default;

    static std::string ipToString(const sockaddr_in& addr);
    void setTimeout(const int microseconds) const;
    int sock{0};
    std::future<void> thrFut{};
};

inline void Socket::setBlocking() const
{
    setTimeout(0);
}

inline void Socket::setNonBlocking() const
{
    setTimeout(1);
}

class TCPSocket : public Socket
{
public:
    explicit TCPSocket(const int socketId = -1) : Socket(tcp, socketId) {}

    void setAddress(const sockaddr_in& addr);
    [[nodiscard]] sockaddr_in getAddress() const;
    int toSend(const char* bytes, const std::size_t length);
    int toSend(const std::string& message);
    void toConnect(
        const std::string& host,
        const uint16_t port,
        const std::function<void()> onConnected =
            []()
        {
        });
    void toReceive(const bool detach = false);
    std::function<void(const std::string&)> onMessageReceived{};
    std::function<void(char*, const int)> onRawMessageReceived{};
    std::function<void(const int)> onSocketClosed{};
    std::atomic<bool> activeRelease{false};

private:
    static void toRecv(TCPSocket* socket);
};

class TCPServer : public Socket
{
public:
    explicit TCPServer();

    void toBind(const std::string& host, const uint16_t port);
    void toBind(const uint16_t port);
    void toListen();
    void toAccept(const bool detach = false);
    std::function<void(TCPSocket*)> onNewConnection{};

private:
    static void toAccept(TCPServer* server);
    static constexpr int retryTimes{20};
};

class UDPSocket : public Socket
{
public:
    explicit UDPSocket(const int socketId = -1) : Socket(Type::udp, socketId){};

    int toSendTo(const char* bytes, const std::size_t length, const std::string& host, const uint16_t port);
    int toSendTo(const std::string& message, const std::string& host, const uint16_t port);
    int toSend(const char* bytes, const std::size_t length);
    int toSend(const std::string& message);
    void toConnect(const std::string& host, const uint16_t port);
    void toReceive(const bool detach = false);
    void toReceiveFrom(const bool detach = false);
    std::function<void(const std::string&, const std::string&, const uint16_t)> onMessageReceived{};
    std::function<void(char*, const int, const std::string&, const uint16_t)> onRawMessageReceived{};

private:
    static void toRecv(UDPSocket* socket);
    static void toRecvFrom(UDPSocket* socket);
};

class UDPServer : public UDPSocket
{
public:
    void toBind(const std::string& host, const uint16_t port);
    void toBind(const uint16_t port);
    void setBroadcast();
};
} // namespace utility::socket
