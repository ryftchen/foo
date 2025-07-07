//! @file filter.cpp
//! @author ryftchen
//! @brief The definitions (filter) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "filter.hpp"

#include <cmath>
#include <cstring>

namespace date_structure::filter
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(readability-magic-numbers)
//! @brief The MurmurHash2 (64-bit) function.
//! @param key - key to hash
//! @param length - length of the key
//! @param seed - hash seed
//! @return hash value
static std::uint64_t murmurHash2X64(const void* const key, const int length, const std::uint32_t seed) noexcept
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
        reinterpret_cast<const unsigned char*>(data1); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
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

Bloom::Bloom(const std::uint32_t capacity, const double falsePositiveProb, const std::uint32_t hashSeed) :
    capacity{capacity},
    filterBitsNum{calculateParamM(capacity, falsePositiveProb)},
    hashFuncNum{calculateParamK(filterBitsNum, capacity)},
    hashSeed{hashSeed},
    filterSize{filterBitsNum / byteBits},
    filter{std::make_unique<std::uint8_t[]>(filterSize)},
    hashPos{std::make_unique<std::uint32_t[]>(hashFuncNum)}
{
    if ((capacity == 0) || (falsePositiveProb <= 0.0) || (falsePositiveProb >= 1.0))
    {
        throw std::runtime_error{"Invalid capacity or false positive probability."};
    }

    std::memset(filter.get(), 0, filterSize * sizeof(std::uint8_t));
}

bool Bloom::insert(const void* const key, const int length)
{
    if (length <= 0)
    {
        return false;
    }

    bloomHash(key, length);
    for (std::uint32_t i = 0; i < hashFuncNum; ++i)
    {
        setBit(filter.get(), hashPos[i]);
    }
    ++entries;

    return entries <= capacity;
}

bool Bloom::mayContain(const void* const key, const int length)
{
    if (!key || (length <= 0))
    {
        return false;
    }

    bloomHash(key, length);
    for (std::uint32_t i = 0; i < hashFuncNum; ++i)
    {
        if (getBit(filter.get(), hashPos[i]) == 0)
        {
            return false;
        }
    }

    return true;
}

void Bloom::clear()
{
    entries = 0;
    std::memset(filter.get(), 0, filterSize * sizeof(std::uint8_t));
}

void Bloom::bloomHash(const void* const key, const int length)
{
    const std::uint64_t hash1 = murmurHash2X64(key, length, hashSeed),
                        hash2 = murmurHash2X64(key, length, static_cast<std::uint32_t>((hash1 >> 32) ^ hash1));
    for (std::uint32_t i = 0; i < hashFuncNum; ++i)
    {
        hashPos[i] = (hash1 + i * hash2) % filterBitsNum;
    }
}

void Bloom::setBit(std::uint8_t filter[], const std::uint32_t pos)
{
    filter[pos / byteBits] |= 1 << (pos % byteBits);
}

std::uint8_t Bloom::getBit(const std::uint8_t filter[], const std::uint32_t pos)
{
    return filter[pos / byteBits] & (1 << (pos % byteBits));
}

std::uint32_t Bloom::calculateParamM(const std::uint32_t n, const double p)
{
    std::uint32_t m = std::ceil(-1.0 * n * std::log(p) / (std::numbers::ln2 * std::numbers::ln2));
    m = (m - m % 64) + 64;
    return m;
}

std::uint32_t Bloom::calculateParamK(const std::uint32_t m, const std::uint32_t n)
{
    const std::uint32_t k = std::round(std::numbers::ln2 * m / n);
    return k;
}

Quotient::Quotient(const std::uint8_t qBits, const std::uint8_t rBits, const std::uint32_t hashSeed) :
    qBits{qBits},
    rBits{rBits},
    elemBits{static_cast<std::uint8_t>(rBits + 3)},
    indexMask{lowMask(qBits)},
    rMask{lowMask(rBits)},
    elemMask{lowMask(elemBits)},
    capacity{static_cast<std::uint64_t>(1 << qBits)},
    hashSeed{hashSeed},
    filterSize{filterSizeInBytes(qBits, rBits)},
    filter{std::make_unique<std::uint64_t[]>(filterSize)}
{
    if ((qBits == 0) || (rBits == 0) || ((qBits + rBits) > 64))
    {
        throw std::runtime_error{"Invalid quotient or remainder bits."};
    }

    std::memset(filter.get(), 0, filterSize * sizeof(std::uint64_t));
}

