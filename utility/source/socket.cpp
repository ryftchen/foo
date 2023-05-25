//! @file socket.cpp
//! @author ryftchen
//! @brief The definitions (socket) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "socket.hpp"
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

namespace utility::socket
{
Socket::Socket(const Type sockType, const int socketId)
{
    if (-1 == socketId)
    {
        if ((sock = ::socket(AF_INET, sockType, 0)) == -1)
        {
            std::cerr << "<SOCKET> Socket creation error, errno: " << errno << '.' << std::endl;
        }
    }
    else
    {
        sock = socketId;
    }
}

void Socket::toClose() const
{
    shutdown(sock, SHUT_RDWR);
    close(sock);
}

void Socket::waitIfAlive()
{
    if (!thrFut.valid())
    {
        throw std::logic_error("<SOCKET> Never use asynchronous.");
    }

    if (thrFut.wait_until(std::chrono::system_clock::now()) != std::future_status::ready)
    {
        thrFut.wait();
    }
}

std::string Socket::getRemoteAddress() const
{
    return ipToString(address);
}

int Socket::getRemotePort() const
{
    return ntohs(address.sin_port);
}

int Socket::getFileDescriptor() const
{
    return sock;
}

std::string Socket::ipToString(const sockaddr_in& addr)
{
    char ip[INET_ADDRSTRLEN];
    ip[0] = '\0';
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string{ip};
}

void Socket::setTimeout(const int microseconds) const
{
    struct timeval tv
    {
    };
    tv.tv_sec = 0;
    tv.tv_usec = microseconds;

    setsockopt(
        sock,
        SOL_SOCKET,
        SO_RCVTIMEO,
        reinterpret_cast<char*>(&tv), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        sizeof(tv));
    setsockopt(
        sock,
        SOL_SOCKET,
        SO_SNDTIMEO,
        reinterpret_cast<char*>(&tv), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        sizeof(tv));
}

void TCPSocket::setAddress(const sockaddr_in& addr)
{
    address = addr;
}

sockaddr_in TCPSocket::getAddress() const
{
    return address;
}

int TCPSocket::toSend(const char* bytes, const std::size_t length)
{
    return send(sock, bytes, length, 0);
}

int TCPSocket::toSend(const std::string& message)
{
    return toSend(message.c_str(), message.length());
}

void TCPSocket::toConnect(const std::string& host, const uint16_t port, const std::function<void()> onConnected)
{
    struct addrinfo hints
    {
    }, *res, *it;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = 0;
    if ((status = getaddrinfo(host.c_str(), nullptr, &hints, &res)) != 0)
    {
        throw std::runtime_error(
            "<SOCKET> Invalid address, status: " + std::string(gai_strerror(status))
            + ", errno: " + std::to_string(errno) + '.');
    }

    for (it = res; nullptr != it; it = it->ai_next)
    {
        if (AF_INET == it->ai_family)
        {
            std::memcpy(static_cast<void*>(&address), static_cast<void*>(it->ai_addr), sizeof(sockaddr_in));
            break;
        }
    }

    freeaddrinfo(res);

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = static_cast<std::uint32_t>(address.sin_addr.s_addr);

    setBlocking();
    if (connect(
            sock,
            reinterpret_cast<const sockaddr*>(&address), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            sizeof(sockaddr_in))
        == -1)
    {
        throw std::runtime_error("<SOCKET> Failed to connect to the host, errno: " + std::to_string(errno) + '.');
    }

    onConnected();
    toReceive();
}

void TCPSocket::toReceive(const bool detach)
{
    if (!detach)
    {
        thrFut = std::async(std::launch::async, toRecv, this);
    }
    else
    {
        std::thread t(toRecv, this);
        t.detach();
    }
}

void TCPSocket::toRecv(TCPSocket* socket)
{
    char tempBuffer[socket->bufferSize];
    tempBuffer[0] = '\0';
    int messageLength = 0;
    while ((messageLength = recv(socket->sock, tempBuffer, socket->bufferSize, 0)) > 0)
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
    if (socket->activeRelease.load())
    {
        delete socket;
    }
}

TCPServer::TCPServer() : Socket(tcp)
{
    int opt1 = 1, opt2 = 0;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt1, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(int));
}

