//! @file sort.hpp
//! @author ryftchen
//! @brief The declarations (sort) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <queue>

//! @brief The algorithm module.
namespace algorithm // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Sort-related functions in the algorithm module.
namespace sort
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "ALGO_SORT";
}
extern const char* version() noexcept;

//! @brief Sort methods.
//! @tparam Elem - type of element
template <typename Elem>
class Sort
{
public:
    //! @brief Bubble.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> bubble(const Elem* const array, const std::uint32_t length);
    //! @brief Selection.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> selection(const Elem* const array, const std::uint32_t length);
    //! @brief Insertion.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> insertion(const Elem* const array, const std::uint32_t length);
    //! @brief Shell.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> shell(const Elem* const array, const std::uint32_t length);
    //! @brief Merge.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> merge(const Elem* const array, const std::uint32_t length);
    //! @brief Quick.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> quick(const Elem* const array, const std::uint32_t length);
    //! @brief Heap.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> heap(const Elem* const array, const std::uint32_t length);
    //! @brief Counting.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> counting(const Elem* const array, const std::uint32_t length);
    //! @brief Bucket.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> bucket(const Elem* const array, const std::uint32_t length);
    //! @brief Radix.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<Elem> radix(const Elem* const array, const std::uint32_t length);

private:
    //! @brief Recursion for the merge method.
    //! @param sorting - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void mergeSortRecursive(Elem* const sorting, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Recursion for the quick method.
    //! @param sorting - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void quickSortRecursive(Elem* const sorting, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Build maximum heap for the heap method.
    //! @param sorting - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void buildMaxHeap(Elem* const sorting, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Least significant digit (LSD) for the radix method.
    //! @param sorting - array to be sorted
    //! @param length - length of array
    //! @param maxDigit - maximum digit
    //! @param bucketSize - bucket size
    //! @param indexOffset - bucket index offset
    static void leastSignificantDigit(
        Elem* const sorting,
        const std::uint32_t length,
        const std::uint32_t maxDigit,
        const std::uint32_t bucketSize,
        const std::uint32_t indexOffset);
};

