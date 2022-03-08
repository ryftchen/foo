#pragma once
#include <iostream>
#include <memory>
#include <mutex>

#define SORT_ARRAY_RANGE_1 -50
#define SORT_ARRAY_RANGE_2 150
#define SORT_ARRAY_LENGTH 53
#define SORT_PRINT_MAX_ALIGN 16
#define SORT_PRINT_MAX_COLUMN 10
#define SORT_RUN_BEGIN "\r\n----------   BEGIN SORT   ----------"
#define SORT_RUN_END "\r\n----------    END SORT    ----------"
#define SORT_BUBBLE "\r\n*Bubble    method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_SELECTION "\r\n*Selection method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_INSERTION "\r\n*Insertion method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_SHELL "\r\n*Shell     method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_MERGE "\r\n*Merge     method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_QUICK "\r\n*Quick     method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_HEAP "\r\n*Heap      method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_COUNTING "\r\n*Counting  method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_BUCKET "\r\n*Bucket    method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_RADIX "\r\n*Radix     method:\r\n%s\r\n==>Run time: %8.5fms\n"
#define SORT_RADIX_DEC 10
#define SORT_RADIX_NATURAL_NUMBER_BUCKET 10
#define SORT_RADIX_NEGATIVE_INTEGER_BUCKET 9

template <class T>
class Sort
{
public:
    Sort(const uint32_t length, const T left, const T right);
    virtual ~Sort();
    Sort<T> &operator=(const Sort &rhs);
    Sort(const Sort &sort);
    void bubbleSort(T *const array, const uint32_t length) const;
    void selectionSort(T *const array, const uint32_t length) const;
    void insertionSort(T *const array, const uint32_t length) const;
    void shellSort(T *const array, const uint32_t length) const;
    void mergeSort(T *const array, const uint32_t length) const;
    void quickSort(T *const array, const uint32_t length) const;
    void heapSort(T *const array, const uint32_t length) const;
    void countingSort(T *const array, const uint32_t length) const;
    void bucketSort(T *const array, const uint32_t length) const;
    void radixSort(T *const array, const uint32_t length) const;
    const std::unique_ptr<T[]> &getRandomArray() const;
    uint32_t getLength() const;
    template <typename U>
    requires std::is_integral<U>::value void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;
    template <typename U>
    requires std::is_floating_point<U>::value void setRandomArray(
        T array[],
        const uint32_t length,
        const T left,
        const T right) const;

private:
    mutable std::mutex sortMutex;
    const std::unique_ptr<T[]> randomArray;
    const uint32_t length;
    const T left;
    const T right;
    void deepCopyFromSort(const Sort &sort) const;
    static void mergeSortRecursive(T *const sortArray, const uint32_t begin, const uint32_t end);
    static void quickSortRecursive(T *const sortArray, const uint32_t begin, const uint32_t end);
    static void buildMaxHeap(T *const sortArray, const uint32_t begin, const uint32_t end);

protected:
    char *formatArray(
        const T *const __restrict array,
        const uint32_t length,
        char *const __restrict buffer,
        const uint32_t bufferSize) const;
};
