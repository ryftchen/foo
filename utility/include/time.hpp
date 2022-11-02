#pragma once

#include <sys/time.h>
#include <chrono>
#include <random>
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
constexpr uint32_t secToUsec = 1000000;

class Time final
{
public:
    virtual ~Time() = default;

    inline void setBeginTime();
    inline void setEndTime();
    [[nodiscard]] inline double getTimeInterval() const;
    void setBlockingTimer(auto func, const uint32_t delay);
    inline void resetBlockingTimer();

private:
    std::chrono::steady_clock::time_point beginTime;
    std::chrono::steady_clock::time_point endTime;
    std::atomic<bool> isBlockingTimerRunning{true};

protected:
    friend inline void millisecondLevelSleep(const uint32_t interval);
};

inline void millisecondLevelSleep(const uint32_t interval)
{
    std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::operator""ms(interval));
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

inline void Time::resetBlockingTimer()
{
    isBlockingTimerRunning = false;
}

extern std::string getCurrentSystemTime();
extern std::mt19937 getRandomSeedByTime();
} // namespace util_time
