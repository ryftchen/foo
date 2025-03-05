//! @file time.cpp
//! @author ryftchen
//! @brief The definitions (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "time.hpp"

namespace utility::time
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

Time::Time()
{
    resetBeginTime();
}

void Time::resetBeginTime()
{
    beginTime = std::chrono::high_resolution_clock::now();
}

double Time::calcElapsedTime() const
{
    return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - beginTime).count();
}

//! @brief Create a one-shot timer with blocking.
//! @param termination - termination condition
//! @param timeout - timeout period (ms)
//! @return the value is 0 if the termination condition is met, otherwise -1 on timeout
int blockingTimer(const std::function<bool()>& termination, const int timeout)
{
    for (const Time timer{}; (timeout < 0) || (timer.calcElapsedTime() <= timeout);)
    {
        if (termination())
        {
            return 0;
        }
        std::this_thread::yield();
    }

    return -1;
}

//! @brief Get the current system time, like "1970-01-01 00:00:00.000000 UTC".
//! @return current system time
std::string getCurrentSystemTime()
{
    constexpr std::uint16_t dateLen = 32, startYear = 1900;
    char date[dateLen] = {'\0'};
    const auto now = std::chrono::system_clock::now();
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;
    const std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};

    ::localtime_r(&tt, &tm);
    std::snprintf(
        date,
        dateLen,
        "%04u-%02u-%02u %02u:%02u:%02u.%06lu %.3s",
        tm.tm_year + startYear,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        microseconds.count(),
        tm.tm_zone);

    return std::string{date};
}
} // namespace utility::time
