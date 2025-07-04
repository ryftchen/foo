//! @file filter.cpp
//! @author ryftchen
//! @brief The definitions (filter) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "filter.hpp"

#include <cmath>
#include <cstring>
#include <numbers>

namespace date_structure::filter
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory, readability-magic-numbers)
namespace bloom
{
//! @brief Anonymous namespace.
inline namespace
{
//! @brief Number of bits in a byte.
constexpr std::uint8_t byteBits = 8;
} // namespace

//! @brief Calculate the parameters for the Bloom filter.
//! @param n - expected number of elements in the filter
//! @param p - desired false positive probability
//! @param m - number of bits in the filter
//! @param k - number of hash functions to use
static void calculateParameter(const std::uint32_t n, const double p, std::uint32_t& m, std::uint32_t& k)
{
    m = std::ceil(-1.0 * n * std::log(p) / (std::numbers::ln2 * std::numbers::ln2));
    m = (m - m % 64) + 64;
    k = std::round(std::numbers::ln2 * m / n);
}

//! @brief Initialize a Bloom filter.
//! @param bf - Bloom filter
//! @param hashSeed - hash seed
//! @param capacity -  expected number of elements in the filter
//! @param falsePositiveProb - desired false positive probability
//! @return success or failure
bool init(
    BloomFilter* const bf, const std::uint32_t hashSeed, const std::uint32_t capacity, const double falsePositiveProb)
{
    if (!bf || (falsePositiveProb <= 0.0) || (falsePositiveProb >= 1.0))
    {
        return false;
    }

    ::delete[] bf->filter;
    ::delete[] bf->hashPos;
    std::memset(bf, 0, sizeof(BloomFilter));

    bf->capacity = capacity;
    bf->falsePositiveProb = falsePositiveProb;
    bf->hashSeed = hashSeed;
    calculateParameter(bf->capacity, bf->falsePositiveProb, bf->filterBitNum, bf->hashFuncNum);
    bf->filterSize = bf->filterBitNum / byteBits;

    bf->filter = ::new (std::nothrow) std::uint8_t[bf->filterSize];
    if (!bf->filter)
    {
        return false;
    }
    bf->hashPos = ::new (std::nothrow) std::uint32_t[bf->hashFuncNum * sizeof(std::uint32_t)];
    if (!bf->hashPos)
    {
        return false;
    }

    std::memset(bf->filter, 0, bf->filterSize);
    bf->isInitialized = true;

    return true;
}

//! @brief Deinitialize a Bloom filter.
//! @param bf - Bloom filter
//! @return success or failure
bool deinit(BloomFilter* const bf)
{
    if (!bf)
    {
        return false;
    }

    bf->isInitialized = false;
    bf->counter = 0;

    ::delete[] bf->filter;
    ::delete[] bf->hashPos;

    return true;
}

//! @brief Clear a Bloom filter.
//! @param bf - Bloom filter
//! @return success or failure
bool clear(BloomFilter* const bf)
{
    if (!bf)
    {
        return false;
    }

    std::memset(bf->filter, 0, bf->filterSize);
    bf->isInitialized = true;
    bf->counter = 0;

    return true;
}

//! @brief Hash a key and calculate the positions in the Bloom filter.
//! @param bf - Bloom filter
//! @param key - key to hash
//! @param length - length of the key
static void bloomHash(BloomFilter* const bf, const void* const key, const int length)
{
    const std::uint64_t hash1 = murmurHash2X64(key, length, bf->hashSeed),
                        hash2 = murmurHash2X64(key, length, static_cast<std::uint32_t>((hash1 >> 32) ^ hash1));
    for (std::uint32_t i = 0; i < bf->hashFuncNum; ++i)
    {
        bf->hashPos[i] = (hash1 + i * hash2) % bf->filterBitNum;
    }
}

//! @brief Set a bit in the filter.
//! @param filter - filter to set the bit in
//! @param hashPos - hash position to set the bit
static void setBit(std::uint8_t* const filter, const std::uint32_t hashPos)
{
    filter[hashPos / byteBits] |= 1 << (hashPos % byteBits);
}

//! @brief Get a bit from the filter.
//! @param filter - filter to get the bit from
//! @param hashPos - hash position to get the bit
//! @return bit from the filter
static std::uint8_t getBit(const std::uint8_t* const filter, const std::uint32_t hashPos)
{
    return filter[hashPos / byteBits] & (1 << (hashPos % byteBits));
}

//! @brief Insert a key into the Bloom filter.
//! @param bf - Bloom filter
//! @param key - key to hash
//! @param length - length of the key
//! @return success or failure
bool insert(BloomFilter* const bf, const void* const key, const int length)
{
    if (!bf || !key || (length <= 0))
    {
        return false;
    }

    if (!bf->isInitialized)
    {
        std::memset(bf->filter, 0, bf->filterSize);
        bf->isInitialized = true;
    }

    bloomHash(bf, key, length);
    for (std::uint32_t i = 0; i < bf->hashFuncNum; ++i)
    {
        setBit(bf->filter, bf->hashPos[i]);
    }
    ++bf->counter;

    return bf->counter <= bf->capacity;
}

//! @brief Check whether a key may be in the Bloom filter.
//! @param bf - Bloom filter
//! @param key - key to hash
//! @param length - length of the key
//! @return may contain or not
bool mayContain(BloomFilter* const bf, const void* const key, const int length)
{
    if (!bf || !key || (length <= 0))
    {
        return false;
    }

    bloomHash(bf, key, length);
    for (std::uint32_t i = 0; i < bf->hashFuncNum; ++i)
    {
        if (getBit(bf->filter, bf->hashPos[i]) == 0)
        {
            return false;
        }
    }

    return true;
}
} // namespace bloom

