#pragma once

#include <mutex>
#include "time.hpp"

namespace algo_match
{
constexpr uint32_t maxNumPerLineOfPrint = 50;
constexpr uint32_t maxDigit = 100000;
constexpr uint32_t maxASCII = 256;
constexpr int mpfrBase = 10;
constexpr std::string_view singlePattern{"12345"};

class Match
{
public:
    explicit Match(const uint32_t length);
    virtual ~Match();
    Match(const Match& rhs) = delete;
    Match& operator=(const Match& rhs) = delete;

    static int rkMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen);
    static int kmpMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen);
    static int bmMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen);
    static int horspoolMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen);
    static int sundayMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen);

    [[nodiscard]] const std::unique_ptr<char[]>& getSearchingText() const;
    [[nodiscard]] uint32_t getLength() const;
    static void setSearchingText(char* text, const uint32_t length);

private:
    mutable std::mutex matchMutex;
    const std::unique_ptr<char[]> searchingText;
    const uint32_t length;

    static int inline calculatePrecision(const uint32_t digit);
    static void fillBadCharRuleTable(uint32_t badCharRuleTable[], const char* pattern, const uint32_t patternLen);
    static void fillGoodSuffixRuleTable(uint32_t goodSuffixRuleTable[], const char* pattern, uint32_t const patternLen);
    static void fillBadCharShiftTableForHorspool(
        uint32_t badCharShiftTable[],
        const char* pattern,
        const uint32_t patternLen);
    static void fillBadCharShiftTableForSunday(
        uint32_t badCharShiftTable[],
        const char* pattern,
        const uint32_t patternLen);
};

int inline Match::calculatePrecision(const uint32_t digit)
{
    return static_cast<int>(std::ceil(static_cast<double>(digit) * std::log2(mpfrBase)));
}
} // namespace algo_match
