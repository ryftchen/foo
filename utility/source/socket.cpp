//! @file socket.cpp
//! @author ryftchen
//! @brief The definitions (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "socket.hpp"

#include <sys/poll.h>
#include <netdb.h>
#include <cstring>
#include <vector>

namespace utility::socket
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

Socket::Socket(const Type socketType, const int socketId)
{
    ::pthread_spin_init(&sockLock, PTHREAD_PROCESS_PRIVATE);
    if (-1 == socketId)
    {
        spinLock();
        sock = ::socket(AF_INET, socketType, 0);
        spinUnlock();
        if (-1 == sock)
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
    ::pthread_spin_destroy(&sockLock);
}

std::string Socket::transportAddress() const
{
    return ipString(sockAddr);
}

int Socket::transportPort() const
{
    return ::ntohs(sockAddr.sin_port);
}

void Socket::toClose()
{
    asyncExit();

    spinLock();
    ::shutdown(sock, ::SHUT_RDWR);
    ::close(sock);
    spinUnlock();
}

void Socket::asyncExit()
{
    exitReady.store(true);
}

bool Socket::shouldExit() const
{
    return exitReady.load();
}

void Socket::waitIfAlive()
{
    if (!task.valid())
    {
        throw std::logic_error("Never use asynchronous.");
    }

    if (task.wait_until(std::chrono::system_clock::now()) != std::future_status::ready)
    {
        task.wait();
    }
}

void Socket::spinLock()
{
    ::pthread_spin_lock(&sockLock);
}

void Socket::spinUnlock()
{
    ::pthread_spin_unlock(&sockLock);
}

std::string Socket::ipString(const ::sockaddr_in& addr)
{
    char ip[INET_ADDRSTRLEN] = {'\0'};
    ::inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string{ip};
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

    if (const int status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &res); 0 != status)
    {
        throw std::runtime_error(
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + std::to_string(errno)
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

    spinLock();
    const int status = ::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in));
    spinUnlock();
    if (-1 == status)
    {
        throw std::runtime_error("Failed to connect to the socket, errno: " + std::to_string(errno) + '.');
    }

    onConnected();
    toReceive();
}

void TCPSocket::toReceive(const bool detach)
{
    auto self = shared_from_this();
    if (!detach)
    {
        task = std::async(std::launch::async, toRecv, self);
    }
    else
    {
        std::thread thd(toRecv, self);
        thd.detach();
    }
}

void TCPSocket::toRecv(const std::shared_ptr<TCPSocket> socket)
{
    char tempBuffer[bufferSize];
    tempBuffer[0] = '\0';
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    constexpr int timeout = 10;
    for (;;)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (status < 0)
        {
            throw std::runtime_error("Not the expected wait result for poll.");
        }
        else if (0 == status)
        {
            continue;
        }

        if (pollFDs.at(0).revents & POLLIN)
        {
            socket->spinLock();
            const int messageLength = ::recv(socket->sock, tempBuffer, bufferSize, 0);
            socket->spinUnlock();
            if (messageLength <= 0)
            {
                break;
            }

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
        if (socket->shouldExit())
        {
            break;
        }
    }

    socket->toClose();
    if (socket->onSocketClosed)
    {
        socket->onSocketClosed(errno);
    }
}

TCPServer::TCPServer() : Socket(Type::tcp)
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

    if (::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
    {
        throw std::runtime_error("Failed to bind the socket, errno: " + std::to_string(errno) + '.');
    }
}

void TCPServer::toBind(const std::uint16_t port)
{
    toBind("0.0.0.0", port);
}

void TCPServer::toListen()
{
    constexpr int retryTimes = 10;
    if (::listen(sock, retryTimes) == -1)
    {
        throw std::runtime_error("Server could not listen on the socket, errno: " + std::to_string(errno) + '.');
    }
}

void TCPServer::toAccept(const bool detach)
{
    auto weakSelf = std::weak_ptr<TCPServer>(shared_from_this());
    if (!detach)
    {
        task = std::async(
            std::launch::async,
            [weakSelf]
            {
                if (auto sharedSelf = weakSelf.lock())
                {
                    sharedSelf->toAccept(sharedSelf);
                }
            });
    }
    else
    {
        std::thread thd(
            [weakSelf]
            {
                if (auto sharedSelf = weakSelf.lock())
                {
                    sharedSelf->toAccept(sharedSelf);
                }
            });
        thd.detach();
    }
}

