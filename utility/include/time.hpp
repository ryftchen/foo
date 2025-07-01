//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Timing-related functions in the utility module.
namespace time
{
extern const char* version() noexcept;

//! @brief Timer.
class Timer
{
public:
    //! @brief Construct a new Timer object.
    //! @param callback - timeout callback function
    explicit Timer(std::function<void()> callback) : callback{std::move(callback)} {}
    //! @brief Destroy the Timer object.
    virtual ~Timer();

    //! @brief Start the timer.
    //! @param interval - time interval
    //! @param isPeriodic - whether to repeat periodically or not
    void start(const std::chrono::milliseconds& interval, const bool isPeriodic = false);
    //! @brief Stop the timer.
    void stop();
    //! @brief Check if the timer is running.
    //! @return be running or not
    bool isRunning() const;

private:
    //! @brief The callback function that is executed on the timer trigger.
    const std::function<void()> callback;
    //! @brief Mutex for controlling thread.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for queue. Use with mtx.
    std::condition_variable_any cond;
    //! @brief Working thread.
    std::jthread worker;
};

//! @brief Stopwatch.
class Stopwatch
{
public:
    //! @brief Construct a new Stopwatch object.
    Stopwatch();
    //! @brief Destroy the Stopwatch object.
    virtual ~Stopwatch() = default;

    //! @brief Reset the beginning time.
    void reset();
    //! @brief Calculate the elapsed time.
    //! @tparam Rep - type of number of ticks
    //! @tparam Period - type of tick period
    //! @return elapsed time
    template <typename Rep = double, typename Period = std::milli>
    Rep elapsedTime() const;

private:
    //! @brief Beginning time.
    std::chrono::high_resolution_clock::time_point beginTime;
};

template <typename Rep, typename Period>
Rep Stopwatch::elapsedTime() const
{
    return std::chrono::duration<Rep, Period>(std::chrono::high_resolution_clock::now() - beginTime).count();
}

extern std::string currentStandardTime();
} // namespace time
} // namespace utility
