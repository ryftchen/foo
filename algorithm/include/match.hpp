#pragma once

#include <cmath>
#include <memory>
#include <mutex>

namespace algo_match
{
inline constexpr std::string_view singlePattern{"12345"};
constexpr uint32_t maxNumPerLineOfPrint = 50;
constexpr uint32_t maxDigit = 100000;
constexpr uint32_t maxASCII = 256;
constexpr int mpfrBase = 10;

class Match
{
public:
    explicit Match(const uint32_t length);
    virtual ~Match();
    Match(const Match& rhs) = delete;
    Match& operator=(const Match& rhs) = delete;

    [[nodiscard]] static int rkMethod(
        const char* text,
        const char* pattern,
        const uint32_t textLen,
        const uint32_t patternLen);
    [[nodiscard]] static int kmpMethod(
        const char* text,
        const char* pattern,
        const uint32_t textLen,
        const uint32_t patternLen);
    [[nodiscard]] static int bmMethod(
        const char* text,
        const char* pattern,
        const uint32_t textLen,
        const uint32_t patternLen);
    [[nodiscard]] static int horspoolMethod(
        const char* text,
        const char* pattern,
        const uint32_t textLen,
        const uint32_t patternLen);
    [[nodiscard]] static int sundayMethod(
        const char* text,
        const char* pattern,
        const uint32_t textLen,
        const uint32_t patternLen);

    [[nodiscard]] const std::unique_ptr<char[]>& getSearchingText() const;
    [[nodiscard]] uint32_t getLength() const;
    static void setSearchingText(char* text, const uint32_t length);

private:
    mutable std::mutex matchMutex;
    const std::unique_ptr<char[]> searchingText;
    const uint32_t length;

    static inline int calculatePrecision(const uint32_t digit);
    static void fillBadCharRuleTable(uint32_t badCharRuleTable[], const char* pattern, const uint32_t patternLen);
    static void fillGoodSuffixRuleTable(uint32_t goodSuffixRuleTable[], const char* pattern, const uint32_t patternLen);
    static void fillBadCharShiftTableForHorspool(
        uint32_t badCharShiftTable[],
        const char* pattern,
        const uint32_t patternLen);
    static void fillBadCharShiftTableForSunday(
        uint32_t badCharShiftTable[],
        const char* pattern,
        const uint32_t patternLen);
};

inline int Match::calculatePrecision(const uint32_t digit)
{
    return static_cast<int>(std::ceil(static_cast<double>(digit) * std::log2(mpfrBase)));
}
} // namespace algo_match
