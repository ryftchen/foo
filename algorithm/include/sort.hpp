//! @file sort.hpp
//! @author ryftchen
//! @brief The declarations (sort) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include "utility/include/time.hpp"

//! @brief Namespace for sort-related functions in the algorithm module.
namespace algorithm::sort
{
//! @brief Maximum alignment length per element of printing.
constexpr uint32_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr uint32_t maxColumnOfPrint = 10;

//! @brief Check whether it is the number type.
//! @tparam T type of inspection to be performed
//! @return be number or not
template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

//! @brief Class for the solution of sort.
//! @tparam T type of the solution of sort
template <class T>
class SortSolution
{
public:
    //! @brief Construct a new SortSolution object.
    //! @param length length of array
    //! @param left the left boundary of the array
    //! @param right the right boundary of the array
    SortSolution(const uint32_t length, const T left, const T right);
    //! @brief Destroy the SortSolution object.
    virtual ~SortSolution() = default;
    //! @brief Construct a new SortSolution object.
    //! @param rhs right-hand side
    SortSolution(const SortSolution& rhs);
    //! @brief The operator (!=) overloading of SortSolution class.
    //! @param rhs right-hand side
    //! @return reference of SortSolution object
    SortSolution<T>& operator=(const SortSolution& rhs);

    //! @brief The bubble method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> bubbleMethod(T* const array, const uint32_t length) const;
    //! @brief The selection method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> selectionMethod(T* const array, const uint32_t length) const;
    //! @brief The insertion method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> insertionMethod(T* const array, const uint32_t length) const;
    //! @brief The shell method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> shellMethod(T* const array, const uint32_t length) const;
    //! @brief The merge method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> mergeMethod(T* const array, const uint32_t length) const;
    //! @brief The quick method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> quickMethod(T* const array, const uint32_t length) const;
    //! @brief The heap method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> heapMethod(T* const array, const uint32_t length) const;
    //! @brief The counting method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> countingMethod(T* const array, const uint32_t length) const;
    //! @brief The bucket method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> bucketMethod(T* const array, const uint32_t length) const;
    //! @brief The radix method.
    //! @param array array to be sorted
    //! @param length length of array
    //! @return array after sort
    std::vector<T> radixMethod(T* const array, const uint32_t length) const;

    //! @brief Get the random array.
    //! @return random array
    inline const std::unique_ptr<T[]>& getRandomArray() const;
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] inline uint32_t getLength() const;
    //! @brief Set the random array.
    //! @tparam V the specific type of integral
    //! @param array random array
    //! @param length length of the random array
    //! @param left the left boundary of the random array
    //! @param right the left right of the random array
    template <typename V>
    requires std::is_integral<V>::value void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;
    //! @brief Set the random array.
    //! @tparam V the specific type of floating point
    //! @param array random array
    //! @param length length of the random array
    //! @param left the left boundary of the random array
    //! @param right the left right of the random array
    template <typename V>
    requires std::is_floating_point<V>::value void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;

private:
    //! @brief random array.
    const std::unique_ptr<T[]> randomArray;
    //! @brief Length of the random array.
    const uint32_t length;

    //! @brief Deep copy for copy constructor.
    //! @param rhs right-hand side
    void deepCopy(const SortSolution& rhs) const;
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

protected:
    //! @brief Format array for printing.
    //! @tparam V type of array
    //! @param array array to be formatted
    //! @param length length of array
    //! @param buffer buffer for filling the formatted array
    //! @param bufferSize size of buffer
    //! @return buffer after format
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
