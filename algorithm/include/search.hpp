#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include "utility/include/time.hpp"

namespace algorithm::search
{
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

template <class T>
class SearchSolution
{
public:
    SearchSolution(const uint32_t length, const T left, const T right);
    virtual ~SearchSolution() = default;
    SearchSolution(const SearchSolution& rhs);
    SearchSolution<T>& operator=(const SearchSolution& rhs);

    int binaryMethod(const T* const array, const uint32_t length, const T key) const;
    int interpolationMethod(const T* const array, const uint32_t length, const T key) const;
    int fibonacciMethod(const T* const array, const uint32_t length, const T key) const;

    inline const std::unique_ptr<T[]>& getOrderedArray() const;
    [[nodiscard]] inline uint32_t getLength() const;
    inline T getSearchedKey() const;
    template <typename V>
    requires std::is_integral<V>::value void setOrderedArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;
    template <typename V>
    requires std::is_floating_point<V>::value void setOrderedArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;

private:
    const std::unique_ptr<T[]> orderedArray;
    const uint32_t length;
    T searchedKey{0};

    void deepCopy(const SearchSolution& rhs) const;
    static std::vector<uint32_t> generateFibonacciNumber(const uint32_t max);

protected:
    template <typename V>
    requires(isNumber<V>()) char* formatArray(
        const T* const array,
        const uint32_t length,
        char* const buffer,
        const uint32_t bufferSize) const;
};

template <class T>
SearchSolution<T>::SearchSolution(const uint32_t length, const T left, const T right) :
    length(length), orderedArray(std::make_unique<T[]>(length))
{
    setOrderedArray<T>(orderedArray.get(), length, left, right);
    searchedKey = orderedArray[length / 2];
}

template <class T>
SearchSolution<T>::SearchSolution(const SearchSolution& rhs) :
    length(rhs.length), orderedArray(std::make_unique<T[]>(rhs.length))
{
    deepCopy(rhs);
    searchedKey = orderedArray[length / 2];
}

template <class T>
SearchSolution<T>& SearchSolution<T>::operator=(const SearchSolution& rhs)
{
    deepCopy(rhs);
    return *this;
}

template <class T>
void SearchSolution<T>::deepCopy(const SearchSolution& rhs) const
{
    std::memcpy(this->orderedArray.get(), rhs.orderedArray.get(), this->length * sizeof(T));
}

template <class T>
inline const std::unique_ptr<T[]>& SearchSolution<T>::getOrderedArray() const
{
    return orderedArray;
}

template <class T>
inline uint32_t SearchSolution<T>::getLength() const
{
    return length;
}

template <class T>
inline T SearchSolution<T>::getSearchedKey() const
{
    return searchedKey;
}

template <class T>
template <typename V>
requires std::is_integral<V>::value void SearchSolution<T>::setOrderedArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right) const
{
    std::mt19937 seed{utility::time::getRandomSeedByTime()};
    std::uniform_int_distribution<int> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }
    std::sort(array, array + length);

#ifndef _NO_PRINT_AT_RUNTIME
    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " ordered integral numbers from " << left << " to " << right << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << "\r\n"
              << std::endl;
#endif
}

template <class T>
template <typename V>
requires std::is_floating_point<V>::value void SearchSolution<T>::setOrderedArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right) const
{
    std::mt19937 seed{utility::time::getRandomSeedByTime()};
    std::uniform_real_distribution<double> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }
    std::sort(array, array + length);

#ifndef _NO_PRINT_AT_RUNTIME
    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " ordered floating point numbers from " << left << " to " << right
              << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << "\r\n"
              << std::endl;
#endif
}

template <class T>
template <typename V>
requires(isNumber<V>()) char* SearchSolution<T>::formatArray(
    const T* const array,
    const uint32_t length,
    char* const buffer,
    const uint32_t bufferSize) const
{
    uint32_t align = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        align = std::max(static_cast<uint32_t>(std::to_string(*(array + i)).length()), align);
    }

    const char* format = " ";
    if constexpr (std::is_integral<T>::value)
    {
        format = "%*d ";
    }
    else if constexpr (std::is_floating_point<T>::value)
    {
        format = "%*.5f ";
    }

    int formatSize = 0;
    uint32_t completeSize = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, format, align + 1, *(array + i));
        if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
        {
            break;
        }
        completeSize += formatSize;

        if ((0 == (i + 1) % maxColumnOfPrint) && ((i + 1) != length))
        {
            formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
            if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
            {
                break;
            }
            completeSize += formatSize;
        }
    }

    return buffer;
}
} // namespace algorithm::search