namespace quotient
{
//! @brief Anonymous namespace.
inline namespace
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 1)
    //! @brief The iterator for the quotient filter.
    typedef struct TagIterator
    {
        //! @brief Current index in the filter.
        std::uint64_t index;
        //! @brief Current quotient value.
        std::uint64_t quotient;
        //! @brief Number of visited elements.
        std::uint64_t visited;
    } FilterIterator;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus
} // namespace

//! @brief Get the low mask for a given number of bits.
//! @param n - number of bits
//! @return low mask
static std::uint64_t lowMask(const std::uint64_t n)
{
    return (1ULL << n) - 1ULL;
}

//! @brief Calculate the size of the quotient filter table.
//! @param q - number of quotient bits
//! @param r - number of reminder bits
//! @return size of the table in bytes
static std::size_t tableSize(const std::uint32_t q, const std::uint32_t r)
{
    const std::size_t bits = static_cast<std::size_t>(1 << q) * (r + 3), bytes = bits / 8;
    return (bits % 8) ? (bytes + 1) : bytes;
}

//! @brief Initialize a quotient filter.
//! @param qf - quotient filter
//! @param hashSeed - hash seed
//! @param q - number of quotient bits
//! @param r - number of reminder bits
//! @return success or failure
bool init(QuotientFilter* const qf, const std::uint32_t hashSeed, const std::uint32_t q, const std::uint32_t r)
{
    if (!qf || (q == 0) || (r == 0) || ((q + r) > 64))
    {
        return false;
    }

    qf->qBits = q;
    qf->rBits = r;
    qf->elemBits = qf->rBits + 3;
    qf->entries = 0;
    qf->hashSeed = hashSeed;
    qf->indexMask = lowMask(q);
    qf->rMask = lowMask(r);
    qf->elemMask = lowMask(qf->elemBits);
    qf->capacity = 1 << q;
    qf->table = ::new (std::nothrow) std::uint64_t[tableSize(q, r)]();

    return qf->table != nullptr;
}

//! @brief Deinitialize a quotient filter.
//! @param qf - quotient filter
//! @return success or failure
bool deinit(QuotientFilter* const qf)
{
    if (!qf)
    {
        return false;
    }

    ::delete[] qf->table;

    return true;
}

//! @brief Clear a quotient filter.
//! @param qf - quotient filter
//! @return success or failure
bool clear(QuotientFilter* const qf)
{
    if (!qf)
    {
        return false;
    }

    qf->entries = 0;
    std::memset(qf->table, 0, tableSize(qf->qBits, qf->rBits));

    return true;
}

