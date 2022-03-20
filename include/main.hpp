#pragma once
#include <libgen.h>
#include <unistd.h>
#include <iostream>
#include "exception.hpp"

static void switchToProjectPath() __attribute__((constructor));

#undef basename
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
            throw std::runtime_error("System error.");
        }

        const int status = chdir(dirname(dirname(absolutePath)));
        if (-1 == status)
        {
            throw std::runtime_error("System error.");
        }
    }
    catch (std::runtime_error const& error)
    {
        std::cerr << error.what() << std::endl;
        exit(-1);
    }
}
