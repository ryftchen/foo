#pragma once

#include <iostream>

namespace util_hash
{
#define HASH_BKDR_SEED 131
#define HASH_BKDR_SIZE 0x7FFFFFFF
#define HASH_BKDR(str) util_hash::operator""_bkdrHash(str, std::size_t(0))

uint32_t bkdrHash(const char* str);

constexpr uint32_t bkdrHashInCompile(const char* const str, const uint32_t hash = 0) noexcept
{
    return *str ? bkdrHashInCompile(str + 1, (hash * HASH_BKDR_SEED + *str) & HASH_BKDR_SIZE)
                : hash;
}
constexpr uint64_t operator""_bkdrHash(const char* const str, const std::size_t /*unused*/) noexcept
{
    return bkdrHashInCompile(str);
}
} // namespace util_hash