//! @brief Get the element at a given index.
//! @param qf - quotient filter
//! @param index - index of the element
//! @return element at the index
static std::uint64_t getElement(const QuotientFilter* const qf, const std::uint64_t index)
{
    const std::size_t bitPos = qf->elemBits * index, slotPos = bitPos % 64;
    const int spillBits = (slotPos + qf->elemBits) - 64;
    std::size_t tabPos = bitPos / 64;
    std::uint64_t elem = (qf->table[tabPos] >> slotPos) & qf->elemMask;

    if (spillBits > 0)
    {
        ++tabPos;
        const std::uint64_t x = qf->table[tabPos] & lowMask(spillBits);
        elem |= x << (qf->elemBits - spillBits);
    }

    return elem;
}

//! @brief Set the element at a given index.
//! @param qf - quotient filter
//! @param index - index of the element
//! @param elem - element to set
static void setElement(QuotientFilter* const qf, const std::uint64_t index, std::uint64_t elem)
{
    const std::size_t bitPos = qf->elemBits * index, slotPos = bitPos % 64;
    const int spillBits = (slotPos + qf->elemBits) - 64;
    std::size_t tabPos = bitPos / 64;

    elem &= qf->elemMask;
    qf->table[tabPos] &= ~(qf->elemMask << slotPos);
    qf->table[tabPos] |= elem << slotPos;
    if (spillBits > 0)
    {
        ++tabPos;
        qf->table[tabPos] &= ~lowMask(spillBits);
        qf->table[tabPos] |= elem >> (qf->elemBits - spillBits);
    }
}

//! @brief Increase an index.
//! @param qf - quotient filter
//! @param index - index to increment
//! @return increased index
static std::uint64_t increase(const QuotientFilter* const qf, const std::uint64_t index)
{
    return (index + 1) & qf->indexMask;
}

//! @brief Decrease an index.
//! @param qf - quotient filter
//! @param index - index to decrement
//! @return decreased index
static std::uint64_t decrease(const QuotientFilter* const qf, const std::uint64_t index)
{
    return (index - 1) & qf->indexMask;
}

//! @brief Check whether an element is occupied.
//! @param elem - element to check
//! @return be occupied or not
static bool isOccupied(const std::uint64_t elem)
{
    return (elem & 1) != 0;
}

//! @brief Set an element as occupied.
//! @param elem - element to set
//! @return element with occupied status set
static std::uint64_t setOccupied(const std::uint64_t elem)
{
    return elem | 1;
}

//! @brief Clear the occupied status of an element.
//! @param elem - element to clear
//! @return element with occupied status cleared
static std::uint64_t clearOccupied(const std::uint64_t elem)
{
    return elem & ~1;
}

//! @brief Check whether an element is a continuation.
//! @param elem - element to check
//! @return be a continuation or not
static bool isContinuation(const std::uint64_t elem)
{
    return (elem & 2) != 0;
}

//! @brief Set an element as a continuation.
//! @param elem - element to set
//! @return element with continuation status set
static std::uint64_t setContinuation(const std::uint64_t elem)
{
    return elem | 2;
}

//! @brief Clear the continuation status of an element.
//! @param elem - element to clear
//! @return element with continuation status cleared
static std::uint64_t clearContinuation(const std::uint64_t elem)
{
    return elem & ~2;
}

//! @brief Check whether an element is shifted.
//! @param elem - element to check
//! @return be shifted or not
static bool isShifted(const std::uint64_t elem)
{
    return (elem & 4) != 0;
}

//! @brief Set an element as shifted.
//! @param elem - element to set
//! @return element with shifted status set
static std::uint64_t setShifted(const std::uint64_t elem)
{
    return elem | 4;
}

//! @brief Clear the shifted status of an element.
//! @param elem - element to clear
//! @return element with shifted status cleared
static std::uint64_t clearShifted(const std::uint64_t elem)
{
    return elem & ~4;
}

//! @brief Get the remainder from an element.
//! @param elem - element to get the remainder from
//! @return remainder of the element
static std::uint64_t getRemainder(const std::uint64_t elem)
{
    return elem >> 3;
}

