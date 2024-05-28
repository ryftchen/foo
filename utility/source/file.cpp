//! @file file.cpp
//! @author ryftchen
//! @brief The definitions (file) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "file.hpp"

#include <algorithm>
#include <filesystem>

namespace utility::file
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version()
{
    static const char* const ver = "0.1.0";
    return ver;
}

void ReadWriteLock::readLock()
{
    std::shared_lock<std::shared_mutex> rdLock(rwLock);
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(
            lock,
            [this]()
            {
                return (writer.load() == 0);
            });
        reader.fetch_add(1);
    }
}

void ReadWriteLock::readUnlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    reader.fetch_sub(1);
    lock.unlock();
    cv.notify_all();
    lock.lock();
}

void ReadWriteLock::writeLock()
{
    std::unique_lock<std::shared_mutex> wrLock(rwLock);
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        cv.wait(
            lock,
            [this]()
            {
                return ((reader.load() == 0) && (writer.load() == 0));
            });
        writer.fetch_add(1);
    }
}

void ReadWriteLock::writeUnlock()
{
    std::unique_lock<std::mutex> lock(mtx);
    writer.fetch_sub(1);
    lock.unlock();
    cv.notify_all();
    lock.lock();
}

ReadWriteGuard::ReadWriteGuard(const LockMode mode, ReadWriteLock& lock) : lock(lock), mode(mode)
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

ReadWriteGuard::~ReadWriteGuard()
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
            "Failed to open file " + std::filesystem::path(filename).filename().string() + " for reading.");
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
            "Failed to open file " + std::filesystem::path(filename).filename().string() + " for writing.");
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

//! @brief Get the file contents.
//! @param filename - file path
//! @param reverse - reverse or not
//! @param totalRows - number of rows
//! @return file contents
std::list<std::string> getFileContents(const std::string& filename, const bool reverse, const std::uint64_t totalRows)
{
    std::ifstream ifs = openFile(filename);
    ifs.seekg(std::ios::beg);

    std::string line;
    std::list<std::string> contents;
    if (!reverse)
    {
        while ((contents.size() < totalRows) && std::getline(ifs, line))
        {
            contents.emplace_back(line);
        }
    }
    else
    {
        const std::uint64_t lineNum =
                                std::count(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>(), '\n'),
                            startLine = (lineNum > totalRows) ? (lineNum - totalRows + 1) : 1;
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

    closeFile(ifs);
    return contents;
}
} // namespace utility::file
