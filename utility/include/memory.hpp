//! @file memory.hpp
//! @author ryftchen
//! @brief The declarations (memory) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <bit>
#include <mutex>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Memory-pool-related functions in the utility module.
namespace memory
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_MEMORY";
}
extern const char* version() noexcept;

//! @brief Memory pool.
//! @tparam Res - type of resource to allocate
//! @tparam BlockSize - size of the chunk's memory pool allocates
template <typename Res, std::size_t BlockSize = 4096>
class Memory
{
public:
    //! @brief Construct a new Memory object.
    Memory() = default;
    //! @brief Destroy the Memory object.
    virtual ~Memory();
    //! @brief Construct a new Memory object.
    Memory(const Memory&) = delete;
    //! @brief Construct a new Memory object.
    //! @tparam Other - type of resource to allocate
    template <typename Other>
    Memory(const Memory<Other>&) = delete;
    //! @brief Construct a new Memory object.
    //! @param memory - object for move constructor
    Memory(Memory&& memory) noexcept;
    //! @brief The operator (=) overloading of Memory class.
    //! @return reference of the Memory object
    Memory& operator=(const Memory&) = delete;
    //! @brief The operator (=) overloading of Memory class.
    //! @param memory - object for move assignment operator
    //! @return reference of the Memory object
    Memory& operator=(Memory&& memory) noexcept;

    //! @brief New an element.
    //! @tparam Args - type of arguments for constructing the resource
    //! @param args - arguments for constructing the resource
    //! @return pointer of the allocated resource
    template <typename... Args>
    Res* newEntry(Args&&... args);
    //! @brief Delete an element.
    //! @param res - pointer of the allocated resource
    void deleteEntry(Res* const res);

private:
    //! @brief Union for the slot that stores element information.
    union alignas(alignof(Res)) Slot
    {
        //! @brief Allocated resource.
        Res element;
        //! @brief Next pointer of the slot.
        Slot* next;
    };

    //! @brief Pointer to the current block.
    Slot* currentBlock{nullptr};
    //! @brief Pointer to the current slot.
    Slot* currentSlot{nullptr};
    //! @brief Pointer to the last slot.
    const Slot* lastSlot{nullptr};
    //! @brief Pointer to the free slots.
    Slot* freeSlots{nullptr};
    //! @brief Mutex for controlling resource.
    mutable std::recursive_mutex mtx;

    //! @brief Construct the resource.
    //! @tparam Alloc - type of allocated resource
    //! @tparam Args - type of arguments for constructing the resource
    //! @param alloc - pointer of the allocated resource
    //! @param args - arguments for constructing the resource
    template <typename Alloc, typename... Args>
    void construct(Alloc* const alloc, Args&&... args);
    //! @brief Destroy the resource.
    //! @tparam Alloc - type of allocated resource
    //! @param alloc - pointer of the allocated resource
    template <typename Alloc>
    void destroy(const Alloc* const alloc);
    //! @brief Allocate resource.
    //! @param size - resource size
    //! @param hint - address hint to suggest where allocation could occur
    //! @return pointer of the allocated resource
    Res* allocate(const std::size_t size = 1, const Res* hint = nullptr);
    //! @brief Deallocate resource.
    //! @param res - pointer of the allocated resource
    //! @param size - resource size
    void deallocate(Res* const res, const std::size_t size = 1);
    //! @brief Create the block.
    void createBlock();
    //! @brief Calculate the padding size for the pointer of data in the element.
    //! @param data - pointer of data in the element
    //! @param align - align size
    //! @return padding size
    std::size_t pointerPadding(const std::byte* const data, const std::size_t align) const;

    static_assert(BlockSize >= (2 * sizeof(Slot)));
};

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
template <typename Res, std::size_t BlockSize>
Memory<Res, BlockSize>::~Memory()
{
    auto* curr = currentBlock;
    while (curr)
    {
        auto* const prev = curr->next;
        operator delete(reinterpret_cast<void*>(curr));
        curr = prev;
    }
}