void TCPServer::toBind(const std::string& host, const uint16_t port)
{
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) == -1)
    {
        throw std::runtime_error(
            "<SOCKET> Invalid address, address type not supported, errno: " + std::to_string(errno) + '.');
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    setBlocking();
    if (bind(
            sock,
            reinterpret_cast<const sockaddr*>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                &address),
            sizeof(address))
        == -1)
    {
        throw std::runtime_error("<SOCKET> Cannot bind the socket, errno: " + std::to_string(errno) + '.');
    }
}

void TCPServer::toBind(const uint16_t port)
{
    toBind("0.0.0.0", port);
}

void TCPServer::toListen()
{
    if (listen(sock, retryTimes) == -1)
    {
        throw std::runtime_error("<SOCKET> Server can't listen on the socket, errno: " + std::to_string(errno) + '.');
    }
}

void TCPServer::toAccept(const bool detach)
{
    if (!detach)
    {
        thrFut = std::async(
            std::launch::async,
            [=]
            {
                toAccept(this);
            });
    }
    else
    {
        std::thread t(
            [=]
            {
                toAccept(this);
            });
        t.detach();
    }
}

void TCPServer::toAccept(TCPServer* server)
{
    sockaddr_in newSocketInfo{};
    socklen_t newSocketInfoLength = sizeof(newSocketInfo);

    int newSock = 0;
    while (true)
    {
        if ((newSock = accept(
                 server->sock,
                 reinterpret_cast<sockaddr*>(&newSocketInfo), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                 &newSocketInfoLength))
            == -1)
        {
            if ((EBADF == errno) || (EINVAL == errno))
            {
                return;
            }

            throw std::runtime_error(
                "<SOCKET> Error while accepting a new connection, errno: " + std::to_string(errno) + '.');
        }

        TCPSocket* newSocket = new TCPSocket(newSock);
        newSocket->activeRelease.store(true);
        newSocket->setAddress(newSocketInfo);

        server->onNewConnection(newSocket);
        newSocket->toReceive(true);
    }
}

int UDPSocket::toSendTo(const char* bytes, const std::size_t length, const std::string& host, const uint16_t port)
{
    sockaddr_in hostAddr{};

    struct addrinfo hints
    {
    }, *res, *it;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = 0;
    if ((status = getaddrinfo(host.c_str(), nullptr, &hints, &res)) != 0)
    {
        throw std::runtime_error(
            "<SOCKET> Invalid address, status: " + std::string(gai_strerror(status))
            + ", errno: " + std::to_string(errno) + '.');
    }

    for (it = res; nullptr != it; it = it->ai_next)
    {
        if (AF_INET == it->ai_family)
        {
            std::memcpy(static_cast<void*>(&hostAddr), static_cast<void*>(it->ai_addr), sizeof(sockaddr_in));
            break;
        }
    }

    freeaddrinfo(res);

    hostAddr.sin_port = htons(port);
    hostAddr.sin_family = AF_INET;

    int sent = 0;
    if ((sent = sendto(
             sock,
             bytes,
             length,
             0,
             reinterpret_cast<sockaddr*>(&hostAddr), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
             sizeof(hostAddr)))
        == -1)
    {
        throw std::runtime_error("<SOCKET> Unable to send message to address, errno: " + std::to_string(errno) + '.');
    }

    return sent;
}

int UDPSocket::toSendTo(const std::string& message, const std::string& host, const uint16_t port)
{
    return toSendTo(message.c_str(), message.length(), host, port);
}

int UDPSocket::toSend(const char* bytes, const std::size_t length)
{
    return send(sock, bytes, length, 0);
}

int UDPSocket::toSend(const std::string& message)
{
    return toSend(message.c_str(), message.length());
}