bool Quotient::insert(const void* const key, const int length)
{
    return (key && (length > 0)) ? insert(quotientHash(key, length)) : false;
}

bool Quotient::mayContain(const void* const key, const int length)
{
    return (key && (length > 0)) ? mayContain(quotientHash(key, length)) : false;
}

bool Quotient::remove(const void* const key, const int length)
{
    return (key && (length > 0)) ? remove(quotientHash(key, length)) : false;
}

void Quotient::clear()
{
    entries = 0;
    std::memset(filter.get(), 0, filterSize * sizeof(std::uint64_t));
}

bool Quotient::insert(const std::uint64_t hash)
{
    if (entries >= capacity)
    {
        return false;
    }

    const std::uint64_t hq = hashToQuotient(hash), hr = hashToRemainder(hash), hqElem = getElement(*this, hq);
    std::uint64_t entry = (hr << 3) & ~7;
    if (isEmptyElement(hqElem))
    {
        setElement(*this, hq, setOccupied(entry));
        ++entries;
        return true;
    }
    if (!isOccupied(hqElem))
    {
        setElement(*this, hq, setOccupied(hqElem));
    }

    const std::uint64_t start = findRunIndex(hq);
    std::uint64_t slot = start;
    if (isOccupied(hqElem))
    {
        do
        {
            const std::uint64_t rem = getRemainder(getElement(*this, slot));
            if (rem == hr)
            {
                return true;
            }
            if (rem > hr)
            {
                break;
            }
            slot = increase(*this, slot);
        }
        while (isContinuation(getElement(*this, slot)));

        if (slot == start)
        {
            const std::uint64_t oldHead = getElement(*this, start);
            setElement(*this, start, setContinuation(oldHead));
        }
        else
        {
            entry = setContinuation(entry);
        }
    }

    if (slot != hq)
    {
        entry = setShifted(entry);
    }

    insertAt(slot, entry);
    ++entries;

    return true;
}

void Quotient::insertAt(const std::uint64_t start, const std::uint64_t elem)
{
    bool isEmpty = false;
    std::uint64_t slot = start, currElem = elem;
    do
    {
        std::uint64_t prevElem = getElement(*this, slot);
        isEmpty = isEmptyElement(prevElem);
        if (!isEmpty)
        {
            prevElem = setShifted(prevElem);
            if (isOccupied(prevElem))
            {
                currElem = setOccupied(currElem);
                prevElem = clearOccupied(prevElem);
            }
        }
        setElement(*this, slot, currElem);
        currElem = prevElem;
        slot = increase(*this, slot);
    }
    while (!isEmpty);
}

void Quotient::insertFrom(const Quotient& qf)
{
    Iterator iter{};
    start(qf, iter);
    while (!done(qf, iter))
    {
        insert(next(qf, iter));
    }
}

bool Quotient::isCompatibleWith(const Quotient& qf) const
{
    return (qf.qBits == qBits) && (qf.rBits == rBits) && (qf.hashSeed == hashSeed);
}

bool Quotient::mayContain(const std::uint64_t hash)
{
    const std::uint64_t hq = hashToQuotient(hash), hr = hashToRemainder(hash), hqElem = getElement(*this, hq);
    if (!isOccupied(hqElem))
    {
        return false;
    }

    std::uint64_t slot = findRunIndex(hq);
    do
    {
        const std::uint64_t rem = getRemainder(getElement(*this, slot));
        if (rem == hr)
        {
            return true;
        }
        if (rem > hr)
        {
            return false;
        }
        slot = increase(*this, slot);
    }
    while (isContinuation(getElement(*this, slot)));

    return false;
}

