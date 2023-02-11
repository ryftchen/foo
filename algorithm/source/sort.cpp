//! @file sort.cpp
//! @author ryftchen
//! @brief The definitions (sort) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "sort.hpp"
#include <queue>
#ifdef __RUNTIME_PRINTING
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

//! @brief Display sort result.
#define SORT_RESULT(opt) "\r\n*%-9s method: (" #opt ")\r\n%s\r\n==>Run time: %8.5f ms\n"
//! @brief Print sort result content.
#define SORT_PRINT_RESULT_CONTENT(method)                                                                   \
    do                                                                                                      \
    {                                                                                                       \
        const uint32_t arrayBufferSize = length * maxAlignOfPrint;                                          \
        char arrayBuffer[arrayBufferSize + 1];                                                              \
        arrayBuffer[0] = '\0';                                                                              \
        COMMON_PRINT(                                                                                       \
            SORT_RESULT(asc),                                                                               \
            method,                                                                                         \
            TargetBuilder<T>::template formatArray<T>(sortArray, length, arrayBuffer, arrayBufferSize + 1), \
            SORT_RUNTIME_INTERVAL);                                                                         \
    }                                                                                                       \
    while (0)
//! @brief Store sort beginning runtime.
#define SORT_RUNTIME_BEGIN TIME_BEGIN(timing)
//! @brief Store sort ending runtime.
#define SORT_RUNTIME_END TIME_END(timing)
//! @brief Calculate sort runtime interval.
#define SORT_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else
// #define NDEBUG
#include <cassert>

//! @brief Print sort result content.
#define SORT_PRINT_RESULT_CONTENT(method)
//! @brief Store sort beginning runtime.
#define SORT_RUNTIME_BEGIN
//! @brief Store sort ending runtime.
#define SORT_RUNTIME_END
#endif

