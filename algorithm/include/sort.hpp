//! @file sort.hpp
//! @author ryftchen
//! @brief The declarations (sort) in the algorithm module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <queue>
#include <stdexcept>

//! @brief The algorithm module.
namespace algorithm // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Sort-related functions in the algorithm module.
namespace sort
{
extern const char* version() noexcept;

//! @brief Sort methods.
//! @tparam T - type of sort methods
template <class T>
class Sort
{
public:
    //! @brief Destroy the Sort object.
    virtual ~Sort() = default;

    //! @brief Bubble.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> bubble(const T* const array, const std::uint32_t length);
    //! @brief Selection.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> selection(const T* const array, const std::uint32_t length);
    //! @brief Insertion.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> insertion(const T* const array, const std::uint32_t length);
    //! @brief Shell.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> shell(const T* const array, const std::uint32_t length);
    //! @brief Merge.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> merge(const T* const array, const std::uint32_t length);
    //! @brief Quick.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> quick(const T* const array, const std::uint32_t length);
    //! @brief Heap.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> heap(const T* const array, const std::uint32_t length);
    //! @brief Counting.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> counting(const T* const array, const std::uint32_t length);
    //! @brief Bucket.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> bucket(const T* const array, const std::uint32_t length);
    //! @brief Radix.
    //! @param array - array to be sorted
    //! @param length - length of array
    //! @return array after sort
    static std::vector<T> radix(const T* const array, const std::uint32_t length);

private:
    //! @brief Recursive for the merge method.
    //! @param sorting - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void mergeSortRecursive(T* const sorting, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Recursive for the quick method.
    //! @param sorting - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void quickSortRecursive(T* const sorting, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Build maximum heap for the heap method.
    //! @param sorting - array to be sorted
    //! @param begin - index of beginning
    //! @param end - index of ending
    static void buildMaxHeap(T* const sorting, const std::uint32_t begin, const std::uint32_t end);
    //! @brief Least significant digit (LSD) for the radix method.
    //! @param sorting - array to be sorted
    //! @param length - length of array
    //! @param maxDigit - maximum digit
    //! @param bucketSize - bucket size
    //! @param indexOffset - bucket index offset
    static void leastSignificantDigit(
        T* const sorting,
        const std::uint32_t length,
        const std::uint32_t maxDigit,
        const std::uint32_t bucketSize,
        const std::uint32_t indexOffset);
};

template <class T>
std::vector<T> Sort<T>::bubble(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

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

template <class T>
std::vector<T> Sort<T>::selection(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

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

template <class T>
std::vector<T> Sort<T>::insertion(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

    for (std::uint32_t i = 1; i < length; ++i)
    {
        std::int64_t n = i - 1;
        T temp = sorting[i];
        while ((n >= 0) && (sorting[n] > temp))
        {
            sorting[n + 1] = sorting[n];
            --n;
        }
        sorting[n + 1] = temp;
    }

    return sorting;
}

template <class T>
std::vector<T> Sort<T>::shell(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

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

template <class T>
std::vector<T> Sort<T>::merge(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

    mergeSortRecursive(sorting.data(), 0, length - 1);

    return sorting;
}

template <class T>
void Sort<T>::mergeSortRecursive(T* const sorting, const std::uint32_t begin, const std::uint32_t end)
{
    if (begin >= end)
    {
        return;
    }

    const std::uint32_t mid = (begin + end) / 2;
    mergeSortRecursive(sorting, begin, mid);
    mergeSortRecursive(sorting, mid + 1, end);

    std::uint32_t leftIdx = 0, rightIdx = 0;
    std::vector<T> leftSub(sorting + begin, sorting + mid + 1), rightSub(sorting + mid + 1, sorting + end + 1);
    leftSub.insert(leftSub.cend(), std::numeric_limits<T>::max());
    rightSub.insert(rightSub.cend(), std::numeric_limits<T>::max());
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

template <class T>
std::vector<T> Sort<T>::quick(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

    quickSortRecursive(sorting.data(), 0, length - 1);

    return sorting;
}

template <class T>
void Sort<T>::quickSortRecursive(T* const sorting, const std::uint32_t begin, const std::uint32_t end)
{
    if (begin >= end)
    {
        return;
    }

    T pivot = sorting[end];
    std::uint32_t leftIdx = begin, rightIdx = end - 1;
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

    if (leftIdx)
    {
        quickSortRecursive(sorting, begin, leftIdx - 1);
    }
    quickSortRecursive(sorting, leftIdx + 1, end);
}

template <class T>
std::vector<T> Sort<T>::heap(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

    for (std::int64_t i = length / 2 + 1; i >= 0; --i)
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

template <class T>
void Sort<T>::buildMaxHeap(T* const sorting, const std::uint32_t begin, const std::uint32_t end)
{
    std::uint32_t parent = begin, child = parent * 2 + 1;
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

template <class T>
std::vector<T> Sort<T>::counting(const T* const array, const std::uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
        throw std::runtime_error("The array type is not integral.");
    }

    std::vector<T> sorting(array, array + length);

    T max = std::numeric_limits<T>::min(), min = std::numeric_limits<T>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sorting[i], max);
        min = std::min(sorting[i], min);
    }

    const T countingLen = max - min + 1;
    std::vector<T> counting(countingLen);
    for (std::uint32_t i = 0; i < length; ++i)
    {
        ++counting[sorting[i] - min];
    }
    std::uint32_t index = 0;
    for (T j = 0; j < countingLen; ++j)
    {
        while (counting[j])
        {
            sorting[index++] = j + min;
            --counting[j];
        }
    }

    return sorting;
}

template <class T>
std::vector<T> Sort<T>::bucket(const T* const array, const std::uint32_t length)
{
    std::vector<T> sorting(array, array + length);

    T max = std::numeric_limits<T>::min(), min = std::numeric_limits<T>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sorting[i], max);
        min = std::min(sorting[i], min);
    }

    const std::uint32_t bucketSize = length;
    const double intervalSpan = static_cast<double>(max - min) / static_cast<double>(bucketSize - 1);
    std::vector<std::vector<T>> container(bucketSize, std::vector<T>{});
    for (std::uint32_t i = 0; i < length; ++i)
    {
        const std::uint32_t bucketIdx = std::floor(static_cast<double>(sorting[i] - min) / intervalSpan + 1) - 1;
        container[bucketIdx].emplace_back(sorting[i]);
    }

    for (std::uint32_t index = 0; auto& bucketUpd : container)
    {
        std::sort(bucketUpd.begin(), bucketUpd.end());
        for (const auto bucketElem : bucketUpd)
        {
            sorting[index++] = bucketElem;
        }
    }

    return sorting;
}

template <class T>
std::vector<T> Sort<T>::radix(const T* const array, const std::uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
        throw std::runtime_error("The array type is not integral.");
    }

    std::vector<T> sorting(array, array + length);

    T max = std::numeric_limits<T>::min(), min = std::numeric_limits<T>::max();
    bool positive = false, negative = false;
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sorting[i], max);
        min = std::min(sorting[i], min);
        (sorting[i] > 0) ? positive = true : ((sorting[i] < 0) ? negative = true : sorting[i]);
    }
    T absMax = std::max(max, -min);
    constexpr std::uint32_t base = 10;
    std::uint32_t maxDigit = 0;
    while (absMax)
    {
        absMax /= base;
        ++maxDigit;
    }

    constexpr std::uint32_t naturalNumberBucket = 10, negativeIntegerBucket = 9;
    const std::uint32_t bucketSize =
                            (positive ^ negative) ? naturalNumberBucket : (naturalNumberBucket + negativeIntegerBucket),
                        indexOffset = (!negative) ? 0 : negativeIntegerBucket;
    leastSignificantDigit(sorting.data(), length, maxDigit, bucketSize, indexOffset);

    return sorting;
}

