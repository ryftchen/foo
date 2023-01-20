//! @file hash.hpp
//! @author ryftchen
//! @brief The declarations (hash) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <cstdint>

//! @brief Hash-related functions in the utility module.
namespace utility::hash
{
//! @brief The hash seed for BKDR hash function.
constexpr uint32_t bkdrHashSeed = 131;
//! @brief The hash size for BKDR hash function.
constexpr int bkdrHashSize = 0x7FFFFFFF;
constexpr uint32_t rollingHashBase = 10;
constexpr uint32_t rollingHashMod = 19260817;

//! @brief The Brian-Kernighan Dennis-Ritchie hash function in compile time.
//! @param str input data
//! @param hash previous hash value
//! @return hash value
constexpr uint32_t bkdrHashInCompile(const char* const str, const uint32_t hash = 0) noexcept
{
    return (*str ? bkdrHashInCompile(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashSize) : hash);
}

//! @brief The operator ("") overloading with BKDR hash function.
//! @param str input data
//! @return hash value
constexpr uint64_t operator""_bkdrHash(const char* const str, const std::size_t /*unused*/) noexcept
{
    return bkdrHashInCompile(str);
}

extern uint32_t bkdrHash(const char* str);
extern int rollingHash(const char* str, const uint32_t length);
} // namespace utility::hash
