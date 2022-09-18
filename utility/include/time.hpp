#pragma once

#include <sys/time.h>
#include <chrono>
#include <random>
#include <string>
#include <thread>

#define TIME_BEGIN(timer)  \
    util_time::Time timer; \
    timer.setBeginTime()
#define TIME_END(timer) timer.setEndTime()
#define TIME_INTERVAL(timer) timer.getTimeInterval()

namespace util_time
{
constexpr uint32_t dateLength = 32;
constexpr uint32_t dateStartYear = 1900;

class Time final
{
public:
    virtual ~Time() = default;
    void inline setBeginTime();
    void inline setEndTime();
    [[nodiscard]] double inline getTimeInterval() const;

private:
    std::chrono::steady_clock::time_point beginTime;
    std::chrono::steady_clock::time_point endTime;
};

void inline Time::setBeginTime()
{
    beginTime = std::chrono::steady_clock::now();
}

void inline Time::setEndTime()
{
    endTime = std::chrono::steady_clock::now();
}

double inline Time::getTimeInterval() const
{
    const std::chrono::duration<double, std::milli> timeInterval =
        std::chrono::duration<double, std::milli>(endTime - beginTime);
    return timeInterval.count();
}

void inline millisecondLevelSleep(const uint32_t interval)
{
    std::this_thread::sleep_until(
        std::chrono::steady_clock::now() + std::chrono::operator""ms(interval));
}

std::string getCurrentSystemTime();
std::mt19937 getRandomSeedByTime();
} // namespace util_time
