//! @file match.cpp
//! @author ryftchen
//! @brief The definitions (match) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "match.hpp"
#include <algorithm>
#include <cstring>

namespace algorithm::match
{
int Match::rk(const char* text, const char* pattern, const std::uint32_t textLen, const std::uint32_t patternLen)
{
    int shift = -1;
    constexpr std::uint64_t rollingHashBase = 10;
    constexpr std::uint64_t rollingHashMod = 19260817;
    std::uint64_t textHash =
        rollingHash(std::string{text}.substr(0, patternLen).c_str(), patternLen, rollingHashBase, rollingHashMod);
    const std::uint64_t patternHash = rollingHash(pattern, patternLen, rollingHashBase, rollingHashMod);
    if (textHash != patternHash)
    {
        std::uint64_t pow = 1;
        for (std::uint32_t j = 0; j < patternLen - 1; ++j)
        {
            pow = (pow * rollingHashBase) % rollingHashMod;
        }

        for (std::uint32_t i = 1; i <= textLen - patternLen; ++i)
        {
            textHash = (textHash - static_cast<std::uint64_t>(text[i - 1]) * pow);
            textHash = (textHash % rollingHashMod + rollingHashMod) % rollingHashMod;
            textHash =
                (textHash * rollingHashBase + static_cast<std::uint64_t>(text[i + patternLen - 1])) % rollingHashMod;
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

    return shift;
}

std::uint64_t Match::rollingHash(
    const char* str,
    const std::uint64_t length,
    const std::uint64_t hashBase,
    const std::uint64_t hashMod)
{
    std::uint64_t hash = 0;
    for (std::uint64_t i = 0; i < length; ++i)
    {
        hash = ((hash * hashBase) % hashMod + static_cast<std::uint64_t>(str[i])) % hashMod;
    }
    return hash;
}

int Match::kmp(const char* text, const char* pattern, const std::uint32_t textLen, const std::uint32_t patternLen)
{
    int shift = -1;
    std::uint32_t next[patternLen + 1];

    std::memset(next, 0, (patternLen + 1) * sizeof(std::uint32_t));
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

int Match::bm(const char* text, const char* pattern, const std::uint32_t textLen, const std::uint32_t patternLen)
{
    int shift = -1;
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

void Match::fillBadCharRuleTable(std::uint32_t badCharRuleTable[], const char* pattern, const std::uint32_t patternLen)
{
    for (uint16_t i = 0; i < maxASCII; ++i)
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
    const char* pattern,
    const std::uint32_t patternLen)
{
    std::uint32_t lastPrefixIndex = 1;
    for (int pos = (patternLen - 1); pos >= 0; --pos)
    {
        bool isPrefix = true;
        std::uint32_t suffixLen = patternLen - (pos + 1);
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

int Match::horspool(const char* text, const char* pattern, const std::uint32_t textLen, const std::uint32_t patternLen)
{
    int shift = -1;
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
    const char* pattern,
    const std::uint32_t patternLen)
{
    for (uint16_t i = 0; i < maxASCII; ++i)
    {
        badCharShiftTable[i] = patternLen;
    }

    for (std::uint32_t j = 0; j < (patternLen - 1); ++j)
    {
        badCharShiftTable[pattern[j]] = patternLen - 1 - j;
    }
}

int Match::sunday(const char* text, const char* pattern, const std::uint32_t textLen, const std::uint32_t patternLen)
{
    int shift = -1;
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
                goto gotoReturn;
            }
        }

        textIndex += badCharShiftTable[text[textIndex + patternLen]];
    }

gotoReturn:
    return shift;
}

void Match::fillBadCharShiftTableForSunday(
    std::uint32_t badCharShiftTable[],
    const char* pattern,
    const std::uint32_t patternLen)
{
    for (uint16_t i = 0; i < maxASCII; ++i)
    {
        badCharShiftTable[i] = patternLen + 1;
    }

    for (std::uint32_t j = 0; j < patternLen; ++j)
    {
        badCharShiftTable[pattern[j]] = patternLen - j;
    }
}
} // namespace algorithm::match
