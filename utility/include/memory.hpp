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
extern const char* version() noexcept;

//! @brief Memory pool.
//! @tparam T - type of resource to allocate
//! @tparam BlockSize - size of the chunk's memory pool allocates
template <typename T, std::size_t BlockSize = 4096>
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
    //! @tparam U - type of resource to allocate
    template <typename U>
    Memory(const Memory<U>&) = delete;
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
    inline T* newEntry(Args&&... args);
    //! @brief Delete an element.
    //! @param res - pointer of the allocated resource
    inline void deleteEntry(T* const res);
    //! @brief Get the maximum number of elements.
    //! @return size of capacity
    inline std::size_t capacity() const noexcept;
    //! @brief Get the pointer of the allocated resource.
    //! @param res - reference of the allocated resource
    //! @return pointer of the allocated resource
    inline T* address(T& res) const noexcept;
    //! @brief Get the const pointer of the allocated resource.
    //! @param res - const reference of the allocated resource
    //! @return const pointer of the allocated resource
    inline const T* address(const T& res) const noexcept;

private:
    //! @brief Union for the slot that stores element information.
    union alignas(alignof(T)) Slot
    {
        //! @brief Allocated resource.
        T element;
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
    //! @tparam U - type of allocated resource
    //! @tparam Args - type of arguments for constructing the resource
    //! @param res - pointer of the allocated resource
    //! @param args - arguments for constructing the resource
    template <typename U, typename... Args>
    inline void construct(U* const res, Args&&... args);
    //! @brief Destroy the resource.
    //! @tparam U - type of allocated resource
    //! @param res - pointer of the allocated resource
    template <typename U>
    inline void destroy(const U* const res);
    //! @brief Allocate resource.
    //! @param size - resource size
    //! @param hint - address hint to suggest where allocation could occur
    //! @return pointer of the allocated resource
    inline T* allocate(const std::size_t size = 1, const T* hint = nullptr);
    //! @brief Deallocate resource.
    //! @param res - pointer of the allocated resource
    //! @param size - resource size
    inline void deallocate(T* const res, const std::size_t size = 1);
    //! @brief Create a block.
    inline void createBlock();
    //! @brief Calculate the padding size for the pointer of data in the element.
    //! @param data - pointer of data in the element
    //! @param align - align size
    //! @return padding size
    inline std::size_t pointerPadding(const std::byte* const data, const std::size_t align) const noexcept;

    static_assert(BlockSize >= (2 * sizeof(Slot)));
};

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>::~Memory()
{
    auto* curr = currentBlock;
    while (curr)
    {
        auto* const prev = curr->next;
        operator delete(reinterpret_cast<void*>(curr));
        curr = prev;
    }
}

template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>::Memory(Memory&& memory) noexcept :
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

template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>& Memory<T, BlockSize>::operator=(Memory&& memory) noexcept
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

template <typename T, std::size_t BlockSize>
template <typename... Args>
inline T* Memory<T, BlockSize>::newEntry(Args&&... args)
{
    const std::lock_guard<std::recursive_mutex> lock(mtx);
    auto* const res = allocate();
    construct<T>(res, std::forward<Args>(args)...);

    return res;
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deleteEntry(T* const res)
{
    const std::lock_guard<std::recursive_mutex> lock(mtx);
    if (res)
    {
        destroy(res);
        deallocate(res);
    }
}

template <typename T, std::size_t BlockSize>
inline std::size_t Memory<T, BlockSize>::capacity() const noexcept
{
    constexpr std::size_t max = std::numeric_limits<std::size_t>::max() / BlockSize;
    return (BlockSize - sizeof(Slot*)) / sizeof(Slot) * max;
}

template <typename T, std::size_t BlockSize>
inline T* Memory<T, BlockSize>::address(T& res) const noexcept
{
    return std::addressof(res);
}

template <typename T, std::size_t BlockSize>
inline const T* Memory<T, BlockSize>::address(const T& res) const noexcept
{
    return std::addressof(res);
}

template <typename T, std::size_t BlockSize>
template <typename U, typename... Args>
inline void Memory<T, BlockSize>::construct(U* const res, Args&&... args)
{
    ::new (res) U(std::forward<Args>(args)...);
}

template <typename T, std::size_t BlockSize>
template <typename U>
inline void Memory<T, BlockSize>::destroy(const U* const res)
{
    res->~U();
}

template <typename T, std::size_t BlockSize>
inline T* Memory<T, BlockSize>::allocate(const std::size_t /*size*/, const T* /*hint*/)
{
    if (freeSlots)
    {
        auto* const res = std::launder(reinterpret_cast<T*>(freeSlots));
        freeSlots = freeSlots->next;
        return res;
    }

    if (!currentSlot || (currentSlot >= lastSlot))
    {
        createBlock();
    }

    return reinterpret_cast<T*>(currentSlot++);
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deallocate(T* const res, const std::size_t /*size*/)
{
    if (res)
    {
        reinterpret_cast<Slot*>(res)->next = freeSlots;
        freeSlots = reinterpret_cast<Slot*>(res);
    }
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::createBlock()
{
    auto* const newBlock = reinterpret_cast<std::byte*>(operator new(BlockSize));
    reinterpret_cast<Slot*>(newBlock)->next = currentBlock;
    currentBlock = reinterpret_cast<Slot*>(newBlock);

    auto* const body = std::next(newBlock, sizeof(Slot*));
    const std::size_t offset = pointerPadding(body, alignof(Slot));
    currentSlot = reinterpret_cast<Slot*>(body + offset);
    lastSlot = reinterpret_cast<Slot*>(newBlock + BlockSize);
}

template <typename T, std::size_t BlockSize>
inline std::size_t Memory<T, BlockSize>::pointerPadding(
    const std::byte* const data, const std::size_t align) const noexcept
{
    const auto padding = std::bit_cast<std::uintptr_t>(data);
    return (align - (padding % align)) % align;
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
} // namespace memory
} // namespace utility
