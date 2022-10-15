#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include "utility/include/time.hpp"

namespace algo_search
{
inline constexpr double arrayRange1 = -50.0;
inline constexpr double arrayRange2 = 150.0;
inline constexpr uint32_t arrayLength = 53;
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

template <class T>
class Search
{
public:
    Search(const uint32_t length, const T left, const T right);
    virtual ~Search() = default;
    Search(const Search& rhs);
    Search<T>& operator=(const Search& rhs);

    [[nodiscard]] int binarySearch(const T* const array, const uint32_t length, const T key) const;
    [[nodiscard]] int interpolationSearch(const T* const array, const uint32_t length, const T key) const;
    [[nodiscard]] int fibonacciSearch(const T* const array, const uint32_t length, const T key) const;

    const std::unique_ptr<T[]>& getOrderedArray() const;
    uint32_t getLength() const;
    T getSearchedKey() const;
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
    mutable std::mutex searchMutex;
    const std::unique_ptr<T[]> orderedArray;
    const uint32_t length;
    const T left;
    const T right;
    T searchedKey{0};

    void deepCopyFromSearch(const Search& search) const;
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
Search<T>::Search(const uint32_t length, const T left, const T right) :
    length(length), left(left), right(right), orderedArray(std::make_unique<T[]>(length))
{
    setOrderedArray<T>(orderedArray.get(), length, left, right);
    searchedKey = orderedArray[length / 2];
}

template <class T>
Search<T>::Search(const Search& rhs) :
    length(rhs.length), left(rhs.left), right(rhs.right), orderedArray(std::make_unique<T[]>(rhs.length))
{
    deepCopyFromSearch(rhs);
    searchedKey = orderedArray[length / 2];
}

template <class T>
Search<T>& Search<T>::operator=(const Search& rhs)
{
    deepCopyFromSearch(rhs);
    return *this;
}

template <class T>
void Search<T>::deepCopyFromSearch(const Search& search) const
{
    std::memcpy(this->orderedArray.get(), search.orderedArray.get(), this->length * sizeof(T));
}

template <class T>
const std::unique_ptr<T[]>& Search<T>::getOrderedArray() const
{
    std::unique_lock<std::mutex> lock(searchMutex);
    return orderedArray;
}

template <class T>
uint32_t Search<T>::getLength() const
{
    std::unique_lock<std::mutex> lock(searchMutex);
    return length;
}

template <class T>
T Search<T>::getSearchedKey() const
{
    std::unique_lock<std::mutex> lock(searchMutex);
    return searchedKey;
}

template <class T>
template <typename V>
requires std::is_integral<V>::value void Search<T>::setOrderedArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right) const
{
    std::mt19937 seed{util_time::getRandomSeedByTime()};
    std::uniform_int_distribution<int> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }
    std::sort(array, array + length);

    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " ordered integral numbers from " << left << " to " << right << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << "\r\n"
              << std::endl;
}

template <class T>
template <typename V>
requires std::is_floating_point<V>::value void Search<T>::setOrderedArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right) const
{
    std::mt19937 seed{util_time::getRandomSeedByTime()};
    std::uniform_real_distribution<double> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }
    std::sort(array, array + length);

    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " ordered floating point numbers from " << left << " to " << right
              << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << "\r\n"
              << std::endl;
}

template <class T>
template <typename V>
requires(isNumber<V>()) char* Search<T>::formatArray(
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
} // namespace algo_search
