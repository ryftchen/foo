#include "search.hpp"
#include "utility/include/common.hpp"

#define SEARCH_RESULT "*%-13s method: Found the key \"%.5f\" appearing at index %d.  ==>Run time: %8.5f ms\n"
#define SEARCH_NONE_RESULT "*%-13s method: Could not find the key \"%.5f\".  ==>Run time: %8.5f ms\n"
#define SEARCH_PRINT_RESULT_CONTENT(method)                                         \
    do                                                                              \
    {                                                                               \
        if (-1 != index)                                                            \
        {                                                                           \
            COMMON_PRINT(SEARCH_RESULT, method, key, index, TIME_INTERVAL(timing)); \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            COMMON_PRINT(SEARCH_NONE_RESULT, method, key, TIME_INTERVAL(timing));   \
        }                                                                           \
    }                                                                               \
    while (0)

namespace algo_search
{
template class Search<double>;
template int Search<double>::binarySearch(const double* const array, const uint32_t length, const double key) const;
template int Search<double>::interpolationSearch(const double* const array, const uint32_t length, const double key)
    const;
template int Search<double>::fibonacciSearch(const double* const array, const uint32_t length, const double key) const;

// binary
template <class T>
int Search<T>::binarySearch(const T* const array, const uint32_t length, const T key) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
    SEARCH_PRINT_RESULT_CONTENT("Binary");
    return index;
}

// interpolation
template <class T>
int Search<T>::interpolationSearch(const T* const array, const uint32_t length, const T key) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
    SEARCH_PRINT_RESULT_CONTENT("Interpolation");
    return index;
}

// Fibonacci
template <class T>
int Search<T>::fibonacciSearch(const T* const array, const uint32_t length, const T key) const
{
    TIME_BEGIN(timing);
    int index = -1;
    uint32_t lower = 0, upper = length - 1;
    std::vector<uint32_t> fib = generateFibonacciNumber(length);
    uint32_t n = fib.size() - 1;
    if (constexpr uint32_t minSize = 3; n < minSize)
    {
        COMMON_PRINT("*%-13s method: The length %u isn't enough.\n", "Fibonacci", length);
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

    TIME_END(timing);
    SEARCH_PRINT_RESULT_CONTENT("Fibonacci");
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
} // namespace algo_search
