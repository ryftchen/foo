#include "sort.hpp"
#include <algorithm>
#include <queue>
#include <vector>

template class Sort<int>;
template void Sort<int>::bubbleSort(int* const, const uint32_t) const;
template void Sort<int>::selectionSort(int* const, const uint32_t) const;
template void Sort<int>::insertionSort(int* const, const uint32_t) const;
template void Sort<int>::shellSort(int* const, const uint32_t) const;
template void Sort<int>::mergeSort(int* const, const uint32_t) const;
template void Sort<int>::quickSort(int* const, const uint32_t) const;
template void Sort<int>::heapSort(int* const, const uint32_t) const;
template void Sort<int>::countingSort(int* const, const uint32_t) const;
template void Sort<int>::bucketSort(int* const, const uint32_t) const;
template void Sort<int>::radixSort(int* const, const uint32_t) const;

// Bubble method
template <class T>
void Sort<T>::bubbleSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

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

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_BUBBLE, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}

// Selection method
template <class T>
void Sort<T>::selectionSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

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

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_SELECTION, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}

// Insertion method
template <class T>
void Sort<T>::insertionSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

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

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_INSERTION, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}

// Shell method
template <class T>
void Sort<T>::shellSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

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

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_SHELL, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}

// Merge method
template <class T>
void Sort<T>::mergeSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    mergeSortRecursive(sortArray, 0, length - 1);

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_MERGE, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}
template <class T>
void Sort<T>::mergeSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end)
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
    leftSubArray.insert(leftSubArray.end(), std::numeric_limits<T>::max());
    rightSubArray.insert(rightSubArray.end(), std::numeric_limits<T>::max());
    for (uint32_t i = begin; i <= end; ++i)
    {
        if (leftSubArray[leftIndex] < rightSubArray[rightIndex])
        {
            sortArray[i] = leftSubArray[leftIndex];
            ++leftIndex;
        }
        else
        {
            sortArray[i] = rightSubArray[rightIndex];
            ++rightIndex;
        }
    }
}

// Quick method
template <class T>
void Sort<T>::quickSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    quickSortRecursive(sortArray, 0, length - 1);

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_QUICK, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}
template <class T>
void Sort<T>::quickSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end)
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

    if (0 != leftIndex)
    {
        quickSortRecursive(sortArray, begin, leftIndex - 1);
    }
    quickSortRecursive(sortArray, leftIndex + 1, end);
}

// Heap method
template <class T>
void Sort<T>::heapSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    for (int i = length / 2 + 1; i >= 0; --i)
    {
        buildMaxHeap(sortArray, i, length - 1);
    }
    for (int i = length - 1; i > 0; --i)
    {
        std::swap(sortArray[0], sortArray[i]);
        buildMaxHeap(sortArray, 0, i - 1);
    }

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_HEAP, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1), TIME_INTERVAL);
}
template <class T>
void Sort<T>::buildMaxHeap(T* const sortArray, const uint32_t begin, const uint32_t end)
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
        else
        {
            std::swap(sortArray[parent], sortArray[child]);
            parent = child;
            child = parent * 2 + 1;
        }
    }
}

// Counting method
template <class T>
void Sort<T>::countingSort(T* const array, const uint32_t length) const
{
    if (!std::is_integral_v<T>)
    {
        FORMAT_PRINT("\r\n*Counting  method:\r\nThe type of array isn't integral.\n");
        return;
    }

    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    for (uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
    }

    const T countingLen = max - min + 1;
    std::unique_ptr<T[]> counting = std::make_unique<T[]>(countingLen);
    for (uint32_t i = 0; i < length; ++i)
    {
        ++counting[sortArray[i] - min];
    }
    uint32_t index = 0;
    for (T j = 0; j < countingLen; ++j)
    {
        while (counting[j] > 0)
        {
            sortArray[index++] = j + min;
            --counting[j];
        }
    }

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_COUNTING, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}

