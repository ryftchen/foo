//! @file match.cpp
//! @author ryftchen
//! @brief The definitions (match) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include "match.hpp"
#include <mpfr.h>
#include <cstring>
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

//! @brief Display match result.
#define MATCH_RESULT(opt) \
    "*%-16s method: Found the pattern \"%s\" starting(" #opt ") at index %d.  ==>Run time: %8.5f ms\n"
//! @brief Display none match result.
#define MATCH_NONE_RESULT "*%-16s method: Could not find the pattern \"%s\".  ==>Run time: %8.5f ms\n"
//! @brief Print match result content.
#define MATCH_PRINT_RESULT_CONTENT(method)                                                   \
    do                                                                                       \
    {                                                                                        \
        if (-1 != shift)                                                                     \
        {                                                                                    \
            COMMON_PRINT(MATCH_RESULT(1st), method, pattern, shift, MATCH_RUNTIME_INTERVAL); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
            COMMON_PRINT(MATCH_NONE_RESULT, method, pattern, MATCH_RUNTIME_INTERVAL);        \
        }                                                                                    \
    }                                                                                        \
    while (0)
//! @brief Store match beginning runtime.
#define MATCH_RUNTIME_BEGIN TIME_BEGIN(timing)
//! @brief Store match ending runtime.
#define MATCH_RUNTIME_END TIME_END(timing)
//! @brief Calculate match runtime interval.
#define MATCH_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else
// #define NDEBUG
#include <cassert>

//! @brief Print match result content.
#define MATCH_PRINT_RESULT_CONTENT(method)
//! @brief Store match beginning runtime.
#define MATCH_RUNTIME_BEGIN
//! @brief Store match ending runtime.
#define MATCH_RUNTIME_END
#endif

namespace algorithm::match
{
int MatchSolution::rkMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen)
{
    MATCH_RUNTIME_BEGIN;
    int shift = -1;
    constexpr uint32_t rollingHashBase = 10;
    constexpr uint32_t rollingHashMod = 19260817;
    long long textHash =
        rollingHash(std::string{text}.substr(0, patternLen).c_str(), patternLen, rollingHashBase, rollingHashMod);
    long long patternHash = rollingHash(pattern, patternLen, rollingHashBase, rollingHashMod);
    if (textHash != patternHash)
    {
        long long pow = 1;
        for (uint32_t j = 0; j < patternLen - 1; ++j)
        {
            pow = (pow * rollingHashBase) % rollingHashMod;
        }

        for (uint32_t i = 1; i <= textLen - patternLen; ++i)
        {
            textHash = (textHash - static_cast<long long>(text[i - 1]) * pow);
            textHash = (textHash % rollingHashMod + rollingHashMod) % rollingHashMod;
            textHash = (textHash * rollingHashBase + static_cast<int>(text[i + patternLen - 1])) % rollingHashMod;
            if (textHash == patternHash)
            {
                shift = i;
                break;
            }
        }
    }
    else
    {
        shift = 0;
    }

    MATCH_RUNTIME_END;
    MATCH_PRINT_RESULT_CONTENT("RabinKarp");
    return shift;
}

int MatchSolution::rollingHash(const char* str, const uint32_t length, const uint32_t hashBase, const uint32_t hashMod)
{
    int hash = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        hash = ((hash * hashBase) % hashMod + static_cast<int>(str[i])) % hashMod;
    }
    return hash;
}

int MatchSolution::kmpMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen)
{
    MATCH_RUNTIME_BEGIN;
    int shift = -1;
    uint32_t next[patternLen + 1];

    std::memset(next, 0, (patternLen + 1) * sizeof(uint32_t));
    for (uint32_t i = 1; i < patternLen; ++i)
    {
        uint32_t j = next[i + 1];
        while ((j > 0) && (pattern[j] != pattern[i]))
        {
            j = next[j];
        }

        if ((j > 0) || (pattern[j] == pattern[i]))
        {
            next[i + 1] = j + 1;
        }
    }

    for (uint32_t i = 0, j = 0; i < textLen; ++i)
    {
        if (*(text + i) == *(pattern + j))
        {
            if (++j == patternLen)
            {
                shift = i - j + 1;
                break;
            }
        }
        else if (j > 0)
        {
            j = next[j];
            --i;
        }
    }

    MATCH_RUNTIME_END;
    MATCH_PRINT_RESULT_CONTENT("KnuthMorrisPratt");
    return shift;
}

