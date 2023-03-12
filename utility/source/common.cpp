//! @file common.cpp
//! @author ryftchen
//! @brief The definitions (common) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "common.hpp"
#include <cstring>
#include <filesystem>
#include <iterator>
#include <list>
#include <stdexcept>
#include <vector>

namespace utility::common
{
void FileReadWriteLock::readLock()
{
    std::shared_lock<std::shared_mutex> rdLock(rwLock);
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(
            lock,
            [this]() -> decltype(auto)
            {
                return (writer.load() == 0);
            });
        reader.fetch_add(1);
    }
}

void FileReadWriteLock::readUnlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    reader.fetch_sub(1);
    lock.unlock();
    cv.notify_all();
    lock.lock();
}

void FileReadWriteLock::writeLock()
{
    std::unique_lock<std::shared_mutex> wrLock(rwLock);
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(
            lock,
            [this]() -> decltype(auto)
            {
                return ((reader.load() == 0) && (writer.load() == 0));
            });
        writer.fetch_add(1);
    }
}

void FileReadWriteLock::writeUnlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    writer.fetch_sub(1);
    lock.unlock();
    cv.notify_all();
    lock.lock();
}

FileReadWriteGuard::FileReadWriteGuard(const LockMode mode, FileReadWriteLock& lock) : mode(mode), lock(lock)
{
    switch (mode)
    {
        case LockMode::read:
            lock.readLock();
            break;
        case LockMode::write:
            lock.writeLock();
            break;
        default:
            break;
    }
}

FileReadWriteGuard::~FileReadWriteGuard()
{
    switch (mode)
    {
        case LockMode::read:
            lock.readUnlock();
            break;
        case LockMode::write:
            lock.writeUnlock();
            break;
        default:
            break;
    }
}

//! @brief Execute the command line.
//! @param cmd - target command line to be executed
//! @param timeout - timeout period
//! @return command line output
std::string executeCommand(const std::string& cmd, const uint32_t timeout)
{
    std::FILE* pipe = popen(cmd.c_str(), "r");
    if (nullptr == pipe)
    {
        throw std::runtime_error("<COMMON> Cannot open pipe when trying to execute command.");
    }

    std::string output;
    std::vector<char> buffer(maxBufferSize);
    const auto startTime = std::chrono::steady_clock::now();
    while (true)
    {
        if (timeout > 0)
        {
            const auto elapsedTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
            if (elapsedTime.count() > timeout)
            {
                pclose(pipe);
                throw std::runtime_error("<COMMON> Execute command timeout.");
            }
        }

        const std::size_t n = std::fread(buffer.data(), 1, buffer.size(), pipe);
        if (0 == n)
        {
            break;
        }
        output.append(buffer.data(), n);
    }

    const int exitStatus = pclose(pipe);
    if (-1 == exitStatus)
    {
        throw std::runtime_error("<COMMON> Cannot close pipe when trying to execute command.");
    }
    if (WIFEXITED(exitStatus))
    {
        const int exitCode = WEXITSTATUS(exitStatus);
        if (0 != exitCode)
        {
            throw std::runtime_error(
                "<COMMON> Returns exit code " + std::to_string(exitCode) + " when the command is executed.");
        }
    }
    else if (WIFSIGNALED(exitStatus))
    {
        const int signal = WTERMSIG(exitStatus);
        throw std::runtime_error(
            "<COMMON> Terminated by signal " + std::to_string(signal) + " when the command is executed.");
    }
    else
    {
        throw std::runtime_error("<COMMON> The termination status is unknown when the command is executed.");
    }

    return output;
};

//! @brief Open file.
//! @param filename - file path
//! @return input file stream.
std::ifstream openFile(const std::string& filename)
{
    std::ifstream ifs;
    ifs.open(filename, std::ios_base::in);
    if (!ifs)
    {
        throw std::runtime_error(
            "<COMMON> Failed to open file " + std::filesystem::path(filename).filename().string() + " for reading.");
    }
    return ifs;
}

//! @brief Open file.
//! @param filename - file path
//! @param isOverwrite - be overwrite or not
//! @return output file stream
std::ofstream openFile(const std::string& filename, const bool isOverwrite)
{
    std::ofstream ofs;
    const std::ios_base::openmode mode = std::ios_base::out | (isOverwrite ? std::ios_base::trunc : std::ios_base::app);
    ofs.open(filename, mode);
    if (!ofs)
    {
        throw std::runtime_error(
            "<COMMON> Failed to open file " + std::filesystem::path(filename).filename().string() + " for writing.");
    }
    return ofs;
}

//! @brief Close file.
//! @param ifs - input file stream
void closeFile(std::ifstream& ifs)
{
    if (ifs.is_open())
    {
        ifs.close();
    }
}

//! @brief Close file.
//! @param ofs - output file stream
void closeFile(std::ofstream& ofs)
{
    if (ofs.is_open())
    {
        ofs.close();
    }
}

//! @brief Display file contents.
//! @param property - file property
//! @param setting - display setting
void displayFileContents(const FileProperty& property, const DisplaySetting& setting)
{
    FileReadWriteGuard guard(FileReadWriteGuard::LockMode::read, property.lock);
    std::ifstream ifs = openFile(property.path);

    DisplaySetting::FormatStyle formatStyle = setting.style;
    if (nullptr == formatStyle)
    {
        formatStyle = [](std::string& line) -> std::string&
        {
            return line;
        };
    }

    std::string line;
    std::list<std::string> content(0);
    ifs.seekg(std::ios::beg);
    if (!setting.isInverted)
    {
        while ((content.size() < setting.maxLine) && std::getline(ifs, line))
        {
            content.emplace_back(formatStyle(line));
        }
    }
    else
    {
        std::ifstream statistics(property.path);
        const uint32_t lineNum = std::count(
                           std::istreambuf_iterator<char>(statistics), std::istreambuf_iterator<char>(), '\n'),
                       startLine = (lineNum > setting.maxLine) ? (lineNum - setting.maxLine + 1) : 1;
        for (uint32_t i = 0; i < (startLine - 1); ++i)
        {
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        while (std::getline(ifs, line))
        {
            content.emplace_front(formatStyle(line));
        }
    }
    std::copy(content.cbegin(), content.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));

    closeFile(ifs);
}
} // namespace utility::common