bool Quotient::remove(const std::uint64_t hash)
{
    const std::uint64_t highBits = hash >> (qBits + rBits);
    if (highBits)
    {
        return false;
    }

    const std::uint64_t hq = hashToQuotient(hash), hr = hashToRemainder(hash);
    std::uint64_t hqElem = getElement(*this, hq);
    if (!isOccupied(hqElem) || (entries == 0))
    {
        return true;
    }

    std::uint64_t slot = findRunIndex(hq), rem = 0;
    do
    {
        rem = getRemainder(getElement(*this, slot));
        if (rem >= hr)
        {
            break;
        }
        slot = increase(*this, slot);
    }
    while (isContinuation(getElement(*this, slot)));
    if (rem != hr)
    {
        return true;
    }

    const std::uint64_t entryToRemove = (slot == hq) ? hqElem : getElement(*this, slot);
    const bool toReplaceRunStart = isRunStart(entryToRemove);
    if (toReplaceRunStart)
    {
        const std::uint64_t nextElem = getElement(*this, increase(*this, slot));
        if (!isContinuation(nextElem))
        {
            hqElem = clearOccupied(hqElem);
            setElement(*this, hq, hqElem);
        }
    }

    removeAt(slot, hq);
    if (toReplaceRunStart)
    {
        const std::uint64_t nextElem = getElement(*this, slot);
        std::uint64_t updatedNextElem = isContinuation(nextElem) ? clearContinuation(nextElem) : nextElem;
        if ((slot == hq) && isRunStart(updatedNextElem))
        {
            updatedNextElem = clearShifted(updatedNextElem);
        }
        if (updatedNextElem != nextElem)
        {
            setElement(*this, slot, updatedNextElem);
        }
    }
    --entries;

    return true;
}

void Quotient::removeAt(const std::uint64_t start, const std::uint64_t quot)
{
    const std::uint64_t orig = start;
    std::uint64_t slot = start, currElem = getElement(*this, slot), scanPos = increase(*this, slot), quotient = quot;
    for (;;)
    {
        const std::uint64_t nextElem = getElement(*this, scanPos);
        if (isEmptyElement(nextElem) || isClusterStart(nextElem) || (scanPos == orig))
        {
            setElement(*this, slot, 0);
            return;
        }

        const bool isCurrOccupied = isOccupied(currElem);
        std::uint64_t updatedNextElem = nextElem;
        if (isRunStart(nextElem))
        {
            do
            {
                quotient = increase(*this, quotient);
            }
            while (!isOccupied(getElement(*this, quotient)));

            if (isCurrOccupied && (slot == quotient))
            {
                updatedNextElem = clearShifted(nextElem);
            }
        }

        setElement(*this, slot, isCurrOccupied ? setOccupied(updatedNextElem) : clearOccupied(updatedNextElem));
        slot = scanPos;
        scanPos = increase(*this, scanPos);
        currElem = nextElem;
    }
}

std::uint64_t Quotient::quotientHash(const void* const key, const int length) const
{
    return murmurHash2X64(key, length, hashSeed) & lowMask(qBits + rBits);
}

std::uint64_t Quotient::hashToQuotient(const std::uint64_t hash) const
{
    return (hash >> rBits) & indexMask;
}

std::uint64_t Quotient::hashToRemainder(const std::uint64_t hash) const
{
    return hash & rMask;
}

std::uint64_t Quotient::findRunIndex(const std::uint64_t quot) const
{
    std::uint64_t start = quot;
    while (isShifted(getElement(*this, start)))
    {
        start = decrease(*this, start);
    }

    std::uint64_t slot = start;
    while (start != quot)
    {
        do
        {
            slot = increase(*this, slot);
        }
        while (isContinuation(getElement(*this, slot)));

        do
        {
            start = increase(*this, start);
        }
        while (!isOccupied(getElement(*this, start)));
    }

    return slot;
}

void Quotient::start(const Quotient& qf, Iterator& iter)
{
    iter.visited = qf.entries;
    if (qf.entries == 0)
    {
        return;
    }

    std::uint64_t start = 0;
    for (start = 0; start < qf.capacity; ++start)
    {
        if (isClusterStart(getElement(qf, start)))
        {
            break;
        }
    }

    iter.visited = 0;
    iter.index = start;
}

bool Quotient::done(const Quotient& qf, const Iterator& iter)
{
    return qf.entries == iter.visited;
}

