//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <sys/time.h>
#include <chrono>
#include <random>
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
//! @brief Length of time format.
constexpr uint32_t dateLength = 32;
//! @brief Start year of time format.
constexpr uint32_t dateStartYear = 1900;
//! @brief Multiplier from the second to the millisecond.
constexpr uint32_t secToUsec = 1000000;

//! @brief Timer.
class Time final
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
    //! @brief Set the blocking timer.
    //! @param func callable function
    //! @param interval time interval
    void setBlockingTimer(auto func, const uint32_t interval);
    //! @brief Reset the blocking timer.
    inline void resetBlockingTimer();

private:
    //! @brief Beginning time.
    std::chrono::steady_clock::time_point beginTime;
    //! @brief Ending time.
    std::chrono::steady_clock::time_point endTime;
    //! @brief Flag to indicate whether the blocking timer is running.
    std::atomic<bool> isBlockingTimerRunning{true};

protected:
    friend inline void millisecondLevelSleep(const uint32_t duration);
};

//! @brief Perform millisecond-level sleep.
//! @param duration sleep duration
inline void millisecondLevelSleep(const uint32_t duration)
{
    std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::operator""ms(duration));
}

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

void Time::setBlockingTimer(auto func, const uint32_t interval)
{
    isBlockingTimerRunning.store(true);
    std::thread timerThread(
        [=]()
        {
            while (isBlockingTimerRunning.load())
            {
                millisecondLevelSleep(interval);
                func();
            }
        });

    pthread_setname_np(timerThread.native_handle(), "timer");
    timerThread.join();
}

inline void Time::resetBlockingTimer()
{
    isBlockingTimerRunning.store(false);
}

extern std::string getCurrentSystemTime();
} // namespace utility::time
