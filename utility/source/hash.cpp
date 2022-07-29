#include "hash.hpp"

// BKDR Hash
uint32_t bkdrHash(const char* str)
{
    const uint32_t seed = HASH_BKDR_SEED; // 31 131 1313 13131 131313 etc...
    uint32_t hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & HASH_BKDR_SIZE);
}
