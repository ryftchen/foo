//! @file filter.hpp
//! @author ryftchen
//! @brief The declarations (filter) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <memory>

//! @brief The data structure module.
namespace data_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Filter-related functions in the data structure module.
namespace filter
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "DS_FILTER";
}
extern const char* version() noexcept;

//! @brief The Bloom filter. A space-efficient probabilistic data structure.
class Bloom
{
public:
    //! @brief Construct a new Bloom object.
    //! @param capacity -  expected number of elements in the filter
    //! @param falsePositiveProb - desired false positive probability
    //! @param hashSeed - hash seed
    Bloom(const std::uint32_t capacity, const double falsePositiveProb, const std::uint32_t hashSeed);

    //! @brief Insert a key into the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    //! @return success or failure
    bool insert(const void* const key, const int length);
    //! @brief Check whether a key may be in the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    //! @return may contain or not
    bool mayContain(const void* const key, const int length);
    //! @brief Clear the filter.
    void clear();

private:
    //! @brief The capacity of the filter.
    const std::uint32_t capacity{0};
    //! @brief Number of bits.
    const std::uint32_t filterBitsNum{0};
    //! @brief Number of hash functions.
    const std::uint32_t hashFuncNum{0};
    //! @brief The hash seed.
    const std::uint32_t hashSeed{0};
    //! @brief Size of the filter in bytes.
    const std::uint32_t filterSize{0};
    //! @brief The filter data.
    const std::unique_ptr<std::uint8_t[]> filter;
    //! @brief The position of the hash values.
    const std::unique_ptr<std::uint32_t[]> hashPos;
    //! @brief Total number of entries.
    std::uint32_t entries{0};
    //! @brief Number of bits in a byte.
    static constexpr std::uint8_t byteBits{8};

    //! @brief Hash a key and calculate the positions in the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    void bloomHash(const void* const key, const int length);
    //! @brief Set a bit in the filter.
    //! @param filter - filter to set the bit in
    //! @param pos - hash position to set the bit
    static void setBit(std::uint8_t filter[], const std::uint32_t pos);
    //! @brief Get a bit from the filter.
    //! @param filter - filter to get the bit from
    //! @param pos - hash position to get the bit
    //! @return bit from the filter
    static std::uint8_t getBit(const std::uint8_t filter[], const std::uint32_t pos);
    //! @brief Calculate the parameter m.
    //! @param n - expected number of elements in the filter
    //! @param p - desired false positive probability
    //! @return number of bits in the filter
    static std::uint32_t calculateParamM(const std::uint32_t n, const double p);
    //! @brief Calculate the parameter k.
    //! @param m - number of bits in the filter
    //! @param n - expected number of elements in the filter
    //! @return number of hash functions to use
    static std::uint32_t calculateParamK(const std::uint32_t m, const std::uint32_t n);
};

//! @brief The quotient filter. A space-efficient probabilistic data structure.
class Quotient
{
public:
    //! @brief Construct a new Quotient object.
    //! @param qBits - number of quotient bits
    //! @param rBits - number of reminder bits
    //! @param hashSeed - hash seed
    Quotient(const std::uint8_t qBits, const std::uint8_t rBits, const std::uint32_t hashSeed);

    //! @brief Insert a key into the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    //! @return success or failure
    bool insert(const void* const key, const int length);
    //! @brief Check whether a key may be in the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    //! @return may contain or not
    bool mayContain(const void* const key, const int length);
    //! @brief Clear the filter.
    void clear();
    //! @brief Remove a key from the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    //! @return success or failure
    bool remove(const void* const key, const int length);
    //! @brief Merge other quotient filters into self.
    //! @tparam QFs - type of other input quotient filters
    //! @param qf - first input quotient filter
    //! @param others - other input quotient filters
    //! @return success or failure
    template <typename... QFs>
    bool merge(const Quotient& qf, const QFs&... others);

private:
    //! @brief Number of quotient bits.
    const std::uint8_t qBits{0};
    //! @brief Number of reminder bits.
    const std::uint8_t rBits{0};
    //! @brief Number of bits per element.
    const std::uint8_t elemBits{0};
    //! @brief Mask for extracting the index.
    const std::uint64_t indexMask{0};
    //! @brief Mask for extracting the reminder.
    const std::uint64_t rMask{0};
    //! @brief Mask for extracting the element.
    const std::uint64_t elemMask{0};
    //! @brief The capacity of the filter.
    const std::uint64_t capacity{0};
    //! @brief The hash seed.
    const std::uint32_t hashSeed{0};
    //! @brief Size of the filter in bytes.
    const std::uint64_t filterSize{0};
    //! @brief The filter data.
    const std::unique_ptr<std::uint64_t[]> filter;
    //! @brief Total number of entries.
    std::uint64_t entries{0};

    //! @brief The iterator for the filter.
    struct Iterator
    {
        //! @brief Current index in the filter.
        std::uint64_t index{0};
        //! @brief Current quotient value.
        std::uint64_t quotient{0};
        //! @brief Number of visited elements.
        std::uint64_t visited{0};
    };

