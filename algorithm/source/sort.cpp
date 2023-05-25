//! @file sort.cpp
//! @author ryftchen
//! @brief The definitions (sort) in the algorithm module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "sort.hpp"
// #define NDEBUG
#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>
#include <queue>

namespace algorithm::sort
{
template class Sort<int>;
template std::vector<int> Sort<int>::bubble(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::selection(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::insertion(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::shell(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::merge(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::quick(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::heap(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::counting(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::bucket(int* const array, const std::uint32_t length);
template std::vector<int> Sort<int>::radix(int* const array, const std::uint32_t length);

template <class T>
std::vector<T> Sort<T>::bubble(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::selection(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::insertion(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::shell(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::merge(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::quick(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::heap(T* const array, const std::uint32_t length)
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
std::vector<T> Sort<T>::counting(T* const array, const std::uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
        throw std::runtime_error("The array type isn't integral.");
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
std::vector<T> Sort<T>::bucket(T* const array, const std::uint32_t length)
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
    std::vector<std::vector<T>> aggregation(bucketNum, std::vector<T>{});
    for (std::uint32_t i = 0; i < length; ++i)
    {
        // min+(max-min)/(bucketNum-1)*(bucketIndex-1)<=array[i]
        const std::uint32_t bucketIndex = std::floor(static_cast<double>(sortArray[i] - min) / intervalSpan + 1) - 1;
        aggregation.at(bucketIndex).emplace_back(sortArray[i]);
    }

    std::uint32_t index = 0;
    for (auto& bucketUpdate : aggregation)
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
std::vector<T> Sort<T>::radix(T* const array, const std::uint32_t length)
{
    if (!std::is_integral_v<T>)
    {
        throw std::runtime_error("The array type isn't integral.");
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
    std::vector<std::queue<T>> aggregation(bucketNum, bucket);
    const std::uint32_t offset = (!negative) ? 0 : negativeIntegerBucket;
    for (std::uint32_t i = 0; i < length; ++i)
    {
        const int sign = (sortArray[i] > 0) ? 1 : -1;
        const std::uint32_t bucketIndex = std::abs(sortArray[i]) / 1 % base * sign + offset;
        aggregation[bucketIndex].push(sortArray[i]);
        ++countingNew[bucketIndex];
    }

    constexpr std::uint32_t decimal = 10;
    for (std::uint32_t i = 1, pow = decimal; i < digitMax; ++i, pow *= base)
    {
        std::memcpy(countingOld.get(), countingNew.get(), bucketNum * sizeof(T));
        std::memset(countingNew.get(), 0, bucketNum * sizeof(T));
        for (auto bucketIter = aggregation.begin(); aggregation.end() != bucketIter; ++bucketIter)
        {
            if (!bucketIter->size())
            {
                continue;
            }
            const std::uint32_t countingIndex = bucketIter - aggregation.begin();
            while (countingOld[countingIndex])
            {
                auto bucketElement = bucketIter->front();
                const int sign = (bucketElement > 0) ? 1 : -1;
                const std::uint32_t bucketIndex = std::abs(bucketElement) / pow % base * sign + offset;
                aggregation[bucketIndex].push(bucketElement);
                ++countingNew[bucketIndex];
                bucketIter->pop();
                --countingOld[countingIndex];
            }
        }
    }

    std::uint32_t index = 0;
    for (auto& bucketInfo : aggregation)
    {
        while (bucketInfo.size())
        {
            sortArray[index++] = bucketInfo.front();
            bucketInfo.pop();
        }
    }

    return std::vector<T>(sortArray, sortArray + length);
}
} // namespace algorithm::sort
