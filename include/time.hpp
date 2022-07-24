#pragma once
#include <sys/time.h>
#include <chrono>
#include <string>
#include "file.hpp"

std::string getCurrentSystemTime(char* const date);

#define TIME_DATE_LENGTH BUFFER_SIZE_32
#define TIME_DATE_YEAR_START 1900
#define TIME_BEGIN \
    Time time;     \
    time.setBeginTime()
#define TIME_END time.setEndTime()
#define TIME_INTERVAL time.getTimeInterval()
#define TIME_GET_SEED(seed)           \
    timeval timeSeed{};               \
    gettimeofday(&timeSeed, nullptr); \
    std::mt19937 seed(timeSeed.tv_sec * 1000000 + timeSeed.tv_usec)
#define TIME_GET_CURRENT_DATE                         \
    (                                                 \
        {                                             \
            char date[TIME_DATE_LENGTH + 1] = {'\0'}; \
            getCurrentSystemTime(date);               \
        })

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
