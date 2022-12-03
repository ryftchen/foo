#include "search.hpp"
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

#define SEARCH_RESULT "*%-13s method: Found the key \"%.5f\" appearing at index %d.  ==>Run time: %8.5f ms\n"
#define SEARCH_NONE_RESULT "*%-13s method: Could not find the key \"%.5f\".  ==>Run time: %8.5f ms\n"
#define SEARCH_PRINT_RESULT_CONTENT(method)                                        \
    do                                                                             \
    {                                                                              \
        if (-1 != index)                                                           \
        {                                                                          \
            COMMON_PRINT(SEARCH_RESULT, method, key, index, SEARCH_TIME_INTERVAL); \
        }                                                                          \
        else                                                                       \
        {                                                                          \
            COMMON_PRINT(SEARCH_NONE_RESULT, method, key, SEARCH_TIME_INTERVAL);   \
        }                                                                          \
    }                                                                              \
    while (0)
#define SEARCH_RUNTIME_BEGIN TIME_BEGIN(timing)
#define SEARCH_RUNTIME_END TIME_END(timing)
#define SEARCH_TIME_INTERVAL TIME_INTERVAL(timing)
#else

#define SEARCH_PRINT_RESULT_CONTENT(method)
#define SEARCH_RUNTIME_BEGIN
#define SEARCH_RUNTIME_END
#endif

namespace algorithm::search
{
template class SearchSolution<double>;
template int SearchSolution<double>::binaryMethod(const double* const array, const uint32_t length, const double key)
    const;
template int SearchSolution<double>::interpolationMethod(
    const double* const array,
    const uint32_t length,
    const double key) const;
template int SearchSolution<double>::fibonacciMethod(const double* const array, const uint32_t length, const double key)
    const;

// Binary
template <class T>
int SearchSolution<T>::binaryMethod(const T* const array, const uint32_t length, const T key) const
{
    SEARCH_RUNTIME_BEGIN;
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

    SEARCH_RUNTIME_END;
    SEARCH_PRINT_RESULT_CONTENT("Binary");
    return index;
}

// Interpolation
template <class T>
int SearchSolution<T>::interpolationMethod(const T* const array, const uint32_t length, const T key) const
{
    SEARCH_RUNTIME_BEGIN;
    int index = -1;
    uint32_t lower = 0, upper = length - 1;

    while (lower <= upper)
    {
        uint32_t mid = lower + (upper - lower) * (key - array[lower]) / (array[upper] - array[lower]);
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

    SEARCH_RUNTIME_END;
    SEARCH_PRINT_RESULT_CONTENT("Interpolation");
    return index;
}

// Fibonacci
template <class T>
int SearchSolution<T>::fibonacciMethod(const T* const array, const uint32_t length, const T key) const
{
    SEARCH_RUNTIME_BEGIN;
    int index = -1;
    uint32_t lower = 0, upper = length - 1;
    std::vector<uint32_t> fib = generateFibonacciNumber(length);
    uint32_t n = fib.size() - 1;
    if (constexpr uint32_t minSize = 3; n < minSize)
    {
#ifndef _NO_PRINT_AT_RUNTIME
        COMMON_PRINT("*Fibonacci     method: The length %u isn't enough.\n", length);
#endif
        return index;
    }

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

    SEARCH_RUNTIME_END;
    SEARCH_PRINT_RESULT_CONTENT("Fibonacci");
    return index;
}

template <class T>
std::vector<uint32_t> SearchSolution<T>::generateFibonacciNumber(const uint32_t max)
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
