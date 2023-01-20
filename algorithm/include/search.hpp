//! @file search.hpp
//! @author ryftchen
//! @brief The declarations (search) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <cstring>
#include <iostream>
#include <mutex>
#include "utility/include/time.hpp"

//! @brief Namespace for search-related functions in the algorithm module.
namespace algorithm::search
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

//! @brief Class for the solution of search.
//! @tparam T type of the solution of search
template <class T>
class SearchSolution
{
public:
    //! @brief Construct a new SearchSolution object.
    //! @param length length of array
    //! @param left the left boundary of the array
    //! @param right the right boundary of the array
    SearchSolution(const uint32_t length, const T left, const T right);
    //! @brief Destroy the SearchSolution object.
    virtual ~SearchSolution() = default;
    //! @brief Construct a new SearchSolution object.
    //! @param rhs right-hand side
    SearchSolution(const SearchSolution& rhs);
    //! @brief The operator (!=) overloading of SearchSolution class.
    //! @param rhs right-hand side
    //! @return reference of SearchSolution object
    SearchSolution<T>& operator=(const SearchSolution& rhs);

    //! @brief The binary method.
    //! @param array array to be searched
    //! @param length length of array
    //! @param key search key
    //! @return index of the first occurrence of key
    int binaryMethod(const T* const array, const uint32_t length, const T key) const;
    //! @brief The interpolation method.
    //! @param array array to be searched
    //! @param length length of array
    //! @param key search key
    //! @return index of the first occurrence of key
    int interpolationMethod(const T* const array, const uint32_t length, const T key) const;
    //! @brief The fibonacci method.
    //! @param array array to be searched
    //! @param length length of array
    //! @param key search key
    //! @return index of the first occurrence of key
    int fibonacciMethod(const T* const array, const uint32_t length, const T key) const;

    //! @brief Get the ordered array.
    //! @return ordered array
    inline const std::unique_ptr<T[]>& getOrderedArray() const;
    //! @brief Get the length.
    //! @return length
    [[nodiscard]] inline uint32_t getLength() const;
    //! @brief Get the search key.
    //! @return T search key
    inline T getSearchKey() const;
    //! @brief Set the ordered array.
    //! @tparam V the specific type of integral
    //! @param array ordered array
    //! @param length length of the ordered array
    //! @param left the left boundary of the ordered array
    //! @param right the left right of the ordered array
    template <typename V>
    requires std::is_integral<V>::value void setOrderedArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;
    //! @brief Set the ordered array.
    //! @tparam V the specific type of floating point
    //! @param array ordered array
    //! @param length length of the ordered array
    //! @param left the left boundary of the ordered array
    //! @param right the left right of the ordered array
    template <typename V>
    requires std::is_floating_point<V>::value void setOrderedArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;

private:
    //! @brief Ordered array.
    const std::unique_ptr<T[]> orderedArray;
    //! @brief Length of the ordered array.
    const uint32_t length;
    //! @brief Search key.
    T searchKey{0};

    //! @brief Deep copy for copy constructor.
    //! @param rhs right-hand side
    void deepCopy(const SearchSolution& rhs) const;
    //! @brief Generate fibonacci number.
    //! @param max the smallest integer greater than or equal to the maximum of the Fibonacci series
    //! @return fibonacci sequences
    static std::vector<uint32_t> generateFibonacciNumber(const uint32_t max);

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
SearchSolution<T>::SearchSolution(const uint32_t length, const T left, const T right) :
    length(length), orderedArray(std::make_unique<T[]>(length))
{
    setOrderedArray<T>(orderedArray.get(), length, left, right);
    searchKey = orderedArray[length / 2];
}

template <class T>
SearchSolution<T>::SearchSolution(const SearchSolution& rhs) :
    length(rhs.length), orderedArray(std::make_unique<T[]>(rhs.length))
{
    deepCopy(rhs);
    searchKey = orderedArray[length / 2];
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
inline T SearchSolution<T>::getSearchKey() const
{
    return searchKey;
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