int MatchSolution::bmMethod(const char* text, const char* pattern, const uint32_t textLen, const uint32_t patternLen)
{
    MATCH_RUNTIME_BEGIN;
    int shift = -1;
    uint32_t badCharRuleTable[maxASCII], goodSuffixIndexTable[maxASCII];

    fillBadCharRuleTable(badCharRuleTable, pattern, patternLen);
    fillGoodSuffixRuleTable(goodSuffixIndexTable, pattern, patternLen);

    uint32_t textIndex = patternLen - 1;
    while (textIndex < textLen)
    {
        uint32_t patternIndex = patternLen - 1;
        while ((patternIndex > 0) && (text[textIndex] == pattern[patternIndex]))
        {
            --textIndex;
            --patternIndex;
        }
        if ((0 == patternIndex) && (text[textIndex] == pattern[patternIndex]))
        {
            shift = textIndex;
            break;
        }

        textIndex += std::max(badCharRuleTable[text[textIndex]], goodSuffixIndexTable[patternIndex]);
    }

    MATCH_RUNTIME_END;
    MATCH_PRINT_RESULT_CONTENT("BoyerMoore");
    return shift;
}

void MatchSolution::fillBadCharRuleTable(uint32_t badCharRuleTable[], const char* pattern, const uint32_t patternLen)
{
    for (uint32_t i = 0; i < maxASCII; ++i)
    {
        badCharRuleTable[i] = patternLen;
    }

    for (uint32_t j = 0; j < patternLen; ++j)
    {
        badCharRuleTable[pattern[j]] = patternLen - 1 - j;
    }
}

void MatchSolution::fillGoodSuffixRuleTable(
    uint32_t goodSuffixRuleTable[],
    const char* pattern,
    const uint32_t patternLen)
{
    uint32_t lastPrefixIndex = 1;
    for (int pos = (patternLen - 1); pos >= 0; --pos)
    {
        bool isPrefix = true;
        uint32_t suffixLen = patternLen - (pos + 1);
        for (uint32_t i = 0; i < suffixLen; ++i)
        {
            if (pattern[i] != pattern[pos + 1 + i])
            {
                isPrefix = false;
                break;
            }
        }
        if (isPrefix)
        {
            lastPrefixIndex = pos + 1;
        }

        goodSuffixRuleTable[pos] = lastPrefixIndex + (patternLen - 1 - pos);
    }

    for (uint32_t pos = 0; pos < (patternLen - 1); ++pos)
    {
        uint32_t suffixLen = 0;
        for (suffixLen = 0; (pattern[pos - suffixLen] == pattern[patternLen - 1 - suffixLen]) && (suffixLen <= pos);
             ++suffixLen)
        {
        }
        if (pattern[pos - suffixLen] != pattern[patternLen - 1 - suffixLen])
        {
            goodSuffixRuleTable[patternLen - 1 - suffixLen] = patternLen - 1 - pos + suffixLen;
        }
    }
}

int MatchSolution::horspoolMethod(
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen)
{
    MATCH_RUNTIME_BEGIN;
    int shift = -1;
    uint32_t badCharShiftTable[maxASCII];

    fillBadCharShiftTableForHorspool(badCharShiftTable, pattern, patternLen);

    uint32_t moveLen = patternLen - 1;
    while (moveLen <= (textLen - 1))
    {
        uint32_t matchLen = 0;
        while ((matchLen <= (patternLen - 1)) && (pattern[patternLen - 1 - matchLen] == text[moveLen - matchLen]))
        {
            ++matchLen;
        }

        if (matchLen == patternLen)
        {
            shift = moveLen - patternLen + 1;
            break;
        }

        moveLen += badCharShiftTable[text[moveLen]];
    }

    MATCH_RUNTIME_END;
    MATCH_PRINT_RESULT_CONTENT("Horspool");
    return shift;
}

