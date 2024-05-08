//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <thread>

//! @brief Store beginning time.
#define TIME_BEGIN(timing)      \
    utility::time::Time timing; \
    timing.setBeginTime()
//! @brief Store ending time.
#define TIME_END(timing) timing.setEndTime()
//! @brief Calculate time interval.
#define TIME_INTERVAL(timing) timing.getTimeInterval()

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Timing-related functions in the utility module.
namespace time
{
extern const char* version();

//! @brief Timing.
class Time
{
public:
    //! @brief Destroy the Time object.
    virtual ~Time() = default;

    //! @brief Set the beginning time.
    void setBeginTime();
    //! @brief Set the ending time.
    void setEndTime();
    //! @brief Get the time interval.
    [[nodiscard]] double getTimeInterval() const;

private:
    //! @brief Beginning time.
    std::chrono::steady_clock::time_point beginTime;
    //! @brief Ending time.
    std::chrono::steady_clock::time_point endTime;
};

//! @brief Blocking Timer.
class BlockingTimer
{
public:
    //! @brief Destroy the BlockingTimer object.
    virtual ~BlockingTimer() = default;

    //! @brief Set the blocking timer.
    //! @param func - callable function
    //! @param interval - time interval
    void set(const auto& func, const std::uint32_t interval);
    //! @brief Reset the blocking timer.
    void reset();

private:
    //! @brief Flag to indicate whether the blocking timer is running.
    std::atomic<bool> isRunning{true};
};

extern void millisecondLevelSleep(const std::uint32_t duration);

void BlockingTimer::set(const auto& func, const std::uint32_t interval)
{
    isRunning.store(true);
    std::thread timer(
        [=, this]()
        {
            while (isRunning.load())
            {
                millisecondLevelSleep(interval);
                func();
            }
        });
    timer.join();
}

extern std::string getCurrentSystemTime();
} // namespace time
} // namespace utility
