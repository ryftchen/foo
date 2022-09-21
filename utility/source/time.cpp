#include "time.hpp"

namespace util_time
{
std::string getCurrentSystemTime()
{
    char date[dateLength + 1] = {'\0'};
    const time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const tm* tm = localtime(&tt);

    std::snprintf(
        date,
        dateLength + 1,
        "%u-%02u-%02u %02u:%02u:%02u",
        static_cast<uint32_t>(tm->tm_year) + dateStartYear,
        static_cast<uint32_t>(tm->tm_mon) + 1,
        static_cast<uint32_t>(tm->tm_mday),
        static_cast<uint32_t>(tm->tm_hour),
        static_cast<uint32_t>(tm->tm_min),
        static_cast<uint32_t>(tm->tm_sec));

    std::string dateStr(date);
    return dateStr;
}

std::mt19937 getRandomSeedByTime()
{
    timeval timeSeed{};
    constexpr uint32_t secToUsec = 1000000;
    gettimeofday(&timeSeed, nullptr);

    return std::mt19937(timeSeed.tv_sec * secToUsec + timeSeed.tv_usec);
}
} // namespace util_time
