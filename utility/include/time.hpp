//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <chrono>
#include <functional>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Timing-related functions in the utility module.
namespace time
{
extern const char* version() noexcept;

//! @brief Timing.
class Time
{
public:
    //! @brief Construct a new Time object.
    Time();
    //! @brief Destroy the Time object.
    virtual ~Time() = default;

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
    std::chrono::high_resolution_clock::time_point beginTime{};
};

template <typename Rep, typename Period>
Rep Time::elapsedTime() const
{
    return std::chrono::duration<Rep, Period>(std::chrono::high_resolution_clock::now() - beginTime).count();
}

extern int blockingTimer(const std::function<bool()>& termination, const int timeout = -1);
extern void millisecondLevelSleep(const std::size_t duration);
extern std::string currentSystemTime();
} // namespace time
} // namespace utility
