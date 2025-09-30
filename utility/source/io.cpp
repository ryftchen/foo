//! @file io.cpp
//! @author ryftchen
//! @brief The definitions (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "io.hpp"

#include <sys/epoll.h>
#include <sys/file.h>
#include <algorithm>
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

        if (event.events & ::EPOLLIN)
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

//! @brief Get the file contents.
//! @param filename - file path
//! @param toLock - lock or not
//! @param toReverse - reverse or not
//! @param lineLimit - maximum number of lines
//! @return file contents
std::list<std::string> getFileContents(
    const std::string_view filename, const bool toLock, const bool toReverse, const std::size_t lineLimit)
{
    FileReader fileReader(filename);
    fileReader.open();
    if (toLock)
    {
        fileReader.lock();
    }

    auto& input = fileReader.stream();
    input.seekg(0, std::ios::beg);
    std::string line{};
    std::list<std::string> contents{};
    if (!toReverse)
    {
        while ((contents.size() < lineLimit) && std::getline(input, line))
        {
            contents.emplace_back(line);
        }
    }
    else
    {
        const std::size_t numOfLines =
                              std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>{}, '\n'),
                          startLine = (numOfLines > lineLimit) ? (numOfLines - lineLimit + 1) : 1;
        input.clear();
        input.seekg(0, std::ios::beg);
        for (std::size_t i = 0; i < (startLine - 1); ++i)
        {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        while (std::getline(input, line))
        {
            contents.emplace_front(line);
        }
    }

    if (toLock)
    {
        fileReader.unlock();
    }
    fileReader.close();

    return contents;
}

FDStreamBuffer::~FDStreamBuffer()
{
    close();
}

void FDStreamBuffer::reset(const int newFD)
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
    readBuffer.reset();
    writeBuffer.reset();
    fd = newFD;
}

void FDStreamBuffer::close()
{
    if (fd < 0)
    {
        return;
    }

    flush();
    ::close(fd);
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
    readBuffer.reset();
    writeBuffer.reset();
    fd = -1;
}

int FDStreamBuffer::flush()
{
    if ((fd < 0) || !writeBuffer)
    {
        return 0;
    }

    const char* ptr = pbase();
    while (ptr < pptr())
    {
        const int writtenSize = ::write(fd, ptr, pptr() - ptr);
        if (writtenSize <= 0)
        {
            return -1;
        }
        ptr += writtenSize;
    }
    setp(writeBuffer.get(), writeBuffer.get() + bufferSize);

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

    if (!readBuffer)
    {
        readBuffer = std::make_unique<char[]>(bufferSize);
    }

    const int readSize = ::read(fd, readBuffer.get(), bufferSize);
    if (readSize <= 0)
    {
        return traits_type::eof();
    }
    setg(readBuffer.get(), readBuffer.get(), readBuffer.get() + readSize);

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

    if (!writeBuffer)
    {
        writeBuffer = std::make_unique<char[]>(bufferSize);
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
    if ((fd < 0) || ((mode & std::ios_base::out) && (sync() == -1)))
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
            newOffset =
                (((mode & std::ios_base::in) && gptr()) ? (off - (egptr() - gptr())) : off) + ::lseek(fd, 0, SEEK_CUR);
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
    if ((fd < 0) || !gptr() || !egptr())
    {
        return 0;
    }

    return egptr() - gptr();
}

FileReader::~FileReader()
{
    close();
}

bool FileReader::isOpen() const
{
    return fd >= 0;
}

void FileReader::open()
{
    if (isOpen())
    {
        return;
    }

    fd = ::open(name.c_str(), O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        throw std::runtime_error{
            "Failed to open " + std::filesystem::path(name).filename().string() + " file for reading."};
    }
    strBuf.reset(fd);
}

void FileReader::close()
{
    if (!isOpen())
    {
        return;
    }

    strBuf.close();
    ::close(fd);
    fd = -1;
}

void FileReader::lock() const
{
    if (::flock(fd, LOCK_SH | LOCK_NB))
    {
        throw std::runtime_error{"Failed to lock file descriptor " + std::to_string(fd) + " for reading."};
    }
}

void FileReader::unlock() const
{
    if (::flock(fd, LOCK_UN))
    {
        throw std::runtime_error{"Failed to unlock file descriptor " + std::to_string(fd) + " for reading."};
    }
}

std::istream& FileReader::stream()
{
    return input;
}

FileWriter::~FileWriter()
{
    close();
}

bool FileWriter::isOpen() const
{
    return fd >= 0;
}

void FileWriter::open(const bool overwrite)
{
    if (isOpen())
    {
        return;
    }

    fd = ::open(
        name.c_str(), O_CREAT | (overwrite ? O_TRUNC : O_APPEND) | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        throw std::runtime_error{
            "Failed to open " + std::filesystem::path(name).filename().string() + " file for writing."};
    }
    strBuf.reset(fd);
}

void FileWriter::close()
{
    if (!isOpen())
    {
        return;
    }

    strBuf.close();
    ::close(fd);
    fd = -1;
}

void FileWriter::lock() const
{
    if (::flock(fd, LOCK_EX | LOCK_NB))
    {
        throw std::runtime_error{"Failed to lock file descriptor " + std::to_string(fd) + " for writing."};
    }
}

void FileWriter::unlock() const
{
    if (::flock(fd, LOCK_UN))
    {
        throw std::runtime_error{"Failed to unlock file descriptor " + std::to_string(fd) + " for writing."};
    }
}

std::ostream& FileWriter::stream()
{
    return output;
}
} // namespace utility::io
