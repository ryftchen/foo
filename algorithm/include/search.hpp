//! @file search.hpp
//! @author ryftchen
//! @brief The declarations (search) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

//! @brief The algorithm module.
namespace algorithm // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Search-related functions in the algorithm module.
namespace search
{
extern const char* version() noexcept;

//! @brief Search methods.
//! @tparam T - type of search methods
template <class T>
class Search
{
public:
    //! @brief Destroy the Search object.
    virtual ~Search() = default;

    //! @brief Binary.
    //! @param array - ordered array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static std::int64_t binary(const T* const array, const std::uint32_t length, const T key);
    //! @brief Interpolation.
    //! @param array - ordered array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static std::int64_t interpolation(const T* const array, const std::uint32_t length, const T key);
    //! @brief Fibonacci.
    //! @param array - ordered array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static std::int64_t fibonacci(const T* const array, const std::uint32_t length, const T key);

private:
    //! @brief Generate Fibonacci number.
    //! @param limit - the smallest integer that is not greater than the maximum value of the Fibonacci sequence
    //! @return Fibonacci sequence
    static std::vector<std::uint32_t> generateFibonacciNumber(const std::uint32_t limit);
};

template <class T>
std::int64_t Search<T>::binary(const T* const array, const std::uint32_t length, const T key)
{
    std::int64_t index = -1;
    std::uint32_t lower = 0, upper = length - 1;
    if ((key < array[lower]) || (key > array[upper]))
    {
        return index;
    }

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
std::int64_t Search<T>::interpolation(const T* const array, const std::uint32_t length, const T key)
{
    std::int64_t index = -1;
    std::uint32_t lower = 0, upper = length - 1;
    if ((key < array[lower]) || (key > array[upper]))
    {
        return index;
    }

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
std::int64_t Search<T>::fibonacci(const T* const array, const std::uint32_t length, const T key)
{
    std::int64_t index = -1;
    std::uint32_t lower = 0, upper = length - 1;
    if ((key < array[lower]) || (key > array[upper]))
    {
        return index;
    }
    const auto& fib = generateFibonacciNumber(length);
    if (constexpr std::uint8_t minSize = 3; minSize > static_cast<std::int32_t>(fib.size() - 1))
    {
        throw std::logic_error("An array size of " + std::to_string(length) + " is not sufficient.");
    }

    std::uint32_t n = fib.size() - 1;
    std::vector<T> complement(array, array + (fib[n] - 1));
    for (std::uint32_t i = upper; i < (fib[n] - 1); ++i)
    {
        complement[i] = array[upper];
    }

    while ((lower <= upper) && (n >= 1))
    {
        std::uint32_t mid = lower + fib[n - 1] - 1;
        if (complement[mid] > key)
        {
            upper = mid - 1;
            --n;
        }
        else if (complement[mid] < key)
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
std::vector<std::uint32_t> Search<T>::generateFibonacciNumber(const std::uint32_t limit)
{
    if (0 == limit)
    {
        return {};
    }

    const double phi = (1.0 + std::sqrt(5.0)) / 2.0, // golden ratio
        estimate = std::log(limit * std::sqrt(5.0)) / std::log(phi); // Fn≈(ϕ^n)/(5^(1/2))
    std::vector<std::uint32_t> fibonacci{};
    fibonacci.reserve(static_cast<std::uint32_t>(estimate) + 1);
    std::uint32_t f1 = 0, f2 = 1;
    for (;;)
    {
        const std::uint32_t temp = f1 + f2;
        f1 = f2;
        f2 = temp;
        fibonacci.emplace_back(f1);

        if (f1 > limit)
        {
            break;
        }
    }

    return fibonacci;
}
} // namespace search
} // namespace algorithm
