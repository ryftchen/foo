//! @file common.hpp
//! @author ryftchen
//! @brief The declarations (common) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <ext/stdio_filebuf.h>
#include <sys/file.h>
// #define NDEBUG
#include <cassert>
#include <filesystem>
#include <iostream>

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
//! @brief Alias for print style.
typedef std::string& (*PrintStyle)(std::string& line);

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
//! @brief Print without style.
inline constexpr PrintStyle nullStyle = nullptr;
//! @brief Maximum number of lines to print.
constexpr uint32_t maxLineNumForPrintFile = 1000;
//! @brief Maximum size of output per line.
constexpr uint32_t maxBufferSize = 4096;

//! @brief Enumerate specific lock operation types.
enum class LockOperationType
{
    lock,
    unlock
};

//! @brief Enumerate specific file lock types.
enum class FileLockType
{
    readerLock,
    writerLock
};

//! @brief Throw an exception when calling system API.
//! @param api system API name
inline void throwCallSystemAPIException(const std::string& api)
{
    throw std::runtime_error("common: Failed to call system API: " + api + ".");
}

//! @brief Throw an exception when operating the lock.
//! @param name filename
//! @param lockOperation lock operation type
//! @param fileLock file lock type
inline void throwOperateLockException(
    const std::string& name,
    const LockOperationType lockOperation,
    const FileLockType fileLock)
{
    const std::string operate = (LockOperationType::lock == lockOperation) ? "lock" : "unlock",
                      type = (FileLockType::readerLock == fileLock) ? "reader" : "writer";
    throw std::runtime_error("common: Failed to " + operate + " " + type + " lock: " + name + ".");
}

//! @brief Throw an exception when operating the file.
//! @param name filename
//! @param isToOpen to open or not
inline void throwOperateFileException(const std::string& name, const bool isToOpen)
{
    const std::string operate = isToOpen ? "open" : "close";
    throw std::runtime_error("common: Failed to " + operate + " file: " + name + ".");
}

//! @brief Try to operate the file lock.
//! @tparam T type of file stream
//! @param file file stream
//! @param pathname target file to be operated
//! @param lockOperation lock operation type
//! @param fileLock file lock type
template <class T>
void tryToOperateFileLock(
    T& file,
    const char* const pathname,
    const LockOperationType lockOperation,
    const FileLockType fileLock)
{
    const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>*const>(file.rdbuf())->fd(),
              operate = (LockOperationType::lock == lockOperation)
        ? (((FileLockType::readerLock == fileLock) ? LOCK_SH : LOCK_EX) | LOCK_NB)
        : LOCK_UN;
    if (flock(fd, operate))
    {
        file.close();
        throwOperateLockException(std::filesystem::path(pathname).filename().string(), lockOperation, fileLock);
    }
}

//! @brief Splice strings into constexpr type.
//! @tparam Strings target strings to be spliced
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
//! @tparam target strings to be spliced
template <std::string_view const&... Strings>
static constexpr auto joinStr = Join<Strings...>::value;

extern void executeCommand(const char* const cmd);
extern void printFile(
    const char* const pathname,
    const bool reverse = false,
    const uint32_t maxLine = maxLineNumForPrintFile,
    PrintStyle style = nullStyle);
} // namespace utility::common
