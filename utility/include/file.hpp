//! @file file.hpp
//! @author ryftchen
//! @brief The declarations (file) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <shared_mutex>
#include <string>

//! @brief File-operation-related functions in the utility module.
namespace utility::file
{
//! @brief Maximum display number of lines.
constexpr std::uint32_t maxDisNumOfLines = 1000;

//! @brief Lock for reading and writing files.
class FileReadWriteLock
{
public:
    //! @brief Construct a new FileReadWriteLock object.
    FileReadWriteLock() = default;
    //! @brief Destroy the FileReadWriteLock object.
    virtual ~FileReadWriteLock() = default;

    //! @brief Acquire a read lock.
    void readLock();
    //! @brief Release a read lock.
    void readUnlock();
    //! @brief Acquire a write lock.
    void writeLock();
    //! @brief Release a write lock.
    void writeUnlock();

private:
    //! @brief Handling of shared and exclusive locks.
    std::shared_mutex rwLock{};
    //! @brief Counter of readers that have acquired the shared lock.
    std::atomic_uint_fast16_t reader{0};
    //! @brief Counter of writers that have acquired the exclusive lock.
    std::atomic_uint_fast16_t writer{0};
    //! @brief Mutex for counters.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for counters. Use with mtx.
    std::condition_variable cv;
};

//! @brief Manage the lifetime of a lock on a file.
class FileReadWriteGuard
{
public:
    //! @brief Enumerate specific file lock modes.
    enum class LockMode : std::uint8_t
    {
        read,
        write
    };

    //! @brief Construct a new FileReadWriteGuard object.
    //! @param mode - lock mode
    //! @param lock - object managed by the guard
    FileReadWriteGuard(const LockMode mode, FileReadWriteLock& lock);
    //! @brief Destroy the FileReadWriteGuard object.
    virtual ~FileReadWriteGuard();

private:
    //! @brief Object managed by the guard.
    FileReadWriteLock& lock;
    //! @brief Lock mode.
    const LockMode mode;
};

//! @brief Property of the file.
struct FileProperty
{
    //! @brief File path.
    const std::string path;
    //! @brief File lock.
    FileReadWriteLock& lock;
};

//! @brief Setting to display content.
struct DisplaySetting
{
    //! @brief Alias for format style.
    typedef const std::string& (*FormatStyle)(std::string& line);

    //! @brief Be inverted or not.
    bool isInverted{false};
    //! @brief Number of lines to display.
    std::uint32_t numOfLines{maxDisNumOfLines};
    //! @brief Format style.
    FormatStyle style{nullptr};
};

extern std::ifstream openFile(const std::string& filename);
extern std::ofstream openFile(const std::string& filename, const bool isOverwrite);
extern void closeFile(std::ifstream& ifs);
extern void closeFile(std::ofstream& ofs);
extern std::string displayFileContents(const FileProperty& property, const DisplaySetting& setting = {});
} // namespace utility::file
