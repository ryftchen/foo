#pragma once

#include <ext/stdio_filebuf.h>
#include <sys/file.h>
// #define NDEBUG
#include <cassert>
#include <filesystem>
#include <iostream>

#define FORMAT_TO_STRING(format, args...)                                     \
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
#define FORMAT_PRINT(format, args...) std::cout << FORMAT_TO_STRING(format, ##args)

namespace util_file
{
void executeCommand(const char* const cmd);
[[noreturn]] void throwRunCommandLineException(const std::string& str);
[[noreturn]] void throwCallSystemApiException(const std::string& str);
[[noreturn]] void throwOperateFileException(const std::string& name, const bool isToOpen);
[[noreturn]] void throwOperateLockException(
    const std::string& name, const bool isToLock, const bool isReader);

inline constexpr std::string_view redForeground = "\033[0;31;40m";
inline constexpr std::string_view greenForeground = "\033[0;32;40m";
inline constexpr std::string_view yellowForeground = "\033[0;33;40m";
inline constexpr std::string_view colorEnd = "\033[0m";
constexpr uint32_t maxLineNumForPrintFile = 1000;
constexpr uint32_t maxBufferSize = 4096;

typedef std::string& (*PrintStyle)(std::string& line);
inline constexpr PrintStyle nullStyle = nullptr;
void printFile(
    const char* const pathname, const bool reverse = false,
    const uint32_t maxLine = maxLineNumForPrintFile, PrintStyle style = nullStyle);

template <std::string_view const&... Strings>
struct Join
{
    static constexpr auto impl() noexcept
    {
        constexpr std::size_t length = (Strings.size() + ... + 0);
        std::array<char, length + 1> array{};
        auto append = [i = 0, &array](auto const& str) mutable
        {
            for (auto ch : str)
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

template <class T>
void tryToOperateFileLock(
    T& file, const char* const pathname, const bool isToLock, const bool isReader)
{
    const int fd = static_cast<__gnu_cxx::stdio_filebuf<char>* const>(file.rdbuf())->fd();
    const int operate = isToLock ? ((isReader ? LOCK_SH : LOCK_EX) | LOCK_NB) : LOCK_UN;
    if (flock(fd, operate))
    {
        file.close();
        throwOperateLockException(
            std::filesystem::path(pathname).filename().string(), isToLock, isReader);
    }
}
} // namespace util_file
