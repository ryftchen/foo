//! @file io.cpp
//! @author ryftchen
//! @brief The definitions (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "io.hpp"

#include <sys/epoll.h>
#include <sys/file.h>
#include <unistd.h>
#include <cstring>
#include <deque>
#include <filesystem>
#include <iostream>
#include <ranges>

namespace utility::io
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Execute the command line.
//! @param command - target command line to be executed
//! @return command line output
std::string executeCommand(const std::string& command)
{
    auto* const pipe = ::popen(command.c_str(), "r"); // NOLINT(cert-env33-c)
    if (!pipe)
    {
        throw std::runtime_error{"Could not open pipe when trying to execute command."};
    }

    std::string output{};
    for (std::vector<char> buffer(4096);;)
    {
        const std::size_t readLen = std::fread(buffer.data(), sizeof(char), buffer.size(), pipe);
        if (readLen == 0)
        {
            break;
        }
        output.append(buffer.data(), readLen);
    }

    const int status = ::pclose(pipe);
    if (WIFEXITED(status) && (WEXITSTATUS(status) != EXIT_SUCCESS))
    {
        throw std::runtime_error{"The command returned exit code " + std::to_string(WEXITSTATUS(status)) + '.'};
    }
    if (WIFSIGNALED(status))
    {
        throw std::runtime_error{"The command was terminated by signal " + std::to_string(WTERMSIG(status)) + '.'};
    }
    return output;
}

//! @brief Wait for input from the user.
//! @param operation - handling for inputs (interrupt waiting if the return value is true, otherwise continue waiting)
//! @param timeout - timeout period (ms)
void waitForUserInput(const std::function<bool(const std::string&)>& operation, const int timeout)
{
    const int epollFD = ::epoll_create1(0);
    if (epollFD == -1)
    {
        throw std::runtime_error{"Could not create epoll when trying to wait for user input."};
    }

    ::epoll_event event{};
    event.events = ::EPOLLIN;
    event.data.fd = STDIN_FILENO;
    if (::epoll_ctl(epollFD, EPOLL_CTL_ADD, STDIN_FILENO, &event) == -1)
    {
        ::close(epollFD);
        throw std::runtime_error{"Could not control epoll when trying to wait for user input."};
    }

    for (;;)
    {
        const int status = ::epoll_wait(epollFD, &event, 1, timeout);
        if (status == -1)
        {
            ::close(epollFD);
            throw std::runtime_error{"Failed to wait epoll when waiting for user input."};
        }
        if (status == 0)
        {
            break;
        }

        if ((event.events & ::EPOLLIN) != 0)
        {
            std::string input{};
            std::getline(std::cin, input);
            if (!operation || operation(input))
            {
                break;
            }
        }
    }

    ::close(epollFD);
}

//! @brief Read lines from the file.
//! @param filename - file path
//! @param lock - lock or not
//! @param reverse - reverse or not
//! @param limit - maximum number of lines
//! @return file contents
std::vector<std::string> readFileLines(
    const std::string_view filename, const bool lock, const bool reverse, const int limit)
{
    FileReader reader(filename);
    reader.open();
    if (lock)
    {
        reader.lock();
    }

    auto& stream = reader.stream();
    stream.seekg(0, std::ios::beg);
    std::vector<std::string> contents{};
    if (std::string line{}; !reverse)
    {
        contents.reserve((limit >= 0) ? limit : 1024);
        while (std::getline(stream, line))
        {
            if ((limit >= 0) && (static_cast<int>(contents.size()) == limit))
            {
                break;
            }
            contents.emplace_back(std::move(line));
        }
        contents.shrink_to_fit();
    }
    else
    {
        std::deque<std::string> buffer{};
        while (std::getline(stream, line))
        {
            buffer.emplace_back(std::move(line));
            if ((limit >= 0) && (static_cast<int>(buffer.size()) > limit))
            {
                buffer.pop_front();
            }
        }
        contents.reserve(buffer.size());
        for (auto& temp : std::ranges::reverse_view(buffer))
        {
            contents.emplace_back(std::move(temp));
        }
    }

    if (lock)
    {
        reader.unlock();
    }
    reader.close();
    return contents;
}

FDStreamBuffer::~FDStreamBuffer()
{
    reset();
}

void FDStreamBuffer::set(const int newFD)
{
    if (fd == newFD)
    {
        return;
    }

    if (fd >= 0)
    {
        sync();
        ::close(fd);
    }
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
    readBuffer.fill(0);
    writeBuffer.fill(0);
    fd = newFD;
}

void FDStreamBuffer::reset()
{
    if (fd < 0)
    {
        return;
    }

    flush();
    ::close(fd);
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
    readBuffer.fill(0);
    writeBuffer.fill(0);
    fd = -1;
}

int FDStreamBuffer::flush()
{
    if (fd < 0)
    {
        return 0;
    }

    const char* ptr = pbase();
    while (ptr < pptr())
    {
        const ::ssize_t writtenSize = ::write(fd, ptr, pptr() - ptr);
        if (writtenSize <= 0)
        {
            return -1;
        }
        ptr += writtenSize;
    }
    setp(writeBuffer.data(), writeBuffer.data() + writeBuffer.size());
    return 0;
}

