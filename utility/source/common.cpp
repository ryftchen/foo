//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "common.hpp"
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <stdexcept>
#include <vector>

namespace utility::common
{
//! @brief The Brian-Kernighan Dennis-Ritchie hash function.
//! @param data - input data
//! @return hash value
std::size_t bkdrHash(const char* data)
{
    std::size_t hash = 0;
    while (*data)
    {
        hash = hash * bkdrHashSeed + (*data++);
    }
    return (hash & bkdrHashSize);
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

    for (const unsigned char c : data)
    {
        const auto numVal = static_cast<unsigned int>(c);
        offset = 16 - counter % 3 * 8; // NOLINT(readability-magic-numbers)
        bitStream += numVal << offset;
        switch (offset)
        {
            case 16:
                encoded += base64Chars.at(bitStream >> 18 & 0x3F); // NOLINT(readability-magic-numbers)
                break;
            case 8:
                encoded += base64Chars.at(bitStream >> 12 & 0x3F); // NOLINT(readability-magic-numbers)
                break;
            case 0:
                if (3 != counter) // NOLINT(readability-magic-numbers)
                {
                    encoded += base64Chars.at(bitStream >> 6 & 0x3F); // NOLINT(readability-magic-numbers)
                    encoded += base64Chars.at(bitStream & 0x3F); // NOLINT(readability-magic-numbers)
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
            encoded += base64Chars.at(bitStream >> 12 & 0x3F); // NOLINT(readability-magic-numbers)
            encoded += "==";
            break;
        case 8:
            encoded += base64Chars.at(bitStream >> 6 & 0x3F); // NOLINT(readability-magic-numbers)
            encoded += '=';
            break;
        default:
            break;
    }
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

    for (const unsigned char c : data)
    {
        const auto numVal = base64Chars.find(c);
        if (std::string::npos != numVal)
        {
            offset = 18 - counter % 4 * 6; // NOLINT(readability-magic-numbers)
            bitStream += numVal << offset;
            switch (offset)
            {
                case 12: // NOLINT(readability-magic-numbers)
                    decoded += static_cast<char>(bitStream >> 16 & 0xFF); // NOLINT(readability-magic-numbers)
                    break;
                case 6: // NOLINT(readability-magic-numbers)
                    decoded += static_cast<char>(bitStream >> 8 & 0xFF); // NOLINT(readability-magic-numbers)
                    break;
                case 0:
                    if (4 != counter)
                    {
                        decoded += static_cast<char>(bitStream & 0xFF); // NOLINT(readability-magic-numbers)
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

    return decoded;
}

//! @brief Format as a string.
//! @param format - null-terminated multibyte string specifying how to interpret the data
//! @param ... - arguments
//! @return string after formatting
std::string formatString(const char* const format, ...)
{
    ::va_list list;
    ::va_start(list, format);
    int bufferSize = std::vsnprintf(nullptr, 0, format, list);
    ::va_end(list);
    assert(bufferSize >= 0);

    ::va_start(list, format);
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    std::vsnprintf(buffer, bufferSize + 1, format, list);
    ::va_end(list);

    return std::string{buffer};
}

//! @brief Execute the command line.
//! @param command - target command line to be executed
//! @param timeout - timeout period
//! @return command line output
std::string executeCommand(const std::string& command, const std::uint32_t timeout)
{
    std::FILE* pipe = ::popen(command.c_str(), "r");
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
        const int exitCode = WEXITSTATUS(exitStatus);
        if (EXIT_SUCCESS != exitCode)
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
} // namespace utility::common
