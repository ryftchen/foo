#include "time.hpp"

namespace util_time
{
std::string getCurrentSystemTime()
{
    char date[dateLength + 1] = {'\0'};
    const auto now = std::chrono::system_clock::now();
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % secToUsec;
    const time_t tt = std::chrono::system_clock::to_time_t(now);
    const tm* tm = localtime(&tt);

    std::snprintf(
        date,
        dateLength + 1,
        "%04u-%02u-%02u %02u:%02u:%02u.%06lu %.3s",
        tm->tm_year + dateStartYear,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec,
        microseconds.count(),
        tm->tm_zone);
    std::string dateStr(date);

    return dateStr;
}

std::mt19937 getRandomSeedByTime()
{
    timeval timeSeed{};
    gettimeofday(&timeSeed, nullptr);

    return std::mt19937(timeSeed.tv_sec * secToUsec + timeSeed.tv_usec);
}
} // namespace util_time
