//! @file memory.tpp
//! @author ryftchen
//! @brief The definitions (memory) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#include <cstdint>
#include <utility>

#pragma once

#include "memory.hpp"

namespace utility::memory
{
template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>::~Memory() noexcept
{
    Slot* curr = currentBlock;
    while (nullptr != curr)
    {
        Slot* prev = curr->next;
        operator delete(reinterpret_cast<void*>(curr));
        curr = prev;
    }
}

template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>::Memory(Memory&& memory) noexcept :
    currentBlock(memory.currentBlock),
    currentSlot(memory.currentSlot),
    lastSlot(memory.lastSlot),
    freeSlots(memory.freeSlots)
{
    memory.currentBlock = nullptr;
}

template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>& Memory<T, BlockSize>::operator=(Memory&& memory) noexcept
{
    if (this != &memory)
    {
        std::swap(currentBlock, memory.currentBlock);
        currentSlot = memory.currentSlot;
        lastSlot = memory.lastSlot;
        freeSlots = memory.freeSlots;
    }
    return *this;
}

template <typename T, std::size_t BlockSize>
inline T* Memory<T, BlockSize>::address(T& obj) const noexcept
{
    return &obj;
}

template <typename T, std::size_t BlockSize>
inline const T* Memory<T, BlockSize>::address(const T& obj) const noexcept
{
    return &obj;
}

template <typename T, std::size_t BlockSize>
inline T* Memory<T, BlockSize>::allocate(const std::size_t /*size*/, const T* /*hint*/)
{
    if (nullptr != freeSlots)
    {
        T* result = reinterpret_cast<T*>(freeSlots);
        freeSlots = freeSlots->next;
        return result;
    }
    else
    {
        if (currentSlot >= lastSlot)
        {
            allocateBlock();
        }
        return reinterpret_cast<T*>(currentSlot++);
    }
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deallocate(T* obj, const std::size_t /*size*/)
{
    if (nullptr != obj)
    {
        reinterpret_cast<Slot*>(obj)->next = freeSlots;
        freeSlots = reinterpret_cast<Slot*>(obj);
    }
}

template <typename T, std::size_t BlockSize>
inline std::size_t Memory<T, BlockSize>::maxSize() const noexcept
{
    const std::size_t maxBlocks = -1 / BlockSize;
    return (BlockSize - sizeof(char*)) / sizeof(Slot) * maxBlocks;
}

template <typename T, std::size_t BlockSize>
template <class U, class... Args>
inline void Memory<T, BlockSize>::construct(U* obj, Args&&... args)
{
    new (obj) U(std::forward<Args>(args)...);
}

template <typename T, std::size_t BlockSize>
template <class U>
inline void Memory<T, BlockSize>::destroy(U* obj)
{
    obj->~U();
}

template <typename T, std::size_t BlockSize>
template <class... Args>
inline T* Memory<T, BlockSize>::newElement(Args&&... args)
{
    T* result = allocate();
    construct<T>(result, std::forward<Args>(args)...);
    return result;
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deleteElement(T* obj)
{
    if (nullptr != obj)
    {
        obj->~T();
        deallocate(obj);
    }
}

template <typename T, std::size_t BlockSize>
inline std::size_t Memory<T, BlockSize>::padPointer(char* data, const std::size_t align) const noexcept
{
    const std::uintptr_t result = reinterpret_cast<std::uintptr_t>(data);
    return ((align - result) % align);
}

template <typename T, std::size_t BlockSize>
void Memory<T, BlockSize>::allocateBlock()
{
    char* newBlock = reinterpret_cast<char*>(operator new(BlockSize));
    reinterpret_cast<Slot*>(newBlock)->next = currentBlock;
    currentBlock = reinterpret_cast<Slot*>(newBlock);

    char* body = newBlock + sizeof(Slot*);
    const std::size_t bodyPadding = padPointer(body, alignof(Slot));
    currentSlot = reinterpret_cast<Slot*>(body + bodyPadding);
    lastSlot = reinterpret_cast<Slot*>(newBlock + BlockSize - sizeof(Slot) + 1);
}
} // namespace utility::memory
