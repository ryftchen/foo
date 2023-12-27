//! @file sort.tpp
//! @author ryftchen
//! @brief The definitions (sort) in the algorithm module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include "sort.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>
#include <queue>

namespace algorithm::sort
{
template <class T>
std::vector<T> Sort<T>::bubble(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (std::uint32_t i = 0; i < length - 1; ++i)
    {
        for (std::uint32_t j = 0; j < length - 1 - i; ++j)
        {
            if (sortArray[j] > sortArray[j + 1])
            {
                std::swap(sortArray[j], sortArray[j + 1]);
            }
        }
    }

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> Sort<T>::selection(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (std::uint32_t i = 0; i < length - 1; ++i)
    {
        std::uint32_t min = i;
        for (std::uint32_t j = i + 1; j < length; ++j)
        {
            if (sortArray[j] < sortArray[min])
            {
                min = j;
            }
        }
        std::swap(sortArray[i], sortArray[min]);
    }

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> Sort<T>::insertion(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    for (std::uint32_t i = 1; i < length; ++i)
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

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> Sort<T>::shell(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    std::uint32_t gap = length / 2;
    while (gap >= 1)
    {
        for (std::uint32_t i = gap; i < length; ++i)
        {
            for (std::uint32_t j = i; (j >= gap) && (sortArray[j] < sortArray[j - gap]); j -= gap)
            {
                std::swap(sortArray[j], sortArray[j - gap]);
            }
        }
        gap /= 2;
    }

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> Sort<T>::merge(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    mergeSortRecursive(sortArray, 0, length - 1);

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
void Sort<T>::mergeSortRecursive(T* const sortArray, const std::uint32_t begin, const std::uint32_t end)
{
    if (begin >= end)
    {
        return;
    }
    const std::uint32_t mid = (begin + end) / 2;
    mergeSortRecursive(sortArray, begin, mid);
    mergeSortRecursive(sortArray, mid + 1, end);

    std::uint32_t leftIndex = 0, rightIndex = 0;
    std::vector<T> leftSubArray(sortArray + begin, sortArray + mid + 1);
    std::vector<T> rightSubArray(sortArray + mid + 1, sortArray + end + 1);
    leftSubArray.insert(leftSubArray.cend(), std::numeric_limits<T>::max());
    rightSubArray.insert(rightSubArray.cend(), std::numeric_limits<T>::max());
    for (std::uint32_t i = begin; i <= end; ++i)
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
std::vector<T> Sort<T>::quick(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    quickSortRecursive(sortArray, 0, length - 1);

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
void Sort<T>::quickSortRecursive(T* const sortArray, const std::uint32_t begin, const std::uint32_t end)
{
    if (begin >= end)
    {
        return;
    }
    T pivot = sortArray[end];
    std::uint32_t leftIndex = begin, rightIndex = end - 1;

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
std::vector<T> Sort<T>::heap(const T* const array, const std::uint32_t length)
{
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

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
void Sort<T>::buildMaxHeap(T* const sortArray, const std::uint32_t begin, const std::uint32_t end)
{
    std::uint32_t parent = begin, child = parent * 2 + 1;
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
std::vector<T> Sort<T>::counting(const T* const array, const std::uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
        throw std::runtime_error("The array type is not integral.");
    }

    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
    }

    const T countingLen = max - min + 1;
    assert(countingLen > 0);
    std::unique_ptr<T[]> counting = std::make_unique<T[]>(countingLen);
    for (std::uint32_t i = 0; i < length; ++i)
    {
        ++counting[sortArray[i] - min];
    }
    std::uint32_t index = 0;
    for (T j = 0; j < countingLen; ++j)
    {
        while (counting[j])
        {
            sortArray[index++] = j + min;
            --counting[j];
        }
    }

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> Sort<T>::bucket(const T* const array, const std::uint32_t length)
{
    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min(), min = std::numeric_limits<T>::max();
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
    }

    const std::uint32_t bucketNum = length;
    const double intervalSpan = static_cast<double>(max - min) / static_cast<double>(bucketNum - 1);
    std::vector<std::vector<T>> container(bucketNum, std::vector<T>{});
    for (std::uint32_t i = 0; i < length; ++i)
    {
        // min+(max-min)/(bucketNum-1)*(bucketIndex-1)<=array[i]
        const std::uint32_t bucketIndex = std::floor(static_cast<double>(sortArray[i] - min) / intervalSpan + 1) - 1;
        container.at(bucketIndex).emplace_back(sortArray[i]);
    }

    std::uint32_t index = 0;
    for (auto& bucketUpdate : container)
    {
        std::sort(bucketUpdate.begin(), bucketUpdate.end());
        for (const auto bucketElement : bucketUpdate)
        {
            sortArray[index++] = bucketElement;
        }
    }

    return std::vector<T>(sortArray, sortArray + length);
}

template <class T>
std::vector<T> Sort<T>::radix(const T* const array, const std::uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
        throw std::runtime_error("The array type is not integral.");
    }

    T sortArray[length];
    sortArray[0] = '\0';
    std::memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min(), min = std::numeric_limits<T>::max();
    bool positive = false, negative = false;
    for (std::uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
        (sortArray[i] > 0) ? positive = true : ((sortArray[i] < 0) ? negative = true : sortArray[i]);
    }
    T absMax = std::max(max, -min);
    std::uint32_t digitMax = 0;
    constexpr std::uint32_t base = 10;
    while (absMax)
    {
        absMax /= base;
        ++digitMax;
    }

    constexpr std::uint32_t naturalNumberBucket = 10, negativeIntegerBucket = 9;
    const std::uint32_t bucketNum =
        (positive ^ negative) ? naturalNumberBucket : (naturalNumberBucket + negativeIntegerBucket);
    assert(bucketNum > 0);
    std::unique_ptr<T[]> countingOld = std::make_unique<T[]>(bucketNum), countingNew = std::make_unique<T[]>(bucketNum);
    std::queue<T> bucket;
    std::vector<std::queue<T>> container(bucketNum, bucket);
    const std::uint32_t offset = (!negative) ? 0 : negativeIntegerBucket;
    for (std::uint32_t i = 0; i < length; ++i)
    {
        const int sign = (sortArray[i] > 0) ? 1 : -1;
        const std::uint32_t bucketIndex = std::abs(sortArray[i]) / 1 % base * sign + offset;
        container.at(bucketIndex).push(sortArray[i]);
        ++countingNew[bucketIndex];
    }

    constexpr std::uint32_t decimal = 10;
    for (std::uint32_t i = 1, pow = decimal; i < digitMax; ++i, pow *= base)
    {
        std::memcpy(countingOld.get(), countingNew.get(), bucketNum * sizeof(T));
        std::memset(countingNew.get(), 0, bucketNum * sizeof(T));
        for (auto bucketIter = container.begin(); container.end() != bucketIter; ++bucketIter)
        {
            if (!bucketIter->size())
            {
                continue;
            }
            const std::uint32_t countingIndex = bucketIter - container.begin();
            while (countingOld[countingIndex])
            {
                const T bucketElement = bucketIter->front();
                const int sign = (bucketElement > 0) ? 1 : -1;
                const std::uint32_t bucketIndex = std::abs(bucketElement) / pow % base * sign + offset;
                container.at(bucketIndex).push(bucketElement);
                ++countingNew[bucketIndex];
                bucketIter->pop();
                --countingOld[countingIndex];
            }
        }
    }

    std::uint32_t index = 0;
    for (auto& bucketInfo : container)
    {
        while (!bucketInfo.empty())
        {
            sortArray[index++] = bucketInfo.front();
            bucketInfo.pop();
        }
    }

    return std::vector<T>(sortArray, sortArray + length);
}
} // namespace algorithm::sort