void MatchSolution::fillBadCharShiftTableForHorspool(
    uint32_t badCharShiftTable[],
    const char* pattern,
    const uint32_t patternLen)
{
    for (uint32_t i = 0; i < maxASCII; ++i)
    {
        badCharShiftTable[i] = patternLen;
    }

    for (uint32_t j = 0; j < (patternLen - 1); ++j)
    {
        badCharShiftTable[pattern[j]] = patternLen - 1 - j;
    }
}

int MatchSolution::sundayMethod(
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen)
{
    MATCH_RUNTIME_BEGIN;
    int shift = -1;
    uint32_t badCharShiftTable[maxASCII];

    fillBadCharShiftTableForSunday(badCharShiftTable, pattern, patternLen);

    [&]
    {
        uint32_t textIndex = 0;
        while (textIndex <= (textLen - patternLen))
        {
            uint32_t matchLen = 0;
            while (text[textIndex + matchLen] == pattern[matchLen])
            {
                ++matchLen;
                if (matchLen == patternLen)
                {
                    shift = textIndex;
                    return;
                }
            }

            textIndex += badCharShiftTable[text[textIndex + patternLen]];
        }
    }();

    MATCH_RUNTIME_END;
    MATCH_PRINT_RESULT_CONTENT("Sunday");
    return shift;
}

void MatchSolution::fillBadCharShiftTableForSunday(
    uint32_t badCharShiftTable[],
    const char* pattern,
    const uint32_t patternLen)
{
    for (uint32_t i = 0; i < maxASCII; ++i)
    {
        badCharShiftTable[i] = patternLen + 1;
    }

    for (uint32_t j = 0; j < patternLen; ++j)
    {
        badCharShiftTable[pattern[j]] = patternLen - j;
    }
}

TargetBuilder::TargetBuilder(const uint32_t textLen, const std::string_view singlePattern) :
    marchingText(std::make_unique<char[]>(calculatePrecision(textLen))), singlePattern(singlePattern)
{
    setMatchingText(marchingText.get(), textLen);
}

TargetBuilder::~TargetBuilder()
{
    mpfr_free_cache();
}

void TargetBuilder::setMatchingText(char* text, const uint32_t textLen)
{
    assert((nullptr != text) && (textLen > 0));
    mpfr_t x;
    mpfr_init2(x, calculatePrecision(textLen));
    mpfr_const_pi(x, MPFR_RNDN);
    mpfr_exp_t mpfrDecimalLocation;
    mpfr_get_str(text, &mpfrDecimalLocation, mpfrBase, 0, x, MPFR_RNDN);
    mpfr_clear(x);

    assert('\0' != *text);
    text[textLen] = '\0';

#ifndef _NO_PRINT_AT_RUNTIME
    std::string out(text);
    out.insert(1, ".");
    std::cout << "\r\nπ " << textLen << " digits:\r\n"
              << out.substr(0, std::min(textLen, maxNumPerLineOfPrint)) << std::endl;
    if (textLen > maxNumPerLineOfPrint)
    {
        std::cout << "...\r\n...\r\n..." << std::endl;
        if (textLen > maxNumPerLineOfPrint)
        {
            std::cout
                << ((textLen > (maxNumPerLineOfPrint * 2))
                        ? out.substr(out.length() - maxNumPerLineOfPrint, out.length())
                        : out.substr(maxNumPerLineOfPrint + 1, out.length()))
                << std::endl;
        }
    }
    std::cout << std::endl;
#endif
}
} // namespace algorithm::match