//! @brief Check whether an element is empty.
//! @param elem - element to check
//! @return be empty or not
static bool isEmptyElement(const std::uint64_t elem)
{
    return (elem & 7) == 0;
}

//! @brief Check whether an element is a cluster start.
//! @param elem - element to check
//! @return be a cluster start or not
static bool isClusterStart(const std::uint64_t elem)
{
    return isOccupied(elem) && !isContinuation(elem) && !isShifted(elem);
}

//! @brief Check whether an element is a run start.
//! @param elem - element to check
//! @return be a run start or not
static bool isRunStart(const std::uint64_t elem)
{
    return !isContinuation(elem) && (isOccupied(elem) || isShifted(elem));
}

//! @brief Convert the hash value into a quotient.
//! @param qf - quotient filter
//! @param hash - hash value
//! @return quotient
static std::uint64_t hashToQuotient(const QuotientFilter* const qf, const std::uint64_t hash)
{
    return (hash >> qf->rBits) & qf->indexMask;
}

//! @brief Convert the hash value into a reminder.
//! @param qf - quotient filter
//! @param hash - hash value
//! @return reminder
static std::uint64_t hashToRemainder(const QuotientFilter* const qf, const std::uint64_t hash)
{
    return hash & qf->rMask;
}

//! @brief Find the start index of a run.
//! @param qf - quotient filter
//! @param fq - quotient converted from hash value
//! @return start index of a run
static std::uint64_t findRunIndex(const QuotientFilter* const qf, const std::uint64_t fq)
{
    std::uint64_t b = fq;
    while (isShifted(getElement(qf, b)))
    {
        b = decrease(qf, b);
    }

    std::uint64_t s = b;
    while (b != fq)
    {
        do
        {
            s = increase(qf, s);
        }
        while (isContinuation(getElement(qf, s)));

        do
        {
            b = increase(qf, b);
        }
        while (!isOccupied(getElement(qf, b)));
    }

    return s;
}

//! @brief Insert an element into the quotient filter at a given start index.
//! @param qf - quotient filter
//! @param start - start index to insert the element
//! @param elem - element to insert
static void insertInto(QuotientFilter* const qf, std::uint64_t start, const std::uint64_t elem)
{
    bool empty = false;
    std::uint64_t curr = elem;

    do
    {
        std::uint64_t prev = getElement(qf, start);
        empty = isEmptyElement(prev);
        if (!empty)
        {
            prev = setShifted(prev);
            if (isOccupied(prev))
            {
                curr = setOccupied(curr);
                prev = clearOccupied(prev);
            }
        }
        setElement(qf, start, curr);
        curr = prev;
        start = increase(qf, start);
    }
    while (!empty);
}

//! @brief Hash a key for the quotient filter.
//! @param qf - quotient filter
//! @param key - key to hash
//! @param length - length of the key
//! @return hash value
static std::uint64_t quotientHash(const QuotientFilter* const qf, const void* const key, const int length)
{
    return murmurHash2X64(key, length, qf->hashSeed) & lowMask(qf->qBits + qf->rBits);
}

//! @brief Insert a hash value into the quotient filter.
//! @param qf - quotient filter
//! @param hash - hash value
//! @return success or failure
static bool insert(QuotientFilter* const qf, const std::uint64_t hash)
{
    if (qf->entries >= qf->capacity)
    {
        return false;
    }

    const std::uint64_t fq = hashToQuotient(qf, hash), fr = hashToRemainder(qf, hash), fqElem = getElement(qf, fq);
    std::uint64_t entry = (fr << 3) & ~7;
    if (isEmptyElement(fqElem))
    {
        setElement(qf, fq, setOccupied(entry));
        ++qf->entries;
        return true;
    }

    if (!isOccupied(fqElem))
    {
        setElement(qf, fq, setOccupied(fqElem));
    }

    const std::uint64_t start = findRunIndex(qf, fq);
    std::uint64_t s = start;
    if (isOccupied(fqElem))
    {
        do
        {
            const std::uint64_t rem = getRemainder(getElement(qf, s));
            if (rem == fr)
            {
                return true;
            }
            if (rem > fr)
            {
                break;
            }
            s = increase(qf, s);
        }
        while (isContinuation(getElement(qf, s)));

        if (s == start)
        {
            const std::uint64_t oldHead = getElement(qf, start);
            setElement(qf, start, setContinuation(oldHead));
        }
        else
        {
            entry = setContinuation(entry);
        }
    }

    if (s != fq)
    {
        entry = setShifted(entry);
    }

    insertInto(qf, s, entry);
    ++qf->entries;

    return true;
}

