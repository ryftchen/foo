//! @file match.cpp
//! @author ryftchen
//! @brief The definitions (match) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "match.hpp"

#include <vector>

namespace algorithm::match
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version()
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::int64_t Match::rk(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
    std::int64_t shift = -1;
    constexpr std::int64_t rollingHashBase = maxASCII, rollingHashMod = 1e9 + 7;
    std::int64_t textHash = 0, patternHash = 0, pow = 1;

    for (std::uint32_t i = 0; i < (patternLen - 1); ++i)
    {
        pow = (pow * rollingHashBase) % rollingHashMod;
    }

    for (std::uint32_t i = 0; i < patternLen; ++i)
    {
        patternHash = (rollingHashBase * patternHash + pattern[i]) % rollingHashMod;
        textHash = (rollingHashBase * textHash + text[i]) % rollingHashMod;
    }

    for (std::uint32_t i = 0; i <= (textLen - patternLen); ++i)
    {
        if (patternHash == textHash)
        {
            std::uint32_t j = 0;
            for (j = 0; j < patternLen; ++j)
            {
                if (text[i + j] != pattern[j])
                {
                    break;
                }
            }

            if (j == patternLen)
            {
                shift = i;
                break;
            }
        }

        if (i < textLen - patternLen)
        {
            textHash = (rollingHashBase * (textHash - text[i] * pow) + text[i + patternLen]) % rollingHashMod;
            if (textHash < 0)
            {
                textHash += rollingHashMod;
            }
        }
    }

    return shift;
}

std::int64_t Match::kmp(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
    std::int64_t shift = -1;
    std::vector<std::uint32_t> next(patternLen + 1, 0);

    for (std::uint32_t i = 1; i < patternLen; ++i)
    {
        std::uint32_t j = next[i + 1];
        while ((j > 0) && (pattern[j] != pattern[i]))
        {
            j = next[j];
        }

        if ((j > 0) || (pattern[j] == pattern[i]))
        {
            next[i + 1] = j + 1;
        }
    }

    for (std::uint32_t i = 0, j = 0; i < textLen; ++i)
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

    return shift;
}

std::int64_t Match::bm(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
    std::int64_t shift = -1;
    std::uint32_t badCharRuleTable[maxASCII], goodSuffixIndexTable[maxASCII];

    fillBadCharRuleTable(badCharRuleTable, pattern, patternLen);
    fillGoodSuffixRuleTable(goodSuffixIndexTable, pattern, patternLen);

    std::uint32_t textIndex = patternLen - 1;
    while (textIndex < textLen)
    {
        std::uint32_t patternIndex = patternLen - 1;
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

    return shift;
}

void Match::fillBadCharRuleTable(
    std::uint32_t badCharRuleTable[],
    const unsigned char* const pattern,
    const std::uint32_t patternLen)
{
    for (std::uint16_t i = 0; i < maxASCII; ++i)
    {
        badCharRuleTable[i] = patternLen;
    }

    for (std::uint32_t j = 0; j < patternLen; ++j)
    {
        badCharRuleTable[pattern[j]] = patternLen - 1 - j;
    }
}

void Match::fillGoodSuffixRuleTable(
    std::uint32_t goodSuffixRuleTable[],
    const unsigned char* const pattern,
    const std::uint32_t patternLen)
{
    std::uint32_t lastPrefixIndex = 1;
    for (std::int64_t pos = (patternLen - 1); pos >= 0; --pos)
    {
        const std::uint32_t suffixLen = patternLen - (pos + 1);
        bool isPrefix = true;
        for (std::uint32_t i = 0; i < suffixLen; ++i)
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

    for (std::uint32_t pos = 0; pos < (patternLen - 1); ++pos)
    {
        std::uint32_t suffixLen = 0;
        while ((suffixLen <= pos) && (suffixLen <= (patternLen - 1))
               && (pattern[pos - suffixLen] == pattern[patternLen - 1 - suffixLen]))
        {
            ++suffixLen;
        }
        if ((suffixLen <= pos) && (suffixLen <= (patternLen - 1))
            && (pattern[pos - suffixLen] != pattern[patternLen - 1 - suffixLen]))
        {
            goodSuffixRuleTable[patternLen - 1 - suffixLen] = patternLen - 1 - pos + suffixLen;
        }
    }
}

std::int64_t Match::horspool(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
    std::int64_t shift = -1;
    std::uint32_t badCharShiftTable[maxASCII];

    fillBadCharShiftTableForHorspool(badCharShiftTable, pattern, patternLen);

    std::uint32_t moveLen = patternLen - 1;
    while (moveLen <= (textLen - 1))
    {
        std::uint32_t matchLen = 0;
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

    return shift;
}

void Match::fillBadCharShiftTableForHorspool(
    std::uint32_t badCharShiftTable[],
    const unsigned char* const pattern,
    const std::uint32_t patternLen)
{
    for (std::uint16_t i = 0; i < maxASCII; ++i)
    {
        badCharShiftTable[i] = patternLen;
    }

    for (std::uint32_t j = 0; j < (patternLen - 1); ++j)
    {
        badCharShiftTable[pattern[j]] = patternLen - 1 - j;
    }
}

std::int64_t Match::sunday(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
    std::int64_t shift = -1;
    std::uint32_t badCharShiftTable[maxASCII];

    fillBadCharShiftTableForSunday(badCharShiftTable, pattern, patternLen);

    std::uint32_t textIndex = 0;
    while (textIndex <= (textLen - patternLen))
    {
        std::uint32_t matchLen = 0;
        while (text[textIndex + matchLen] == pattern[matchLen])
        {
            ++matchLen;
            if (matchLen == patternLen)
            {
                shift = textIndex;
                return shift;
            }
        }

        textIndex += badCharShiftTable[text[textIndex + patternLen]];
    }

    return shift;
}

void Match::fillBadCharShiftTableForSunday(
    std::uint32_t badCharShiftTable[],
    const unsigned char* const pattern,
    const std::uint32_t patternLen)
{
    for (std::uint16_t i = 0; i < maxASCII; ++i)
    {
        badCharShiftTable[i] = patternLen + 1;
    }

    for (std::uint32_t j = 0; j < patternLen; ++j)
    {
        badCharShiftTable[pattern[j]] = patternLen - j;
    }
}
} // namespace algorithm::match
