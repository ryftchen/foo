//! @file memory.tpp
//! @author ryftchen
//! @brief The definitions (memory) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include "memory.hpp"

namespace utility::memory
{
template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>::~Memory() noexcept
{
    SlotPointer curr = currentBlock;
    while (nullptr != curr)
    {
        SlotPointer prev = curr->next;
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
inline typename Memory<T, BlockSize>::Pointer Memory<T, BlockSize>::address(Reference x) const noexcept
{
    return &x;
}

template <typename T, std::size_t BlockSize>
inline typename Memory<T, BlockSize>::ConstPointer Memory<T, BlockSize>::address(ConstReference x) const noexcept
{
    return &x;
}

template <typename T, std::size_t BlockSize>
inline typename Memory<T, BlockSize>::Pointer Memory<T, BlockSize>::allocate(SizeType /*n*/, ConstPointer /*hint*/)
{
    if (nullptr != freeSlots)
    {
        Pointer result = reinterpret_cast<Pointer>(freeSlots);
        freeSlots = freeSlots->next;
        return result;
    }
    else
    {
        if (currentSlot >= lastSlot)
        {
            allocateBlock();
        }
        return reinterpret_cast<Pointer>(currentSlot++);
    }
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deallocate(Pointer p, SizeType /*n*/)
{
    if (nullptr != p)
    {
        reinterpret_cast<SlotPointer>(p)->next = freeSlots;
        freeSlots = reinterpret_cast<SlotPointer>(p);
    }
}

template <typename T, std::size_t BlockSize>
inline typename Memory<T, BlockSize>::SizeType Memory<T, BlockSize>::maxSize() const noexcept
{
    SizeType maxBlocks = -1 / BlockSize;
    return (BlockSize - sizeof(DataPointer)) / sizeof(SlotType) * maxBlocks;
}

template <typename T, std::size_t BlockSize>
template <class U, class... Args>
inline void Memory<T, BlockSize>::construct(U* p, Args&&... args)
{
    new (p) U(std::forward<Args>(args)...);
}

template <typename T, std::size_t BlockSize>
template <class U>
inline void Memory<T, BlockSize>::destroy(U* p)
{
    p->~U();
}

template <typename T, std::size_t BlockSize>
template <class... Args>
inline typename Memory<T, BlockSize>::Pointer Memory<T, BlockSize>::newElement(Args&&... args)
{
    Pointer result = allocate();
    construct<ValueType>(result, std::forward<Args>(args)...);
    return result;
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deleteElement(Pointer p)
{
    if (nullptr != p)
    {
        p->~ValueType();
        deallocate(p);
    }
}

template <typename T, std::size_t BlockSize>
inline typename Memory<T, BlockSize>::SizeType Memory<T, BlockSize>::padPointer(DataPointer p, SizeType align)
    const noexcept
{
    std::uintptr_t result = reinterpret_cast<std::uintptr_t>(p);
    return ((align - result) % align);
}

template <typename T, std::size_t BlockSize>
void Memory<T, BlockSize>::allocateBlock()
{
    DataPointer newBlock = reinterpret_cast<DataPointer>(operator new(BlockSize));
    reinterpret_cast<SlotPointer>(newBlock)->next = currentBlock;
    currentBlock = reinterpret_cast<SlotPointer>(newBlock);

    DataPointer body = newBlock + sizeof(SlotPointer);
    SizeType bodyPadding = padPointer(body, alignof(SlotType));
    currentSlot = reinterpret_cast<SlotPointer>(body + bodyPadding);
    lastSlot = reinterpret_cast<SlotPointer>(newBlock + BlockSize - sizeof(SlotType) + 1);
}
} // namespace utility::memory
