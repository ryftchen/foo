//! @file match.hpp
//! @author ryftchen
//! @brief The declarations (match) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <cmath>
#include <memory>
#include <mutex>

//! @brief Match-related functions in the algorithm module.
namespace algorithm::match
{
//! @brief Maximum number per line of printing.
constexpr uint32_t maxNumPerLineOfPrint = 50;
//! @brief Maximum digit for the target text.
constexpr uint32_t maxDigit = 100000;
//! @brief Maximum ASCII value.
constexpr uint32_t maxASCII = 256;
//! @brief Base number for converting the digit to precision.
constexpr int mpfrBase = 10;

//! @brief Solution of match.
class MatchSolution
{
public:
    //! @brief Construct a new MatchSolution object.
    //! @param textLen length of matching text
    //! @param pattern single pattern
    explicit MatchSolution(const uint32_t textLen, const std::string_view pattern);
    //! @brief Destroy the MatchSolution object.
    virtual ~MatchSolution();
    //! @brief Construct a new MatchSolution object.
    MatchSolution(const MatchSolution&) = delete;
    //! @brief The operator (=) overloading of MatchSolution class.
    //! @return reference of MatchSolution object
    MatchSolution& operator=(const MatchSolution&) = delete;

    //! @brief The Rabin-Karp method.
    //! @param text matching text
    //! @param pattern single pattern
    //! @param textLen length of matching text
    //! @param patternLen length of single pattern
    //! @return index in matching text
    int rkMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    //! @brief The Knuth-Morris-Pratt method.
    //! @param text matching text
    //! @param pattern single pattern
    //! @param textLen length of matching text
    //! @param patternLen length of single pattern
    //! @return index in matching text
    int kmpMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    //! @brief The Boyer-Moore method.
    //! @param text matching text
    //! @param pattern single pattern
    //! @param textLen length of matching text
    //! @param patternLen length of single pattern
    //! @return index in matching text
    int bmMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    //! @brief The Horspool method.
    //! @param text matching text
    //! @param pattern single pattern
    //! @param textLen length of matching text
    //! @param patternLen length of single pattern
    //! @return index in matching text
    int horspoolMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;
    //! @brief The Sunday method.
    //! @param text matching text
    //! @param pattern single pattern
    //! @param textLen length of matching text
    //! @param patternLen length of single pattern
    //! @return index in matching text
    int sundayMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen) const;

    //! @brief Get the matching text.
    //! @return matching text
    [[nodiscard]] inline const std::unique_ptr<char[]>& getMatchingText() const;
    //! @brief Get the single pattern.
    //! @return single pattern
    [[nodiscard]] inline std::string_view getSinglePattern() const;
    //! @brief Set the matching text.
    //! @param text matching text to be set
    //! @param textLen length of matching text
    static void setMatchingText(char* text, const uint32_t textLen);

private:
    //! @brief Matching text.
    const std::unique_ptr<char[]> marchingText;
    //! @brief Single pattern.
    const std::string_view singlePattern;

    //! @brief Calculate precision by digit.
    //! @param digit digit for the target text
    //! @return precision converted from digit
    static inline int calculatePrecision(const uint32_t digit);
    //! @brief Fill bad character rule table.
    //! @param badCharRuleTable bad character rule table
    //! @param pattern single pattern
    //! @param patternLen length of single pattern
    static void fillBadCharRuleTable(uint32_t badCharRuleTable[], const char* pattern, const uint32_t patternLen);
    //! @brief Fill good suffix rule table.
    //! @param goodSuffixRuleTable good suffix rule table
    //! @param pattern single pattern
    //! @param patternLen length of single pattern
    static void fillGoodSuffixRuleTable(uint32_t goodSuffixRuleTable[], const char* pattern, const uint32_t patternLen);
    //! @brief Fill the bad character shift table of the Horspool method.
    //! @param badCharShiftTable bad character shift table
    //! @param pattern single pattern
    //! @param patternLen length of single pattern
    static void fillBadCharShiftTableForHorspool(
        uint32_t badCharShiftTable[],
        const char* pattern,
        const uint32_t patternLen);
    //! @brief Fill the bad character shift table of the Sunday method.
    //! @param badCharShiftTable bad character shift table
    //! @param pattern single pattern
    //! @param patternLen length of single pattern
    static void fillBadCharShiftTableForSunday(
        uint32_t badCharShiftTable[],
        const char* pattern,
        const uint32_t patternLen);
};

inline const std::unique_ptr<char[]>& MatchSolution::getMatchingText() const
{
    return marchingText;
}

inline std::string_view MatchSolution::getSinglePattern() const
{
    return singlePattern;
}

inline int MatchSolution::calculatePrecision(const uint32_t digit)
{
    return static_cast<int>(std::ceil(static_cast<double>(digit) * std::log2(mpfrBase)));
}
} // namespace algorithm::match
