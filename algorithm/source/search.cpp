#include "search.hpp"
#include "file.hpp"

#define SEARCH_RESULT(opt) \
    "*%-13s method: Found the key \"%.5f\" appearing(" #opt ") at index %d.  ==>Run time: %8.5f ms\n"
#define SEARCH_NO_RESULT(opt) "*%-13s method: Could not find the key \"%.5f\".  ==>Run time: %8.5f ms\n"
#define SEARCH_PRINT_RESULT_CONTENT(method)                                              \
    do                                                                                   \
    {                                                                                    \
        if (-1 != index)                                                                 \
        {                                                                                \
            FORMAT_PRINT(SEARCH_RESULT(1st), method, key, index, TIME_INTERVAL(timing)); \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            FORMAT_PRINT(SEARCH_NO_RESULT(1st), method, key, TIME_INTERVAL(timing));     \
        }                                                                                \
    }                                                                                    \
    while (0)

namespace algo_search
{
template class Search<double>;
template int Search<double>::binarySearch(double* const array, const uint32_t length, const double key) const;
template int Search<double>::interpolationSearch(double* const array, const uint32_t length, const double key) const;
template int Search<double>::fibonacciSearch(double* const array, const uint32_t length, const double key) const;

// Binary method
template <class T>
int Search<T>::binarySearch(T* const array, const uint32_t length, const T key) const
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

// Interpolation method
template <class T>
int Search<T>::interpolationSearch(T* const array, const uint32_t length, const T key) const
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

// Fibonacci method
template <class T>
int Search<T>::fibonacciSearch(T* const array, const uint32_t length, const T key) const
{
    TIME_BEGIN(timing);
    int index = -1;
    uint32_t lower = 0, upper = length - 1;
    std::vector<uint32_t> fib = generateFibonacciNumber(length);
    uint32_t n = fib.size() - 1;
    if (constexpr uint32_t minSize = 3; n < minSize)
    {
        FORMAT_PRINT("*%-13s method: The length %u isn't enough.\n", "Fibonacci", length);
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
