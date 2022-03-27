#pragma once
#include <libgen.h>
#undef basename
#include <unistd.h>
#define NDEBUG
#include <cassert>
#include <cstring>
#include <iostream>
#include "exception.hpp"

static void switchToProjectPath() __attribute__((constructor));

#define FILENAME(x) (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)
#define PRINT_COLOR_RED "\033[0;31;40m"
#define PRINT_COLOR_GREEN "\033[0;32;40m"
#define PRINT_COLOR_YELLOW "\033[0;33;40m"
#define PRINT_COLOR_END "\033[0m"
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

static void switchToProjectPath()
{
    try
    {
        const int bufferSize = 4096;
        char absolutePath[bufferSize] = {'\0'};
        const int length = readlink("/proc/self/exe", absolutePath, bufferSize);
        if ((length < 0) || (length >= bufferSize))
        {
            throw CallFunctionError("readlink()");
        }

        const int status = chdir(dirname(dirname(absolutePath)));
        if (-1 == status)
        {
            throw CallFunctionError("chdir()");
        }
    }
    catch (CallFunctionError const& error)
    {
        std::cerr << error.what() << std::endl;
        exit(-1);
    }
}
