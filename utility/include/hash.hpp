#pragma once

#include <iostream>

namespace util_hash
{
constexpr uint32_t bkdrHashSeed = 131;
constexpr int bkdrHashSize = 0x7FFFFFFF;
constexpr unsigned rollingHashBase = 10;
constexpr unsigned rollingHashMod = 19260817;

constexpr uint32_t bkdrHashInCompile(const char* const str, const uint32_t hash = 0) noexcept
{
    return *str ? bkdrHashInCompile(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashSize) : hash;
}

constexpr uint64_t operator""_bkdrHash(const char* const str, const std::size_t /*unused*/) noexcept
{
    return bkdrHashInCompile(str);
}

uint32_t bkdrHash(const char* str);
int rollingHash(const char* str, const uint32_t length);
} // namespace util_hash
