#pragma once
#include <unistd.h>
#define NDEBUG
#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>
#include "exception.hpp"

[[using gnu: constructor]] static void switchToProjectPath();

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
#define PRINT_COLOR_RED "\033[0;31;40m"
#define PRINT_COLOR_GREEN "\033[0;32;40m"
#define PRINT_COLOR_YELLOW "\033[0;33;40m"
#define PRINT_COLOR_END "\033[0m"
#define BUFFER_SIZE_MAX 4096

static void switchToProjectPath()
{
    try
    {
        char absolutePath[BUFFER_SIZE_MAX] = {'\0'};
        const int length = readlink("/proc/self/exe", absolutePath, BUFFER_SIZE_MAX);
        if ((length < 0) || (length >= BUFFER_SIZE_MAX))
        {
            throw CallFunctionError("readlink()");
        }

        std::filesystem::path buildPath(std::filesystem::path{absolutePath}.parent_path());
        assert(buildPath.has_parent_path());
        std::filesystem::current_path(buildPath.parent_path());
    }
    catch (CallFunctionError const& error)
    {
        std::cerr << error.what() << std::endl;
        exit(-1);
    }
}
