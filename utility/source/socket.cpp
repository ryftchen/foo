//! @file socket.cpp
//! @author ryftchen
//! @brief The definitions (socket) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "socket.hpp"

#include <sys/poll.h>
#include <netdb.h>
#include <algorithm>
#include <array>
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
    std::array<char, INET_ADDRSTRLEN> ip{};
    ::inet_ntop(AF_INET, &addr.sin_addr, ip.data(), ip.size());

    return std::string{ip.data()};
}

//! @brief Get the errno string safely.
//! @return errno string
static std::string errnoString()
{
    std::array<char, 64> buffer{};
#ifdef _GNU_SOURCE
    return ::strerror_r(errno, buffer.data(), buffer.size());
#else
    return (::strerror_r(errno, buffer.data(), buffer.size()) == 0) ? std::string{buffer.data()} : "Unknown error";
#endif // _GNU_SOURCE
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
Socket::Socket(const Type socketType, const int socketId)
{
    ::pthread_spin_init(&sockLock, ::PTHREAD_PROCESS_PRIVATE);
    if (socketId != -1)
    {
        sock = socketId;
        return;
    }

    sock = ::socket(AF_INET, static_cast<std::uint8_t>(socketType), 0);
    if (sock == -1)
    {
        throw std::runtime_error{"Socket creation error, errno: " + errnoString() + '.'};
    }
}

Socket::~Socket()
{
    toClose();
    ::pthread_spin_destroy(&sockLock);
}

void Socket::toClose()
{
    requestStop();

    const Guard lock(*this);
    ::shutdown(sock, ::SHUT_RDWR);
    ::close(sock);
}

void Socket::toJoin()
{
    if (asyncTask.valid() && (asyncTask.wait_until(std::chrono::system_clock::now()) != std::future_status::ready))
    {
        asyncTask.wait();
    }

    while (!stopRequested())
    {
        std::this_thread::yield();
    }
}

void Socket::requestStop()
{
    exitReady.store(true);
}

bool Socket::stopRequested() const
{
    return exitReady.load();
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
    const Guard lock(*this);
    return ::send(sock, bytes, length, 0);
}

int TCPSocket::toSend(const std::string_view message)
{
    return toSend(message.data(), message.length());
}

void TCPSocket::toConnect(const std::string& ip, const std::uint16_t port)
{
    ::addrinfo hints{}, *addrInfo = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_STREAM;

    if (const int status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &addrInfo); status != 0)
    {
        throw std::runtime_error{
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + errnoString() + '.'};
    }

    for (const ::addrinfo* entry = addrInfo; entry != nullptr; entry = entry->ai_next)
    {
        if (entry->ai_family == AF_INET)
        {
            std::memcpy(static_cast<void*>(&sockAddr), static_cast<void*>(entry->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(addrInfo);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    sockAddr.sin_addr.s_addr = static_cast<std::uint32_t>(sockAddr.sin_addr.s_addr);
    if (::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in)) == -1)
    {
        throw std::runtime_error{"Failed to connect to the socket, errno: " + errnoString() + '.'};
    }

    toReceive();
}

void TCPSocket::toReceive(const bool detach)
{
    if (auto self = shared_from_this(); !detach)
    {
        launchAsyncTask(toRecv, self);
    }
    else
    {
        std::thread(toRecv, self).detach();
    }
}

void TCPSocket::subscribeMessage(MessageCallback callback)
{
    msgCb.store(std::make_shared<decltype(callback)>(std::move(callback)), std::memory_order_release);
}

void TCPSocket::subscribeRawMessage(RawMessageCallback callback)
{
    rawMsgCb.store(std::make_shared<decltype(callback)>(std::move(callback)), std::memory_order_release);
}

void TCPSocket::toRecv(const std::shared_ptr<TCPSocket> socket) // NOLINT(performance-unnecessary-value-param)
{
    std::array<char, bufferSize> tempBuffer{};
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    for (constexpr int timeout = 10; !socket->stopRequested();)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (status == -1)
        {
            throw std::runtime_error{"Not the expected wait result for poll, errno: " + errnoString() + '.'};
        }
        if (status == 0)
        {
            continue;
        }

        if (int msgLen = 0; pollFDs.at(0).revents & POLLIN)
        {
            if (const Guard lock(*socket); true)
            {
                msgLen = ::recv(socket->sock, tempBuffer.data(), tempBuffer.size(), 0);
                if (msgLen <= 0)
                {
                    break;
                }
            }

            tempBuffer[msgLen] = '\0';
            socket->onMessage(std::string(tempBuffer.data(), msgLen));
            socket->onRawMessage(tempBuffer.data(), msgLen);
        }
    }

    socket->toClose();
}

void TCPSocket::onMessage(const std::string_view message) const
{
    const auto& callback = msgCb.load(std::memory_order_acquire);
    if (callback && *callback)
    {
        (*callback)(message);
    }
}

void TCPSocket::onRawMessage(char* buffer, const int length) const
{
    const auto& callback = rawMsgCb.load(std::memory_order_acquire);
    if (callback && *callback)
    {
        (*callback)(buffer, length);
    }
}

TCPServer::TCPServer() : Socket(Type::tcp)
{
    const Guard lock(*this);
    int opt1 = 1, opt2 = 0;
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt1, sizeof(opt1));
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(opt2));
}

