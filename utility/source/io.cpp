//! @file io.cpp
//! @author ryftchen
//! @brief The definitions (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "io.hpp"

#include <sys/epoll.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>

namespace utility::io
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Open file.
//! @param filename - file path
//! @return input file stream
std::ifstream openFile(const std::string& filename)
{
    std::ifstream ifs;
    ifs.open(filename, std::ios_base::in);
    if (!ifs)
    {
        throw std::runtime_error(
            "Failed to open " + std::filesystem::path(filename).filename().string() + " file for reading.");
    }
    return ifs;
}

//! @brief Open file.
//! @param filename - file path
//! @param overwrite - overwrite or not
//! @return output file stream
std::ofstream openFile(const std::string& filename, const bool overwrite)
{
    std::ofstream ofs;
    const std::ios_base::openmode mode = std::ios_base::out | (overwrite ? std::ios_base::trunc : std::ios_base::app);
    ofs.open(filename, mode);
    if (!ofs)
    {
        throw std::runtime_error(
            "Failed to open " + std::filesystem::path(filename).filename().string() + " file for writing.");
    }
    return ofs;
}

//! @brief File descriptor read lock operation.
//! @param ifs - input file stream
void fdReadLock(std::ifstream& ifs)
{
    const int fd = static_cast<::__gnu_cxx::stdio_filebuf<char>*>(ifs.rdbuf())->fd();
    if (::flock(fd, LOCK_SH | LOCK_NB))
    {
        throw std::runtime_error("Failed to lock file descriptor for reading.");
    }
}

//! @brief File descriptor write lock operation.
//! @param ofs - output file stream
void fdWriteLock(std::ofstream& ofs)
{
    const int fd = static_cast<::__gnu_cxx::stdio_filebuf<char>*>(ofs.rdbuf())->fd();
    if (::flock(fd, LOCK_EX | LOCK_NB))
    {
        throw std::runtime_error("Failed to lock file descriptor for writing.");
    }
}

//! @brief Get the file contents.
//! @param filename - file path
//! @param reverse - reverse or not
//! @param lock - lock or not
//! @param rows - number of rows
//! @return file contents
std::list<std::string> getFileContents(
    const std::string& filename, const bool lock, const bool reverse, const std::uint64_t rows)
{
    std::ifstream ifs = openFile(filename);
    if (lock)
    {
        fdReadLock(ifs);
    }
    ifs.seekg(std::ios::beg);

    std::string line;
    std::list<std::string> contents;
    if (!reverse)
    {
        while ((contents.size() < rows) && std::getline(ifs, line))
        {
            contents.emplace_back(line);
        }
    }
    else
    {
        const std::uint64_t totalLineNum =
                                std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'),
                            startLine = (totalLineNum > rows) ? (totalLineNum - rows + 1) : 1;
        ifs.seekg(std::ios::beg);
        for (std::uint64_t i = 0; i < (startLine - 1); ++i)
        {
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        while (std::getline(ifs, line))
        {
            contents.emplace_front(line);
        }
    }

    if (lock)
    {
        fdUnlock(ifs);
    }
    closeFile(ifs);
    return contents;
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
    const int epollFD = ::epoll_create1(0);
    if (-1 == epollFD)
    {
        throw std::runtime_error("Could not create epoll when trying to wait for user input.");
    }

    struct ::epoll_event event
    {
    };
    event.events = ::EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (::epoll_ctl(epollFD, EPOLL_CTL_ADD, STDIN_FILENO, &event))
    {
        ::close(epollFD);
        throw std::runtime_error("Could not control epoll when trying to wait for user input.");
    }

    for (;;)
    {
        const int status = ::epoll_wait(epollFD, &event, 1, timeout);
        if (-1 == status)
        {
            ::close(epollFD);
            throw std::runtime_error("Not the expected wait result for epoll.");
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

    ::close(epollFD);
}
} // namespace utility::io