    //! @brief Insert a hash value into the filter.
    //! @param hash - hash value
    //! @return success or failure
    bool insert(const std::uint64_t hash);
    //! @brief Insert an element into the filter at a given start index.
    //! @param start - start index to insert the element
    //! @param elem - element to insert
    void insertAt(const std::uint64_t start, const std::uint64_t elem);
    //! @brief Insert all the hash values from the other quotient filter.
    //! @param qf - other quotient filter
    void insertFrom(const Quotient& qf);
    //! @brief Check whether it is compatible with the other quotient filter.
    //! @param qf - other quotient filter
    //! @return be compatible or not
    [[nodiscard]] bool isCompatibleWith(const Quotient& qf) const;
    //! @brief Check whether a hash value may be in the filter.
    //! @param hash - hash value
    //! @return may contain or not
    bool mayContain(const std::uint64_t hash);
    //! @brief Remove a hash value from the filter.
    //! @param hash - hash value
    //! @return success or failure
    bool remove(const std::uint64_t hash);
    //! @brief Remove an element from the filter at a given start index.
    //! @param start - start index to remove the element
    //! @param quot - quotient involving the element to be removed
    void removeAt(const std::uint64_t start, const std::uint64_t quot);
    //! @brief Hash a key for the filter.
    //! @param key - key to hash
    //! @param length - length of the key
    //! @return hash value
    std::uint64_t quotientHash(const void* const key, const int length) const;
    //! @brief Convert the hash value into a quotient.
    //! @param hash - hash value
    //! @return quotient
    [[nodiscard]] std::uint64_t hashToQuotient(const std::uint64_t hash) const;
    //! @brief Convert the hash value into a reminder.
    //! @param hash - hash value
    //! @return reminder
    [[nodiscard]] std::uint64_t hashToRemainder(const std::uint64_t hash) const;
    //! @brief Find the start index of a run.
    //! @param quot - quotient converted from hash value
    //! @return start index of a run
    [[nodiscard]] std::uint64_t findRunIndex(const std::uint64_t quot) const;

    //! @brief Start iterating over the filter.
    //! @param qf - quotient filter
    //! @param iter - filter iterator
    static void start(const Quotient& qf, Iterator& iter);
    //! @brief Check whether the iterator has visited all entries.
    //! @param qf - quotient filter
    //! @param iter - filter iterator
    //! @return be done or not
    static bool done(const Quotient& qf, const Iterator& iter);
    //! @brief Get the next hash value from the filter.
    //! @param qf - quotient filter
    //! @param iter - filter iterator
    //! @return next hash value
    static std::uint64_t next(const Quotient& qf, Iterator& iter);
    //! @brief Set the element at a given index.
    //! @param qf - quotient filter
    //! @param index - index of the element
    //! @param elem - element to set
    static void setElement(Quotient& qf, const std::uint64_t index, std::uint64_t elem);
    //! @brief Get the element at a given index.
    //! @param qf - quotient filter
    //! @param index - index of the element
    //! @return element at the index
    static std::uint64_t getElement(const Quotient& qf, const std::uint64_t index);
    //! @brief Increase an index.
    //! @param qf - quotient filter
    //! @param index - index to increment
    //! @return increased index
    static std::uint64_t increase(const Quotient& qf, const std::uint64_t index);
    //! @brief Decrease an index.
    //! @param qf - quotient filter
    //! @param index - index to decrement
    //! @return decreased index
    static std::uint64_t decrease(const Quotient& qf, const std::uint64_t index);
    //! @brief Check whether an element is occupied.
    //! @param elem - element to check
    //! @return be occupied or not
    static bool isOccupied(const std::uint64_t elem);
    //! @brief Set an element as occupied.
    //! @param elem - element to set
    //! @return element with occupied status set
    static std::uint64_t setOccupied(const std::uint64_t elem);
    //! @brief Clear the occupied status of an element.
    //! @param elem - element to clear
    //! @return element with occupied status cleared
    static std::uint64_t clearOccupied(const std::uint64_t elem);
    //! @brief Check whether an element is a continuation.
    //! @param elem - element to check
    //! @return be a continuation or not
    static bool isContinuation(const std::uint64_t elem);
    //! @brief Set an element as a continuation.
    //! @param elem - element to set
    //! @return element with continuation status set
    static std::uint64_t setContinuation(const std::uint64_t elem);
    //! @brief Clear the continuation status of an element.
    //! @param elem - element to clear
    //! @return element with continuation status cleared
    static std::uint64_t clearContinuation(const std::uint64_t elem);
    //! @brief Check whether an element is shifted.
    //! @param elem - element to check
    //! @return be shifted or not
    static bool isShifted(const std::uint64_t elem);
    //! @brief Set an element as shifted.
    //! @param elem - element to set
    //! @return element with shifted status set
    static std::uint64_t setShifted(const std::uint64_t elem);
    //! @brief Clear the shifted status of an element.
    //! @param elem - element to clear
    //! @return element with shifted status cleared
    static std::uint64_t clearShifted(const std::uint64_t elem);
    //! @brief Get the remainder from an element.
    //! @param elem - element to get the remainder from
    //! @return remainder of the element
    static std::uint64_t getRemainder(const std::uint64_t elem);
    //! @brief Check whether an element is empty.
    //! @param elem - element to check
    //! @return be empty or not
    static bool isEmptyElement(const std::uint64_t elem);
    //! @brief Check whether an element is a cluster start.
    //! @param elem - element to check
    //! @return be a cluster start or not
    static bool isClusterStart(const std::uint64_t elem);
    //! @brief Check whether an element is a run start.
    //! @param elem - element to check
    //! @return be a run start or not
    static bool isRunStart(const std::uint64_t elem);
    //! @brief Get the low mask for a given number of bits.
    //! @param n - number of bits
    //! @return low mask
    static std::uint64_t lowMask(const std::uint64_t n);
    //! @brief Get the size of the filter in bytes.
    //! @param q - number of quotient bits
    //! @param r - number of reminder bits
    //! @return size of the filter in bytes
    static std::uint64_t filterSizeInBytes(const std::uint8_t q, const std::uint8_t r);
};

template <typename... QFs>
bool Quotient::merge(const Quotient& qf, const QFs&... others)
{
    if (!isCompatibleWith(qf) || !(isCompatibleWith(others) && ...))
    {
        return false;
    }

    insertFrom(qf);
    (insertFrom(others), ...);
    return true;
}
} // namespace filter
} // namespace data_structure
