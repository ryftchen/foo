//! @file time.cpp
//! @author ryftchen
//! @brief The definitions (time) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "time.hpp"

namespace utility::time
{
void Time::setBeginTime()
{
    beginTime = std::chrono::steady_clock::now();
}

void Time::setEndTime()
{
    endTime = std::chrono::steady_clock::now();
}

double Time::getTimeInterval() const
{
    const std::chrono::duration<double, std::milli> timeInterval =
        std::chrono::duration<double, std::milli>(endTime - beginTime);
    return timeInterval.count();
}

void BlockingTimer::reset()
{
    isRunning.store(false);
}

//! @brief Perform millisecond-level sleep.
//! @param duration - sleep duration
void millisecondLevelSleep(const std::uint32_t duration)
{
    std::this_thread::sleep_for(std::chrono::operator""ms(duration));
}

//! @brief Get the current system time, like "1970-01-01 00:00:00.000000 UTC".
//! @return current system time
std::string getCurrentSystemTime()
{
    constexpr std::uint16_t dateLength = 32, dateStartYear = 1900;
    char date[dateLength] = {'\0'};
    const auto now = std::chrono::system_clock::now();
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;
    const std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    ::localtime_r(&tt, &tm);

    std::snprintf(
        date,
        dateLength,
        "%04u-%02u-%02u %02u:%02u:%02u.%06lu %.3s",
        tm.tm_year + dateStartYear,
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
