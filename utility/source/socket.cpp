//! @file socket.cpp
//! @author ryftchen
//! @brief The definitions (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "socket.hpp"

#include <netdb.h>
#include <cstring>

namespace utility::socket
{
Socket::Socket(const Type socketType, const int socketId)
{
    if (-1 == socketId)
    {
        if ((sock = ::socket(AF_INET, socketType, 0)) == -1)
        {
            throw std::runtime_error("Socket creation error, errno: " + std::to_string(errno) + '.');
        }
    }
    else
    {
        sock = socketId;
    }
}

Socket::~Socket()
{
    toClose();
}

void Socket::toClose() const
{
    ::shutdown(sock, ::SHUT_RDWR);
    ::close(sock);
}

std::string Socket::getTransportAddress() const
{
    return ipString(sockAddr);
}

int Socket::getTransportPort() const
{
    return ::ntohs(sockAddr.sin_port);
}

int Socket::getFileDescriptor() const
{
    return sock;
}

void Socket::waitIfAlive()
{
    if (!fut.valid())
    {
        throw std::logic_error("Never use asynchronous.");
    }

    if (fut.wait_until(std::chrono::system_clock::now()) != std::future_status::ready)
    {
        fut.wait();
    }
}

void Socket::setBlocking() const
{
    setTimeout(0);
}

void Socket::setNonBlocking() const
{
    setTimeout(1);
}

std::string Socket::ipString(const ::sockaddr_in& addr)
{
    char ip[INET_ADDRSTRLEN] = {'\0'};
    ::inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string{ip};
}

void Socket::setTimeout(const int microseconds) const
{
    ::timeval tv{};
    tv.tv_sec = 0;
    tv.tv_usec = microseconds;

    ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&tv), sizeof(tv));
    ::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&tv), sizeof(tv));
}

void TCPSocket::setAddress(const ::sockaddr_in& addr)
{
    sockAddr = addr;
}

::sockaddr_in TCPSocket::getAddress() const
{
    return sockAddr;
}

int TCPSocket::toSend(const char* const bytes, const std::size_t length)
{
    return ::send(sock, bytes, length, 0);
}

int TCPSocket::toSend(const std::string& message)
{
    return toSend(message.c_str(), message.length());
}

void TCPSocket::toConnect(const std::string& ip, const std::uint16_t port, const std::function<void()> onConnected)
{
    ::addrinfo hints{}, *res = nullptr, *it = nullptr;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_STREAM;

    int status = 0;
    if ((status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &res)) != 0)
    {
        throw std::runtime_error(
            "Invalid address, status: " + std::string(::gai_strerror(status)) + ", errno: " + std::to_string(errno)
            + '.');
    }

    for (it = res; nullptr != it; it = it->ai_next)
    {
        if (AF_INET == it->ai_family)
        {
            std::memcpy(static_cast<void*>(&sockAddr), static_cast<void*>(it->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(res);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    sockAddr.sin_addr.s_addr = static_cast<std::uint32_t>(sockAddr.sin_addr.s_addr);

    setBlocking();
    if (::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in)) == -1)
    {
        throw std::runtime_error("Could not connect to the socket, errno: " + std::to_string(errno) + '.');
    }

    onConnected();
    toReceive();
}

void TCPSocket::toReceive(const bool detach)
{
    if (!detach)
    {
        fut = std::async(std::launch::async, toRecv, this);
    }
    else
    {
        std::thread t(toRecv, this);
        t.detach();
    }
}

void TCPSocket::toRecv(const TCPSocket* const socket)
{
    char tempBuffer[socket->bufferSize];
    tempBuffer[0] = '\0';
    int messageLength = 0;
    while ((messageLength = ::recv(socket->sock, tempBuffer, socket->bufferSize, 0)) > 0)
    {
        tempBuffer[messageLength] = '\0';
        if (socket->onMessageReceived)
        {
            socket->onMessageReceived(std::string(tempBuffer, messageLength));
        }

        if (socket->onRawMessageReceived)
        {
            socket->onRawMessageReceived(tempBuffer, messageLength);
        }
    }

    socket->toClose();
    if (socket->onSocketClosed)
    {
        socket->onSocketClosed(errno);
    }
    if (socket->autoRelease.load())
    {
        delete socket;
    }
}

TCPServer::TCPServer() : Socket(tcp)
{
    int opt1 = 1, opt2 = 0;
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt1, sizeof(int));
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(int));
}

void TCPServer::toBind(const std::string& ip, const std::uint16_t port)
{
    if (::inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr) == -1)
    {
        throw std::runtime_error(
            "Invalid address, address type is not supported, errno: " + std::to_string(errno) + '.');
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);

    setBlocking();
    if (::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
    {
        throw std::runtime_error("Could not bind the socket, errno: " + std::to_string(errno) + '.');
    }
}

void TCPServer::toBind(const std::uint16_t port)
{
    toBind("0.0.0.0", port);
}

void TCPServer::toListen()
{
    if (::listen(sock, retryTimes) == -1)
    {
        throw std::runtime_error("Server could not listen on the socket, errno: " + std::to_string(errno) + '.');
    }
}

void TCPServer::toAccept(const bool detach)
{
    if (!detach)
    {
        fut = std::async(
            std::launch::async,
            [=, this]
            {
                toAccept(this);
            });
    }
    else
    {
        std::thread t(
            [=, this]
            {
                toAccept(this);
            });
        t.detach();
    }
}

