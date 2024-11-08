//! @file io.cpp
//! @author ryftchen
//! @brief The definitions (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "io.hpp"

#include <sys/epoll.h>
#include <sys/file.h>
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

FDStreamBuffer::~FDStreamBuffer()
{
    close();
}

int FDStreamBuffer::fd() const
{
    return fileDescriptor;
}

void FDStreamBuffer::fd(const int newFD)
{
    if (fileDescriptor == newFD)
    {
        return;
    }

    if (fileDescriptor >= 0)
    {
        sync();
        ::close(fileDescriptor);
    }
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
    readBuffer.reset();
    writeBuffer.reset();
    fileDescriptor = newFD;
}

void FDStreamBuffer::close()
{
    if (fileDescriptor < 0)
    {
        return;
    }

    flush();
    ::close(fileDescriptor);
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
    readBuffer.reset();
    writeBuffer.reset();
    fileDescriptor = -1;
}

int FDStreamBuffer::flush()
{
    if ((fileDescriptor < 0) || !writeBuffer)
    {
        return 0;
    }

    const char* ptr = pbase();
    while (ptr < pptr())
    {
        const int writtenSize = ::write(fileDescriptor, ptr, pptr() - ptr);
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
        throw std::runtime_error("Read pointer has not reached the end of the buffer.");
    }
    if (fileDescriptor < 0)
    {
        return traits_type::eof();
    }

    if (!readBuffer)
    {
        readBuffer = std::make_unique<char[]>(bufferSize);
    }

    const int readSize = ::read(fileDescriptor, readBuffer.get(), bufferSize);
    if (readSize <= 0)
    {
        return traits_type::eof();
    }

    setg(readBuffer.get(), readBuffer.get(), readBuffer.get() + readSize);
    return traits_type::to_int_type(*gptr());
}

FDStreamBuffer::int_type FDStreamBuffer::overflow(int_type c)
{
    if (pptr() != epptr())
    {
        throw std::runtime_error("Write pointer has not reached the end of the buffer.");
    }
    if (fileDescriptor < 0)
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

std::streampos FDStreamBuffer::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode mode)
{
    if (fileDescriptor < 0)
    {
        return -1;
    }
    if (mode & std::ios_base::out)
    {
        if (sync() == -1)
        {
            return -1;
        }
    }

    ::off_t newOffset = 0;
    if (std::ios_base::beg == way)
    {
        newOffset = off;
    }
    else if (std::ios_base::cur == way)
    {
        if ((mode & std::ios_base::in) && gptr())
        {
            off -= (egptr() - gptr());
        }
        newOffset = ::lseek(fileDescriptor, 0, SEEK_CUR) + off;
    }
    else if (std::ios_base::end == way)
    {
        newOffset = ::lseek(fileDescriptor, 0, SEEK_END) + off;
    }
    else
    {
        return -1;
    }

    if (::lseek(fileDescriptor, newOffset, SEEK_SET) == -1)
    {
        return -1;
    }

    setg(nullptr, nullptr, nullptr);
    return newOffset;
}

std::streampos FDStreamBuffer::seekpos(std::streampos sp, std::ios_base::openmode mode)
{
    return seekoff(sp, std::ios_base::beg, mode);
}

std::streamsize FDStreamBuffer::showmanyc()
{
    if ((fileDescriptor < 0) || !gptr() || !egptr())
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
        throw std::runtime_error(
            "Failed to open " + std::filesystem::path(name).filename().string() + " file for reading.");
    }
    strBuf.fd(fd);
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
        throw std::runtime_error("Failed to lock file descriptor " + std::to_string(fd) + " for reading.");
    }
}

void FileReader::unlock() const
{
    if (::flock(fd, LOCK_UN))
    {
        throw std::runtime_error("Failed to unlock file descriptor " + std::to_string(fd) + " for reading.");
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
        throw std::runtime_error(
            "Failed to open " + std::filesystem::path(name).filename().string() + " file for writing.");
    }
    strBuf.fd(fd);
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
        throw std::runtime_error("Failed to lock file descriptor " + std::to_string(fd) + " for writing.");
    }
}

void FileWriter::unlock() const
{
    if (::flock(fd, LOCK_UN))
    {
        throw std::runtime_error("Failed to unlock file descriptor " + std::to_string(fd) + " for writing.");
    }
}

std::ostream& FileWriter::stream()
{
    return output;
}

//! @brief Execute the command line.
//! @param command - target command line to be executed
//! @param timeout - timeout period (ms)
//! @return command line output
std::string executeCommand(const std::string_view command, const std::uint32_t timeout)
{
    std::FILE* const pipe = ::popen(command.data(), "r");
    if (nullptr == pipe)
    {
        throw std::runtime_error("Could not open pipe when trying to execute command.");
    }

    std::string output{};
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
//! @param action - handling for the input (interrupt waiting if the return value is true, otherwise continue waiting)
//! @param timeout - timeout period (ms)
void waitForUserInput(const std::function<bool(const std::string_view)>& action, const int timeout)
{
    const int epollFD = ::epoll_create1(0);
    if (-1 == epollFD)
    {
        throw std::runtime_error("Could not create epoll when trying to wait for user input.");
    }

    ::epoll_event event{};
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
            std::string input{};
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

//! @brief Get the file contents.
//! @param filename - file path
//! @param toLock - lock or not
//! @param toReverse - reverse or not
//! @param totalRows - number of rows
//! @return file contents
std::list<std::string> getFileContents(
    const std::string_view filename, const bool toLock, const bool toReverse, const std::uint64_t totalRows)
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
        while ((contents.size() < totalRows) && std::getline(input, line))
        {
            contents.emplace_back(line);
        }
    }
    else
    {
        const std::uint64_t numOfLines = std::count(
                                std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'),
                            startLine = (numOfLines > totalRows) ? (numOfLines - totalRows + 1) : 1;
        input.clear();
        input.seekg(0, std::ios::beg);
        for (std::uint64_t i = 0; i < (startLine - 1); ++i)
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
} // namespace utility::io
