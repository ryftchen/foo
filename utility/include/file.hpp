#pragma once
#include <ext/stdio_filebuf.h>
#include <sys/file.h>
#define NDEBUG
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
#define BUFFER_SIZE_16 16
#define BUFFER_SIZE_32 32
#define BUFFER_SIZE_4096 4096
#define BUFFER_SIZE_MAX BUFFER_SIZE_4096
#define PRINT_COLOR_RED "\033[0;31;40m"
#define PRINT_COLOR_GREEN "\033[0;32;40m"
#define PRINT_COLOR_YELLOW "\033[0;33;40m"
#define PRINT_COLOR_END "\033[0m"
#define PRINT_FILE_MAX_LINE 1000

typedef std::string (*PrintStyle)(std::string& line);
constexpr static PrintStyle nullStyle = nullptr;
void executeCommand(const char* const cmd);
void printFile(
    const char* const pathname, const bool reverse = false,
    const uint32_t maxLine = PRINT_FILE_MAX_LINE, PrintStyle style = nullStyle);
[[noreturn]] void throwRunCommandLineException(const std::string& str);
[[noreturn]] void throwCallSystemApiException(const std::string& str);
[[noreturn]] void throwOperateFileException(const std::string& name, const bool isToOpen);
[[noreturn]] void throwOperateLockException(
    const std::string& name, const bool isToLock, const bool isReader);

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
