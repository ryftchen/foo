#pragma once
#include <iostream>
#include <unistd.h>
#include "./exception.hpp"
#include "./log.hpp"

void switchToProjectPath()
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

        uint32_t cnt = 0;
        for (int i = length; i >= 0; --i)
        {
            if ('/' == absolutePath[i])
            {
                ++cnt;
            }
            if (2 == cnt)
            {
                absolutePath[i + 1] = '\0';
                break;
            }
        }

        const int status = chdir(absolutePath);
        if (-1 == status)
        {
            throw std::runtime_error("System error.");
        }
    }
    catch (std::runtime_error const &error)
    {
        LOGGER(Log::Level::levelError, error.what());
        exit(-1);
    }
}