template <typename Elem>
std::vector<Elem> Sort<Elem>::bubble(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    for (std::uint32_t i = 0; i < length - 1; ++i)
    {
        for (std::uint32_t j = 0; j < length - 1 - i; ++j)
        {
            if (sorting[j] > sorting[j + 1])
            {
                std::swap(sorting[j], sorting[j + 1]);
            }
        }
    }
    return sorting;
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::selection(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    for (std::uint32_t i = 0; i < length - 1; ++i)
    {
        std::uint32_t min = i;
        for (std::uint32_t j = i + 1; j < length; ++j)
        {
            if (sorting[j] < sorting[min])
            {
                min = j;
            }
        }
        std::swap(sorting[i], sorting[min]);
    }
    return sorting;
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::insertion(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    for (std::uint32_t i = 1; i < length; ++i)
    {
        std::int64_t n = i - 1;
        Elem temp = sorting[i];
        while ((n >= 0) && (sorting[n] > temp))
        {
            sorting[n + 1] = sorting[n];
            --n;
        }
        sorting[n + 1] = temp;
    }
    return sorting;
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::shell(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    std::uint32_t gap = length / 2;
    while (gap >= 1)
    {
        for (std::uint32_t i = gap; i < length; ++i)
        {
            for (std::uint32_t j = i; (j >= gap) && (sorting[j] < sorting[j - gap]); j -= gap)
            {
                std::swap(sorting[j], sorting[j - gap]);
            }
        }
        gap /= 2;
    }
    return sorting;
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::merge(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    mergeSortRecursive(sorting.data(), 0, length - 1);
    return sorting;
}

template <typename Elem>
void Sort<Elem>::mergeSortRecursive(Elem* const sorting, const std::uint32_t begin, const std::uint32_t end)
{
    if (begin >= end)
    {
        return;
    }

    const std::uint32_t mid = std::midpoint(begin, end);
    mergeSortRecursive(sorting, begin, mid);
    mergeSortRecursive(sorting, mid + 1, end);

    std::uint32_t leftIdx = 0;
    std::uint32_t rightIdx = 0;
    std::vector<Elem> leftSub(sorting + begin, sorting + mid + 1);
    std::vector<Elem> rightSub(sorting + mid + 1, sorting + end + 1);
    leftSub.emplace(leftSub.cend(), std::numeric_limits<Elem>::max());
    rightSub.emplace(rightSub.cend(), std::numeric_limits<Elem>::max());
    for (std::uint32_t i = begin; i <= end; ++i)
    {
        if (leftSub[leftIdx] < rightSub[rightIdx])
        {
            sorting[i] = leftSub[leftIdx];
            ++leftIdx;
        }
        else
        {
            sorting[i] = rightSub[rightIdx];
            ++rightIdx;
        }
    }
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::quick(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    quickSortRecursive(sorting.data(), 0, length - 1);
    return sorting;
}

template <typename Elem>
void Sort<Elem>::quickSortRecursive(Elem* const sorting, const std::uint32_t begin, const std::uint32_t end)
{
    if (begin >= end)
    {
        return;
    }

    Elem pivot = sorting[end];
    std::uint32_t leftIdx = begin;
    std::uint32_t rightIdx = end - 1;
    while (leftIdx < rightIdx)
    {
        while ((leftIdx < rightIdx) && (sorting[leftIdx] < pivot))
        {
            ++leftIdx;
        }
        while ((leftIdx < rightIdx) && (sorting[rightIdx] >= pivot))
        {
            --rightIdx;
        }
        std::swap(sorting[leftIdx], sorting[rightIdx]);
    }
    if (sorting[leftIdx] >= sorting[end])
    {
        std::swap(sorting[leftIdx], sorting[end]);
    }
    else
    {
        ++leftIdx;
    }

    if (leftIdx != 0)
    {
        quickSortRecursive(sorting, begin, leftIdx - 1);
    }
    quickSortRecursive(sorting, leftIdx + 1, end);
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::heap(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    for (std::int64_t i = (length / 2) + 1; i >= 0; --i)
    {
        buildMaxHeap(sorting.data(), i, length - 1);
    }
    for (std::int64_t i = length - 1; i > 0; --i)
    {
        std::swap(sorting[0], sorting[i]);
        buildMaxHeap(sorting.data(), 0, i - 1);
    }
    return sorting;
}

template <typename Elem>
void Sort<Elem>::buildMaxHeap(Elem* const sorting, const std::uint32_t begin, const std::uint32_t end)
{
    std::uint32_t parent = begin;
    std::uint32_t child = (parent * 2) + 1;
    while (child <= end)
    {
        if (((child + 1) <= end) && (sorting[child] < sorting[child + 1]))
        {
            ++child;
        }
        if (sorting[parent] > sorting[child])
        {
            return;
        }

        std::swap(sorting[parent], sorting[child]);
        parent = child;
        child = parent * 2 + 1;
    }
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::counting(const Elem* const array, const std::uint32_t length)
{
    if constexpr (!std::is_integral_v<Elem>)
    {
        return {};
    }
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    Elem max = std::numeric_limits<Elem>::min();
    Elem min = std::numeric_limits<Elem>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sorting[i], max);
        min = std::min(sorting[i], min);
    }

    const Elem countSize = max - min + 1;
    std::vector<Elem> count(countSize);
    for (std::uint32_t i = 0; i < length; ++i)
    {
        ++count[sorting[i] - min];
    }
    std::uint32_t index = 0;
    for (Elem j = 0; j < countSize; ++j)
    {
        while (count[j])
        {
            sorting[index++] = j + min;
            --count[j];
        }
    }
    return sorting;
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::bucket(const Elem* const array, const std::uint32_t length)
{
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    Elem max = std::numeric_limits<Elem>::min();
    Elem min = std::numeric_limits<Elem>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sorting[i], max);
        min = std::min(sorting[i], min);
    }

    const std::uint32_t bucketSize = length;
    const double intervalSpan = static_cast<double>(max - min) / static_cast<double>(bucketSize - 1);
    std::vector<std::vector<Elem>> bucket(bucketSize, std::vector<Elem>{});
    for (std::uint32_t i = 0; i < length; ++i)
    {
        const std::uint32_t bucketIdx = std::floor((static_cast<double>(sorting[i] - min) / intervalSpan) + 1) - 1;
        bucket[bucketIdx].emplace_back(sorting[i]);
    }

    for (std::uint32_t index = 0; auto& each : bucket)
    {
        std::sort(each.begin(), each.end());
        for (const auto elem : each)
        {
            sorting[index++] = elem;
        }
    }
    return sorting;
}

template <typename Elem>
std::vector<Elem> Sort<Elem>::radix(const Elem* const array, const std::uint32_t length)
{
    if constexpr (!std::is_integral_v<Elem>)
    {
        return {};
    }
    if (!array || (length == 0))
    {
        return {};
    }

    std::vector<Elem> sorting(array, array + length);
    Elem max = std::numeric_limits<Elem>::min();
    Elem min = std::numeric_limits<Elem>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sorting[i], max);
        min = std::min(sorting[i], min);
    }

    Elem absMax = std::max(max, -min);
    constexpr std::uint32_t base = 10;
    std::uint32_t maxDigit = 0;
    while (absMax)
    {
        absMax /= base;
        ++maxDigit;
    }

    constexpr std::uint32_t naturalNumberRdx = 10;
    constexpr std::uint32_t negativeIntegerRdx = 9;
    const bool hasPositive = (max > 0);
    const bool hasNegative = (min < 0);
    const std::uint32_t bucketSize =
        (hasPositive ^ hasNegative) ? naturalNumberRdx : (naturalNumberRdx + negativeIntegerRdx);
    const std::uint32_t indexOffset = (!hasNegative) ? 0 : negativeIntegerRdx;
    leastSignificantDigit(sorting.data(), length, maxDigit, bucketSize, indexOffset);
    return sorting;
}

template <typename Elem>
void Sort<Elem>::leastSignificantDigit(
    Elem* const sorting,
    const std::uint32_t length,
    const std::uint32_t maxDigit,
    const std::uint32_t bucketSize,
    const std::uint32_t indexOffset)
{
    constexpr std::uint32_t base = 10;
    std::vector<Elem> countOld(bucketSize, 0);
    std::vector<Elem> countNew(bucketSize, 0);
    std::vector<std::queue<Elem>> bucket(bucketSize, std::queue<Elem>{});
    for (std::uint32_t i = 0; i < length; ++i)
    {
        const std::int8_t sign = (sorting[i] > 0) ? 1 : -1;
        const std::uint32_t bucketIdx = (std::abs(sorting[i]) / 1 % base * sign) + indexOffset;
        bucket[bucketIdx].push(sorting[i]);
        ++countNew[bucketIdx];
    }

    constexpr std::uint32_t decimal = 10;
    for (std::uint32_t i = 1, pow = decimal; i < maxDigit; ++i, pow *= base)
    {
        countOld = countNew;
        std::fill(countNew.begin(), countNew.end(), 0);
        for (auto bucketIter = bucket.begin(); bucketIter != bucket.end(); ++bucketIter)
        {
            if (bucketIter->empty())
            {
                continue;
            }

            const std::uint32_t countIdx = bucketIter - bucket.cbegin();
            while (countOld[countIdx])
            {
                const Elem elem = bucketIter->front();
                const std::int8_t sign = (elem > 0) ? 1 : -1;
                const std::uint32_t bucketIdx = (std::abs(elem) / pow % base * sign) + indexOffset;
                bucket[bucketIdx].push(elem);
                ++countNew[bucketIdx];

                bucketIter->pop();
                --countOld[countIdx];
            }
        }
    }

    for (std::uint32_t index = 0; auto& each : bucket)
    {
        while (!each.empty())
        {
            sorting[index++] = each.front();
            each.pop();
        }
    }
}
} // namespace sort
} // namespace algorithm
