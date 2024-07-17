//! @file io.hpp
//! @author ryftchen
//! @brief The declarations (io) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#include <functional>
#include <limits>
#include <list>

//! @brief The utility module.
namespace utility // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief I/O-operation-related functions in the utility module.
namespace io
{
extern const char* version() noexcept;

extern void fdReadLock(std::ifstream& ifs);
extern void fdWriteLock(std::ofstream& ofs);
//! @brief File descriptor unlock operation.
//! @tparam T - type of file stream
//! @param fs - file stream
template <typename T>
requires (std::is_same<T, std::ifstream>::value || std::is_same<T, std::ofstream>::value)
void fdUnlock(T& fs)
{
    const int fd = static_cast<::__gnu_cxx::stdio_filebuf<char>*>(fs.rdbuf())->fd();
    if (::flock(fd, LOCK_UN))
    {
        throw std::runtime_error("Could not unlock file descriptor.");
    }
}

extern std::ifstream openFile(const std::string& filename);
extern std::ofstream openFile(const std::string& filename, const bool overwrite);
//! @brief Close file.
//! @tparam T - type of file stream
//! @param fs - file stream
template <typename T>
requires (std::is_same<T, std::ifstream>::value || std::is_same<T, std::ofstream>::value)
void closeFile(T& fs)
{
    if (fs.is_open())
    {
        fs.close();
    }
}

extern std::list<std::string> getFileContents(
    const std::string& filename,
    const bool lock,
    const bool reverse = false,
    const std::uint64_t totalRows = std::numeric_limits<std::uint64_t>::max());
extern std::string executeCommand(const std::string& command, const std::uint32_t timeout = 0);
extern void waitForUserInput(const std::function<bool(const std::string&)>& action, const int timeout = -1);
} // namespace io
} // namespace utility
