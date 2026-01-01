//! @file match.hpp
//! @author ryftchen
//! @brief The declarations (match) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <array>
#include <cstdint>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Match-related functions in the algorithm module.
namespace match
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "ALGO_MATCH";
}
extern const char* version() noexcept;

//! @brief Match methods.
class Match
{
public:
    //! @brief Rabin-Karp.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static std::int64_t rk(
        const unsigned char* const text,
        const unsigned char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Knuth-Morris-Pratt.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static std::int64_t kmp(
        const unsigned char* const text,
        const unsigned char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Boyer-Moore.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static std::int64_t bm(
        const unsigned char* const text,
        const unsigned char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Horspool.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static std::int64_t horspool(
        const unsigned char* const text,
        const unsigned char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Sunday.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static std::int64_t sunday(
        const unsigned char* const text,
        const unsigned char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);

private:
    //! @brief Maximum ASCII value.
    static constexpr std::uint16_t maxASCII{256};

    //! @brief Fill bad character rule table.
    //! @param badCharRuleTable - bad character rule table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillBadCharRuleTable(
        std::array<std::uint32_t, maxASCII>& badCharRuleTable,
        const unsigned char* const pattern,
        const std::uint32_t patternLen);
    //! @brief Fill good suffix rule table.
    //! @param goodSuffixRuleTable - good suffix rule table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillGoodSuffixRuleTable(
        std::array<std::uint32_t, maxASCII>& goodSuffixRuleTable,
        const unsigned char* const pattern,
        const std::uint32_t patternLen);
    //! @brief Fill the bad character shift table of the Horspool method.
    //! @param badCharShiftTable - bad character shift table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillHorspoolBadCharShiftTable(
        std::array<std::uint32_t, maxASCII>& badCharShiftTable,
        const unsigned char* const pattern,
        const std::uint32_t patternLen);
    //! @brief Fill the bad character shift table of the Sunday method.
    //! @param badCharShiftTable - bad character shift table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillSundayBadCharShiftTable(
        std::array<std::uint32_t, maxASCII>& badCharShiftTable,
        const unsigned char* const pattern,
        const std::uint32_t patternLen);
};
} // namespace match
} // namespace algorithm