namespace algorithm::sort
{
template class SortSolution<int>;
template std::vector<int> SortSolution<int>::bubbleMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::selectionMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::insertionMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::shellMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::mergeMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::quickMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::heapMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::countingMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::bucketMethod(int* const array, const uint32_t length);
template std::vector<int> SortSolution<int>::radixMethod(int* const array, const uint32_t length);

template <class T>
std::vector<T> SortSolution<T>::bubbleMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (uint32_t i = 0; i < length - 1; ++i)
    {
        for (uint32_t j = 0; j < length - 1 - i; ++j)
        {
            if (sortArray[j] > sortArray[j + 1])
            {
                std::swap(sortArray[j], sortArray[j + 1]);
            }
        }
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Bubble");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> SortSolution<T>::selectionMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (uint32_t i = 0; i < length - 1; ++i)
    {
        uint32_t min = i;
        for (uint32_t j = i + 1; j < length; ++j)
        {
            if (sortArray[j] < sortArray[min])
            {
                min = j;
            }
        }
        std::swap(sortArray[i], sortArray[min]);
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Selection");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> SortSolution<T>::insertionMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (uint32_t i = 1; i < length; ++i)
    {
        int n = i - 1;
        T temp = sortArray[i];
        while ((n >= 0) && (sortArray[n] > temp))
        {
            sortArray[n + 1] = sortArray[n];
            --n;
        }
        sortArray[n + 1] = temp;
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Insertion");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> SortSolution<T>::shellMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    uint32_t gap = length / 2;
    while (gap >= 1)
    {
        for (uint32_t i = gap; i < length; ++i)
        {
            for (uint32_t j = i; (j >= gap) && (sortArray[j] < sortArray[j - gap]); j -= gap)
            {
                std::swap(sortArray[j], sortArray[j - gap]);
            }
        }
        gap /= 2;
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Shell");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> SortSolution<T>::mergeMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    mergeSortRecursive(sortArray, 0, length - 1);

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Merge");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
void SortSolution<T>::mergeSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end)
{
    if (begin >= end)
    {
        return;
    }
    const uint32_t mid = (begin + end) / 2;
    mergeSortRecursive(sortArray, begin, mid);
    mergeSortRecursive(sortArray, mid + 1, end);

    uint32_t leftIndex = 0, rightIndex = 0;
    std::vector<T> leftSubArray(sortArray + begin, sortArray + mid + 1);
    std::vector<T> rightSubArray(sortArray + mid + 1, sortArray + end + 1);
    leftSubArray.insert(leftSubArray.cend(), std::numeric_limits<T>::max());
    rightSubArray.insert(rightSubArray.cend(), std::numeric_limits<T>::max());
    for (uint32_t i = begin; i <= end; ++i)
    {
        if (leftSubArray.at(leftIndex) < rightSubArray.at(rightIndex))
        {
            sortArray[i] = leftSubArray.at(leftIndex);
            ++leftIndex;
        }
        else
        {
            sortArray[i] = rightSubArray.at(rightIndex);
            ++rightIndex;
        }
    }
}

template <class T>
std::vector<T> SortSolution<T>::quickMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    quickSortRecursive(sortArray, 0, length - 1);

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Quick");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
void SortSolution<T>::quickSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end)
{
    if (begin >= end)
    {
        return;
    }
    T pivot = sortArray[end];
    uint32_t leftIndex = begin;
    uint32_t rightIndex = end - 1;

    while (leftIndex < rightIndex)
    {
        while ((leftIndex < rightIndex) && (sortArray[leftIndex] < pivot))
        {
            ++leftIndex;
        }
        while ((leftIndex < rightIndex) && (sortArray[rightIndex] >= pivot))
        {
            --rightIndex;
        }
        std::swap(sortArray[leftIndex], sortArray[rightIndex]);
    }
    if (sortArray[leftIndex] >= sortArray[end])
    {
        std::swap(sortArray[leftIndex], sortArray[end]);
    }
    else
    {
        ++leftIndex;
    }

    if (leftIndex)
    {
        quickSortRecursive(sortArray, begin, leftIndex - 1);
    }
    quickSortRecursive(sortArray, leftIndex + 1, end);
}

template <class T>
std::vector<T> SortSolution<T>::heapMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (int i = length / 2 + 1; i >= 0; --i)
    {
        buildMaxHeap(sortArray, i, length - 1);
    }
    for (int i = length - 1; i > 0; --i)
    {
        std::swap(sortArray[0], sortArray[i]);
        buildMaxHeap(sortArray, 0, i - 1);
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Heap");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
void SortSolution<T>::buildMaxHeap(T* const sortArray, const uint32_t begin, const uint32_t end)
{
    uint32_t parent = begin;
    uint32_t child = parent * 2 + 1;
    while (child <= end)
    {
        if (((child + 1) <= end) && (sortArray[child] < sortArray[child + 1]))
        {
            ++child;
        }
        if (sortArray[parent] > sortArray[child])
        {
            return;
        }

        std::swap(sortArray[parent], sortArray[child]);
        parent = child;
        child = parent * 2 + 1;
    }
}

template <class T>
std::vector<T> SortSolution<T>::countingMethod(T* const array, const uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
#ifdef __RUNTIME_PRINTING
        COMMON_PRINT("\r\n*Counting  method:\r\nThe array type isn't integral.\n");
#endif
        return std::vector<T>();
    }

    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    for (uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
    }

    const T countingLen = max - min + 1;
    assert(countingLen > 0);
    std::unique_ptr<T[]> counting = std::make_unique<T[]>(countingLen);
    for (uint32_t i = 0; i < length; ++i)
    {
        ++counting[sortArray[i] - min];
    }
    uint32_t index = 0;
    for (T j = 0; j < countingLen; ++j)
    {
        while (counting[j])
        {
            sortArray[index++] = j + min;
            --counting[j];
        }
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Counting");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> SortSolution<T>::bucketMethod(T* const array, const uint32_t length)
{
    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    for (uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
    }

    const uint32_t bucketNum = length;
    const double intervalSpan = static_cast<double>(max - min) / static_cast<double>(bucketNum - 1);
    std::vector<std::vector<T>> aggregation(bucketNum, std::vector<T>{});
    for (uint32_t i = 0; i < length; ++i)
    {
        // min+(max-min)/(bucketNum-1)*(buckIndex-1)<=sortArray[i]
        const uint32_t aggIndex = std::floor(static_cast<double>(sortArray[i] - min) / intervalSpan + 1) - 1;
        aggregation.at(aggIndex).emplace_back(sortArray[i]);
    }

    uint32_t index = 0;
    for (auto& bucketUpdate : aggregation)
    {
        std::sort(bucketUpdate.begin(), bucketUpdate.end());
        for (const auto bucketElement : bucketUpdate)
        {
            sortArray[index++] = bucketElement;
        }
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Bucket");
    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> SortSolution<T>::radixMethod(T* const array, const uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
#ifdef __RUNTIME_PRINTING
        COMMON_PRINT("\r\n*Radix     method:\r\nThe array type isn't integral.\n");
#endif
        return std::vector<T>();
    }

    SORT_RUNTIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    bool positive = false, negative = false;
    for (uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
        (sortArray[i] > 0) ? positive = true : ((sortArray[i] < 0) ? negative = true : sortArray[i]);
    }
    T absMax = std::max(max, -min);
    uint32_t digitMax = 0;
    const uint32_t base = 10;
    while (absMax)
    {
        absMax /= base;
        ++digitMax;
    }

    // -9 ... -1 0 1 ... 9
    constexpr uint32_t naturalNumberBucket = 10, negativeIntegerBucket = 9;
    const uint32_t bucketNum =
        (positive ^ negative) ? naturalNumberBucket : (naturalNumberBucket + negativeIntegerBucket);
    assert(bucketNum > 0);
    std::unique_ptr<T[]> countingOld = std::make_unique<T[]>(bucketNum), countingNew = std::make_unique<T[]>(bucketNum);
    std::queue<T> bucket;
    std::vector<std::queue<T>> aggregation(bucketNum, bucket);
    const uint32_t offset = (!negative) ? 0 : negativeIntegerBucket;
    for (uint32_t i = 0; i < length; ++i)
    {
        const int sign = (sortArray[i] > 0) ? 1 : -1;
        const uint32_t aggIndex = std::abs(sortArray[i]) / 1 % base * sign + offset;
        aggregation[aggIndex].push(sortArray[i]);
        ++countingNew[aggIndex];
    }

    constexpr uint32_t decimal = 10;
    for (uint32_t i = 1, pow = decimal; i < digitMax; ++i, pow *= base)
    {
        std::memcpy(countingOld.get(), countingNew.get(), bucketNum * sizeof(T));
        std::memset(countingNew.get(), 0, bucketNum * sizeof(T));
        for (auto bucketIter = aggregation.begin(); aggregation.end() != bucketIter; ++bucketIter)
        {
            if (!bucketIter->size())
            {
                continue;
            }
            const uint32_t countingIndex = bucketIter - aggregation.begin();
            while (countingOld[countingIndex])
            {
                auto bucketElement = bucketIter->front();
                const int sign = (bucketElement > 0) ? 1 : -1;
                const uint32_t aggIndex = std::abs(bucketElement) / pow % base * sign + offset;
                aggregation[aggIndex].push(bucketElement);
                ++countingNew[aggIndex];
                bucketIter->pop();
                --countingOld[countingIndex];
            }
        }
    }

    uint32_t index = 0;
    for (auto& bucketInfo : aggregation)
    {
        while (bucketInfo.size())
        {
            sortArray[index++] = bucketInfo.front();
            bucketInfo.pop();
        }
    }

    SORT_RUNTIME_END;
    SORT_PRINT_RESULT_CONTENT("Radix");
    return std::vector<T>(sortArray, sortArray + length);
}
} // namespace algorithm::sort