template <typename Res, std::size_t BlockSize>
Memory<Res, BlockSize>::Memory(Memory&& memory) noexcept :
    currentBlock{memory.currentBlock},
    currentSlot{memory.currentSlot},
    lastSlot{memory.lastSlot},
    freeSlots{memory.freeSlots}
{
    memory.currentBlock = nullptr;
    memory.currentSlot = nullptr;
    memory.lastSlot = nullptr;
    memory.freeSlots = nullptr;
}

template <typename Res, std::size_t BlockSize>
Memory<Res, BlockSize>& Memory<Res, BlockSize>::operator=(Memory&& memory) noexcept
{
    if (&memory != this)
    {
        std::swap(currentBlock, memory.currentBlock);
        std::swap(currentSlot, memory.currentSlot);
        std::swap(lastSlot, memory.lastSlot);
        std::swap(freeSlots, memory.freeSlots);
    }
    return *this;
}

template <typename Res, std::size_t BlockSize>
template <typename... Args>
Res* Memory<Res, BlockSize>::newEntry(Args&&... args)
{
    const std::lock_guard<std::recursive_mutex> lock(mtx);
    auto* const res = allocate();
    construct<Res>(res, std::forward<Args>(args)...);
    return res;
}

template <typename Res, std::size_t BlockSize>
void Memory<Res, BlockSize>::deleteEntry(Res* const res)
{
    const std::lock_guard<std::recursive_mutex> lock(mtx);
    if (res)
    {
        destroy(res);
        deallocate(res);
    }
}

template <typename Res, std::size_t BlockSize>
template <typename Alloc, typename... Args>
void Memory<Res, BlockSize>::construct(Alloc* const alloc, Args&&... args)
{
    if (alloc)
    {
        std::construct_at(alloc, std::forward<Args>(args)...);
    }
}

template <typename Res, std::size_t BlockSize>
template <typename Alloc>
void Memory<Res, BlockSize>::destroy(const Alloc* const alloc)
{
    if (alloc)
    {
        std::destroy_at(alloc);
    }
}

template <typename Res, std::size_t BlockSize>
Res* Memory<Res, BlockSize>::allocate(const std::size_t /*size*/, const Res* /*hint*/)
{
    if (freeSlots)
    {
        auto* const res = std::launder(reinterpret_cast<Res*>(freeSlots));
        freeSlots = freeSlots->next;
        return res;
    }

    if (!currentSlot || (currentSlot >= lastSlot))
    {
        createBlock();
    }
    return reinterpret_cast<Res*>(currentSlot++);
}

template <typename Res, std::size_t BlockSize>
void Memory<Res, BlockSize>::deallocate(Res* const res, const std::size_t /*size*/)
{
    if (res)
    {
        reinterpret_cast<Slot*>(res)->next = freeSlots;
        freeSlots = reinterpret_cast<Slot*>(res);
    }
}

template <typename Res, std::size_t BlockSize>
void Memory<Res, BlockSize>::createBlock()
{
    auto* const newBlock = reinterpret_cast<std::byte*>(operator new(BlockSize));
    reinterpret_cast<Slot*>(newBlock)->next = currentBlock;
    currentBlock = reinterpret_cast<Slot*>(newBlock);

    auto* const body = std::next(newBlock, sizeof(Slot*));
    const std::size_t offset = pointerPadding(body, alignof(Slot));
    currentSlot = reinterpret_cast<Slot*>(body + offset);
    lastSlot = reinterpret_cast<Slot*>(newBlock + BlockSize);
}

template <typename Res, std::size_t BlockSize>
std::size_t Memory<Res, BlockSize>::pointerPadding(const std::byte* const data, const std::size_t align) const
{
    if (align != 0)
    {
        const auto padding = std::bit_cast<std::uintptr_t>(data);
        return (align - (padding % align)) % align;
    }
    return 0;
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
} // namespace memory
} // namespace utility
