//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "common.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <chrono>
#include <cstdarg>
#include <iostream>
#include <vector>

namespace utility::common
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief The Brian-Kernighan Dennis-Ritchie hash function.
//! @param str - input data
//! @return hash value
std::size_t bkdrHash(const char* str)
{
    std::size_t hash = 0;
    while (*str)
    {
        hash = hash * bkdrHashSeed + (*str++);
    }
    return hash & bkdrHashSize;
}

//! @brief Base64 encoding.
//! @param data - decoded data
//! @return encoded data
std::string base64Encode(const std::string& data)
{
    std::size_t counter = 0, offset = 0;
    std::uint32_t bitStream = 0;
    std::string encoded;
    constexpr std::string_view base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                             "abcdefghijklmnopqrstuvwxyz"
                                             "0123456789+/";
    // NOLINTBEGIN (readability-magic-numbers)
    for (const unsigned char c : data)
    {
        const auto numVal = static_cast<unsigned int>(c);
        offset = 16 - counter % 3 * 8;
        bitStream += numVal << offset;
        switch (offset)
        {
            case 16:
                encoded += base64Chars.at(bitStream >> 18 & 0x3F);
                break;
            case 8:
                encoded += base64Chars.at(bitStream >> 12 & 0x3F);
                break;
            case 0:
                if (3 != counter)
                {
                    encoded += base64Chars.at(bitStream >> 6 & 0x3F);
                    encoded += base64Chars.at(bitStream & 0x3F);
                    bitStream = 0;
                }
                break;
            default:
                break;
        }
        ++counter;
    }

    switch (offset)
    {
        case 16:
            encoded += base64Chars.at(bitStream >> 12 & 0x3F);
            encoded += "==";
            break;
        case 8:
            encoded += base64Chars.at(bitStream >> 6 & 0x3F);
            encoded += '=';
            break;
        default:
            break;
    }
    // NOLINTEND (readability-magic-numbers)
    return encoded;
}

//! @brief Base64 decoding.
//! @param data - encoded data
//! @return decoded data
std::string base64Decode(const std::string& data)
{
    std::size_t counter = 0, offset = 0;
    std::uint32_t bitStream = 0;
    std::string decoded;
    constexpr std::string_view base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                             "abcdefghijklmnopqrstuvwxyz"
                                             "0123456789+/";
    // NOLINTBEGIN (readability-magic-numbers)
    for (const unsigned char c : data)
    {
        if (const auto numVal = base64Chars.find(c); std::string::npos != numVal)
        {
            offset = 18 - counter % 4 * 6;
            bitStream += numVal << offset;
            switch (offset)
            {
                case 12:
                    decoded += static_cast<char>(bitStream >> 16 & 0xFF);
                    break;
                case 6:
                    decoded += static_cast<char>(bitStream >> 8 & 0xFF);
                    break;
                case 0:
                    if (4 != counter)
                    {
                        decoded += static_cast<char>(bitStream & 0xFF);
                        bitStream = 0;
                    }
                    break;
                default:
                    break;
            }
        }
        else if ('=' != c)
        {
            throw std::runtime_error("Invalid base64 encoded data.");
        }
        ++counter;
    }
    // NOLINTEND (readability-magic-numbers)
    return decoded;
}

//! @brief Format as a string.
//! @param format - null-terminated multibyte string specifying how to interpret the data
//! @param ... - arguments
//! @return string after formatting
std::string formatString(const char* const format, ...)
{
    std::va_list list;
    ::va_start(list, format);
    int bufferSize = std::vsnprintf(nullptr, 0, format, list);
    ::va_end(list);
    if (bufferSize < 0)
    {
        throw std::runtime_error("Could not format string.");
    }

    ::va_start(list, format);
    std::vector<char> buffer(bufferSize + 1);
    std::vsnprintf(buffer.data(), bufferSize + 1, format, list);
    ::va_end(list);

    return std::string{buffer.cbegin(), buffer.cbegin() + bufferSize};
}

//! @brief Execute the command line.
//! @param command - target command line to be executed
//! @param timeout - timeout period (ms)
//! @return command line output
std::string executeCommand(const std::string& command, const std::uint32_t timeout)
{
    std::FILE* const pipe = ::popen(command.c_str(), "r");
    if (nullptr == pipe)
    {
        throw std::runtime_error("Could not open pipe when trying to execute command.");
    }

    std::string output;
    std::vector<char> buffer(4096);
    const auto startTime = std::chrono::steady_clock::now();
    for (;;)
    {
        if (timeout > 0)
        {
            const auto elapsedTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
            if (elapsedTime.count() > timeout)
            {
                ::pclose(pipe);
                throw std::runtime_error("Execute command timeout.");
            }
        }

        const std::size_t readLen = std::fread(buffer.data(), sizeof(char), buffer.size(), pipe);
        if (0 == readLen)
        {
            break;
        }
        output.append(buffer.data(), readLen);
    }

    const int exitStatus = ::pclose(pipe);
    if (-1 == exitStatus)
    {
        throw std::runtime_error("Could not close pipe when trying to execute command.");
    }
    if (WIFEXITED(exitStatus))
    {
        if (const int exitCode = WEXITSTATUS(exitStatus); EXIT_SUCCESS != exitCode)
        {
            throw std::runtime_error(
                "Returns exit code " + std::to_string(exitCode) + " when the command is executed.");
        }
    }
    else if (WIFSIGNALED(exitStatus))
    {
        const int signal = WTERMSIG(exitStatus);
        throw std::runtime_error("Terminated by signal " + std::to_string(signal) + " when the command is executed.");
    }
    else
    {
        throw std::runtime_error("The termination status is unknown when the command is executed.");
    }

    return output;
};

//! @brief Wait for input from the user.
//! @param action - handling for the input
//! @param timeout - timeout period (ms)
void waitForUserInput(const std::function<bool(const std::string&)>& action, const int timeout)
{
    const int epollFd = ::epoll_create1(0);
    if (-1 == epollFd)
    {
        throw std::runtime_error("Could not create epoll file descriptor.");
    }

    struct ::epoll_event event
    {
    };
    event.events = ::EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (::epoll_ctl(epollFd, EPOLL_CTL_ADD, STDIN_FILENO, &event))
    {
        ::close(epollFd);
        throw std::runtime_error("Could not add file descriptor to epoll.");
    }

    for (;;)
    {
        const int status = ::epoll_wait(epollFd, &event, 1, timeout);
        if (-1 == status)
        {
            ::close(epollFd);
            throw std::runtime_error("Failed to wait for epoll.");
        }
        else if ((0 != status) && (event.events & ::EPOLLIN))
        {
            std::string input;
            std::getline(std::cin, input);
            if (action(input))
            {
                break;
            }
            continue;
        }
        break;
    }
    ::close(epollFd);
}
} // namespace utility::common