FDStreamBuffer::int_type FDStreamBuffer::underflow()
{
    if (gptr() != egptr())
    {
        throw std::runtime_error{"Read pointer has not reached the end of the buffer."};
    }
    if (fd < 0)
    {
        return traits_type::eof();
    }

    const ::ssize_t readSize = ::read(fd, readBuffer.data(), readBuffer.size());
    if (readSize <= 0)
    {
        return traits_type::eof();
    }
    setg(readBuffer.data(), readBuffer.data(), readBuffer.data() + readSize);
    return traits_type::to_int_type(*gptr());
}

FDStreamBuffer::int_type FDStreamBuffer::overflow(const int_type c)
{
    if (pptr() != epptr())
    {
        throw std::runtime_error{"Write pointer has not reached the end of the buffer."};
    }
    if (fd < 0)
    {
        return traits_type::eof();
    }

    if (sync() == -1)
    {
        return traits_type::eof();
    }
    if (c != traits_type::eof())
    {
        *pptr() = traits_type::to_char_type(c);
        pbump(1);
    }
    return c;
}

int FDStreamBuffer::sync()
{
    return flush();
}

std::streampos FDStreamBuffer::seekoff(
    const std::streamoff off, const std::ios_base::seekdir way, const std::ios_base::openmode mode)
{
    if ((fd < 0) || (((mode & std::ios_base::out) != 0) && (sync() == -1)))
    {
        return -1;
    }

    ::off_t newOffset = 0;
    switch (way)
    {
        case std::ios_base::beg:
            newOffset = off;
            break;
        case std::ios_base::cur:
            newOffset = ((((mode & std::ios_base::in) != 0) && gptr()) ? (off - (egptr() - gptr())) : off)
                + ::lseek(fd, 0, SEEK_CUR);
            break;
        case std::ios_base::end:
            newOffset = off + ::lseek(fd, 0, SEEK_END);
            break;
        default:
            return -1;
    }

    if (::lseek(fd, newOffset, SEEK_SET) == -1)
    {
        return -1;
    }
    setg(nullptr, nullptr, nullptr);
    return newOffset;
}

std::streampos FDStreamBuffer::seekpos(const std::streampos sp, const std::ios_base::openmode mode)
{
    return seekoff(sp, std::ios_base::beg, mode);
}

std::streamsize FDStreamBuffer::showmanyc()
{
    return ((fd >= 0) && gptr() && egptr()) ? (egptr() - gptr()) : 0;
}

FileReader::~FileReader()
{
    unlock();
    close();
}

bool FileReader::isOpened() const
{
    return fd >= 0;
}

void FileReader::open()
{
    if (isOpened())
    {
        return;
    }

    fd = ::open(name.c_str(), O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        throw std::runtime_error{
            "Failed to open " + std::filesystem::path{name}.filename().string() + " file for reading."};
    }
    strBuf.set(fd);
}

void FileReader::close()
{
    if (!isOpened())
    {
        return;
    }

    strBuf.reset();
    fd = -1;
}

bool FileReader::isLocked() const
{
    return lockActive;
}

void FileReader::lock()
{
    if (isLocked())
    {
        return;
    }

    if (::flock(fd, LOCK_SH | LOCK_NB) != 0)
    {
        throw std::runtime_error{"Failed to lock file descriptor " + std::to_string(fd) + " for reading."};
    }
    lockActive = true;
}

void FileReader::unlock()
{
    if (!isLocked())
    {
        return;
    }

    ::flock(fd, LOCK_UN);
    lockActive = false;
}

std::istream& FileReader::stream() noexcept
{
    return input;
}

FileWriter::~FileWriter()
{
    unlock();
    close();
}

bool FileWriter::isOpened() const
{
    return fd >= 0;
}

void FileWriter::open(const bool overwrite)
{
    if (isOpened())
    {
        return;
    }

    fd = ::open(
        name.c_str(), O_CREAT | (overwrite ? O_TRUNC : O_APPEND) | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        throw std::runtime_error{
            "Failed to open " + std::filesystem::path{name}.filename().string() + " file for writing."};
    }
    strBuf.set(fd);
}

void FileWriter::close()
{
    if (!isOpened())
    {
        return;
    }

    strBuf.reset();
    fd = -1;
}

bool FileWriter::isLocked() const
{
    return lockActive;
}

void FileWriter::lock()
{
    if (isLocked())
    {
        return;
    }

    if (::flock(fd, LOCK_EX | LOCK_NB) != 0)
    {
        throw std::runtime_error{"Failed to lock file descriptor " + std::to_string(fd) + " for writing."};
    }
    lockActive = true;
}

void FileWriter::unlock()
{
    if (!isLocked())
    {
        return;
    }

    ::flock(fd, LOCK_UN);
    lockActive = false;
}

std::ostream& FileWriter::stream() noexcept
{
    return output;
}
} // namespace utility::io
