#pragma once
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
#define BUFFER_SIZE_16 16
#define BUFFER_SIZE_32 32
#define BUFFER_SIZE_4096 4096
#define BUFFER_SIZE_MAX BUFFER_SIZE_4096
#define PRINT_COLOR_RED "\033[0;31;40m"
#define PRINT_COLOR_GREEN "\033[0;32;40m"
#define PRINT_COLOR_YELLOW "\033[0;33;40m"
#define PRINT_COLOR_END "\033[0m"

static void switchToProjectPath()
{
    const std::filesystem::path absolutePath =
        std::filesystem::canonical(std::filesystem::path{"/proc/self/exe"});
    const size_t pos = absolutePath.string().find_last_of("build");
    if (std::string::npos == pos)
    {
        printf("The build directory does not exist. Please check it.\n");
        exit(-1);
    }
    const std::filesystem::path buildPath(
        std::filesystem::path{absolutePath.string().substr(0, pos)});

    if (!buildPath.has_parent_path())
    {
        printf("The project directory does not exist. Please check it.\n");
        exit(-1);
    }
    std::filesystem::current_path(buildPath.parent_path());
}