//! @brief Insert a key into the quotient filter.
//! @param qf - quotient filter
//! @param key - key to hash
//! @param length - length of the key
//! @return success or failure
bool insert(QuotientFilter* const qf, const void* const key, const int length)
{
    return (qf && key && (length > 0)) ? insert(qf, quotientHash(qf, key, length)) : false;
}

//! @brief Check whether a hash value may be in the quotient filter.
//! @param qf - quotient filter
//! @param hash - hash value
//! @return may contain or not
static bool mayContain(const QuotientFilter* const qf, const std::uint64_t hash)
{
    const std::uint64_t fq = hashToQuotient(qf, hash), fr = hashToRemainder(qf, hash), fqElem = getElement(qf, fq);
    if (!isOccupied(fqElem))
    {
        return false;
    }

    std::uint64_t s = findRunIndex(qf, fq);
    do
    {
        const std::uint64_t rem = getRemainder(getElement(qf, s));
        if (rem == fr)
        {
            return true;
        }
        if (rem > fr)
        {
            return false;
        }
        s = increase(qf, s);
    }
    while (isContinuation(getElement(qf, s)));

    return false;
}

//! @brief Check whether a key may be in the quotient filter.
//! @param qf - quotient filter
//! @param key - key to hash
//! @param length - length of the key
//! @return may contain or not
bool mayContain(const QuotientFilter* const qf, const void* const key, const int length)
{
    return (qf && key && (length > 0)) ? mayContain(qf, quotientHash(qf, key, length)) : false;
}

//! @brief Delete an entry.
//! @param qf - quotient filter
//! @param start - start index of the entry to delete
//! @param quotient - quotient of the entry to delete
static void deleteEntry(QuotientFilter* const qf, std::uint64_t start, std::uint64_t quotient)
{
    const std::uint64_t orig = start;
    std::uint64_t curr = getElement(qf, start), sp = increase(qf, start);

    for (;;)
    {
        const std::uint64_t next = getElement(qf, sp);
        if (isEmptyElement(next) || isClusterStart(next) || sp == orig)
        {
            setElement(qf, start, 0);
            return;
        }

        const bool currOccupied = isOccupied(curr);
        std::uint64_t updatedNext = next;
        if (isRunStart(next))
        {
            do
            {
                quotient = increase(qf, quotient);
            }
            while (!isOccupied(getElement(qf, quotient)));

            if (currOccupied && (quotient == start))
            {
                updatedNext = clearShifted(next);
            }
        }

        setElement(qf, start, currOccupied ? setOccupied(updatedNext) : clearOccupied(updatedNext));
        start = sp;
        sp = increase(qf, sp);
        curr = next;
    }
}

//! @brief Remove a hash value from the quotient filter.
//! @param qf - quotient filter
//! @param hash - hash value
//! @return success or failure
static bool remove(QuotientFilter* const qf, const std::uint64_t hash)
{
    const std::uint64_t highBits = hash >> (qf->qBits + qf->rBits);
    if (highBits)
    {
        return false;
    }

    const std::uint64_t fq = hashToQuotient(qf, hash), fr = hashToRemainder(qf, hash);
    std::uint64_t fqElem = getElement(qf, fq);
    if (!isOccupied(fqElem) || !qf->entries)
    {
        return true;
    }

    std::uint64_t s = findRunIndex(qf, fq), rem = 0;
    do
    {
        rem = getRemainder(getElement(qf, s));
        if (rem >= fr)
        {
            break;
        }
        s = increase(qf, s);
    }
    while (isContinuation(getElement(qf, s)));
    if (rem != fr)
    {
        return true;
    }

    const std::uint64_t kill = (s == fq) ? fqElem : getElement(qf, s);
    const bool replaceRunStart = isRunStart(kill);
    if (replaceRunStart)
    {
        const std::uint64_t next = getElement(qf, increase(qf, s));
        if (!isContinuation(next))
        {
            fqElem = clearOccupied(fqElem);
            setElement(qf, fq, fqElem);
        }
    }

    deleteEntry(qf, s, fq);
    if (replaceRunStart)
    {
        const std::uint64_t next = getElement(qf, s);
        std::uint64_t updatedNext = isContinuation(next) ? clearContinuation(next) : next;
        if ((s == fq) && isRunStart(updatedNext))
        {
            updatedNext = clearShifted(updatedNext);
        }
        if (updatedNext != next)
        {
            setElement(qf, s, updatedNext);
        }
    }
    --qf->entries;

    return true;
}

