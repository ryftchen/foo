#pragma once
#include <iostream>

uint32_t bkdrHash(const char *str);

constexpr uint32_t bkdrHashInCompile(const char *const str, const uint32_t hash = 0) noexcept
{
    return *str ? bkdrHashInCompile(str + 1, (hash * 131 + *str) & 0x7FFFFFFF) : hash;
}
constexpr uint64_t operator ""_bkdrHash(const char *const str, const size_t) noexcept
{
    return bkdrHashInCompile(str);
}
