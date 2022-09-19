#pragma once

#include <sys/time.h>
#include <chrono>
#include <random>
#include <string>
#include <thread>

#define TIME_BEGIN(timing)  \
    util_time::Time timing; \
    timing.setBeginTime()
#define TIME_END(timing) timing.setEndTime()
#define TIME_INTERVAL(timing) timing.getTimeInterval()

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
    void setBlockingTimer(auto func, const uint32_t delay);
    void inline resetBlockingTimer();

private:
    std::chrono::steady_clock::time_point beginTime;
    std::chrono::steady_clock::time_point endTime;
    std::atomic<bool> isBlockingTimerRunning{true};

protected:
    friend void inline millisecondLevelSleep(const uint32_t interval);
};

void inline millisecondLevelSleep(const uint32_t interval)
{
    std::this_thread::sleep_until(
        std::chrono::steady_clock::now() + std::chrono::operator""ms(interval));
}

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

void Time::setBlockingTimer(auto func, const uint32_t delay)
{
    isBlockingTimerRunning = true;
    std::thread timerThread(
        [=]()
        {
            while (isBlockingTimerRunning)
            {
                millisecondLevelSleep(delay);
                func();
            }
        });

    pthread_setname_np(timerThread.native_handle(), "timer");
    timerThread.join();
}

void inline Time::resetBlockingTimer()
{
    isBlockingTimerRunning = false;
}

std::string getCurrentSystemTime();
std::mt19937 getRandomSeedByTime();
} // namespace util_time