void TCPServer::toAccept(const std::shared_ptr<TCPServer> server)
{
    ::sockaddr_in newSockAddr{};
    ::socklen_t newSockAddrLength = sizeof(newSockAddr);
    std::vector<std::shared_ptr<TCPSocket>> activeSockets;

    int newSock = 0;
    for (;;)
    {
        newSock = ::accept(server->sock, reinterpret_cast<::sockaddr*>(&newSockAddr), &newSockAddrLength);
        if (-1 == newSock)
        {
            for (const auto& socket : activeSockets)
            {
                socket->asyncExit();
            }
            if ((EBADF == errno) || (EINVAL == errno))
            {
                return;
            }

            throw std::runtime_error("Error while accepting a new connection, errno: " + std::to_string(errno) + '.');
        }

        auto newSocket = std::make_shared<TCPSocket>(newSock);
        newSocket->sockAddr = newSockAddr;

        server->onNewConnection(newSocket);
        newSocket->toReceive(true);
        activeSockets.emplace_back(newSocket);
    }
}

int UDPSocket::toSendTo(
    const char* const bytes, const std::size_t length, const std::string& ip, const std::uint16_t port)
{
    ::sockaddr_in addr{};
    ::addrinfo hints{}, *res = nullptr, *it = nullptr;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    if (const int status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &res); 0 != status)
    {
        throw std::runtime_error(
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + std::to_string(errno)
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

    const int sent = ::sendto(sock, bytes, length, 0, reinterpret_cast<::sockaddr*>(&addr), sizeof(addr));
    if (-1 == sent)
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

    if (const int status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &res); 0 != status)
    {
        throw std::runtime_error(
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + std::to_string(errno)
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

    spinLock();
    const int status = ::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in));
    spinUnlock();
    if (-1 == status)
    {
        throw std::runtime_error("Failed to connect to the socket, errno: " + std::to_string(errno) + '.');
    }
}

void UDPSocket::toReceive(const bool detach)
{
    auto self = shared_from_this();
    if (!detach)
    {
        task = std::async(std::launch::async, toRecv, self);
    }
    else
    {
        std::thread thd(toRecv, self);
        thd.detach();
    }
}

void UDPSocket::toReceiveFrom(const bool detach)
{
    auto self = shared_from_this();
    if (!detach)
    {
        task = std::async(std::launch::async, toRecvFrom, self);
    }
    else
    {
        std::thread thd(toRecvFrom, self);
        thd.detach();
    }
}

void UDPSocket::toRecv(const std::shared_ptr<UDPSocket> socket)
{
    char tempBuffer[bufferSize];
    tempBuffer[0] = '\0';
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    constexpr int timeout = 10;
    for (;;)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (status < 0)
        {
            throw std::runtime_error("Not the expected wait result for poll.");
        }
        else if (0 == status)
        {
            continue;
        }

        if (pollFDs.at(0).revents & POLLIN)
        {
            socket->spinLock();
            const int messageLength = ::recv(socket->sock, tempBuffer, bufferSize, 0);
            socket->spinUnlock();
            if (-1 == messageLength)
            {
                break;
            }

            tempBuffer[messageLength] = '\0';
            if (socket->onMessageReceived)
            {
                socket->onMessageReceived(
                    std::string(tempBuffer, messageLength), socket->transportAddress(), socket->transportPort());
            }
            if (socket->onRawMessageReceived)
            {
                socket->onRawMessageReceived(
                    tempBuffer, messageLength, socket->transportAddress(), socket->transportPort());
            }
        }
        if (socket->shouldExit())
        {
            break;
        }
    }
}

void UDPSocket::toRecvFrom(const std::shared_ptr<UDPSocket> socket)
{
    ::sockaddr_in addr{};
    ::socklen_t hostAddrSize = sizeof(addr);

    char tempBuffer[bufferSize];
    tempBuffer[0] = '\0';
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    constexpr int timeout = 10;
    for (;;)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (status < 0)
        {
            throw std::runtime_error("Not the expected wait result for poll.");
        }
        else if (0 == status)
        {
            continue;
        }

        if (pollFDs.at(0).revents & POLLIN)
        {
            socket->spinLock();
            const int messageLength = ::recvfrom(
                socket->sock, tempBuffer, bufferSize, 0, reinterpret_cast<::sockaddr*>(&addr), &hostAddrSize);
            socket->spinUnlock();
            if (-1 == messageLength)
            {
                break;
            }

            tempBuffer[messageLength] = '\0';
            if (socket->onMessageReceived)
            {
                socket->onMessageReceived(
                    std::string(tempBuffer, messageLength), ipString(addr), ::ntohs(addr.sin_port));
            }
            if (socket->onRawMessageReceived)
            {
                socket->onRawMessageReceived(tempBuffer, messageLength, ipString(addr), ::ntohs(addr.sin_port));
            }
        }
        if (socket->shouldExit())
        {
            break;
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

    if (::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
    {
        throw std::runtime_error("Failed to bind the socket, errno: " + std::to_string(errno) + '.');
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
