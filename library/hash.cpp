#include "../include/hash.hpp"

// BKDRHash
uint32_t bkdrHash(const char *str)
{
    const uint32_t seed = 131;    // 31 131 1313 13131 131313 etc...
    uint32_t hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}
