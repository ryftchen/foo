//! @file search.hpp
//! @author ryftchen
//! @brief The declarations (search) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Search-related functions in the algorithm module.
namespace search
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "ALGO_SEARCH";
}
extern const char* version() noexcept;

//! @brief Search methods.
//! @tparam Elem - type of element
template <typename Elem>
class Search
{
public:
    //! @brief Binary.
    //! @param array - ordered array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static std::int64_t binary(const Elem* const array, const std::uint32_t length, const Elem key);
    //! @brief Interpolation.
    //! @param array - ordered array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static std::int64_t interpolation(const Elem* const array, const std::uint32_t length, const Elem key);
    //! @brief Fibonacci.
    //! @param array - ordered array to be searched
    //! @param length - length of array
    //! @param key - search key
    //! @return index of the first occurrence of key
    static std::int64_t fibonacci(const Elem* const array, const std::uint32_t length, const Elem key);

private:
    //! @brief Generate Fibonacci number.
    //! @param limit - smallest integer that is not greater than the maximum value of the Fibonacci sequence
    //! @return Fibonacci sequence
    static std::vector<std::uint32_t> generateFibonacciNumber(const std::uint32_t limit);
};

template <typename Elem>
std::int64_t Search<Elem>::binary(const Elem* const array, const std::uint32_t length, const Elem key)
{
    if (!array || (length == 0))
    {
        return -1;
    }

    std::uint32_t lower = 0;
    std::uint32_t upper = length - 1;
    if ((key < array[lower]) || (key > array[upper]))
    {
        return -1;
    }

    std::int64_t index = -1;
    while (lower <= upper)
    {
        const std::uint32_t mid = std::midpoint(lower, upper);
        if (key == array[mid])
        {
            index = mid;
            break;
        }
        if (key > array[mid])
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

template <typename Elem>
std::int64_t Search<Elem>::interpolation(const Elem* const array, const std::uint32_t length, const Elem key)
{
    if (!array || (length == 0))
    {
        return -1;
    }

    std::uint32_t lower = 0;
    std::uint32_t upper = length - 1;
    if ((key < array[lower]) || (key > array[upper]))
    {
        return -1;
    }

    std::int64_t index = -1;
    while (lower <= upper)
    {
        if (array[upper] == array[lower])
        {
            index = (key == array[lower]) ? lower : -1;
            break;
        }

        const std::uint32_t mid = lower + ((upper - lower) * (key - array[lower]) / (array[upper] - array[lower]));
        if (key == array[mid])
        {
            index = mid;
            break;
        }
        if (key > array[mid])
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

template <typename Elem>
std::int64_t Search<Elem>::fibonacci(const Elem* const array, const std::uint32_t length, const Elem key)
{
    if (!array || (length == 0))
    {
        return -1;
    }

    std::uint32_t lower = 0;
    std::uint32_t upper = length - 1;
    if ((key < array[lower]) || (key > array[upper]))
    {
        return -1;
    }
    const auto fib = generateFibonacciNumber(length);
    if (constexpr std::uint8_t minSize = 3; fib.size() < minSize)
    {
        return -1;
    }

    std::uint32_t n = fib.size() - 1;
    const std::uint32_t paddedSize = fib[n] - 1;
    std::vector<Elem> complement(paddedSize);
    std::copy(array, array + length, complement.begin());
    for (std::uint32_t i = length; i < paddedSize; ++i)
    {
        complement[i] = array[upper];
    }

    std::int64_t index = -1;
    while ((lower <= upper) && (n >= 2))
    {
        std::uint32_t mid = lower + fib[n - 1] - 1;
        if (mid >= paddedSize)
        {
            mid = paddedSize - 1;
        }

        if (complement[mid] > key)
        {
            if (mid == 0)
            {
                break;
            }
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
            index = (mid <= upper) ? mid : upper;
            break;
        }
    }
    return index;
}

template <typename Elem>
std::vector<std::uint32_t> Search<Elem>::generateFibonacciNumber(const std::uint32_t limit)
{
    if (limit == 0)
    {
        return {};
    }

    const double estimate = std::log(limit * std::sqrt(5.0)) / std::log(std::numbers::phi);
    std::vector<std::uint32_t> fibonacci{};
    fibonacci.reserve(static_cast<std::size_t>(std::ceil(estimate)));
    for (std::uint32_t f1 = 0, f2 = 1;;)
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