std::uint64_t Quotient::next(const Quotient& qf, Iterator& iter)
{
    while (!done(qf, iter))
    {
        const std::uint64_t entry = getElement(qf, iter.index);
        if (isClusterStart(entry))
        {
            iter.quotient = iter.index;
        }
        else if (isRunStart(entry))
        {
            std::uint64_t quotient = iter.quotient;
            do
            {
                quotient = increase(qf, quotient);
            }
            while (!isOccupied(getElement(qf, quotient)));
            iter.quotient = quotient;
        }

        iter.index = increase(qf, iter.index);
        if (!isEmptyElement(entry))
        {
            const std::uint64_t quotient = iter.quotient, rem = getRemainder(entry),
                                hash = (quotient << qf.rBits) | rem;
            ++iter.visited;
            return hash;
        }
    }

    return 0;
}

void Quotient::setElement(Quotient& qf, const std::uint64_t index, std::uint64_t elem)
{
    const std::size_t bitPos = qf.elemBits * index, slotPos = bitPos % 64;
    const int spillBits = (slotPos + qf.elemBits) - 64;
    std::size_t tabPos = bitPos / 64;

    elem &= qf.elemMask;
    qf.filter[tabPos] &= ~(qf.elemMask << slotPos);
    qf.filter[tabPos] |= elem << slotPos;
    if (spillBits > 0)
    {
        ++tabPos;
        qf.filter[tabPos] &= ~lowMask(spillBits);
        qf.filter[tabPos] |= elem >> (qf.elemBits - spillBits);
    }
}

std::uint64_t Quotient::getElement(const Quotient& qf, const std::uint64_t index)
{
    const std::size_t bitPos = qf.elemBits * index, slotPos = bitPos % 64;
    const int spillBits = (slotPos + qf.elemBits) - 64;
    std::size_t tabPos = bitPos / 64;
    std::uint64_t elem = (qf.filter[tabPos] >> slotPos) & qf.elemMask;

    if (spillBits > 0)
    {
        ++tabPos;
        const std::uint64_t x = qf.filter[tabPos] & lowMask(spillBits);
        elem |= x << (qf.elemBits - spillBits);
    }

    return elem;
}

std::uint64_t Quotient::increase(const Quotient& qf, const std::uint64_t index)
{
    return (index + 1) & qf.indexMask;
}

std::uint64_t Quotient::decrease(const Quotient& qf, const std::uint64_t index)
{
    return (index - 1) & qf.indexMask;
}

bool Quotient::isOccupied(const std::uint64_t elem)
{
    return (elem & 1) != 0;
}

std::uint64_t Quotient::setOccupied(const std::uint64_t elem)
{
    return elem | 1;
}

std::uint64_t Quotient::clearOccupied(const std::uint64_t elem)
{
    return elem & ~1;
}

bool Quotient::isContinuation(const std::uint64_t elem)
{
    return (elem & 2) != 0;
}

std::uint64_t Quotient::setContinuation(const std::uint64_t elem)
{
    return elem | 2;
}

std::uint64_t Quotient::clearContinuation(const std::uint64_t elem)
{
    return elem & ~2;
}

bool Quotient::isShifted(const std::uint64_t elem)
{
    return (elem & 4) != 0;
}

std::uint64_t Quotient::setShifted(const std::uint64_t elem)
{
    return elem | 4;
}

std::uint64_t Quotient::clearShifted(const std::uint64_t elem)
{
    return elem & ~4;
}

std::uint64_t Quotient::getRemainder(const std::uint64_t elem)
{
    return elem >> 3;
}

bool Quotient::isEmptyElement(const std::uint64_t elem)
{
    return (elem & 7) == 0;
}

bool Quotient::isClusterStart(const std::uint64_t elem)
{
    return isOccupied(elem) && !isContinuation(elem) && !isShifted(elem);
}

bool Quotient::isRunStart(const std::uint64_t elem)
{
    return !isContinuation(elem) && (isOccupied(elem) || isShifted(elem));
}

std::uint64_t Quotient::lowMask(const std::uint64_t n)
{
    return (1ULL << n) - 1ULL;
}

std::uint64_t Quotient::filterSizeInBytes(const std::uint8_t q, const std::uint8_t r)
{
    const std::uint64_t bits = static_cast<std::uint64_t>(1 << q) * (r + 3), bytes = bits / 8;
    return (bits % 8) ? (bytes + 1) : bytes;
}
// NOLINTEND(readability-magic-numbers)
} // namespace date_structure::filter
