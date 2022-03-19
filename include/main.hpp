#pragma once
#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include "exception.hpp"
#include "log.hpp"

static void switchToProjectPath() __attribute__((constructor));

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
    catch (std::runtime_error const &error)
    {
        LOGGER_ERR(error.what());
        exit(-1);
    }
}
