#include "hash.hpp"

namespace util_hash
{
// BKDR Hash
uint32_t bkdrHash(const char* str)
{
    const uint32_t seed = bkdrHashSeed; // 31 131 1313 13131 131313 etc...
    uint32_t hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & bkdrHashSize);
}

int rollingHash(const char* str, const uint32_t length)
{
    int hash = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        hash = ((hash * rollingHashBase) % rollingHashMod + static_cast<int>(str[i])) % rollingHashMod;
    }
    return hash;
}
} // namespace util_hash
