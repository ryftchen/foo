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

    //! @brief Alias for the type of object to allocate.
    typedef T ValueType;
    //! @brief Alias for the pointer of the object to allocate.
    typedef T* Pointer;
    //! @brief Alias for the reference of the object to allocate.
    typedef T& Reference;
    //! @brief Alias for the const pointer of the object to allocate.
    typedef const T* ConstPointer;
    //! @brief Alias for the const reference of the object to allocate.
    typedef const T& ConstReference;
    //! @brief Alias for size type.
    typedef std::size_t SizeType;

    //! @brief Get the pointer of the allocated object.
    //! @param x - reference of the allocated object
    //! @return pointer of the allocated object
    inline Pointer address(Reference x) const noexcept;
    //! @brief Get the const pointer of the allocated object.
    //! @param x - const reference of the allocated object
    //! @return const pointer of the allocated object
    inline ConstPointer address(ConstReference x) const noexcept;
    //! @brief Allocate resource.
    //! @param n - resource size
    //! @param hint - const pointer of the allocated object
    //! @return pointer of the allocated object
    inline Pointer allocate(SizeType n = 1, ConstPointer hint = 0);
    //! @brief Deallocate resource.
    //! @param p - pointer of the allocated object
    //! @param n - resource size
    inline void deallocate(Pointer p, SizeType n = 1);
    [[nodiscard]] inline SizeType maxSize() const noexcept;

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
    inline Pointer newElement(Args&&... args);
    //! @brief Delete an element.
    //! @param p - pointer of the allocated object
    inline void deleteElement(Pointer p);

private:
    //! @brief Union for the slot that stores element information.
    union Slot
    {
        //! @brief Allocated object.
        ValueType element;
        //! @brief Next pointer of the slot.
        Slot* next;
    };

    //! @brief Alias for the pointer of data in the element.
    typedef char* DataPointer;
    //! @brief Alias for the type of slot.
    typedef Slot SlotType;
    //! @brief Alias for the pointer of the slot.
    typedef Slot* SlotPointer;

    //! @brief The pointer of the current block.
    SlotPointer currentBlock;
    //! @brief The pointer of the current slot.
    SlotPointer currentSlot;
    //! @brief The pointer of the last slot.
    SlotPointer lastSlot;
    //! @brief The pointer of the free slots.
    SlotPointer freeSlots;

    //! @brief Pad the pointer of data in the element.
    //! @param p - pointer of data in the element
    //! @param align - align size
    //! @return size after padding
    inline SizeType padPointer(DataPointer p, SizeType align) const noexcept;
    //! @brief Allocate block.
    void allocateBlock();

    static_assert(BlockSize >= (2 * sizeof(SlotType)), "memory: BlockSize is too small.");
};
} // namespace utility::memory

#include "utility/source/memory.tpp"
