#pragma once
#include <chrono>
#include <string>
#include <sys/time.h>

std::string getCurrentSystemTime(char* const date);

#define TIME_DATE_LENGTH 32
#define TIME_DATE_YEAR_START 1900
#define TIME_BEGIN \
    Time time;     \
    time.setBeginTime()
#define TIME_END time.setEndTime()
#define TIME_INTERVAL time.getTimeInterval()
#define GET_TIME_SEED(seed)           \
    timeval timeSeed;                 \
    gettimeofday(&timeSeed, nullptr); \
    std::mt19937 seed(timeSeed.tv_sec * 1000000 + timeSeed.tv_usec)
#define GET_CURRENT_TIME                     \
    (                                        \
        {                                    \
            char date[TIME_DATE_LENGTH + 1]; \
            date[0] = '\0';                  \
            getCurrentSystemTime(date);      \
        })

class Time final
{
public:
    virtual ~Time() {}
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
