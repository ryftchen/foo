#include "../include/sort.hpp"
#include <algorithm>
#define NODEBUG
#include <cassert>
#include <cstring>
#include <queue>
#include <random>
#include <vector>
#include "../include/time.hpp"

template class Sort<int>;
template Sort<int>::Sort(const uint32_t, const int, const int);
template const std::unique_ptr<int[]> &Sort<int>::getRandomArray() const;
template uint32_t Sort<int>::getLength() const;
template void Sort<int>::bubbleSort(int *const, const uint32_t) const;
template void Sort<int>::selectionSort(int *const, const uint32_t) const;
template void Sort<int>::insertionSort(int *const, const uint32_t) const;
template void Sort<int>::shellSort(int *const, const uint32_t) const;
template void Sort<int>::mergeSort(int *const, const uint32_t) const;
template void Sort<int>::quickSort(int *const, const uint32_t) const;
template void Sort<int>::heapSort(int *const, const uint32_t) const;
template void Sort<int>::countingSort(int *const, const uint32_t) const;
template void Sort<int>::bucketSort(int *const, const uint32_t) const;
template void Sort<int>::radixSort(int *const, const uint32_t) const;

template <class T>
Sort<T>::Sort(const uint32_t length, const T left, const T right)
    : length(length), left(left), right(right), randomArray(std::make_unique<T[]>(length))
{
    std::cout << SORT_RUN_BEGIN << std::endl;
    setRandomArray<T>(randomArray.get(), length, left, right);
}

template <class T>
Sort<T>::~Sort()
{
    std::cout << SORT_RUN_END << std::endl;
}

template <class T>
Sort<T> &Sort<T>::operator=(const Sort &rhs)
{
    deepCopyFromSort(rhs);
    return *this;
}

template <class T>
Sort<T>::Sort(const Sort &sort)
    : length(sort.length)
    , left(sort.length)
    , right(sort.length)
    , randomArray(std::make_unique<T[]>(length))
{
    deepCopyFromSort(sort);
}

template <class T>
void Sort<T>::deepCopyFromSort(const Sort &sort) const
{
    memcpy(this->randomArray.get(), sort.randomArray.get(), this->length * sizeof(T));
}

template <class T>
const std::unique_ptr<T[]> &Sort<T>::getRandomArray() const
{
    std::unique_lock<std::mutex> lock(sortMutex);
    return randomArray;
}

template <class T>
uint32_t Sort<T>::getLength() const
{
    std::unique_lock<std::mutex> lock(sortMutex);
    return length;
}

template <class T>
template <typename U>
requires std::is_integral<U>::value void Sort<T>::setRandomArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right) const
{
    GET_TIME_SEED(seed);
    const char *const rangeFormat = std::is_integral<T>::value ? "%d to %d" : " ";
    std::uniform_int_distribution<int> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }

    const int rangeSize = std::snprintf(nullptr, 0, rangeFormat, left, right);
    assert(rangeSize >= 0);
    char range[rangeSize + 1];
    range[0] = '\0';
    std::snprintf(range, rangeSize + 1, rangeFormat, left, right);

    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(
        "\r\nGenerate %u random integral numbers from %s:\r\n%s\n",
        length,
        range,
        formatArray(array, length, buffer, bufferSize + 1));
}
template <class T>
template <typename U>
requires std::is_floating_point<U>::value void Sort<T>::setRandomArray(
    T array[],
    const uint32_t length,
    const T left,
    const T right) const
{
    GET_TIME_SEED(seed);
    const char *const rangeFormat = std::is_floating_point<T>::value ? "%.5f to %.5f" : " ";
    std::uniform_real_distribution<double> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }

    const int rangeSize = std::snprintf(nullptr, 0, rangeFormat, left, right);
    assert(rangeSize >= 0);
    char range[rangeSize + 1];
    range[0] = '\0';
    std::snprintf(range, rangeSize + 1, rangeFormat, left, right);

    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(
        "\r\nGenerate %u random floating point numbers from %s:\r\n%s\n",
        length,
        range,
        formatArray(array, length, buffer, bufferSize + 1));
}

template <class T>
char *Sort<T>::formatArray(
    const T *const array,
    const uint32_t length,
    char *const buffer,
    const uint32_t bufferSize) const
{
    uint32_t align = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        align = std::max(static_cast<uint32_t>(std::to_string(array[i]).length()), align);
    }

    const char *const format =
        std::is_integral<T>::value ? "%*d " : (std::is_floating_point<T>::value ? "%*.5f " : " ");
    uint32_t completeSize = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        completeSize += std::snprintf(
            buffer + completeSize, bufferSize - completeSize, format, align + 1, array[i]);
        if ((0 == (i + 1) % SORT_PRINT_MAX_COLUMN) && (length != (i + 1)))
        {
            completeSize += std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
        }
    }
    return buffer;
}

