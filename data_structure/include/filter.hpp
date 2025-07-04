//! @file filter.hpp
//! @author ryftchen
//! @brief The declarations (filter) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>

//! @brief The data structure module.
namespace date_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Filter-related functions in the data structure module.
namespace filter
{
extern const char* version() noexcept;

// NOLINTBEGIN(readability-magic-numbers)
//! @brief The MurmurHash2 (64-bit) function.
//! @param key - key to hash
//! @param length - length of the key
//! @param seed - hash seed
//! @return hash value
inline std::uint64_t murmurHash2X64(const void* const key, const int length, const std::uint32_t seed) noexcept
{
    constexpr std::uint64_t mix = 0xc6a4a7935bd1e995;
    constexpr int shift = 47;
    const auto* data1 = static_cast<const std::uint64_t*>(key);
    const std::uint64_t* end = data1 + (length / 8);
    std::uint64_t hash = seed ^ (length * mix);
    while (data1 != end)
    {
        std::uint64_t block = *data1++;
        block *= mix;
        block ^= block >> shift;
        block *= mix;

        hash ^= block;
        hash *= mix;
    }

    const auto* data2 =
        reinterpret_cast<const std::uint8_t*>(data1); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    switch (length & 7)
    {
        case 7:
            hash ^= static_cast<std::uint64_t>(data2[6]) << 48;
            [[fallthrough]];
        case 6:
            hash ^= static_cast<std::uint64_t>(data2[5]) << 40;
            [[fallthrough]];
        case 5:
            hash ^= static_cast<std::uint64_t>(data2[4]) << 32;
            [[fallthrough]];
        case 4:
            hash ^= static_cast<std::uint64_t>(data2[3]) << 24;
            [[fallthrough]];
        case 3:
            hash ^= static_cast<std::uint64_t>(data2[2]) << 16;
            [[fallthrough]];
        case 2:
            hash ^= static_cast<std::uint64_t>(data2[1]) << 8;
            [[fallthrough]];
        case 1:
            hash ^= static_cast<std::uint64_t>(data2[0]);
            hash *= mix;
            [[fallthrough]];
        default:
            break;
    };

    hash ^= hash >> shift;
    hash *= mix;
    hash ^= hash >> shift;

    return hash;
}
// NOLINTEND(readability-magic-numbers)

//! @brief The Bloom filter structure.
namespace bloom
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 1)
    //! @brief The Bloom filter data.
    typedef struct TagFilter
    {
        //! @brief Whether the filter is initialized.
        bool isInitialized;
        //! @brief Padding of reserved bytes for alignment.
        std::uint8_t padding[3];
        //! @brief The capacity of the filter.
        std::uint32_t capacity;
        //! @brief The false positive probability of the filter.
        double falsePositiveProb;
        //! @brief Number of elements.
        std::uint32_t counter;
        //! @brief Number of bits.
        std::uint32_t filterBitNum;
        //! @brief Size of the filter in bytes.
        std::uint32_t filterSize;
        //! @brief Number of hash functions.
        std::uint32_t hashFuncNum;
        //! @brief The hash seed.
        std::uint32_t hashSeed;
        //! @brief The filter data.
        std::uint8_t* filter;
        //! @brief The position of the hash values.
        std::uint32_t* hashPos;
    } BloomFilter;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern bool init(
    BloomFilter* const bf, const std::uint32_t hashSeed, const std::uint32_t capacity, const double falsePositiveProb);
extern bool deinit(BloomFilter* const bf);
extern bool clear(BloomFilter* const bf);
extern bool insert(BloomFilter* const bf, const void* const key, const int length);
extern bool mayContain(BloomFilter* const bf, const void* const key, const int length);
} // namespace bloom

//! @brief The quotient filter structure.
namespace quotient
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 1)
    //! @brief The quotient filter data.
    typedef struct TagFilter
    {
        //! @brief Number of quotient bits.
        std::uint8_t qBits;
        //! @brief Number of reminder bits.
        std::uint8_t rBits;
        //! @brief Number of bits per element.
        std::uint8_t elemBits;
        //! @brief The capacity of the filter.
        std::uint8_t padding[1];
        //! @brief Total number of entries.
        std::uint32_t entries;
        //! @brief The hash seed.
        std::uint32_t hashSeed;
        //! @brief Mask for extracting the index.
        std::uint64_t indexMask;
        //! @brief Mask for extracting the reminder.
        std::uint64_t rMask;
        //! @brief Mask for extracting the element.
        std::uint64_t elemMask;
        //! @brief Padding of reserved bytes for alignment.
        std::uint64_t capacity;
        //! @brief Table of the filter data.
        std::uint64_t* table;
    } QuotientFilter;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern bool init(QuotientFilter* const qf, const std::uint32_t hashSeed, const std::uint32_t q, const std::uint32_t r);
extern bool deinit(QuotientFilter* const qf);
extern bool clear(QuotientFilter* const qf);
extern bool insert(QuotientFilter* const qf, const void* const key, const int length);
extern bool mayContain(const QuotientFilter* const qf, const void* const key, const int length);
extern bool remove(QuotientFilter* const qf, const void* const key, const int length);
extern bool merge(const QuotientFilter* const qfIn1, const QuotientFilter* const qfIn2, QuotientFilter* const qfOut);
} // namespace quotient
} // namespace filter
} // namespace date_structure
