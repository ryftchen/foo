//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <chrono>
#include <thread>

//! @brief Store beginning time.
#define TIME_BEGIN(timing)      \
    utility::time::Time timing; \
    timing.setBeginTime()
//! @brief Store ending time.
#define TIME_END(timing) timing.setEndTime()
//! @brief Calculate time interval.
#define TIME_INTERVAL(timing) timing.getTimeInterval()

//! @brief Timing-related functions in the utility module.
namespace utility::time
{
//! @brief Timing.
class Time
{
public:
    //! @brief Destroy the Time object.
    virtual ~Time() = default;

    //! @brief Set the beginning time.
    inline void setBeginTime();
    //! @brief Set the ending time.
    inline void setEndTime();
    //! @brief Get the time interval.
    [[nodiscard]] inline double getTimeInterval() const;

private:
    //! @brief Beginning time.
    std::chrono::steady_clock::time_point beginTime;
    //! @brief Ending time.
    std::chrono::steady_clock::time_point endTime;
};

inline void Time::setBeginTime()
{
    beginTime = std::chrono::steady_clock::now();
}

inline void Time::setEndTime()
{
    endTime = std::chrono::steady_clock::now();
}

inline double Time::getTimeInterval() const
{
    const std::chrono::duration<double, std::milli> timeInterval =
        std::chrono::duration<double, std::milli>(endTime - beginTime);
    return timeInterval.count();
}

//! @brief Blocking Timer.
class BlockingTimer
{
public:
    //! @brief Destroy the BlockingTimer object.
    virtual ~BlockingTimer() = default;

    //! @brief Set the blocking timer.
    //! @param func - callable function
    //! @param interval - time interval
    void set(auto func, const std::uint32_t interval);
    //! @brief Reset the blocking timer.
    inline void reset();

private:
    //! @brief Flag to indicate whether the blocking timer is running.
    std::atomic<bool> isRunning{true};

protected:
    friend inline void millisecondLevelSleep(const std::uint32_t duration);
};

//! @brief Perform millisecond-level sleep.
//! @param duration - sleep duration
inline void millisecondLevelSleep(const std::uint32_t duration)
{
    std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::operator""ms(duration));
}

void BlockingTimer::set(auto func, const std::uint32_t interval)
{
    isRunning.store(true);
    std::thread timer(
        [=]()
        {
            while (isRunning.load())
            {
                millisecondLevelSleep(interval);
                func();
            }
        });
    timer.join();
}

inline void BlockingTimer::reset()
{
    isRunning.store(false);
}

extern std::string getCurrentSystemTime();
} // namespace utility::time
