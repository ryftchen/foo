//! @file memory.hpp
//! @author ryftchen
//! @brief The declarations (memory) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

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
    Memory(const Memory& /*memory*/) noexcept : Memory(){};
    //! @brief Construct a new Memory object
    //! @tparam U - type of object to allocate
    template <class U>
    explicit Memory(const Memory<U>& /*memory*/) noexcept : Memory(){};
    //! @brief Construct a new Memory object.
    //! @param memory - the old object for move constructor
    Memory(Memory&& memory) noexcept;
    //! @brief The operator (=) overloading of Memory class.
    //! @return reference of the Memory object
    Memory& operator=(const Memory&) = delete;
    //! @brief The operator (=) overloading of Memory class.
    //! @param memory - the old object for move assignment operator
    //! @return reference of the Memory object
    Memory& operator=(Memory&& memory) noexcept;

    //! @brief Get the pointer of the allocated object.
    //! @param obj - reference of the allocated object
    //! @return pointer of the allocated object
    inline T* address(T& obj) const noexcept;
    //! @brief Get the const pointer of the allocated object.
    //! @param obj - const reference of the allocated object
    //! @return const pointer of the allocated object
    inline const T* address(const T& obj) const noexcept;
    //! @brief Allocate resource.
    //! @param size - resource size
    //! @param hint - const pointer of the allocated object
    //! @return pointer of the allocated object
    inline T* allocate(const std::size_t size = 1, const T* hint = 0);
    //! @brief Deallocate resource.
    //! @param obj - pointer of the allocated object
    //! @param size - resource size
    inline void deallocate(T* obj, const std::size_t size = 1);
    [[nodiscard]] inline std::size_t maxSize() const noexcept;

    //! @brief Construct object.
    //! @tparam U - type of allocated object
    //! @tparam Args - type of arguments for constructing the object
    //! @param obj - pointer of the allocated object
    //! @param args - arguments for constructing the object
    template <class U, class... Args>
    inline void construct(U* obj, Args&&... args);
    //! @brief Destroy object.
    //! @tparam U - type of allocated object
    //! @param obj - pointer of the allocated object
    template <class U>
    inline void destroy(U* obj);

    //! @brief New an element.
    //! @tparam Args - type of arguments for constructing the object
    //! @param args - arguments for constructing the object
    //! @return pointer of the allocated object
    template <class... Args>
    inline T* newElement(Args&&... args);
    //! @brief Delete an element.
    //! @param obj - pointer of the allocated object
    inline void deleteElement(T* obj);

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
    //! @param data - pointer of data in the element
    //! @param align - align size
    //! @return size after padding
    inline std::size_t padPointer(char* data, const std::size_t align) const noexcept;
    //! @brief Allocate block.
    void allocateBlock();

    static_assert(BlockSize >= (2 * sizeof(Slot)), "Block size is too small.");
};
} // namespace utility::memory

#include "utility/source/memory.tpp"