// Bubble method
template <class T>
void Sort<T>::bubbleSort(T *const array, const uint32_t length) const
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
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_BUBBLE, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}

// Selection method
template <class T>
void Sort<T>::selectionSort(T *const array, const uint32_t length) const
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
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_SELECTION, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}

// Insertion method
template <class T>
void Sort<T>::insertionSort(T *const array, const uint32_t length) const
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
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_INSERTION, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}

// Shell method
template <class T>
void Sort<T>::shellSort(T *const array, const uint32_t length) const
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
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_SHELL, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}

// Merge method
template <class T>
void Sort<T>::mergeSort(T *const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    mergeSortRecursive(sortArray, 0, length - 1);

    TIME_END;
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_MERGE, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}
template <class T>
void Sort<T>::mergeSortRecursive(T *const sortArray, const uint32_t begin, const uint32_t end)
{
    if (begin >= end)
    {
        return;
    }
    const uint32_t mid = (begin + end) / 2;
    mergeSortRecursive(sortArray, begin, mid);
    mergeSortRecursive(sortArray, mid + 1, end);

    uint32_t leftIndex = 0, rightIndex = 0;
    std::vector<T> leftSubSort(sortArray + begin, sortArray + mid + 1);
    std::vector<T> rightSubSort(sortArray + mid + 1, sortArray + end + 1);
    leftSubSort.insert(leftSubSort.end(), std::numeric_limits<T>::max());
    rightSubSort.insert(rightSubSort.end(), std::numeric_limits<T>::max());
    for (uint32_t i = begin; i <= end; ++i)
    {
        if (leftSubSort[leftIndex] < rightSubSort[rightIndex])
        {
            sortArray[i] = leftSubSort[leftIndex];
            ++leftIndex;
        }
        else
        {
            sortArray[i] = rightSubSort[rightIndex];
            ++rightIndex;
        }
    }
}

// Quick method
template <class T>
void Sort<T>::quickSort(T *const array, const uint32_t length) const
{
    TIME_BEGIN;
    T sortArray[length];
    sortArray[0] = '\0';
    memcpy(sortArray, array, length * sizeof(T));

    quickSortRecursive(sortArray, 0, length - 1);

    TIME_END;
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_QUICK, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}
template <class T>
void Sort<T>::quickSortRecursive(T *const sortArray, const uint32_t begin, const uint32_t end)
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
void Sort<T>::heapSort(T *const array, const uint32_t length) const
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
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_HEAP, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}
template <class T>
void Sort<T>::buildMaxHeap(T *const sortArray, const uint32_t begin, const uint32_t end)
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
void Sort<T>::countingSort(T *const array, const uint32_t length) const
{
    if (!std::is_integral_v<T>)
    {
        printf("\r\n*Counting  method:\r\nThe type of array isn't integral.\n");
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
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_COUNTING, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}

// Bucket method
template <class T>
void Sort<T>::bucketSort(T *const array, const uint32_t length) const
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
    for (auto &bucketUpdate : aggregation)
    {
        std::sort(bucketUpdate.begin(), bucketUpdate.end());
        for (const auto &bucketElement : bucketUpdate)
        {
            sortArray[index++] = bucketElement;
        }
    }

    TIME_END;
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_BUCKET, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}

// Radix method
template <class T>
void Sort<T>::radixSort(T *const array, const uint32_t length) const
{
    if (!std::is_integral_v<T>)
    {
        printf("\r\n*Radix     method:\r\nThe type of array isn't integral.\n");
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
        const uint32_t aggIndex = abs(sortArray[i]) / 1 % base * sign + offset;
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
                const uint32_t aggIndex = abs(bucketElement) / pow % base * sign + offset;
                aggregation[aggIndex].push(bucketElement);
                ++countingNew[aggIndex];
                iterBucket->pop();
                --countingOld[countingIndex];
            }
        }
    }

    uint32_t index = 0;
    for (auto &bucketInfo : aggregation)
    {
        while (bucketInfo.size())
        {
            sortArray[index++] = bucketInfo.front();
            bucketInfo.pop();
        }
    }

    TIME_END;
    const uint32_t bufferSize = length * SORT_PRINT_MAX_ALIGN;
    char buffer[bufferSize + 1];
    buffer[0] = '\0';
    printf(SORT_RADIX, formatArray(sortArray, length, buffer, bufferSize + 1), TIME_INTERVAL);
}
