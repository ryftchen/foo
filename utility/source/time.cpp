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
    reset();
}

void Time::reset()
{
    beginTime = std::chrono::high_resolution_clock::now();
}

//! @brief Get the current system time, like "1970-01-01 00:00:00.000000 UTC".
//! @return current system time
std::string getCurrentSystemTime()
{
    constexpr std::uint16_t dateLen = 32, sinceWhen = 1900;
    char date[dateLen] = {'\0'};
    const auto now = std::chrono::system_clock::now();
    const auto microsec =
        (std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000).count();
    const std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};

    ::localtime_r(&tt, &tm);
    std::snprintf(
        date,
        dateLen,
        "%04u-%02u-%02u %02u:%02u:%02u.%06lu %.3s",
        tm.tm_year + sinceWhen,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        microsec,
        tm.tm_zone);

    return std::string{date};
}

//! @brief Create a one-shot timer with blocking.
//! @param termination - termination condition
//! @param timeout - timeout period (ms)
//! @return the value is 0 if the termination condition is met, otherwise -1 on timeout
int blockingTimer(const std::function<bool()>& termination, const int timeout)
{
    for (const Time timer{}; (timeout < 0) || (timer.elapsedTime() <= timeout);)
    {
        if (termination())
        {
            return 0;
        }
        std::this_thread::yield();
    }

    return -1;
}
} // namespace utility::time