// Bucket method
template <class T>
void Sort<T>::bucketSort(T* const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    for (uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
    }

    const uint32_t bucketNum = length;
    const double intervalSpan = static_cast<double>(max - min) / static_cast<double>(bucketNum - 1);
    std::vector<T> bucket(0);
    std::vector<std::vector<T>> aggregation(bucketNum, bucket);
    for (uint32_t i = 0; i < length; ++i)
    {
        // min+(max-min)/(bucketNum-1)*(buckIndex-1)<=sortArray[i]
        const uint32_t aggIndex =
            floor(static_cast<double>(sortArray[i] - min) / intervalSpan + 1) - 1;
        aggregation[aggIndex].emplace_back(sortArray[i]);
    }
    uint32_t index = 0;
    for (auto& bucketUpdate : aggregation)
    {
        std::sort(bucketUpdate.begin(), bucketUpdate.end());
        for (const auto& bucketElement : bucketUpdate)
        {
            sortArray[index++] = bucketElement;
        }
    }

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_BUCKET, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}

// Radix method
template <class T>
void Sort<T>::radixSort(T* const array, const uint32_t length) const
{
    if (!std::is_integral_v<T>)
    {
        FORMAT_PRINT("\r\n*Radix     method:\r\nThe type of array isn't integral.\n");
        return;
    }

    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    T max = std::numeric_limits<T>::min();
    T min = std::numeric_limits<T>::max();
    bool positive = false, negative = false;
    for (uint32_t i = 0; i < length; ++i)
    {
        max = std::max(sortArray[i], max);
        min = std::min(sortArray[i], min);
        sortArray[i] > 0 ? positive = true : (sortArray[i] < 0 ? negative = true : sortArray[i]);
    }
    T absMax = std::max(max, -min);
    uint32_t digitMax = 0;
    const uint32_t base = 10;
    while (0 != absMax)
    {
        absMax /= base;
        ++digitMax;
    }

    // -9 ... -1 0 1 ... 9
    const uint32_t bucketNum = (positive ^ negative)
        ? SORT_RADIX_NATURAL_NUMBER_BUCKET
        : (SORT_RADIX_NATURAL_NUMBER_BUCKET + SORT_RADIX_NEGATIVE_INTEGER_BUCKET);
    const uint32_t offset = (false == negative) ? 0 : SORT_RADIX_NEGATIVE_INTEGER_BUCKET;
    std::unique_ptr<T[]> countingOld = std::make_unique<T[]>(bucketNum);
    std::unique_ptr<T[]> countingNew = std::make_unique<T[]>(bucketNum);
    std::queue<T> bucket;
    std::vector<std::queue<T>> aggregation(bucketNum, bucket);
    for (uint32_t i = 0; i < length; ++i)
    {
        const int sign = (sortArray[i] > 0) ? 1 : -1;
        const uint32_t aggIndex = std::abs(sortArray[i]) / 1 % base * sign + offset;
        aggregation[aggIndex].push(sortArray[i]);
        ++countingNew[aggIndex];
    }
    for (uint32_t i = 1, pow = SORT_RADIX_DEC; i < digitMax; ++i, pow *= base)
    {
        memcpy(countingOld.get(), countingNew.get(), bucketNum * sizeof(T));
        memset(countingNew.get(), 0, bucketNum * sizeof(T));
        for (auto iterBucket = aggregation.begin(); iterBucket != aggregation.end(); ++iterBucket)
        {
            if (!iterBucket->size())
            {
                continue;
            }
            const uint32_t countingIndex = iterBucket - aggregation.begin();
            while (countingOld[countingIndex])
            {
                auto bucketElement = iterBucket->front();
                const int sign = (bucketElement > 0) ? 1 : -1;
                const uint32_t aggIndex = std::abs(bucketElement) / pow % base * sign + offset;
                aggregation[aggIndex].push(bucketElement);
                ++countingNew[aggIndex];
                iterBucket->pop();
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

    TIME_END;
    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_RADIX, formatArray(sortArray, length, arrayBuffer, arrayBufferSize + 1),
        TIME_INTERVAL);
}
