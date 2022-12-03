#pragma once

#include <cstdint>

namespace utility::hash
{
constexpr uint32_t bkdrHashSeed = 131;
constexpr int bkdrHashSize = 0x7FFFFFFF;
constexpr uint32_t rollingHashBase = 10;
constexpr uint32_t rollingHashMod = 19260817;

constexpr uint32_t bkdrHashInCompile(const char* const str, const uint32_t hash = 0) noexcept
{
    return (*str ? bkdrHashInCompile(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashSize) : hash);
}

constexpr uint64_t operator""_bkdrHash(const char* const str, const std::size_t /*unused*/) noexcept
{
    return bkdrHashInCompile(str);
}

extern uint32_t bkdrHash(const char* str);
extern int rollingHash(const char* str, const uint32_t length);
} // namespace utility::hash
