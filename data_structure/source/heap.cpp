//! @file heap.cpp
//! @author ryftchen
//! @brief The definitions (heap) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "heap.hpp"

#include <cstring>

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
    if (!heap || !key)
    {
        return -1;
    }

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
    if (!heap || (start < 0))
    {
        return;
    }

    int child = start, parent = (child - 1) / 2;
    void* const temp = heap->data[child];
    while (child > 0)
    {
        if (heap->compare(heap->data[parent], temp) >= 0)
        {
            break;
        }

        heap->data[child] = heap->data[parent];
        child = parent;
        parent = (parent - 1) / 2;
    }
    heap->data[child] = temp;
}

//! @brief Filter down the element at the specified index in the max heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterDown(const MaxHeap* const heap, const int start)
{
    if (!heap || (start < 0))
    {
        return;
    }

    const int end = heap->size - 1;
    int child = start, selected = (2 * child) + 1;
    void* const temp = heap->data[child];
    while (selected <= end)
    {
        if ((selected < end) && (heap->compare(heap->data[selected], heap->data[selected + 1]) < 0))
        {
            ++selected;
        }
        if (heap->compare(temp, heap->data[selected]) >= 0)
        {
            break;
        }

        heap->data[child] = heap->data[selected];
        child = selected;
        selected = 2 * selected + 1;
    }
    heap->data[child] = temp;
}

//! @brief Create the max heap.
//! @param cap - capacity of the heap
//! @param cmp - compare function to compare keys
//! @return new max heap
MaxHeap* create(const int cap, const Compare cmp)
{
    if ((cap <= 0) || !cmp)
    {
        return nullptr;
    }

    auto* const heap = ::new (std::nothrow) MaxHeap;
    heap->data = ::new (std::nothrow) void*[cap];
    std::memset(static_cast<void*>(heap->data), 0, sizeof(void*) * cap);
    heap->capacity = cap;
    heap->size = 0;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the max heap.
//! @param heap - heap to destroy
void destroy(const MaxHeap* heap)
{
    if (!heap)
    {
        return;
    }

    ::delete[] heap->data;
    ::delete heap;
    heap = nullptr;
}

//! @brief Insert the data into the max heap.
//! @param heap - heap to insert into
//! @param key - key of the data
//! @return success or failure
bool insert(MaxHeap* const heap, const void* const key)
{
    if (!heap || !key || (heap->size >= heap->capacity))
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
bool remove(MaxHeap* const heap, const void* const key)
{
    if (!heap || !key || (heap->size == 0))
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
    if (!heap)
    {
        return;
    }

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
    if (!heap || !key)
    {
        return -1;
    }

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
    if (!heap || (start < 0))
    {
        return;
    }

    int child = start, parent = (child - 1) / 2;
    void* const temp = heap->data[child];
    while (child > 0)
    {
        if (heap->compare(temp, heap->data[parent]) >= 0)
        {
            break;
        }

        heap->data[child] = heap->data[parent];
        child = parent;
        parent = (parent - 1) / 2;
    }
    heap->data[child] = temp;
}

//! @brief Filter down the element at the specified index in the min heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterDown(const MinHeap* const heap, const int start)
{
    if (!heap || (start < 0))
    {
        return;
    }

    const int end = heap->size - 1;
    int child = start, selected = (2 * child) + 1;
    void* const temp = heap->data[child];
    while (selected <= end)
    {
        if ((selected < end) && (heap->compare(heap->data[selected], heap->data[selected + 1]) > 0))
        {
            ++selected;
        }
        if (heap->compare(temp, heap->data[selected]) <= 0)
        {
            break;
        }

        heap->data[child] = heap->data[selected];
        child = selected;
        selected = 2 * selected + 1;
    }
    heap->data[child] = temp;
}

//! @brief Create the min heap.
//! @param cap - capacity of the heap
//! @param cmp - compare function to compare keys
//! @return new min heap
MinHeap* create(const int cap, const Compare cmp)
{
    if ((cap <= 0) || !cmp)
    {
        return nullptr;
    }

    auto* const heap = ::new (std::nothrow) MinHeap;
    heap->data = ::new (std::nothrow) void*[cap];
    std::memset(static_cast<void*>(heap->data), 0, sizeof(void*) * cap);
    heap->capacity = cap;
    heap->size = 0;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the min heap.
//! @param heap - heap to destroy
void destroy(const MinHeap* heap)
{
    if (!heap)
    {
        return;
    }

    ::delete[] heap->data;
    ::delete heap;
    heap = nullptr;
}

//! @brief Insert the data into the min heap.
//! @param heap - heap to insert into
//! @param key - key of the data
//! @return success or failure
bool insert(MinHeap* const heap, const void* const key)
{
    if (!heap || !key || (heap->size >= heap->capacity))
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
bool remove(MinHeap* const heap, const void* const key)
{
    if (!heap || !key || (heap->size == 0))
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
    if (!heap)
    {
        return;
    }

    for (int i = 0; i < heap->size; ++i)
    {
        op(heap->data[i]);
    }
}
} // namespace min
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::heap
