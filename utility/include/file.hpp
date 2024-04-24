//! @file file.hpp
//! @author ryftchen
//! @brief The declarations (file) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#include <condition_variable>
#include <list>
#include <shared_mutex>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief File-operation-related functions in the utility module.
namespace file
{
//! @brief Enumerate specific file lock modes.
enum class LockMode : std::uint8_t
{
    //! @brief Read.
    read,
    //! @brief Write.
    write
};

//! @brief Lock for reading and writing files.
class ReadWriteLock
{
public:
    //! @brief Construct a new ReadWriteLock object.
    ReadWriteLock() = default;
    //! @brief Destroy the ReadWriteLock object.
    virtual ~ReadWriteLock() = default;

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
    mutable std::mutex mtx{};
    //! @brief The synchronization condition for counters. Use with mtx.
    std::condition_variable cv{};
};

//! @brief Manage the lifetime of a lock on a file.
class ReadWriteGuard
{
public:
    //! @brief Construct a new ReadWriteGuard object.
    //! @param mode - lock mode
    //! @param lock - object managed by the guard
    ReadWriteGuard(const LockMode mode, ReadWriteLock& lock);
    //! @brief Destroy the ReadWriteGuard object.
    virtual ~ReadWriteGuard();

private:
    //! @brief Object managed by the guard.
    ReadWriteLock& lock;
    //! @brief Lock mode.
    const LockMode mode;
};

//! @brief FD lock operation.
//! @tparam T - type of file stream
//! @param file - file stream
//! @param mode - lock mode
template <typename T>
void fdLock(T& file, const LockMode mode)
{
    const int fd = static_cast<::__gnu_cxx::stdio_filebuf<char>* const>(file.rdbuf())->fd(),
              operate = (LockMode::read == mode ? LOCK_SH : LOCK_EX) | LOCK_NB;
    if (::flock(fd, operate))
    {
        throw std::runtime_error("Failed to lock FD.");
    }
}

//! @brief FD unlock operation.
//! @tparam T - type of file stream
//! @param file - file stream
template <typename T>
void fdUnlock(T& file)
{
    const int fd = static_cast<::__gnu_cxx::stdio_filebuf<char>* const>(file.rdbuf())->fd();
    if (::flock(fd, LOCK_UN))
    {
        throw std::runtime_error("Failed to unlock FD.");
    }
}

extern std::ifstream openFile(const std::string& filename);
extern std::ofstream openFile(const std::string& filename, const bool isOverwrite);
extern void closeFile(std::ifstream& ifs);
extern void closeFile(std::ofstream& ofs);
extern std::list<std::string> getFileContents(
    const std::string& filename,
    const bool reverse = false,
    const std::uint32_t totalRows = 1024);
} // namespace file
} // namespace utility