void UDPSocket::toConnect(const std::string& host, const uint16_t port)
{
    struct addrinfo hints
    {
    }, *res, *it;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = 0;
    if ((status = getaddrinfo(host.c_str(), nullptr, &hints, &res)) != 0)
    {
        throw std::runtime_error(
            "<SOCKET> Invalid address, status: " + std::string(gai_strerror(status))
            + ", errno: " + std::to_string(errno) + '.');
    }

    for (it = res; nullptr != it; it = it->ai_next)
    {
        if (AF_INET == it->ai_family)
        {
            std::memcpy(static_cast<void*>(&address), static_cast<void*>(it->ai_addr), sizeof(sockaddr_in));
            break;
        }
    }

    freeaddrinfo(res);

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = static_cast<std::uint32_t>(address.sin_addr.s_addr);

    setBlocking();
    if (connect(
            sock,
            reinterpret_cast<const sockaddr*>(&address), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            sizeof(sockaddr_in))
        == -1)
    {
        throw std::runtime_error("<SOCKET> Failed to connect to the host, errno: " + std::to_string(errno) + '.');
    }
}

void UDPSocket::toReceive(const bool detach)
{
    if (!detach)
    {
        thrFut = std::async(std::launch::async, toRecv, this);
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
        thrFut = std::async(std::launch::async, toRecvFrom, this);
    }
    else
    {
        std::thread t(toRecvFrom, this);
        t.detach();
    }
}

void UDPSocket::toRecv(UDPSocket* socket)
{
    char tempBuffer[socket->bufferSize];
    tempBuffer[0] = '\0';
    int messageLength = 0;
    while ((messageLength = recv(socket->sock, tempBuffer, socket->bufferSize, 0)) != -1)
    {
        tempBuffer[messageLength] = '\0';
        if (socket->onMessageReceived)
        {
            socket->onMessageReceived(
                std::string(tempBuffer, messageLength), ipToString(socket->address), ntohs(socket->address.sin_port));
        }

        if (socket->onRawMessageReceived)
        {
            socket->onRawMessageReceived(
                tempBuffer, messageLength, ipToString(socket->address), ntohs(socket->address.sin_port));
        }
    }
}

void UDPSocket::toRecvFrom(UDPSocket* socket)
{
    sockaddr_in hostAddr{};
    socklen_t hostAddrSize = sizeof(hostAddr);

    char tempBuffer[socket->bufferSize];
    tempBuffer[0] = '\0';
    int messageLength = 0;
    while ((messageLength = recvfrom(
                socket->sock,
                tempBuffer,
                socket->bufferSize,
                0,
                reinterpret_cast<sockaddr*>(&hostAddr), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
                &hostAddrSize))
           != -1)
    {
        tempBuffer[messageLength] = '\0';
        if (socket->onMessageReceived)
        {
            socket->onMessageReceived(
                std::string(tempBuffer, messageLength), ipToString(hostAddr), ntohs(hostAddr.sin_port));
        }

        if (socket->onRawMessageReceived)
        {
            socket->onRawMessageReceived(tempBuffer, messageLength, ipToString(hostAddr), ntohs(hostAddr.sin_port));
        }
    }
}

void UDPServer::toBind(const std::string& host, const uint16_t port)
{
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) == -1)
    {
        throw std::runtime_error(
            "<SOCKET> Invalid address, address type not supported, errno: " + std::to_string(errno) + '.');
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    setBlocking();
    if (bind(
            sock,
            reinterpret_cast<const sockaddr*>(&address), // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            sizeof(address))
        == -1)
    {
        throw std::runtime_error("<SOCKET> Cannot bind the socket, errno: " + std::to_string(errno) + '.');
    }
}

void UDPServer::toBind(const uint16_t port)
{
    toBind("0.0.0.0", port);
}

void UDPServer::setBroadcast()
{
    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) == -1)
    {
        throw std::runtime_error(
            "<SOCKET> Failed to set socket option SO_BROADCAST, errno: " + std::to_string(errno) + '.');
        return;
    }
}
} // namespace utility::socket
