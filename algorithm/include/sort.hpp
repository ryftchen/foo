//! @file sort.hpp
//! @author ryftchen
//! @brief The declarations (sort) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <sys/time.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>

//! @brief Sort-related functions in the algorithm module.
namespace algorithm::sort
{
//! @brief Maximum alignment length per element of printing.
constexpr uint32_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr uint32_t maxColumnOfPrint = 10;

//! @brief Solution of sort.
//! @tparam T type of the solution of sort
template <class T>
class SortSolution
{
public:
    //! @brief Destroy the SortSolution object.
    virtual ~SortSolution() = default;

    //! @brief The bubble method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> bubbleMethod(T* const array, const uint32_t length);
    //! @brief The selection method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> selectionMethod(T* const array, const uint32_t length);
    //! @brief The insertion method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> insertionMethod(T* const array, const uint32_t length);
    //! @brief The shell method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> shellMethod(T* const array, const uint32_t length);
    //! @brief The merge method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> mergeMethod(T* const array, const uint32_t length);
    //! @brief The quick method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> quickMethod(T* const array, const uint32_t length);
    //! @brief The heap method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> heapMethod(T* const array, const uint32_t length);
    //! @brief The counting method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> countingMethod(T* const array, const uint32_t length);
    //! @brief The bucket method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> bucketMethod(T* const array, const uint32_t length);
    //! @brief The radix method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    static std::vector<T> radixMethod(T* const array, const uint32_t length);

private:
    //! @brief Recursive for merge sort.
    //! @param sortArray array to be sorted
    //! @param begin index of beginning
    //! @param end index of ending
    static void mergeSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end);
    //! @brief Recursive for quick sort.
    //! @param sortArray array to be sorted
    //! @param begin index of beginning
    //! @param end index of ending
    static void quickSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end);
    //! @brief Build max heap for heap sort.
    //! @param sortArray array to be sorted
    //! @param begin index of beginning
    //! @param end index of ending
    static void buildMaxHeap(T* const sortArray, const uint32_t begin, const uint32_t end);
};

//! @brief Check whether it is the number type.
//! @tparam T type of inspection to be performed
//! @return be number or not
template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

//! @brief Builder for the target.
//! @tparam T type of builder for the target
template <class T>
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param length length of array
    //! @param left the left boundary of the array
    //! @param right the right boundary of the array
    TargetBuilder(const uint32_t length, const T left, const T right);
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;
    //! @brief Construct a new TargetBuilder object.
    //! @param rhs right-hand side
    TargetBuilder(const TargetBuilder& rhs);
    //! @brief The operator (!=) overloading of TargetBuilder class.
    //! @param rhs right-hand side
    //! @return reference of TargetBuilder object
    TargetBuilder<T>& operator=(const TargetBuilder& rhs);

    //! @brief Get the random array.
    //! @return random array
    inline const std::unique_ptr<T[]>& getRandomArray() const;
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] inline uint32_t getLength() const;
    //! @brief Format array for printing.
    //! @tparam V type of array
    //! @param array array to be formatted
    //! @param length length of array
    //! @param buffer buffer for filling the formatted array
    //! @param bufferSize size of buffer
    //! @return buffer after format
    template <typename V>
    requires(isNumber<V>()) static char* formatArray(
        const T* const array,
        const uint32_t length,
        char* const buffer,
        const uint32_t bufferSize);

private:
    //! @brief Random array.
    const std::unique_ptr<T[]> randomArray;
    //! @brief Length of the random array.
    const uint32_t length;

    //! @brief Deep copy for copy constructor.
    //! @param rhs right-hand side
    void deepCopy(const TargetBuilder& rhs) const;
    //! @brief Set the random array.
    //! @tparam V the specific type of integral
    //! @param array random array
    //! @param length length of the random array
    //! @param left the left boundary of the random array
    //! @param right the left right of the random array
    template <typename V>
    requires std::is_integral<V>::value static void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right);
    //! @brief Set the random array.
    //! @tparam V the specific type of floating point
    //! @param array random array
    //! @param length length of the random array
    //! @param left the left boundary of the random array
    //! @param right the left right of the random array
    template <typename V>
    requires std::is_floating_point<V>::value static void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right);
    //! @brief Get the random seed by time.
    //! @return random seed
    inline static std::mt19937 getRandomSeedByTime();
};

template <class T>
TargetBuilder<T>::TargetBuilder(const uint32_t length, const T left, const T right) :
    length(length), randomArray(std::make_unique<T[]>(length))
{
    setRandomArray<T>(randomArray.get(), length, left, right);
}

template <class T>
TargetBuilder<T>::TargetBuilder(const TargetBuilder& rhs) :
    length(rhs.length), randomArray(std::make_unique<T[]>(rhs.length))
{
    deepCopy(rhs);
}

template <class T>
TargetBuilder<T>& TargetBuilder<T>::operator=(const TargetBuilder& rhs)
{
    deepCopy(rhs);
    return *this;
}

template <class T>
inline const std::unique_ptr<T[]>& TargetBuilder<T>::getRandomArray() const
{
    return randomArray;
}

template <class T>
inline uint32_t TargetBuilder<T>::getLength() const
{
    return length;
}

template <class T>
template <typename V>
requires(isNumber<V>()) char* TargetBuilder<T>::formatArray(
    const T* const array,
    const uint32_t length,
    char* const buffer,
    const uint32_t bufferSize)
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

template <class T>
void TargetBuilder<T>::deepCopy(const TargetBuilder& rhs) const
{
    std::memcpy(this->randomArray.get(), rhs.randomArray.get(), this->length * sizeof(T));
}

template <class T>
template <typename V>
requires std::is_integral<V>::value void TargetBuilder<T>::setRandomArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right)
{
    std::mt19937 seed{getRandomSeedByTime()};
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
requires std::is_floating_point<V>::value void TargetBuilder<T>::setRandomArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right)
{
    std::mt19937 seed{getRandomSeedByTime()};
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
inline std::mt19937 TargetBuilder<T>::getRandomSeedByTime()
{
    constexpr uint32_t secToUsec = 1000000;
    timeval timeSeed{};
    gettimeofday(&timeSeed, nullptr);

    return std::mt19937(timeSeed.tv_sec * secToUsec + timeSeed.tv_usec);
}
} // namespace algorithm::sort