//! @brief Remove a key from the quotient filter.
//! @param qf - quotient filter
//! @param key - key to hash
//! @param length - length of the key
//! @return success or failure
bool remove(QuotientFilter* const qf, const void* const key, const int length)
{
    return (qf && key && (length > 0)) ? remove(qf, quotientHash(qf, key, length)) : false;
}

//! @brief Start iterating over the quotient filter.
//! @param qf - quotient filter
//! @param iter - filter iterator
static void start(const QuotientFilter* const qf, FilterIterator* const iter)
{
    iter->visited = qf->entries;
    if (qf->entries == 0)
    {
        return;
    }

    std::uint64_t start = 0;
    for (start = 0; start < qf->capacity; ++start)
    {
        if (isClusterStart(getElement(qf, start)))
        {
            break;
        }
    }

    iter->visited = 0;
    iter->index = start;
}

//! @brief Check whether the iterator has visited all entries.
//! @param qf - quotient filter
//! @param iter - filter iterator
//! @return be done or not
static bool done(const QuotientFilter* const qf, const FilterIterator* const iter)
{
    return qf->entries == iter->visited;
}

//! @brief Get the next hash value from the quotient filter.
//! @param qf - quotient filter
//! @param iter - filter iterator
//! @return next hash value
static std::uint64_t next(const QuotientFilter* const qf, FilterIterator* const iter)
{
    while (!done(qf, iter))
    {
        const std::uint64_t elem = getElement(qf, iter->index);
        if (isClusterStart(elem))
        {
            iter->quotient = iter->index;
        }
        else if (isRunStart(elem))
        {
            std::uint64_t quot = iter->quotient;
            do
            {
                quot = increase(qf, quot);
            }
            while (!isOccupied(getElement(qf, quot)));
            iter->quotient = quot;
        }

        iter->index = increase(qf, iter->index);

        if (!isEmptyElement(elem))
        {
            const std::uint64_t quot = iter->quotient, rem = getRemainder(elem), hash = (quot << qf->rBits) | rem;
            ++iter->visited;
            return hash;
        }
    }

    return 0;
}

//! @brief Merge two quotient filters into a new one.
//! @param qfIn1 - first input quotient filter
//! @param qfIn2 - second input quotient filter
//! @param qfOut - output quotient filter
//! @return success or failure
bool merge(const QuotientFilter* const qfIn1, const QuotientFilter* const qfIn2, QuotientFilter* const qfOut)
{
    if (!qfIn1 || !qfIn2 || !qfOut)
    {
        return false;
    }
    if ((qfIn1->hashSeed != qfIn2->hashSeed) || (qfIn1->qBits != qfIn2->qBits) || (qfIn1->rBits != qfIn2->rBits))
    {
        return false;
    }

    const std::uint32_t hashSeed = qfIn1->hashSeed, q = qfIn1->qBits, r = qfIn1->rBits;
    if (!init(qfOut, hashSeed, q, r))
    {
        return false;
    }

    FilterIterator iter{};
    start(qfIn1, &iter);
    while (!done(qfIn1, &iter))
    {
        insert(qfOut, next(qfIn1, &iter));
    }
    start(qfIn2, &iter);
    while (!done(qfIn2, &iter))
    {
        insert(qfOut, next(qfIn2, &iter));
    }

    return true;
}
} // namespace quotient
// NOLINTEND(cppcoreguidelines-owning-memory, readability-magic-numbers)
} // namespace date_structure::filter
