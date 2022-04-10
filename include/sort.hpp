#pragma once
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include "log.hpp"

#define SORT_ARRAY_RANGE_1 -50
#define SORT_ARRAY_RANGE_2 150
#define SORT_ARRAY_LENGTH 53
#define SORT_PRINT_MAX_ALIGN 16
#define SORT_PRINT_MAX_COLUMN 10
#define SORT_RUN_BEGIN "\r\n----------   BEGIN SORT   ----------"
#define SORT_RUN_END "\r\n----------    END SORT    ----------"
#define SORT_GENERATE_INTEGRAL_ARRAY \
    "\r\nGenerate %u random integral numbers from %d to %d:\r\n%s\n"
#define SORT_GENERATE_FLOATING_ARRAY \
    "\r\nGenerate %u random floating point numbers from %.5f to %.5f:\r\n%s\n"
#define SORT_BUBBLE "\r\n*Bubble    method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_SELECTION "\r\n*Selection method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_INSERTION "\r\n*Insertion method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_SHELL "\r\n*Shell     method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_MERGE "\r\n*Merge     method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_QUICK "\r\n*Quick     method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_HEAP "\r\n*Heap      method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_COUNTING "\r\n*Counting  method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_BUCKET "\r\n*Bucket    method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_RADIX "\r\n*Radix     method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SORT_RADIX_DEC 10
#define SORT_RADIX_NATURAL_NUMBER_BUCKET 10
#define SORT_RADIX_NEGATIVE_INTEGER_BUCKET 9

template <class T>
class Sort
{
public:
    Sort(const uint32_t length, const T left, const T right);
    virtual ~Sort();
    Sort<T>& operator=(const Sort& rhs);
    Sort(const Sort& sort);
    void bubbleSort(T* const array, const uint32_t length) const;
    void selectionSort(T* const array, const uint32_t length) const;
    void insertionSort(T* const array, const uint32_t length) const;
    void shellSort(T* const array, const uint32_t length) const;
    void mergeSort(T* const array, const uint32_t length) const;
    void quickSort(T* const array, const uint32_t length) const;
    void heapSort(T* const array, const uint32_t length) const;
    void countingSort(T* const array, const uint32_t length) const;
    void bucketSort(T* const array, const uint32_t length) const;
    void radixSort(T* const array, const uint32_t length) const;
    const std::unique_ptr<T[]>& getRandomArray() const;
    uint32_t getLength() const;
    template <typename U>
    requires std::is_integral<U>::value void setRandomArray(
        T array[], const uint32_t length, const T left, const T right) const;
    template <typename U>
    requires std::is_floating_point<U>::value void setRandomArray(
        T array[], const uint32_t length, const T left, const T right) const;

private:
    mutable std::mutex sortMutex;
    const std::unique_ptr<T[]> randomArray;
    const uint32_t length;
    const T left;
    const T right;
    void deepCopyFromSort(const Sort& sort) const;
    static void mergeSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end);
    static void quickSortRecursive(T* const sortArray, const uint32_t begin, const uint32_t end);
    static void buildMaxHeap(T* const sortArray, const uint32_t begin, const uint32_t end);

protected:
    char* formatArray(
        const T* const __restrict array, const uint32_t length, char* const __restrict buffer,
        const uint32_t bufferSize) const;
};

template <class T>
Sort<T>::Sort(const uint32_t length, const T left, const T right) :
    length(length), left(left), right(right), randomArray(std::make_unique<T[]>(length))
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
Sort<T>& Sort<T>::operator=(const Sort& rhs)
{
    deepCopyFromSort(rhs);
    return *this;
}

template <class T>
Sort<T>::Sort(const Sort& sort) :
    length(sort.length), left(sort.length), right(sort.length),
    randomArray(std::make_unique<T[]>(sort.length))
{
    deepCopyFromSort(sort);
}

template <class T>
void Sort<T>::deepCopyFromSort(const Sort& sort) const
{
    memcpy(this->randomArray.get(), sort.randomArray.get(), this->length * sizeof(T));
}

template <class T>
const std::unique_ptr<T[]>& Sort<T>::getRandomArray() const
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
    T array[], const uint32_t length, const T left, const T right) const
{
    TIME_GET_SEED(seed);
    std::uniform_int_distribution<int> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }

    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_GENERATE_INTEGRAL_ARRAY, length, left, right,
        formatArray(array, length, arrayBuffer, arrayBufferSize + 1));
}
template <class T>
template <typename U>
requires std::is_floating_point<U>::value void Sort<T>::setRandomArray(
    T array[], const uint32_t length, const T left, const T right) const
{
    TIME_GET_SEED(seed);
    std::uniform_real_distribution<double> randomX(left, right);
    for (uint32_t i = 0; i < length; ++i)
    {
        array[i] = randomX(seed);
    }

    const uint32_t arrayBufferSize = length * SORT_PRINT_MAX_ALIGN;
    char arrayBuffer[arrayBufferSize + 1];
    arrayBuffer[0] = '\0';
    FORMAT_PRINT(
        SORT_GENERATE_FLOATING_ARRAY, length, left, right,
        formatArray(array, length, arrayBuffer, arrayBufferSize + 1));
}

template <class T>
char* Sort<T>::formatArray(
    const T* const __restrict array, const uint32_t length, char* const __restrict buffer,
    const uint32_t bufferSize) const
{
    uint32_t align = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        align = std::max(static_cast<uint32_t>(std::to_string(*(array + i)).length()), align);
    }

    const char* format = " ";
    if constexpr (std::is_integral<T>::value)
    {
        format = "%*d ";
    }
    else if constexpr (std::is_floating_point<T>::value)
    {
        format = "%*.5f ";
    }
    uint32_t completeSize = 0;
    for (uint32_t i = 0; i < length; ++i)
    {
        completeSize += std::snprintf(
            buffer + completeSize, bufferSize - completeSize, format, align + 1, *(array + i));
        if ((0 == (i + 1) % SORT_PRINT_MAX_COLUMN) && (length != (i + 1)))
        {
            completeSize += std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
        }
    }
    return buffer;
}
