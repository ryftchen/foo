//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include "common.hpp"
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <vector>

namespace utility::common
{
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
    return (hash & bkdrHashSize);
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
    ++bufferSize;

    ::va_start(list, format);
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    std::vsnprintf(buffer, bufferSize + 1, format, list);
    ::va_end(list);

    return std::string{buffer};
}

//! @brief Execute the command line.
//! @param cmd - target command line to be executed
//! @param timeout - timeout period
//! @return command line output
std::string executeCommand(const std::string& cmd, const std::uint32_t timeout)
{
    std::FILE* pipe = ::popen(cmd.c_str(), "r");
    if (nullptr == pipe)
    {
        throw std::runtime_error("Could not open pipe when trying to execute command.");
    }

    std::string output;
    std::vector<char> buffer(maxBufferSize);
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

        const std::size_t len = std::fread(buffer.data(), 1, buffer.size(), pipe);
        if (0 == len)
        {
            break;
        }
        output.append(buffer.data(), len);
    }

    const int exitStatus = ::pclose(pipe);
    if (-1 == exitStatus)
    {
        throw std::runtime_error("Could not close pipe when trying to execute command.");
    }
    if (WIFEXITED(exitStatus))
    {
        const int exitCode = WEXITSTATUS(exitStatus);
        if (0 != exitCode)
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
