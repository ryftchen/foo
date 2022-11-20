#include "match.hpp"
#include <mpfr.h>
#include <cstring>
#include "utility/include/common.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/time.hpp"

#define MATCH_RESULT(opt) \
    "*%-16s method: Found the pattern \"%s\" starting(" #opt ") at index %d.  ==>Run time: %8.5f ms\n"
#define MATCH_NONE_RESULT "*%-16s method: Could not find the pattern \"%s\".  ==>Run time: %8.5f ms\n"
#define MATCH_PRINT_RESULT_CONTENT(method)                                                  \
    do                                                                                      \
    {                                                                                       \
        if (-1 != shift)                                                                    \
        {                                                                                   \
            COMMON_PRINT(MATCH_RESULT(1st), method, pattern, shift, TIME_INTERVAL(timing)); \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            COMMON_PRINT(MATCH_NONE_RESULT, method, pattern, TIME_INTERVAL(timing));        \
        }                                                                                   \
    }                                                                                       \
    while (0)

namespace algo_match
{
MatchSolution::MatchSolution(const uint32_t length) :
    length(length), marchingText(std::make_unique<char[]>(calculatePrecision(length)))
{
    setMatchingText(marchingText.get(), length);
}

MatchSolution::~MatchSolution()
{
    mpfr_free_cache();
}

const std::unique_ptr<char[]>& MatchSolution::getMatchingText() const
{
    std::unique_lock<std::mutex> lock(matchMutex);
    return marchingText;
}

uint32_t MatchSolution::getLength() const
{
    std::unique_lock<std::mutex> lock(matchMutex);
    return length;
}

void MatchSolution::setMatchingText(char* text, const uint32_t length)
{
    assert((nullptr != text) && (length > 0));
    mpfr_t x;
    mpfr_init2(x, calculatePrecision(length));
    mpfr_const_pi(x, MPFR_RNDN);
    mpfr_exp_t mpfrDecimalLocation;
    mpfr_get_str(text, &mpfrDecimalLocation, mpfrBase, 0, x, MPFR_RNDN);
    mpfr_clear(x);

    assert('\0' != *text);
    text[length] = '\0';

    std::string out(text);
    out.insert(1, ".");
    std::cout << "\r\nπ " << length << " digits:\r\n"
              << out.substr(0, std::min(length, maxNumPerLineOfPrint)) << std::endl;
    if (length > maxNumPerLineOfPrint)
    {
        std::cout << "...\r\n...\r\n..." << std::endl;
        if (length > maxNumPerLineOfPrint)
        {
            std::cout
                << ((length > (maxNumPerLineOfPrint * 2))
                        ? out.substr(out.length() - maxNumPerLineOfPrint, out.length())
                        : out.substr(maxNumPerLineOfPrint + 1, out.length()))
                << std::endl;
        }
    }
    std::cout << std::endl;
}

// Rabin-Karp
int MatchSolution::rkMethod( // NOLINT(readability-convert-member-functions-to-static)
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen) const
{
    TIME_BEGIN(timing);
    int shift = -1;
    long long textHash = util_hash::rollingHash(std::string{text}.substr(0, patternLen).c_str(), patternLen);
    long long patternHash = util_hash::rollingHash(pattern, patternLen);
    if (textHash != patternHash)
    {
        long long pow = 1;
        for (uint32_t j = 0; j < patternLen - 1; ++j)
        {
            pow = (pow * util_hash::rollingHashBase) % util_hash::rollingHashMod;
        }

        for (uint32_t i = 1; i <= textLen - patternLen; ++i)
        {
            textHash = (textHash - static_cast<long long>(text[i - 1]) * pow);
            textHash = (textHash % util_hash::rollingHashMod + util_hash::rollingHashMod) % util_hash::rollingHashMod;
            textHash = (textHash * util_hash::rollingHashBase + static_cast<int>(text[i + patternLen - 1]))
                % util_hash::rollingHashMod;
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

    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT("RabinKarp");
    return shift;
}

// Knuth-Morris-Pratt
int MatchSolution::kmpMethod( // NOLINT(readability-convert-member-functions-to-static)
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT("KnuthMorrisPratt");
    return shift;
}

// Boyer-Moore
int MatchSolution::bmMethod( // NOLINT(readability-convert-member-functions-to-static)
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
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

// Horspool
int MatchSolution::horspoolMethod( // NOLINT(readability-convert-member-functions-to-static)
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
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

// Sunday
int MatchSolution::sundayMethod( // NOLINT(readability-convert-member-functions-to-static)
    const char* text,
    const char* pattern,
    const uint32_t textLen,
    const uint32_t patternLen) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
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
} // namespace algo_match
