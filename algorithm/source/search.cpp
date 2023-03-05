//! @file search.cpp
//! @author ryftchen
//! @brief The definitions (search) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "search.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cstring>
#include <memory>
#endif

namespace algorithm::search
{
template class Search<double>;
template int Search<double>::binary(const double* const array, const uint32_t length, const double key);
template int Search<double>::interpolation(const double* const array, const uint32_t length, const double key);
template int Search<double>::fibonacci(const double* const array, const uint32_t length, const double key);

template <class T>
int Search<T>::binary(const T* const array, const uint32_t length, const T key)
{
    int index = -1;
    uint32_t lower = 0, upper = length - 1;

    while (lower <= upper)
    {
        uint32_t mid = (lower + upper) / 2;
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
int Search<T>::interpolation(const T* const array, const uint32_t length, const T key)
{
    int index = -1;
    uint32_t lower = 0, upper = length - 1;

    while (lower <= upper)
    {
        uint32_t mid = lower + (upper - lower) * ((key - array[lower]) / (array[upper] - array[lower]));
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
int Search<T>::fibonacci(const T* const array, const uint32_t length, const T key)
{
    int index = -1;
    std::vector<uint32_t> fib = generateFibonacciNumber(length);
    uint32_t n = fib.size() - 1;
    if (constexpr uint32_t minSize = 3; n < minSize)
    {
        throw std::runtime_error("An array size of " + std::to_string(length) + " isn't sufficient.");
    }

    uint32_t lower = 0, upper = length - 1;
    std::unique_ptr<T[]> complementArray = std::make_unique<T[]>(fib[n] - 1);
    std::memcpy(complementArray.get(), array, length * sizeof(T));
    for (uint32_t i = upper; i < (fib[n] - 1); ++i)
    {
        complementArray[i] = array[upper];
    }

    while (lower <= upper)
    {
        uint32_t mid = lower + fib[n - 1] - 1;
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
std::vector<uint32_t> Search<T>::generateFibonacciNumber(const uint32_t max)
{
    std::vector<uint32_t> fibonacci(0);
    uint32_t f1 = 0.0, f2 = 1.0;
    for (;;)
    {
        const uint32_t temp = f1 + f2;
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
} // namespace algorithm::search
