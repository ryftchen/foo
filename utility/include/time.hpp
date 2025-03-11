//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <thread>

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
    void resetBeginTime();
    //! @brief Calculate the elapsed time.
    //! @tparam Rep - type of number of ticks
    //! @tparam Period - type of tick period
    //! @return elapsed time
    template <typename Rep = double, typename Period = std::milli>
    [[nodiscard]] Rep calcElapsedTime() const;

private:
    //! @brief Beginning time.
    std::chrono::high_resolution_clock::time_point beginTime{};
};

template <typename Rep, typename Period>
Rep Time::calcElapsedTime() const
{
    return std::chrono::duration<Rep, Period>(std::chrono::high_resolution_clock::now() - beginTime).count();
}

//! @brief Perform millisecond-level sleep.
//! @param duration - sleep duration
inline void millisecondLevelSleep(const std::size_t duration)
{
    std::this_thread::sleep_for(std::chrono::operator""ms(duration));
}

extern int blockingTimer(const std::function<bool()>& termination, const int timeout = -1);
extern std::string getCurrentSystemTime();
} // namespace time
} // namespace utility
