#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include "utility/include/time.hpp"

namespace algorithm::sort
{
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

template <class T>
class SortSolution
{
public:
    SortSolution(const uint32_t length, const T left, const T right);
    virtual ~SortSolution() = default;
    SortSolution(const SortSolution& rhs);
    SortSolution<T>& operator=(const SortSolution& rhs);

    std::vector<T> bubbleMethod(T* const array, const uint32_t length) const;
    std::vector<T> selectionMethod(T* const array, const uint32_t length) const;
    std::vector<T> insertionMethod(T* const array, const uint32_t length) const;
    std::vector<T> shellMethod(T* const array, const uint32_t length) const;
    std::vector<T> mergeMethod(T* const array, const uint32_t length) const;
    std::vector<T> quickMethod(T* const array, const uint32_t length) const;
    std::vector<T> heapMethod(T* const array, const uint32_t length) const;
    std::vector<T> countingMethod(T* const array, const uint32_t length) const;
    std::vector<T> bucketMethod(T* const array, const uint32_t length) const;
    std::vector<T> radixMethod(T* const array, const uint32_t length) const;

    inline const std::unique_ptr<T[]>& getRandomArray() const;
    [[nodiscard]] inline uint32_t getLength() const;
    template <typename V>
    requires std::is_integral<V>::value void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;
    template <typename V>
    requires std::is_floating_point<V>::value void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;

private:
    const std::unique_ptr<T[]> randomArray;
    const uint32_t length;

    void deepCopy(const SortSolution& rhs) const;
    static void mergeSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end);
    static void quickSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end);
    static void buildMaxHeap(T* const sortArray, const uint32_t begin, const uint32_t end);

protected:
    template <typename V>
    requires(isNumber<V>()) char* formatArray(
        const T* const array,
        const uint32_t length,
        char* const buffer,
        const uint32_t bufferSize) const;
};

template <class T>
SortSolution<T>::SortSolution(const uint32_t length, const T left, const T right) :
    length(length), randomArray(std::make_unique<T[]>(length))
{
    setRandomArray<T>(randomArray.get(), length, left, right);
}

template <class T>
SortSolution<T>::SortSolution(const SortSolution& rhs) :
    length(rhs.length), randomArray(std::make_unique<T[]>(rhs.length))
{
    deepCopy(rhs);
}

template <class T>
SortSolution<T>& SortSolution<T>::operator=(const SortSolution& rhs)
{
    deepCopy(rhs);
    return *this;
}

template <class T>
void SortSolution<T>::deepCopy(const SortSolution& rhs) const
{
    std::memcpy(this->randomArray.get(), rhs.randomArray.get(), this->length * sizeof(T));
}

template <class T>
inline const std::unique_ptr<T[]>& SortSolution<T>::getRandomArray() const
{
    return randomArray;
}

template <class T>
inline uint32_t SortSolution<T>::getLength() const
{
    return length;
}

template <class T>
template <typename V>
requires std::is_integral<V>::value void SortSolution<T>::setRandomArray(
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

#ifndef _NO_PRINT_AT_RUNTIME
    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " random integral numbers from " << left << " to " << right << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
#endif
}

template <class T>
template <typename V>
requires std::is_floating_point<V>::value void SortSolution<T>::setRandomArray(
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

#ifndef _NO_PRINT_AT_RUNTIME
    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " random floating point numbers from " << left << " to " << right
              << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
#endif
}

template <class T>
template <typename V>
requires(isNumber<V>()) char* SortSolution<T>::formatArray(
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
} // namespace algorithm::sort
