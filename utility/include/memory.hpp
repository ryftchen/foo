//! @file memory.hpp
//! @author ryftchen
//! @brief The declarations (memory) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <cstdint>
#include <utility>

//! @brief Memory-pool-related functions in the utility module.
namespace utility::memory
{
//! @brief Memory pool.
//! @tparam T - type of object to allocate
//! @tparam BlockSize - size of the chunk's memory pool allocates
template <typename T, std::size_t BlockSize = 4096>
class Memory
{
public:
    //! @brief Construct a new Memory object.
    Memory() noexcept : currentBlock(nullptr), currentSlot(nullptr), lastSlot(nullptr), freeSlots(nullptr){};
    //! @brief Destroy the Memory object.
    ~Memory() noexcept;
    //! @brief Construct a new Memory object.
    Memory(const Memory& /*unused*/) noexcept : Memory(){};
    //! @brief Construct a new Memory object
    //! @tparam U - type of object to allocate
    template <class U>
    explicit Memory(const Memory<U>& /*unused*/) noexcept : Memory(){};
    //! @brief Construct a new Memory object.
    //! @param memory - the old object for move constructor
    Memory(Memory&& memory) noexcept;
    //! @brief The operator (=) overloading of Memory class.
    //! @return reference of Memory object
    Memory& operator=(const Memory&) = delete;
    //! @brief The operator (=) overloading of Memory class.
    //! @param memory - the old object for move assignment operator
    //! @return reference of Memory object
    Memory& operator=(Memory&& memory) noexcept;

    //! @brief Get the pointer of the allocated object.
    //! @param x - reference of the allocated object
    //! @return pointer of the allocated object
    inline T* address(T& x) const noexcept;
    //! @brief Get the const pointer of the allocated object.
    //! @param x - const reference of the allocated object
    //! @return const pointer of the allocated object
    inline const T* address(const T& x) const noexcept;
    //! @brief Allocate resource.
    //! @param n - resource size
    //! @param hint - const pointer of the allocated object
    //! @return pointer of the allocated object
    inline T* allocate(const std::size_t n = 1, const T* hint = 0);
    //! @brief Deallocate resource.
    //! @param p - pointer of the allocated object
    //! @param n - resource size
    inline void deallocate(T* p, const std::size_t n = 1);
    [[nodiscard]] inline std::size_t maxSize() const noexcept;

    //! @brief Construct object.
    //! @tparam U - type of allocated object
    //! @tparam Args - type of arguments for constructing the object
    //! @param p - pointer of the allocated object
    //! @param args - arguments for constructing the object
    template <class U, class... Args>
    inline void construct(U* p, Args&&... args);
    //! @brief Destroy object.
    //! @tparam U - type of allocated object
    //! @param p - pointer of the allocated object
    template <class U>
    inline void destroy(U* p);

    //! @brief New an element.
    //! @tparam Args - type of arguments for constructing the object
    //! @param args - arguments for constructing the object
    //! @return pointer of the allocated object
    template <class... Args>
    inline T* newElement(Args&&... args);
    //! @brief Delete an element.
    //! @param p - pointer of the allocated object
    inline void deleteElement(T* p);

private:
    //! @brief Union for the slot that stores element information.
    union Slot
    {
        //! @brief Allocated object.
        T element;
        //! @brief Next pointer of the slot.
        Slot* next;
    };

    //! @brief Pointer to the current block.
    Slot* currentBlock;
    //! @brief Pointer to the current slot.
    Slot* currentSlot;
    //! @brief Pointer to the last slot.
    Slot* lastSlot;
    //! @brief Pointer to the free slots.
    Slot* freeSlots;

    //! @brief Pad the pointer of data in the element.
    //! @param p - pointer of data in the element
    //! @param align - align size
    //! @return size after padding
    inline std::size_t padPointer(char* p, const std::size_t align) const noexcept;
    //! @brief Allocate block.
    void allocateBlock();

    static_assert(BlockSize >= (2 * sizeof(Slot)), "<MEMORY> BlockSize is too small.");
};
} // namespace utility::memory

#include "utility/source/memory.tpp"
