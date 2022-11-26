#pragma once

#include <cmath>
#include <memory>
#include <mutex>

namespace algo_match
{
constexpr uint32_t maxNumPerLineOfPrint = 50;
constexpr uint32_t maxDigit = 100000;
constexpr uint32_t maxASCII = 256;
constexpr int mpfrBase = 10;

class MatchSolution
{
public:
    explicit MatchSolution(const uint32_t length);
    virtual ~MatchSolution();
    MatchSolution(const MatchSolution& rhs) = delete;
    MatchSolution& operator=(const MatchSolution& rhs) = delete;

    int rkMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    int kmpMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    int bmMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    int horspoolMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    int sundayMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;

    [[nodiscard]] inline const std::unique_ptr<char[]>& getMatchingText() const;
    [[nodiscard]] inline uint32_t getLength() const;
    static void setMatchingText(char* text, const uint32_t length);

private:
    const std::unique_ptr<char[]> marchingText;
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

inline const std::unique_ptr<char[]>& MatchSolution::getMatchingText() const
{
    return marchingText;
}

inline uint32_t MatchSolution::getLength() const
{
    return length;
}

inline int MatchSolution::calculatePrecision(const uint32_t digit)
{
    return static_cast<int>(std::ceil(static_cast<double>(digit) * std::log2(mpfrBase)));
}
} // namespace algo_match
