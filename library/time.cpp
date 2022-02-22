#include <cstring>
#include <iostream>
#include "../include/time.hpp"

std::string getCurrentSystemTime(char *const date)
{
    const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const tm *tm = localtime(&tt);

    std::snprintf(date, TIME_DATE_LENGTH + 1, "%u-%02u-%02u %02u:%02u:%02u",
        (uint32_t)tm->tm_year + 1900, (uint32_t)tm->tm_mon + 1, (uint32_t)tm->tm_mday,
        (uint32_t)tm->tm_hour, (uint32_t)tm->tm_min, (uint32_t)tm->tm_sec);
    return std::string(date);
}
