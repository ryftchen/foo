//! @file time.hpp
//! @author ryftchen
//! @brief The declarations (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

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
    //! @brief Destroy the Time object.
    virtual ~Time() = default;

    //! @brief Set the beginning time.
    void setBeginTime();
    //! @brief Set the ending time.
    void setEndTime();
    //! @brief Calculate the time interval.
    [[nodiscard]] double calculateInterval() const;

private:
    //! @brief Beginning time.
    std::chrono::high_resolution_clock::time_point beginTime{};
    //! @brief Ending time.
    std::chrono::high_resolution_clock::time_point endTime{};
};

//! @brief Perform millisecond-level sleep.
//! @param duration - sleep duration
inline void millisecondLevelSleep(const std::uint32_t duration)
{
    std::this_thread::sleep_for(std::chrono::operator""ms(duration));
}

extern int blockingTimer(const std::function<bool()>& termination, const std::uint32_t timeout = 0);
extern std::string getCurrentSystemTime();
} // namespace time
} // namespace utility
