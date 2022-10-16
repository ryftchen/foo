#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include "utility/include/time.hpp"

namespace algo_sort
{
inline constexpr int arrayRange1 = -50;
inline constexpr int arrayRange2 = 150;
inline constexpr uint32_t arrayLength = 53;
constexpr uint32_t maxAlignOfPrint = 16;
constexpr uint32_t maxColumnOfPrint = 10;

template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

template <class T>
class Sort
{
public:
    Sort(const uint32_t length, const T left, const T right);
    virtual ~Sort() = default;
    Sort(const Sort& rhs);
    Sort<T>& operator=(const Sort& rhs);

    [[nodiscard]] std::vector<T> bubbleSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> selectionSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> insertionSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> shellSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> mergeSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> quickSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> heapSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> countingSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> bucketSort(T* const array, const uint32_t length) const;
    [[nodiscard]] std::vector<T> radixSort(T* const array, const uint32_t length) const;

    const std::unique_ptr<T[]>& getRandomArray() const;
    uint32_t getLength() const;
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
    mutable std::mutex sortMutex;
    const std::unique_ptr<T[]> randomArray;
    const uint32_t length;
    const T left;
    const T right;

    void deepCopyFromSort(const Sort& sort) const;
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
Sort<T>::Sort(const uint32_t length, const T left, const T right) :
    length(length), left(left), right(right), randomArray(std::make_unique<T[]>(length))
{
    setRandomArray<T>(randomArray.get(), length, left, right);
}

template <class T>
Sort<T>::Sort(const Sort& rhs) :
    length(rhs.length), left(rhs.length), right(rhs.length), randomArray(std::make_unique<T[]>(rhs.length))
{
    deepCopyFromSort(rhs);
}

template <class T>
Sort<T>& Sort<T>::operator=(const Sort& rhs)
{
    deepCopyFromSort(rhs);
    return *this;
}

template <class T>
void Sort<T>::deepCopyFromSort(const Sort& sort) const
{
    std::memcpy(this->randomArray.get(), sort.randomArray.get(), this->length * sizeof(T));
}

template <class T>
const std::unique_ptr<T[]>& Sort<T>::getRandomArray() const
{
    std::unique_lock<std::mutex> lock(sortMutex);
    return randomArray;
}

template <class T>
uint32_t Sort<T>::getLength() const
{
    std::unique_lock<std::mutex> lock(sortMutex);
    return length;
}

template <class T>
template <typename V>
requires std::is_integral<V>::value void Sort<T>::setRandomArray(
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

    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " random integral numbers from " << left << " to " << right << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
}

template <class T>
template <typename V>
requires std::is_floating_point<V>::value void Sort<T>::setRandomArray(
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

    const uint32_t arrayBufferSize = length * maxAlignOfPrint;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    std::cout << "\r\nGenerate " << length << " random floating point numbers from " << left << " to " << right
              << ":\r\n"
              << formatArray<T>(array, length, arrayBuffer, arrayBufferSize + 1) << std::endl;
}

template <class T>
template <typename V>
requires(isNumber<V>()) char* Sort<T>::formatArray(
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
} // namespace algo_sort
