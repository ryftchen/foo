//! @file socket.cpp
//! @author ryftchen
//! @brief The definitions (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "socket.hpp"

#include <sys/poll.h>
#include <netdb.h>
#include <algorithm>
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

//! @brief Get the ip address from transport information.
//! @param addr - transport information
//! @return ip address string
static std::string ipAddrString(const ::sockaddr_in& addr)
{
    char ip[INET_ADDRSTRLEN] = {'\0'};
    ::inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string{ip};
}

//! @brief Get the errno string safely.
//! @return errno string
static std::string errnoString()
{
    char buffer[64] = {'\0'};
#ifdef _GNU_SOURCE
    return ::strerror_r(errno, buffer, sizeof(buffer));
#else
    return (::strerror_r(errno, buffer, sizeof(buffer)) == 0) ? std::string{buffer} : "Unknown error";
#endif // _GNU_SOURCE
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
Socket::Socket(const Type socketType, const int socketId)
{
    ::pthread_spin_init(&sockLock, ::PTHREAD_PROCESS_PRIVATE);
    if (-1 == socketId)
    {
        const SockGuard lock(*this);
        sock = ::socket(AF_INET, socketType, 0);
        if (-1 == sock)
        {
            throw std::runtime_error{"Socket creation error, errno: " + errnoString() + '.'};
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

void Socket::toClose()
{
    signalExit();

    const SockGuard lock(*this);
    ::shutdown(sock, ::SHUT_RDWR);
    ::close(sock);
}

void Socket::signalExit()
{
    exitReady.store(true);
}

bool Socket::exitSignaled() const
{
    return exitReady.load();
}

void Socket::waitIfAlive()
{
    if (asyncTask.valid() && (asyncTask.wait_until(std::chrono::system_clock::now()) != std::future_status::ready))
    {
        asyncTask.wait();
    }

    while (!exitSignaled())
    {
        std::this_thread::yield();
    }
}

std::string Socket::transportAddress() const
{
    return ipAddrString(sockAddr);
}

int Socket::transportPort() const
{
    return ::ntohs(sockAddr.sin_port);
}

void Socket::spinLock() const
{
    ::pthread_spin_lock(&sockLock);
}

void Socket::spinUnlock() const
{
    ::pthread_spin_unlock(&sockLock);
}

template <typename Func, typename... Args>
void Socket::launchAsyncTask(Func&& func, Args&&... args)
{
    asyncTask = std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...);
}

int TCPSocket::toSend(const char* const bytes, const std::size_t length)
{
    return ::send(sock, bytes, length, 0);
}

int TCPSocket::toSend(const std::string_view message)
{
    return toSend(message.data(), message.length());
}

void TCPSocket::toConnect(const std::string_view ip, const std::uint16_t port)
{
    ::addrinfo hints{}, *addrInfo = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_STREAM;

    if (const int status = ::getaddrinfo(ip.data(), nullptr, &hints, &addrInfo); 0 != status)
    {
        throw std::runtime_error{
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + errnoString() + '.'};
    }

    for (const ::addrinfo* entry = addrInfo; nullptr != entry; entry = entry->ai_next)
    {
        if (AF_INET == entry->ai_family)
        {
            std::memcpy(static_cast<void*>(&sockAddr), static_cast<void*>(entry->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(addrInfo);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    sockAddr.sin_addr.s_addr = static_cast<std::uint32_t>(sockAddr.sin_addr.s_addr);
    if (const SockGuard lock(*this);
        ::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in)) == -1)
    {
        throw std::runtime_error{"Failed to connect to the socket, errno: " + errnoString() + '.'};
    }

    toReceive();
}

void TCPSocket::toReceive(const bool toDetach)
{
    if (auto self = shared_from_this(); !toDetach)
    {
        launchAsyncTask(toRecv, self);
    }
    else
    {
        std::thread(toRecv, self).detach();
    }
}

void TCPSocket::toRecv(const std::shared_ptr<TCPSocket> socket) // NOLINT(performance-unnecessary-value-param)
{
    char tempBuffer[bufferSize];
    tempBuffer[0] = '\0';
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    for (constexpr int timeout = 10; !socket->exitSignaled();)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (-1 == status)
        {
            throw std::runtime_error{"Not the expected wait result for poll, errno: " + errnoString() + '.'};
        }
        if (0 == status)
        {
            continue;
        }

        if (pollFDs.at(0).revents & POLLIN)
        {
            int msgLen = 0;
            if (const SockGuard lock(*socket); true)
            {
                msgLen = ::recv(socket->sock, tempBuffer, bufferSize, 0);
                if (msgLen <= 0)
                {
                    break;
                }
            }

            tempBuffer[msgLen] = '\0';
            if (socket->onMessageReceived)
            {
                socket->onMessageReceived(std::string(tempBuffer, msgLen));
            }
            if (socket->onRawMessageReceived)
            {
                socket->onRawMessageReceived(tempBuffer, msgLen);
            }
        }
    }

    socket->toClose();
}

TCPServer::TCPServer() : Socket(Type::tcp)
{
    int opt1 = 1, opt2 = 0;
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt1, sizeof(opt1));
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(opt2));
}

void TCPServer::toBind(const std::string_view ip, const std::uint16_t port)
{
    if (::inet_pton(AF_INET, ip.data(), &sockAddr.sin_addr) == -1)
    {
        throw std::runtime_error{"Invalid address, address type is not supported, errno: " + errnoString() + '.'};
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    if (::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
    {
        throw std::runtime_error{"Failed to bind the socket, errno: " + errnoString() + '.'};
    }
}

void TCPServer::toBind(const std::uint16_t port)
{
    toBind("0.0.0.0", port);
}

void TCPServer::toListen()
{
    if (constexpr int retryTimes = 10; ::listen(sock, retryTimes) == -1)
    {
        throw std::runtime_error{"Server could not listen on the socket, errno: " + errnoString() + '.'};
    }
}

void TCPServer::toAccept(const bool toDetach)
{
    if (auto weakSelf = std::weak_ptr<TCPServer>(shared_from_this()); !toDetach)
    {
        launchAsyncTask(
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
        std::thread(
            [weakSelf]
            {
                if (auto sharedSelf = weakSelf.lock())
                {
                    sharedSelf->toAccept(sharedSelf);
                }
            })
            .detach();
    }
}

void TCPServer::toAccept(const std::shared_ptr<TCPServer> server) // NOLINT(performance-unnecessary-value-param)
{
    ::sockaddr_in newSockAddr{};
    ::socklen_t newSockAddrLen = sizeof(newSockAddr);

    for (std::vector<std::shared_ptr<TCPSocket>> activeSockets{};;)
    {
        const int newSock = ::accept(server->sock, reinterpret_cast<::sockaddr*>(&newSockAddr), &newSockAddrLen);
        if (-1 == newSock)
        {
            std::for_each(
                activeSockets.cbegin(), activeSockets.cend(), [](const auto& socket) { socket->signalExit(); });
            if ((EBADF == errno) || (EINVAL == errno))
            {
                return;
            }

            throw std::runtime_error{"Error while accepting a new connection, errno: " + errnoString() + '.'};
        }

        auto newSocket = std::make_shared<TCPSocket>(newSock);
        newSocket->sockAddr = newSockAddr;
        if (server->onNewConnection)
        {
            server->onNewConnection(newSocket);
        }

        newSocket->toReceive(true);
        activeSockets.emplace_back(std::move(newSocket));
    }
}

int UDPSocket::toSendTo(
    const char* const bytes, const std::size_t length, const std::string_view ip, const std::uint16_t port)
{
    ::sockaddr_in addr{};
    ::addrinfo hints{}, *addrInfo = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    if (const int status = ::getaddrinfo(ip.data(), nullptr, &hints, &addrInfo); 0 != status)
    {
        throw std::runtime_error{
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + errnoString() + '.'};
    }

    for (const ::addrinfo* entry = addrInfo; nullptr != entry; entry = entry->ai_next)
    {
        if (AF_INET == entry->ai_family)
        {
            std::memcpy(static_cast<void*>(&addr), static_cast<void*>(entry->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(addrInfo);

    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;
    const int sent = ::sendto(sock, bytes, length, 0, reinterpret_cast<::sockaddr*>(&addr), sizeof(addr));
    if (-1 == sent)
    {
        throw std::runtime_error{"Unable to send message to address, errno: " + errnoString() + '.'};
    }

    return sent;
}

int UDPSocket::toSendTo(const std::string_view message, const std::string_view ip, const std::uint16_t port)
{
    return toSendTo(message.data(), message.length(), ip, port);
}

int UDPSocket::toSend(const char* const bytes, const std::size_t length)
{
    return ::send(sock, bytes, length, 0);
}

int UDPSocket::toSend(const std::string_view message)
{
    return toSend(message.data(), message.length());
}

void UDPSocket::toConnect(const std::string_view ip, const std::uint16_t port)
{
    ::addrinfo hints{}, *addrInfo = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    if (const int status = ::getaddrinfo(ip.data(), nullptr, &hints, &addrInfo); 0 != status)
    {
        throw std::runtime_error{
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + errnoString() + '.'};
    }

    for (const ::addrinfo* entry = addrInfo; nullptr != entry; entry = entry->ai_next)
    {
        if (AF_INET == entry->ai_family)
        {
            std::memcpy(static_cast<void*>(&sockAddr), static_cast<void*>(entry->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(addrInfo);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    sockAddr.sin_addr.s_addr = static_cast<std::uint32_t>(sockAddr.sin_addr.s_addr);
    if (const SockGuard lock(*this);
        ::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in)) == -1)
    {
        throw std::runtime_error{"Failed to connect to the socket, errno: " + errnoString() + '.'};
    }
}

void UDPSocket::toReceive(const bool toDetach)
{
    if (auto self = shared_from_this(); !toDetach)
    {
        launchAsyncTask(toRecv, self);
    }
    else
    {
        std::thread(toRecv, self).detach();
    }
}

void UDPSocket::toReceiveFrom(const bool toDetach)
{
    auto self = shared_from_this();
    if (!toDetach)
    {
        launchAsyncTask(toRecvFrom, self);
    }
    else
    {
        std::thread(toRecvFrom, self).detach();
    }
}

void UDPSocket::toRecv(const std::shared_ptr<UDPSocket> socket) // NOLINT(performance-unnecessary-value-param)
{
    char tempBuffer[bufferSize];
    tempBuffer[0] = '\0';
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    for (constexpr int timeout = 10; !socket->exitSignaled();)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (-1 == status)
        {
            throw std::runtime_error{"Not the expected wait result for poll, errno: " + errnoString() + '.'};
        }
        if (0 == status)
        {
            continue;
        }

        if (pollFDs.at(0).revents & POLLIN)
        {
            int msgLen = 0;
            if (const SockGuard lock(*socket); true)
            {
                msgLen = ::recv(socket->sock, tempBuffer, bufferSize, 0);
                if (-1 == msgLen)
                {
                    break;
                }
            }

            tempBuffer[msgLen] = '\0';
            if (socket->onMessageReceived)
            {
                socket->onMessageReceived(
                    std::string_view(tempBuffer, msgLen), socket->transportAddress(), socket->transportPort());
            }
            if (socket->onRawMessageReceived)
            {
                socket->onRawMessageReceived(tempBuffer, msgLen, socket->transportAddress(), socket->transportPort());
            }
        }
    }
}

void UDPSocket::toRecvFrom(const std::shared_ptr<UDPSocket> socket) // NOLINT(performance-unnecessary-value-param)
{
    ::sockaddr_in addr{};
    ::socklen_t hostAddrSize = sizeof(addr);

    char tempBuffer[bufferSize];
    tempBuffer[0] = '\0';
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    for (constexpr int timeout = 10; !socket->exitSignaled();)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (-1 == status)
        {
            throw std::runtime_error{"Not the expected wait result for poll, errno: " + errnoString() + '.'};
        }
        if (0 == status)
        {
            continue;
        }

        if (pollFDs.at(0).revents & POLLIN)
        {
            int msgLen = 0;
            if (const SockGuard lock(*socket); true)
            {
                msgLen = ::recvfrom(
                    socket->sock, tempBuffer, bufferSize, 0, reinterpret_cast<::sockaddr*>(&addr), &hostAddrSize);
                if (-1 == msgLen)
                {
                    break;
                }
            }

            tempBuffer[msgLen] = '\0';
            if (socket->onMessageReceived)
            {
                socket->onMessageReceived(
                    std::string_view(tempBuffer, msgLen), ipAddrString(addr), ::ntohs(addr.sin_port));
            }
            if (socket->onRawMessageReceived)
            {
                socket->onRawMessageReceived(tempBuffer, msgLen, ipAddrString(addr), ::ntohs(addr.sin_port));
            }
        }
    }
}

void UDPServer::toBind(const std::string_view ip, const std::uint16_t port)
{
    if (::inet_pton(AF_INET, ip.data(), &sockAddr.sin_addr) == -1)
    {
        throw std::runtime_error{"Invalid address, address type is not supported, errno: " + errnoString() + '.'};
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    if (::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
    {
        throw std::runtime_error{"Failed to bind the socket, errno: " + errnoString() + '.'};
    }
}

void UDPServer::toBind(const std::uint16_t port)
{
    toBind("0.0.0.0", port);
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
} // namespace utility::socket
