//! @file search.hpp
//! @author ryftchen
//! @brief The declarations (search) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#pragma once

#include <sys/time.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>

//! @brief Search-related functions in the algorithm module.
namespace algorithm::search
{
//! @brief Maximum alignment length per element of printing.
constexpr uint32_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr uint32_t maxColumnOfPrint = 10;

//! @brief Solution of search.
//! @tparam T - type of the solution of search
template <class T>
class SearchSolution
{
public:
    //! @brief Destroy the SearchSolution object.
    virtual ~SearchSolution() = default;

    //! @brief The binary method.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int binaryMethod(const T* const array, const uint32_t length, const T key);
    //! @brief The interpolation method.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int interpolationMethod(const T* const array, const uint32_t length, const T key);
    //! @brief The Fibonacci method.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int fibonacciMethod(const T* const array, const uint32_t length, const T key);

private:
    //! @brief Generate Fibonacci number.
    //! @param max - the smallest integer that is not greater than the maximum value of the Fibonacci sequence
    //! @return Fibonacci sequence
    static std::vector<uint32_t> generateFibonacciNumber(const uint32_t max);
};

//! @brief Check whether it is the number type.
//! @tparam T - type of inspection to be performed
//! @return be number or not
template <typename T>
constexpr bool isNumber()
{
    return (std::is_integral<T>::value || std::is_floating_point<T>::value);
}

//! @brief Builder for the target.
//! @tparam T - type of builder for the target
template <class T>
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param length - length of array
    //! @param left - the left boundary of the array
    //! @param right - the right boundary of the array
    TargetBuilder(const uint32_t length, const T left, const T right);
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;
    //! @brief Construct a new TargetBuilder object.
    //! @param rhs - right-hand side
    TargetBuilder(const TargetBuilder& rhs);
    //! @brief The operator (!=) overloading of TargetBuilder class.
    //! @param rhs - right-hand side
    //! @return reference of TargetBuilder object
    TargetBuilder<T>& operator=(const TargetBuilder& rhs);

    //! @brief Get the ordered array.
    //! @return ordered array
    inline const std::unique_ptr<T[]>& getOrderedArray() const;
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] inline uint32_t getLength() const;
    //! @brief Get the search key.
    //! @return T search key
    inline T getSearchKey() const;

private:
    //! @brief Ordered array.
    const std::unique_ptr<T[]> orderedArray;
    //! @brief Length of the ordered array.
    const uint32_t length;
    //! @brief Search key.
    T searchKey{0};

    //! @brief Deep copy for copy constructor.
    //! @param rhs - right-hand side
    void deepCopy(const TargetBuilder& rhs) const;
    //! @brief Set the ordered array.
    //! @tparam V - the specific type of integral
    //! @param array - ordered array
    //! @param length - length of the ordered array
    //! @param left - the left boundary of the ordered array
    //! @param right - the left right of the ordered array
    template <typename V>
    requires std::is_integral<V>::value static void setOrderedArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right);
    //! @brief Set the ordered array.
    //! @tparam V - the specific type of floating point
    //! @param array - ordered array
    //! @param length - length of the ordered array
    //! @param left - the left boundary of the ordered array
    //! @param right - the left right of the ordered array
    template <typename V>
    requires std::is_floating_point<V>::value static void setOrderedArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right);
    //! @brief Format array for printing.
    //! @tparam V - type of array
    //! @param array - array to be formatted
    //! @param length - length of array
    //! @param buffer - buffer for filling the formatted array
    //! @param bufferSize - size of buffer
    //! @return buffer after format
    template <typename V>
    requires(isNumber<V>()) static char* formatArray(
        const T* const array,
        const uint32_t length,
        char* const buffer,
        const uint32_t bufferSize);
    //! @brief Get the random seed by time.
    //! @return random seed
    inline static std::mt19937 getRandomSeedByTime();
};

template <class T>
TargetBuilder<T>::TargetBuilder(const uint32_t length, const T left, const T right) :
    length(length), orderedArray(std::make_unique<T[]>(length))
{
    setOrderedArray<T>(orderedArray.get(), length, left, right);
    searchKey = orderedArray[length / 2];
}

template <class T>
TargetBuilder<T>::TargetBuilder(const TargetBuilder& rhs) :
    length(rhs.length), orderedArray(std::make_unique<T[]>(rhs.length))
{
    deepCopy(rhs);
    searchKey = orderedArray[length / 2];
}

template <class T>
TargetBuilder<T>& TargetBuilder<T>::operator=(const TargetBuilder& rhs)
{
    deepCopy(rhs);
    return *this;
}

template <class T>
inline const std::unique_ptr<T[]>& TargetBuilder<T>::getOrderedArray() const
{
    return orderedArray;
}

template <class T>
inline uint32_t TargetBuilder<T>::getLength() const
{
    return length;
}

template <class T>
inline T TargetBuilder<T>::getSearchKey() const
{
    return searchKey;
}

template <class T>
void TargetBuilder<T>::deepCopy(const TargetBuilder& rhs) const
{
    std::memcpy(this->orderedArray.get(), rhs.orderedArray.get(), this->length * sizeof(T));
}

template <class T>
template <typename V>
requires std::is_integral<V>::value void TargetBuilder<T>::setOrderedArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right)
{
    std::mt19937 seed{getRandomSeedByTime()};
    std::uniform_int_distribution<int> random(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = random(seed);
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
requires std::is_floating_point<V>::value void TargetBuilder<T>::setOrderedArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right)
{
    std::mt19937 seed{getRandomSeedByTime()};
    std::uniform_real_distribution<double> random(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = random(seed);
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
inline std::mt19937 TargetBuilder<T>::getRandomSeedByTime()
{
    constexpr uint32_t secToUsec = 1000000;
    timeval timeSeed{};
    gettimeofday(&timeSeed, nullptr);

    return std::mt19937(timeSeed.tv_sec * secToUsec + timeSeed.tv_usec);
}
} // namespace algorithm::search
