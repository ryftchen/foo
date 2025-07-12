//! @file heap.cpp
//! @author ryftchen
//! @brief The definitions (heap) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "heap.hpp"

namespace date_structure::heap
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
namespace max
{
//! @brief Get the index of the key in the max heap.
//! @param heap - heap to search in
//! @param key - key of the data
//! @return index of the key if found, otherwise -1
static int getIndex(const MaxHeap* const heap, const void* const key)
{
    for (int i = 0; i < heap->size; ++i)
    {
        if (heap->compare(heap->data[i], key) == 0)
        {
            return i;
        }
    }

    return -1;
}

//! @brief Filter up the element at the specified index in the max heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterUp(const MaxHeap* const heap, const int start)
{
    int c = start, p = (c - 1) / 2;
    void* const temp = heap->data[c];
    while (c > 0)
    {
        if (heap->compare(heap->data[p], temp) >= 0)
        {
            break;
        }

        heap->data[c] = heap->data[p];
        c = p;
        p = (p - 1) / 2;
    }
    heap->data[c] = temp;
}

//! @brief Filter down the element at the specified index in the max heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterDown(const MaxHeap* const heap, const int start)
{
    const int end = heap->size - 1;
    int c = start, l = (2 * c) + 1;
    void* const temp = heap->data[c];
    while (l <= end)
    {
        if ((l < end) && (heap->compare(heap->data[l], heap->data[l + 1]) < 0))
        {
            ++l;
        }
        if (heap->compare(temp, heap->data[l]) >= 0)
        {
            break;
        }

        heap->data[c] = heap->data[l];
        c = l;
        l = 2 * l + 1;
    }
    heap->data[c] = temp;
}

//! @brief Create the max heap.
//! @param cap - capacity of the heap
//! @param cmp - compare function to compare keys
//! @return new max heap
MaxHeap* creation(const int cap, const Compare cmp)
{
    auto* const heap = ::new (std::nothrow) MaxHeap;
    heap->data = ::new (std::nothrow) void*[cap];
    heap->capacity = cap;
    heap->size = 0;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the max heap.
//! @param heap - heap to destroy
void destruction(const MaxHeap* heap)
{
    if (heap)
    {
        ::delete[] heap->data;
        ::delete heap;
        heap = nullptr;
    }
}

//! @brief Insert the data into the max heap.
//! @param heap - heap to insert into
//! @param key - key of the data
//! @return success or failure
bool insertion(MaxHeap* const heap, const void* const key)
{
    if (!heap || (heap->size >= heap->capacity))
    {
        return false;
    }

    heap->data[heap->size] = const_cast<void*>(key);
    filterUp(heap, heap->size);
    ++heap->size;

    return true;
}

//! @brief Delete the data from the max heap.
//! @param heap - heap to delete from
//! @param key - key of the data
//! @return success or failure
bool deletion(MaxHeap* const heap, const void* const key)
{
    if (!heap || (heap->size == 0))
    {
        return false;
    }

    const int index = getIndex(heap, key);
    if (index == -1)
    {
        return false;
    }

    heap->data[index] = heap->data[--heap->size];
    filterDown(heap, index);

    return true;
}

void Traverse::order(const Operation& op) const
{
    for (int i = 0; i < heap->size; ++i)
    {
        op(heap->data[i]);
    }
}
} // namespace max

namespace min
{
//! @brief Get the index of the key in the min heap.
//! @param heap - heap to search in
//! @param key - key of the data
//! @return index of the key if found, otherwise -1
static int getIndex(const MinHeap* const heap, const void* const key)
{
    for (int i = 0; i < heap->size; ++i)
    {
        if (heap->compare(heap->data[i], key) == 0)
        {
            return i;
        }
    }

    return -1;
}

//! @brief Filter up the element at the specified index in the min heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterUp(const MinHeap* const heap, const int start)
{
    int c = start, p = (c - 1) / 2;
    void* const temp = heap->data[c];
    while (c > 0)
    {
        if (heap->compare(temp, heap->data[p]) >= 0)
        {
            break;
        }

        heap->data[c] = heap->data[p];
        c = p;
        p = (p - 1) / 2;
    }
    heap->data[c] = temp;
}

//! @brief Filter down the element at the specified index in the min heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterDown(const MinHeap* const heap, const int start)
{
    const int end = heap->size - 1;
    int c = start, l = (2 * c) + 1;
    void* const temp = heap->data[c];
    while (l <= end)
    {
        if ((l < end) && (heap->compare(heap->data[l], heap->data[l + 1]) > 0))
        {
            ++l;
        }
        if (heap->compare(temp, heap->data[l]) <= 0)
        {
            break;
        }

        heap->data[c] = heap->data[l];
        c = l;
        l = 2 * l + 1;
    }
    heap->data[c] = temp;
}

//! @brief Create the min heap.
//! @param cap - capacity of the heap
//! @param cmp - compare function to compare keys
//! @return new min heap
MinHeap* creation(const int cap, const Compare cmp)
{
    auto* const heap = ::new (std::nothrow) MinHeap;
    heap->data = ::new (std::nothrow) void*[cap];
    heap->capacity = cap;
    heap->size = 0;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the min heap.
//! @param heap - heap to destroy
void destruction(const MinHeap* heap)
{
    if (heap)
    {
        ::delete[] heap->data;
        ::delete heap;
        heap = nullptr;
    }
}

//! @brief Insert the data into the min heap.
//! @param heap - heap to insert into
//! @param key - key of the data
//! @return success or failure
bool insertion(MinHeap* const heap, const void* const key)
{
    if (!heap || (heap->size >= heap->capacity))
    {
        return false;
    }

    heap->data[heap->size] = const_cast<void*>(key);
    filterUp(heap, heap->size);
    ++heap->size;

    return true;
}

//! @brief Delete the data from the min heap.
//! @param heap - heap to delete from
//! @param key - key of the data
//! @return success or failure
bool deletion(MinHeap* const heap, const void* const key)
{
    if (!heap || (heap->size == 0))
    {
        return false;
    }

    const int index = getIndex(heap, key);
    if (index == -1)
    {
        return false;
    }

    heap->data[index] = heap->data[--heap->size];
    filterDown(heap, index);

    return true;
}

void Traverse::order(const Operation& op) const
{
    for (int i = 0; i < heap->size; ++i)
    {
        op(heap->data[i]);
    }
}
} // namespace min
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::heap
