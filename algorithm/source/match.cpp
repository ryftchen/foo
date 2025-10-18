//! @file match.cpp
//! @author ryftchen
//! @brief The definitions (match) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "match.hpp"

#include <cstring>
#include <vector>

namespace algorithm::match
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
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
    if (!text || !pattern || (textLen == 0) || (patternLen == 0) || (textLen < patternLen))
    {
        return -1;
    }

    std::int64_t shift = -1;
    constexpr std::int64_t rollingHashBase = maxASCII;
    constexpr std::int64_t rollingHashMod = 1e9 + 7;
    std::int64_t textHash = 0;
    std::int64_t patternHash = 0;
    std::int64_t pow = 1;

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
        if ((patternHash == textHash) && (std::memcmp(text + i, pattern, patternLen) == 0))
        {
            shift = i;
            break;
        }

        if (i < (textLen - patternLen))
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
    if (!text || !pattern || (textLen == 0) || (patternLen == 0) || (textLen < patternLen))
    {
        return -1;
    }

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
    if (!text || !pattern || (textLen == 0) || (patternLen == 0) || (textLen < patternLen))
    {
        return -1;
    }

    std::int64_t shift = -1;
    std::uint32_t badCharRuleTable[maxASCII] = {};
    std::uint32_t goodSuffixIndexTable[maxASCII] = {};

    fillBadCharRuleTable(badCharRuleTable, pattern, patternLen);
    fillGoodSuffixRuleTable(goodSuffixIndexTable, pattern, patternLen);

    std::uint32_t textIdx = patternLen - 1;
    while (textIdx < textLen)
    {
        std::uint32_t patternIdx = patternLen - 1;
        while ((patternIdx > 0) && (text[textIdx] == pattern[patternIdx]))
        {
            --textIdx;
            --patternIdx;
        }
        if ((patternIdx == 0) && (text[textIdx] == pattern[patternIdx]))
        {
            shift = textIdx;
            break;
        }

        textIdx += std::max(badCharRuleTable[text[textIdx]], goodSuffixIndexTable[patternIdx]);
    }

    return shift;
}

void Match::fillBadCharRuleTable(
    std::uint32_t badCharRuleTable[], const unsigned char* const pattern, const std::uint32_t patternLen)
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
    std::uint32_t goodSuffixRuleTable[], const unsigned char* const pattern, const std::uint32_t patternLen)
{
    std::uint32_t lastPrefixIdx = 1;
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
            lastPrefixIdx = pos + 1;
        }

        goodSuffixRuleTable[pos] = lastPrefixIdx + (patternLen - 1 - pos);
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
    if (!text || !pattern || (textLen == 0) || (patternLen == 0) || (textLen < patternLen))
    {
        return -1;
    }

    std::int64_t shift = -1;
    std::uint32_t badCharShiftTable[maxASCII] = {};

    fillHorspoolBadCharShiftTable(badCharShiftTable, pattern, patternLen);

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

void Match::fillHorspoolBadCharShiftTable(
    std::uint32_t badCharShiftTable[], const unsigned char* const pattern, const std::uint32_t patternLen)
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
    if (!text || !pattern || (textLen == 0) || (patternLen == 0) || (textLen < patternLen))
    {
        return -1;
    }

    std::int64_t shift = -1;
    std::uint32_t badCharShiftTable[maxASCII] = {};

    fillSundayBadCharShiftTable(badCharShiftTable, pattern, patternLen);

    std::uint32_t textIdx = 0;
    while (textIdx <= (textLen - patternLen))
    {
        std::uint32_t matchLen = 0;
        while (text[textIdx + matchLen] == pattern[matchLen])
        {
            ++matchLen;
            if (matchLen == patternLen)
            {
                shift = textIdx;
                return shift;
            }
        }

        textIdx += badCharShiftTable[text[textIdx + patternLen]];
    }

    return shift;
}

void Match::fillSundayBadCharShiftTable(
    std::uint32_t badCharShiftTable[], const unsigned char* const pattern, const std::uint32_t patternLen)
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
