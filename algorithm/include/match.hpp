//! @file match.hpp
//! @author ryftchen
//! @brief The declarations (match) in the algorithm module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <cstdint>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Match-related functions in the algorithm module.
namespace match
{
//! @brief Match methods.
class Match
{
public:
    //! @brief Destroy the Match object.
    virtual ~Match() = default;

    //! @brief Rabin-Karp.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static int rk(
        const char* const text,
        const char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Knuth-Morris-Pratt.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static int kmp(
        const char* const text,
        const char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Boyer-Moore.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static int bm(
        const char* const text,
        const char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Horspool.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static int horspool(
        const char* const text,
        const char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);
    //! @brief Sunday.
    //! @param text - matching text
    //! @param pattern - single pattern
    //! @param textLen - length of matching text
    //! @param patternLen - length of single pattern
    //! @return index in matching text
    static int sunday(
        const char* const text,
        const char* const pattern,
        const std::uint32_t textLen,
        const std::uint32_t patternLen);

private:
    //! @brief Maximum ASCII value.
    static constexpr std::uint16_t maxASCII = 256;

    //! @brief The rolling hash function.
    //! @param str - input data
    //! @param length - length of input data
    //! @param hashBase - hash base
    //! @param hashMod - hash mod
    //! @return hash value
    static std::uint64_t rollingHash(
        const char* const str,
        const std::uint64_t length,
        const std::uint64_t hashBase,
        const std::uint64_t hashMod);
    //! @brief Fill bad character rule table.
    //! @param badCharRuleTable - bad character rule table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillBadCharRuleTable(
        std::uint32_t badCharRuleTable[],
        const char* const pattern,
        const std::uint32_t patternLen);
    //! @brief Fill good suffix rule table.
    //! @param goodSuffixRuleTable - good suffix rule table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillGoodSuffixRuleTable(
        std::uint32_t goodSuffixRuleTable[],
        const char* const pattern,
        const std::uint32_t patternLen);
    //! @brief Fill the bad character shift table of the Horspool method.
    //! @param badCharShiftTable - bad character shift table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillBadCharShiftTableForHorspool(
        std::uint32_t badCharShiftTable[],
        const char* const pattern,
        const std::uint32_t patternLen);
    //! @brief Fill the bad character shift table of the Sunday method.
    //! @param badCharShiftTable - bad character shift table
    //! @param pattern - single pattern
    //! @param patternLen - length of single pattern
    static void fillBadCharShiftTableForSunday(
        std::uint32_t badCharShiftTable[],
        const char* const pattern,
        const std::uint32_t patternLen);
};
} // namespace match
} // namespace algorithm