void TCPServer::toBind(const std::string& ip, const std::uint16_t port)
{
    if (::inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr) == -1)
    {
        throw std::runtime_error{"Invalid address, address type is not supported, errno: " + errnoString() + '.'};
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    if (const Guard lock(*this); ::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
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
    constexpr int retryTimes = 10;
    if (const Guard lock(*this); ::listen(sock, retryTimes) == -1)
    {
        throw std::runtime_error{"Server could not listen on the socket, errno: " + errnoString() + '.'};
    }
}

void TCPServer::toAccept(const bool detach)
{
    if (auto weakSelf = std::weak_ptr<TCPServer>(shared_from_this()); !detach)
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

void TCPServer::subscribeConnection(ConnectionCallback callback)
{
    connCb.store(std::make_shared<decltype(callback)>(std::move(callback)), std::memory_order_release);
}

void TCPServer::toAccept(const std::shared_ptr<TCPServer> server) // NOLINT(performance-unnecessary-value-param)
{
    ::sockaddr_in newSockAddr{};
    ::socklen_t newSockAddrLen = sizeof(newSockAddr);

    for (std::vector<std::shared_ptr<TCPSocket>> activeSockets{};;)
    {
        const int newSock = ::accept(server->sock, reinterpret_cast<::sockaddr*>(&newSockAddr), &newSockAddrLen);
        if (newSock == -1)
        {
            std::for_each(
                activeSockets.cbegin(), activeSockets.cend(), [](const auto& socket) { socket->requestStop(); });
            if ((errno == EBADF) || (errno == EINVAL))
            {
                return;
            }
            throw std::runtime_error{"Error while accepting a new connection, errno: " + errnoString() + '.'};
        }

        auto newSocket = std::make_shared<TCPSocket>(newSock);
        newSocket->sockAddr = newSockAddr;
        server->onConnection(newSocket);

        newSocket->toReceive(true);
        activeSockets.emplace_back(std::move(newSocket));
    }
}

void TCPServer::onConnection(
    const std::shared_ptr<TCPSocket> client) const // NOLINT(performance-unnecessary-value-param)
{
    const auto& callback = connCb.load(std::memory_order_acquire);
    if (callback && *callback)
    {
        (*callback)(client);
    }
}

int UDPSocket::toSendTo(
    const char* const bytes, const std::size_t length, const std::string& ip, const std::uint16_t port)
{
    ::sockaddr_in addr{};
    ::addrinfo hints{}, *addrInfo = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    if (const int status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &addrInfo); status != 0)
    {
        throw std::runtime_error{
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + errnoString() + '.'};
    }

    for (const ::addrinfo* entry = addrInfo; entry != nullptr; entry = entry->ai_next)
    {
        if (entry->ai_family == AF_INET)
        {
            std::memcpy(static_cast<void*>(&addr), static_cast<void*>(entry->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(addrInfo);

    addr.sin_port = ::htons(port);
    addr.sin_family = AF_INET;
    int sent = 0;
    if (const Guard lock(*this); true)
    {
        sent = ::sendto(sock, bytes, length, 0, reinterpret_cast<const ::sockaddr*>(&addr), sizeof(addr));
        if (sent == -1)
        {
            throw std::runtime_error{"Unable to send message to address, errno: " + errnoString() + '.'};
        }
    }

    return sent;
}

int UDPSocket::toSendTo(const std::string_view message, const std::string& ip, const std::uint16_t port)
{
    return toSendTo(message.data(), message.length(), ip, port);
}

int UDPSocket::toSend(const char* const bytes, const std::size_t length)
{
    const Guard lock(*this);
    return ::send(sock, bytes, length, 0);
}

int UDPSocket::toSend(const std::string_view message)
{
    return toSend(message.data(), message.length());
}

void UDPSocket::toConnect(const std::string& ip, const std::uint16_t port)
{
    ::addrinfo hints{}, *addrInfo = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = ::SOCK_DGRAM;

    if (const int status = ::getaddrinfo(ip.c_str(), nullptr, &hints, &addrInfo); status != 0)
    {
        throw std::runtime_error{
            "Invalid address, status: " + std::string{::gai_strerror(status)} + ", errno: " + errnoString() + '.'};
    }

    for (const ::addrinfo* entry = addrInfo; entry != nullptr; entry = entry->ai_next)
    {
        if (entry->ai_family == AF_INET)
        {
            std::memcpy(static_cast<void*>(&sockAddr), static_cast<void*>(entry->ai_addr), sizeof(::sockaddr_in));
            break;
        }
    }
    ::freeaddrinfo(addrInfo);

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    sockAddr.sin_addr.s_addr = static_cast<std::uint32_t>(sockAddr.sin_addr.s_addr);
    if (::connect(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(::sockaddr_in)) == -1)
    {
        throw std::runtime_error{"Failed to connect to the socket, errno: " + errnoString() + '.'};
    }
}

void UDPSocket::toReceive(const bool detach)
{
    if (auto self = shared_from_this(); !detach)
    {
        launchAsyncTask(toRecv, self);
    }
    else
    {
        std::thread(toRecv, self).detach();
    }
}

void UDPSocket::toReceiveFrom(const bool detach)
{
    auto self = shared_from_this();
    if (!detach)
    {
        launchAsyncTask(toRecvFrom, self);
    }
    else
    {
        std::thread(toRecvFrom, self).detach();
    }
}

void UDPSocket::subscribeMessage(MessageCallback callback)
{
    msgCb.store(std::make_shared<decltype(callback)>(std::move(callback)), std::memory_order_release);
}

void UDPSocket::subscribeRawMessage(RawMessageCallback callback)
{
    rawMsgCb.store(std::make_shared<decltype(callback)>(std::move(callback)), std::memory_order_release);
}

void UDPSocket::toRecv(const std::shared_ptr<UDPSocket> socket) // NOLINT(performance-unnecessary-value-param)
{
    std::array<char, bufferSize> tempBuffer{};
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    for (constexpr int timeout = 10; !socket->stopRequested();)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (status == -1)
        {
            throw std::runtime_error{"Not the expected wait result for poll, errno: " + errnoString() + '.'};
        }
        if (status == 0)
        {
            continue;
        }

        if (int msgLen = 0; pollFDs.at(0).revents & POLLIN)
        {
            if (const Guard lock(*socket); true)
            {
                msgLen = ::recv(socket->sock, tempBuffer.data(), tempBuffer.size(), 0);
                if (msgLen == -1)
                {
                    break;
                }
            }

            tempBuffer[msgLen] = '\0';
            socket->onMessage(
                std::string_view(tempBuffer.data(), msgLen), socket->transportAddress(), socket->transportPort());
            socket->onRawMessage(tempBuffer.data(), msgLen, socket->transportAddress(), socket->transportPort());
        }
    }
}

void UDPSocket::toRecvFrom(const std::shared_ptr<UDPSocket> socket) // NOLINT(performance-unnecessary-value-param)
{
    ::sockaddr_in addr{};
    ::socklen_t hostAddrSize = sizeof(addr);

    std::array<char, bufferSize> tempBuffer{};
    std::vector<::pollfd> pollFDs(1);
    pollFDs.at(0).fd = socket->sock;
    pollFDs.at(0).events = POLLIN;
    for (constexpr int timeout = 10; !socket->stopRequested();)
    {
        const int status = ::poll(pollFDs.data(), pollFDs.size(), timeout);
        if (status == -1)
        {
            throw std::runtime_error{"Not the expected wait result for poll, errno: " + errnoString() + '.'};
        }
        if (status == 0)
        {
            continue;
        }

        if (int msgLen = 0; pollFDs.at(0).revents & POLLIN)
        {
            if (const Guard lock(*socket); true)
            {
                msgLen = ::recvfrom(
                    socket->sock,
                    tempBuffer.data(),
                    tempBuffer.size(),
                    0,
                    reinterpret_cast<::sockaddr*>(&addr),
                    &hostAddrSize);
                if (msgLen == -1)
                {
                    break;
                }
            }

            tempBuffer[msgLen] = '\0';
            socket->onMessage(std::string_view(tempBuffer.data(), msgLen), ipAddrString(addr), ::ntohs(addr.sin_port));
            socket->onRawMessage(tempBuffer.data(), msgLen, ipAddrString(addr), ::ntohs(addr.sin_port));
        }
    }
}

void UDPSocket::onMessage(const std::string_view message, const std::string& ip, const std::uint16_t port) const
{
    const auto& callback = msgCb.load(std::memory_order_acquire);
    if (callback && *callback)
    {
        (*callback)(message, ip, port);
    }
}

void UDPSocket::onRawMessage(char* buffer, const int length, const std::string& ip, const std::uint16_t port) const
{
    const auto& callback = rawMsgCb.load(std::memory_order_acquire);
    if (callback && *callback)
    {
        (*callback)(buffer, length, ip, port);
    }
}

void UDPServer::toBind(const std::string& ip, const std::uint16_t port)
{
    if (::inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr) == -1)
    {
        throw std::runtime_error{"Invalid address, address type is not supported, errno: " + errnoString() + '.'};
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = ::htons(port);
    if (const Guard lock(*this); ::bind(sock, reinterpret_cast<const ::sockaddr*>(&sockAddr), sizeof(sockAddr)) == -1)
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
