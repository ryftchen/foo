//! @file time.cpp
//! @author ryftchen
//! @brief The definitions (time) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "time.hpp"

#include <chrono>

namespace utility::time
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

Timer::~Timer()
{
    stop();
}

void Timer::start(const std::chrono::milliseconds& interval, const bool isPeriodic)
{
    if (isRunning())
    {
        return;
    }

    worker = std::jthread(
        [this, interval, isPeriodic](const std::stop_token& token)
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (const auto waitTimeout = [this, interval, &token](std::unique_lock<std::mutex>& lock)
                { return !cond.wait_for(lock, interval, [&token]() { return token.stop_requested(); }); };
                !isPeriodic)
            {
                if (waitTimeout(lock) && callback)
                {
                    callback();
                }
            }
            else
            {
                while (waitTimeout(lock) && callback)
                {
                    callback();
                }
            }
        });
}

void Timer::stop()
{
    if (!isRunning())
    {
        return;
    }

    worker.request_stop();
    cond.notify_all();
    if (worker.joinable())
    {
        worker.join();
    }
}

bool Timer::isRunning() const
{
    return worker.joinable();
}

Stopwatch::Stopwatch()
{
    reset();
}

void Stopwatch::reset()
{
    beginTime = std::chrono::high_resolution_clock::now();
}

//! @brief Get the current standard time (ISO 8601), like "1970-01-01T00:00:00.000000000Z".
//! @return current standard time
std::string currentStandardTime()
{
    return std::format("{:%FT%TZ}", std::chrono::system_clock::now());
}
} // namespace utility::time
