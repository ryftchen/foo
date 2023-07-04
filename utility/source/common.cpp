//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#include "common.hpp"
#include <vector>

namespace utility::common
{
//! @brief The Brian-Kernighan Dennis-Ritchie hash function.
//! @param str - input data
//! @return hash value
std::uint64_t bkdrHash(const char* str)
{
    const std::uint64_t seed = bkdrHashSeed; // 31 131 1313 13131 131313 etc...
    std::uint64_t hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & bkdrHashSize);
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
