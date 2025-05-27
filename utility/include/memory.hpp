//! @file memory.hpp
//! @author ryftchen
//! @brief The declarations (memory) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <utility>

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

    //! @brief Get the pointer of the allocated resource.
    //! @param res - reference of the allocated resource
    //! @return pointer of the allocated resource
    inline T* address(T& res) const noexcept;
    //! @brief Get the const pointer of the allocated resource.
    //! @param res - const reference of the allocated resource
    //! @return const pointer of the allocated resource
    inline const T* address(const T& res) const noexcept;
    //! @brief Allocate resource.
    //! @param size - resource size
    //! @param hint - address hint to suggest where allocation could occur
    //! @return pointer of the allocated resource
    inline T* allocate(const std::size_t size = 1, const T* hint = nullptr);
    //! @brief Deallocate resource.
    //! @param res - pointer of the allocated resource
    //! @param size - resource size
    inline void deallocate(T* const res, const std::size_t size = 1);
    [[nodiscard]] inline std::size_t maxSize() const noexcept;

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

    //! @brief New an element.
    //! @tparam Args - type of arguments for constructing the resource
    //! @param args - arguments for constructing the resource
    //! @return pointer of the allocated resource
    template <typename... Args>
    inline T* newElement(Args&&... args);
    //! @brief Delete an element.
    //! @param res - pointer of the allocated resource
    inline void deleteElement(T* const res);

private:
    //! @brief Union for the slot that stores element information.
    union Slot
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

    //! @brief Pad the pointer of data in the element.
    //! @param data - pointer of data in the element
    //! @param align - align size
    //! @return size after padding
    inline std::size_t padPointer(const char* const data, const std::size_t align) const noexcept;
    //! @brief Allocate block.
    inline void allocateBlock();

    static_assert(BlockSize >= (2 * sizeof(Slot)));
};

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
template <typename T, std::size_t BlockSize>
Memory<T, BlockSize>::~Memory()
{
    Slot* curr = currentBlock;
    while (curr)
    {
        Slot* const prev = curr->next;
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
inline T* Memory<T, BlockSize>::address(T& res) const noexcept
{
    return &res;
}

template <typename T, std::size_t BlockSize>
inline const T* Memory<T, BlockSize>::address(const T& res) const noexcept
{
    return &res;
}

template <typename T, std::size_t BlockSize>
inline T* Memory<T, BlockSize>::allocate(const std::size_t /*size*/, const T* /*hint*/)
{
    if (freeSlots)
    {
        T* const result = reinterpret_cast<T*>(freeSlots);
        freeSlots = freeSlots->next;
        return result;
    }

    if (currentSlot >= lastSlot)
    {
        allocateBlock();
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
inline std::size_t Memory<T, BlockSize>::maxSize() const noexcept
{
    const std::size_t maxBlocks = -1 / BlockSize;
    return (BlockSize - sizeof(char*)) / sizeof(Slot) * maxBlocks;
}

template <typename T, std::size_t BlockSize>
template <typename U, typename... Args>
inline void Memory<T, BlockSize>::construct(U* const res, Args&&... args)
{
    new (res) U(std::forward<Args>(args)...);
}

template <typename T, std::size_t BlockSize>
template <typename U>
inline void Memory<T, BlockSize>::destroy(const U* const res)
{
    res->~U();
}

template <typename T, std::size_t BlockSize>
template <typename... Args>
inline T* Memory<T, BlockSize>::newElement(Args&&... args)
{
    T* const result = allocate();
    construct<T>(result, std::forward<Args>(args)...);

    return result;
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::deleteElement(T* const res)
{
    if (res)
    {
        res->~T();
        deallocate(res);
    }
}

template <typename T, std::size_t BlockSize>
inline std::size_t Memory<T, BlockSize>::padPointer(const char* const data, const std::size_t align) const noexcept
{
    const auto result = reinterpret_cast<std::uintptr_t>(data);
    return (align - result) % align;
}

template <typename T, std::size_t BlockSize>
inline void Memory<T, BlockSize>::allocateBlock()
{
    auto* const newBlock = reinterpret_cast<char*>(operator new(BlockSize));
    reinterpret_cast<Slot*>(newBlock)->next = currentBlock;
    currentBlock = reinterpret_cast<Slot*>(newBlock);

    char* const body = newBlock + sizeof(Slot*);
    const std::size_t bodyPadding = padPointer(body, alignof(Slot));
    currentSlot = reinterpret_cast<Slot*>(body + bodyPadding);
    lastSlot = reinterpret_cast<Slot*>(newBlock + BlockSize - sizeof(Slot) + 1);
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
} // namespace memory
} // namespace utility
