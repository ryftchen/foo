#pragma once

#include "memory.hpp"

namespace util_memory
{
template <typename T, size_t BlockSize>
Memory<T, BlockSize>::Memory() noexcept
{
    currentBlock = nullptr;
    currentSlot = nullptr;
    lastSlot = nullptr;
    freeSlots = nullptr;
}

template <typename T, size_t BlockSize>
Memory<T, BlockSize>::~Memory() noexcept
{
    SlotPointer curr = currentBlock;
    while (nullptr != curr)
    {
        SlotPointer prev = curr->next;
        operator delete(reinterpret_cast<void*>(curr)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        curr = prev;
    }
}

template <typename T, size_t BlockSize>
Memory<T, BlockSize>::Memory(const Memory& /*memory*/) noexcept : Memory()
{
}

template <typename T, size_t BlockSize>
template <class U>
Memory<T, BlockSize>::Memory(const Memory<U>& /*memory*/) noexcept : Memory()
{
}

template <typename T, size_t BlockSize>
Memory<T, BlockSize>::Memory(Memory&& memory) noexcept
{
    currentBlock = memory.currentBlock;
    memory.currentBlock = nullptr;
    currentSlot = memory.currentSlot;
    lastSlot = memory.lastSlot;
    freeSlots = memory.freeSlots;
}

template <typename T, size_t BlockSize>
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

template <typename T, size_t BlockSize>
inline typename Memory<T, BlockSize>::Pointer Memory<T, BlockSize>::address(Reference x) const noexcept
{
    return &x;
}

template <typename T, size_t BlockSize>
inline typename Memory<T, BlockSize>::ConstPointer Memory<T, BlockSize>::address(ConstReference x) const noexcept
{
    return &x;
}

template <typename T, size_t BlockSize>
inline typename Memory<T, BlockSize>::Pointer Memory<T, BlockSize>::allocate(SizeType /*n*/, ConstPointer /*hint*/)
{
    if (nullptr != freeSlots)
    {
        Pointer result = reinterpret_cast<Pointer>(freeSlots); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        freeSlots = freeSlots->next;
        return result;
    }
    else
    {
        if (currentSlot >= lastSlot)
        {
            allocateBlock();
        }
        return reinterpret_cast<Pointer>(currentSlot++); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    }
}

template <typename T, size_t BlockSize>
inline void Memory<T, BlockSize>::deallocate(Pointer p, SizeType /*n*/)
{
    if (nullptr != p)
    {
        reinterpret_cast<SlotPointer>(p)->next = freeSlots; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        freeSlots = reinterpret_cast<SlotPointer>(p); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    }
}

template <typename T, size_t BlockSize>
inline typename Memory<T, BlockSize>::SizeType Memory<T, BlockSize>::maxSize() const noexcept
{
    SizeType maxBlocks = -1 / BlockSize;
    return (BlockSize - sizeof(DataPointer)) / sizeof(SlotType) * maxBlocks;
}

template <typename T, size_t BlockSize>
template <class U, class... Args>
inline void Memory<T, BlockSize>::construct(U* p, Args&&... args)
{
    new (p) U(std::forward<Args>(args)...);
}

template <typename T, size_t BlockSize>
template <class U>
inline void Memory<T, BlockSize>::destroy(U* p)
{
    p->~U();
}

template <typename T, size_t BlockSize>
template <class... Args>
inline typename Memory<T, BlockSize>::Pointer Memory<T, BlockSize>::newElement(Args&&... args)
{
    Pointer result = allocate();
    construct<ValueType>(result, std::forward<Args>(args)...);
    return result;
}

template <typename T, size_t BlockSize>
inline void Memory<T, BlockSize>::deleteElement(Pointer p)
{
    if (nullptr != p)
    {
        p->~ValueType();
        deallocate(p);
    }
}

template <typename T, size_t BlockSize>
inline typename Memory<T, BlockSize>::SizeType Memory<T, BlockSize>::padPointer(DataPointer p, SizeType align)
    const noexcept
{
    uintptr_t result = reinterpret_cast<uintptr_t>(p); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    return ((align - result) % align);
}

template <typename T, size_t BlockSize>
void Memory<T, BlockSize>::allocateBlock()
{
    DataPointer newBlock =
        reinterpret_cast<DataPointer>(operator new(BlockSize)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    reinterpret_cast<SlotPointer>(newBlock)->next = currentBlock; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    currentBlock = reinterpret_cast<SlotPointer>(newBlock); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

    DataPointer body = newBlock + sizeof(SlotPointer);
    SizeType bodyPadding = padPointer(body, alignof(SlotType));
    currentSlot =
        reinterpret_cast<SlotPointer>(body + bodyPadding); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    lastSlot = reinterpret_cast<SlotPointer>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        newBlock + BlockSize - sizeof(SlotType) + 1);
}
} // namespace util_memory
