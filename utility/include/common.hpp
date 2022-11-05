#pragma once

#include <ext/stdio_filebuf.h>
#include <sys/file.h>
// #define NDEBUG
#include <cassert>
#include <filesystem>
#include <iostream>

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
#define COMMON_PRINT(format, args...) std::cout << COMMON_FORMAT_TO_STRING(format, ##args)

namespace util_common
{
typedef std::string& (*PrintStyle)(std::string& line);

inline constexpr std::string_view colorRed{"\033[0;31;40m"};
inline constexpr std::string_view colorGreen{"\033[0;32;40m"};
inline constexpr std::string_view colorYellow{"\033[0;33;40m"};
inline constexpr std::string_view colorBlue{"\033[0;34;40m"};
inline constexpr std::string_view colorGray{"\033[0;37;40m"};
inline constexpr std::string_view colorBold{"\033[1m"};
inline constexpr std::string_view colorUnderLine{"\033[4m"};
inline constexpr std::string_view colorForBackground{"\033[49m"};
inline constexpr std::string_view colorOff{"\033[0m"};
inline constexpr PrintStyle nullStyle = nullptr;
constexpr uint32_t maxLineNumForPrintFile = 1000;
constexpr uint32_t maxBufferSize = 4096;

enum class LockOperateType
{
    lock,
    unlock
};

enum class FileLockType
{
    readerLock,
    writerLock
};

inline void throwRunCommandLineException(const std::string& cmd)
{
    throw std::runtime_error("file: Failed to run common line: " + cmd);
}

inline void throwCallSystemApiException(const std::string& api)
{
    throw std::runtime_error("file: Failed to call system api: " + api);
}

inline void throwOperateLockException(
    const std::string& name,
    const LockOperateType lockOperate,
    const FileLockType fileLock)
{
    const std::string operate = (LockOperateType::lock == lockOperate) ? "lock" : "unlock",
                      type = (FileLockType::readerLock == fileLock) ? "reader" : "writer";
    throw std::runtime_error("file: Failed to " + operate + " " + type + " lock: " + name);
}

inline void throwOperateFileException(const std::string& name, const bool isToOpen)
{
    const std::string operate = isToOpen ? "open" : "close";
    throw std::runtime_error("file: Failed to " + operate + " file: " + name);
}

template <class T>
void tryToOperateFileLock(
    T& file,
    const char* const pathname,
    const LockOperateType lockOperate,
    const FileLockType fileLock)
{
    const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>*const>(file.rdbuf())->fd(),
              operate = (LockOperateType::lock == lockOperate)
        ? (((FileLockType::readerLock == fileLock) ? LOCK_SH : LOCK_EX) | LOCK_NB)
        : LOCK_UN;
    if (flock(fd, operate))
    {
        file.close();
        throwOperateLockException(std::filesystem::path(pathname).filename().string(), lockOperate, fileLock);
    }
}

template <std::string_view const&... Strings>
struct Join
{
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
    static constexpr auto array = impl();
    static constexpr std::string_view value{array.data(), array.size() - 1};
};
template <std::string_view const&... Strings>
static constexpr auto joinStr = Join<Strings...>::value;

extern void executeCommand(const char* const cmd);
extern void printFile(
    const char* const pathname,
    const bool reverse = false,
    const uint32_t maxLine = maxLineNumForPrintFile,
    PrintStyle style = nullStyle);
} // namespace util_common