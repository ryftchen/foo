//! @file file.cpp
//! @author ryftchen
//! @brief The definitions (file) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "file.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>

namespace utility::file
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
            "<FILE> Failed to open file " + std::filesystem::path(filename).filename().string() + " for reading.");
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
            "<FILE> Failed to open file " + std::filesystem::path(filename).filename().string() + " for writing.");
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
//! @return contents string
std::string displayFileContents(const FileProperty& property, const DisplaySetting& setting)
{
    FileReadWriteGuard guard(FileReadWriteGuard::LockMode::read, property.lock);
    std::ifstream ifs = openFile(property.path);

    DisplaySetting::FormatStyle formatStyle = setting.style;
    if (nullptr == formatStyle)
    {
        formatStyle = [](std::string& line) -> const std::string&
        {
            return line;
        };
    }

    std::string line;
    std::list<std::string> content(0);
    ifs.seekg(std::ios::beg);
    if (!setting.isInverted)
    {
        while ((content.size() < setting.numOfLines) && std::getline(ifs, line))
        {
            content.emplace_back(formatStyle(line));
        }
    }
    else
    {
        std::ifstream ifsTemp(property.path);
        const uint32_t lineNum =
                           std::count(std::istreambuf_iterator<char>(ifsTemp), std::istreambuf_iterator<char>(), '\n'),
                       startLine = (lineNum > setting.numOfLines) ? (lineNum - setting.numOfLines + 1) : 1;
        for (uint32_t i = 0; i < (startLine - 1); ++i)
        {
            ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        while (std::getline(ifs, line))
        {
            content.emplace_front(formatStyle(line));
        }
    }
    std::ostringstream os;
    std::copy(content.cbegin(), content.cend(), std::ostream_iterator<std::string>(os, "\n"));

    closeFile(ifs);
    return os.str();
}
} // namespace utility::file
