//! @file search.hpp
//! @author ryftchen
//! @brief The declarations (search) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <cstring>
#include <memory>
#include <vector>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Search-related functions in the algorithm module.
namespace search
{
//! @brief Search methods.
//! @tparam T - type of search methods
template <class T>
class Search
{
public:
    //! @brief Destroy the Search object.
    virtual ~Search() = default;

    //! @brief Binary.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int binary(const T* const array, const std::uint32_t length, const T key);
    //! @brief Interpolation.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int interpolation(const T* const array, const std::uint32_t length, const T key);
    //! @brief Fibonacci.
    //! @param array - array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static int fibonacci(const T* const array, const std::uint32_t length, const T key);

private:
    //! @brief Generate Fibonacci number.
    //! @param max - the smallest integer that is not greater than the maximum value of the Fibonacci sequence
    //! @return Fibonacci sequence
    static std::vector<std::uint32_t> generateFibonacciNumber(const std::uint32_t max);
};

template <class T>
int Search<T>::binary(const T* const array, const std::uint32_t length, const T key)
{
    int index = -1;
    std::uint32_t lower = 0, upper = length - 1;

    while (lower <= upper)
    {
        std::uint32_t mid = (lower + upper) / 2;
        if (key == array[mid])
        {
            index = mid;
            break;
        }
        else if (key > array[mid])
        {
            lower = mid + 1;
        }
        else
        {
            upper = mid - 1;
        }
    }

    return index;
}

template <class T>
int Search<T>::interpolation(const T* const array, const std::uint32_t length, const T key)
{
    int index = -1;
    std::uint32_t lower = 0, upper = length - 1;

    while (lower <= upper)
    {
        std::uint32_t mid = lower + (upper - lower) * ((key - array[lower]) / (array[upper] - array[lower]));
        if (key == array[mid])
        {
            index = mid;
            break;
        }
        else if (key > array[mid])
        {
            lower = mid + 1;
        }
        else
        {
            upper = mid - 1;
        }
    }

    return index;
}

template <class T>
int Search<T>::fibonacci(const T* const array, const std::uint32_t length, const T key)
{
    int index = -1;
    std::vector<std::uint32_t> fib = generateFibonacciNumber(length);
    std::uint32_t n = fib.size() - 1;
    if (constexpr std::uint32_t minSize = 3; n < minSize)
    {
        throw std::runtime_error("An array size of " + std::to_string(length) + " is not sufficient.");
    }

    std::uint32_t lower = 0, upper = length - 1;
    std::unique_ptr<T[]> complementArray = std::make_unique<T[]>(fib[n] - 1);
    std::memcpy(complementArray.get(), array, length * sizeof(T));
    for (std::uint32_t i = upper; i < (fib[n] - 1); ++i)
    {
        complementArray[i] = array[upper];
    }

    while (lower <= upper)
    {
        std::uint32_t mid = lower + fib[n - 1] - 1;
        if (complementArray[mid] > key)
        {
            upper = mid - 1;
            --n;
        }
        else if (complementArray[mid] < key)
        {
            lower = mid + 1;
            n -= 2;
        }
        else
        {
            if (mid <= upper)
            {
                index = mid;
                break;
            }
            else
            {
                index = upper;
                break;
            }
        }
    }

    return index;
}

template <class T>
std::vector<std::uint32_t> Search<T>::generateFibonacciNumber(const std::uint32_t max)
{
    std::vector<std::uint32_t> fibonacci(0);
    std::uint32_t f1 = 0.0, f2 = 1.0;
    for (;;)
    {
        const std::uint32_t temp = f1 + f2;
        f1 = f2;
        f2 = temp;
        fibonacci.emplace_back(f1);

        if (f1 > max)
        {
            break;
        }
    }

    return fibonacci;
}
} // namespace search
} // namespace algorithm
