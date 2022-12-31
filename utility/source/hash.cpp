//! @file hash.cpp
//! @author ryftchen
//! @brief The definitions (hash) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include "hash.hpp"

namespace utility::hash
{
//! @brief The Brian-Kernighan Dennis-Ritchie hash function.
//! @param str input data
//! @return hash value
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

//! @brief The rolling hash function.
//! @param str input data
//! @param length length of input data
//! @return hash value
int rollingHash(const char* str, const uint32_t length)
{
    int hash = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        hash = ((hash * rollingHashBase) % rollingHashMod + static_cast<int>(str[i])) % rollingHashMod;
    }
    return hash;
}
} // namespace utility::hash