template <class T>
void Sort<T>::leastSignificantDigit(
    T* const sorting,
    const std::uint32_t length,
    const std::uint32_t maxDigit,
    const std::uint32_t bucketSize,
    const std::uint32_t indexOffset)
{
    constexpr std::uint32_t base = 10;
    std::vector<T> countingOld(bucketSize, 0), countingNew(bucketSize, 0);
    std::vector<std::queue<T>> container(bucketSize, std::queue<T>{});
    for (std::uint32_t i = 0; i < length; ++i)
    {
        const std::int8_t sign = (sorting[i] > 0) ? 1 : -1;
        const std::uint32_t bucketIdx = std::abs(sorting[i]) / 1 % base * sign + indexOffset;
        container[bucketIdx].push(sorting[i]);
        ++countingNew[bucketIdx];
    }

    constexpr std::uint32_t decimal = 10;
    for (std::uint32_t i = 1, pow = decimal; i < maxDigit; ++i, pow *= base)
    {
        countingOld = countingNew;
        std::fill(countingNew.begin(), countingNew.end(), 0);
        for (auto bucketIter = container.begin(); container.end() != bucketIter; ++bucketIter)
        {
            if (bucketIter->size() == 0)
            {
                continue;
            }

            const std::uint32_t countingIdx = bucketIter - container.begin();
            while (countingOld[countingIdx])
            {
                const T bucketElem = bucketIter->front();
                const std::int8_t sign = (bucketElem > 0) ? 1 : -1;
                const std::uint32_t bucketIdx = std::abs(bucketElem) / pow % base * sign + indexOffset;
                container[bucketIdx].push(bucketElem);
                ++countingNew[bucketIdx];
                bucketIter->pop();
                --countingOld[countingIdx];
            }
        }
    }

    for (std::uint32_t index = 0; auto& bucketInfo : container)
    {
        while (!bucketInfo.empty())
        {
            sorting[index++] = bucketInfo.front();
            bucketInfo.pop();
        }
    }
}
} // namespace sort
} // namespace algorithm