void TCPServer::toAccept(const TCPServer* const server)
{
    ::sockaddr_in newSocketInfo{};
    ::socklen_t newSocketInfoLength = sizeof(newSocketInfo);

    int newSock = 0;
    for (;;)
    {
        if ((newSock = ::accept(server->sock, reinterpret_cast<::sockaddr*>(&newSocketInfo), &newSocketInfoLength))
            == -1)
        {
            if ((EBADF == errno) || (EINVAL == errno))
            {
                return;
            }

            throw std::runtime_error("Error while accepting a new connection, errno: " + std::to_string(errno) + '.');
        }

        TCPSocket* const newSocket = new TCPSocket(newSock);
        newSocket->autoRelease.store(true);
        newSocket->setAddress(newSocketInfo);

        server->onNewConnection(newSocket);
        newSocket->toReceive(true);
    }
}

int UDPSocket::toSendTo(
    const char* const bytes,
    const std::size_t length,
    const std::string& ip,
    const std::uint16_t port)
{
    ::sockaddr_in addr{};
    ::addrinfo hints{}, *res = nullptr, *it = nullptr;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    int status = 0;
    if ((status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &res)) != 0)
    {
        throw std::runtime_error(
            "Invalid address, status: " + std::string(::gai_strerror(status)) + ", errno: " + std::to_string(errno)
            + '.');
    }

    for (it = res; nullptr != it; it = it->ai_next)
    {
        if (AF_INET == it->ai_family)
        {
            std::memcpy(static_cast<void*>(&addr), static_cast<void*>(it->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(res);

    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;

    int sent = 0;
    if ((sent = ::sendto(sock, bytes, length, 0, reinterpret_cast<::sockaddr*>(&addr), sizeof(addr))) == -1)
    {
        throw std::runtime_error("Unable to send message to address, errno: " + std::to_string(errno) + '.');
    }

    return sent;
}

int UDPSocket::toSendTo(const std::string& message, const std::string& ip, const std::uint16_t port)
{
    return toSendTo(message.c_str(), message.length(), ip, port);
}

int UDPSocket::toSend(const char* const bytes, const std::size_t length)
{
    return ::send(sock, bytes, length, 0);
}

int UDPSocket::toSend(const std::string& message)
{
    return toSend(message.c_str(), message.length());
}

void UDPSocket::toConnect(const std::string& ip, const std::uint16_t port)
{
    ::addrinfo hints{}, *res = nullptr, *it = nullptr;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    int status = 0;
    if ((status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &res)) != 0)
    {
        throw std::runtime_error(
            "Invalid address, status: " + std::string(::gai_strerror(status)) + ", errno: " + std::to_string(errno)
            + '.');
    }

    for (it = res; nullptr != it; it = it->ai_next)
    {
        if (AF_INET == it->ai_family)
        {
            std::memcpy(static_cast<void*>(&sockAddr), static_cast<void*>(it->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(res);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    sockAddr.sin_addr.s_addr = static_cast<std::uint32_t>(sockAddr.sin_addr.s_addr);

    setBlocking();
    if (::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in)) == -1)
    {
        throw std::runtime_error("Could not connect to the socket, errno: " + std::to_string(errno) + '.');
    }
}

void UDPSocket::toReceive(const bool detach)
{
    if (!detach)
    {
        fut = std::async(std::launch::async, toRecv, this);
    }
    else
    {
        std::thread t(toRecv, this);
        t.detach();
    }
}

void UDPSocket::toReceiveFrom(const bool detach)
{
    if (!detach)
    {
        fut = std::async(std::launch::async, toRecvFrom, this);
    }
    else
    {
        std::thread t(toRecvFrom, this);
        t.detach();
    }
}

void UDPSocket::toRecv(const UDPSocket* const socket)
{
    char tempBuffer[socket->bufferSize];
    tempBuffer[0] = '\0';
    int messageLength = 0;
    while ((messageLength = ::recv(socket->sock, tempBuffer, socket->bufferSize, 0)) != -1)
    {
        tempBuffer[messageLength] = '\0';
        if (socket->onMessageReceived)
        {
            socket->onMessageReceived(
                std::string(tempBuffer, messageLength), ipString(socket->sockAddr), ::ntohs(socket->sockAddr.sin_port));
        }

        if (socket->onRawMessageReceived)
        {
            socket->onRawMessageReceived(
                tempBuffer, messageLength, ipString(socket->sockAddr), ::ntohs(socket->sockAddr.sin_port));
        }
    }
}

void UDPSocket::toRecvFrom(const UDPSocket* const socket)
{
    ::sockaddr_in addr{};
    ::socklen_t hostAddrSize = sizeof(addr);

    char tempBuffer[socket->bufferSize];
    tempBuffer[0] = '\0';
    int messageLength = 0;
    while ((messageLength = ::recvfrom(
                socket->sock, tempBuffer, socket->bufferSize, 0, reinterpret_cast<::sockaddr*>(&addr), &hostAddrSize))
           != -1)
    {
        tempBuffer[messageLength] = '\0';
        if (socket->onMessageReceived)
        {
            socket->onMessageReceived(std::string(tempBuffer, messageLength), ipString(addr), ::ntohs(addr.sin_port));
        }

        if (socket->onRawMessageReceived)
        {
            socket->onRawMessageReceived(tempBuffer, messageLength, ipString(addr), ::ntohs(addr.sin_port));
        }
    }
}

void UDPServer::toBind(const std::string& ip, const std::uint16_t port)
{
    if (::inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr) == -1)
    {
        throw std::runtime_error(
            "Invalid address, address type is not supported, errno: " + std::to_string(errno) + '.');
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);

    setBlocking();
    if (::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
    {
        throw std::runtime_error("Could not bind the socket, errno: " + std::to_string(errno) + '.');
    }
}

void UDPServer::toBind(const std::uint16_t port)
{
    toBind("0.0.0.0", port);
}

void UDPServer::setBroadcast()
{
    int broadcast = 1;
    ::setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
}
} // namespace utility::socket
