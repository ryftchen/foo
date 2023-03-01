//! @file common.hpp
//! @author ryftchen
//! @brief The declarations (common) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

// #define NDEBUG
#include <cassert>
#include <fstream>
#include <iostream>
#include <shared_mutex>

//! @brief Format as a string.
#define COMMON_FORMAT_TO_STRING(format, args...)                              \
    (                                                                         \
        {                                                                     \
            const int bufferSize = std::snprintf(nullptr, 0, format, ##args); \
            assert(bufferSize >= 0);                                          \
            char buffer[bufferSize + 1];                                      \
            buffer[0] = '\0';                                                 \
            std::snprintf(buffer, bufferSize + 1, format, ##args);            \
            const std::string str(buffer);                                    \
            str;                                                              \
        })
//! @brief Format as a string and printing.
#define COMMON_PRINT(format, args...) std::cout << COMMON_FORMAT_TO_STRING(format, ##args)

//! @brief Common-interface-related functions in the utility module.
namespace utility::common
{
//! @brief ANSI escape codes for red foreground color.
inline constexpr std::string_view colorRed{"\033[0;31;40m"};
//! @brief ANSI escape codes for green foreground color.
inline constexpr std::string_view colorGreen{"\033[0;32;40m"};
//! @brief ANSI escape codes for yellow foreground color.
inline constexpr std::string_view colorYellow{"\033[0;33;40m"};
//! @brief ANSI escape codes for blue foreground color.
inline constexpr std::string_view colorBlue{"\033[0;34;40m"};
//! @brief ANSI escape codes for gray foreground color.
inline constexpr std::string_view colorGray{"\033[0;37;40m"};
//! @brief ANSI escape codes for the bold font.
inline constexpr std::string_view colorBold{"\033[1m"};
//! @brief ANSI escape codes for the underline font.
inline constexpr std::string_view colorUnderLine{"\033[4m"};
//! @brief ANSI escape codes for default background color.
inline constexpr std::string_view colorForBackground{"\033[49m"};
//! @brief ANSI escape codes for ending.
inline constexpr std::string_view colorOff{"\033[0m"};
//! @brief Maximum number of lines to print.
constexpr uint32_t maxLineNumForPrintFile = 1000;
//! @brief Maximum size of output per line.
constexpr uint32_t maxBufferSize = 4096;

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
    //! @brief The synchronization condition for counters. Use with mtx.
    std::condition_variable cv;
    //! @brief Mutex for counters.
    mutable std::mutex mtx;
};

//! @brief Manage the lifetime of a lock on a file.
class FileReadWriteGuard
{
public:
    //! @brief Enumerate specific file lock modes.
    enum class LockMode : uint8_t
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
    std::string path;
    //! @brief File lock.
    FileReadWriteLock& lock;
};

//! @brief Setting to display content.
struct DisplaySetting
{
    //! @brief Alias for format style.
    typedef std::string& (*FormatStyle)(std::string& line);

    //! @brief Be inverted or not.
    bool isInverted{false};
    //! @brief Maximum number of lines to display.
    uint32_t maxLine{maxLineNumForPrintFile};
    //! @brief Format style.
    FormatStyle style{nullptr};
};

//! @brief Splice strings into constexpr type.
//! @tparam Strings - target strings to be spliced
template <std::string_view const&... Strings>
struct Join
{
    //! @brief Implementation of splicing strings.
    //! @return character array
    static constexpr auto impl() noexcept
    {
        constexpr std::size_t length = (Strings.size() + ... + 0);
        std::array<char, length + 1> array{};
        auto append = [i = 0, &array](const auto& str) mutable
        {
            for (const auto ch : str)
            {
                array[i++] = ch;
            }
        };
        (append(Strings), ...);
        array[length] = 0;
        return array;
    }
    //! @brief A sequence of characters.
    static constexpr auto array = impl();
    //! @brief The splicing result. Converted from a sequence of characters.
    static constexpr std::string_view value{array.data(), array.size() - 1};
};
//! @brief Get the result of splicing strings.
//! @tparam target - strings to be spliced
template <std::string_view const&... Strings>
static constexpr auto joinStr = Join<Strings...>::value;

extern std::string executeCommand(const std::string& cmd, const uint32_t timeout = 0);
extern std::ifstream openFile(const std::string& filename);
extern std::ofstream openFile(const std::string& filename, const bool isOverwrite);
extern void closeFile(std::ifstream& ifs);
extern void closeFile(std::ofstream& ofs);
extern void displayFileContents(const FileProperty& property, const DisplaySetting& setting = {});
} // namespace utility::common
