#include "time.hpp"
#include <cstring>
#include <iostream>
#include "main.hpp"

std::string getCurrentSystemTime(char* const date)
{
    const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const tm* tm = localtime(&tt);

    std::snprintf(
        date, TIME_DATE_LENGTH + 1, "%u-%02u-%02u %02u:%02u:%02u",
        static_cast<uint32_t>(tm->tm_year) + TIME_DATE_YEAR_START,
        static_cast<uint32_t>(tm->tm_mon) + 1, static_cast<uint32_t>(tm->tm_mday),
        static_cast<uint32_t>(tm->tm_hour), static_cast<uint32_t>(tm->tm_min),
        static_cast<uint32_t>(tm->tm_sec));
    return date;
}
