#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace utility::memory
{
template <typename T, size_t BlockSize = 4096>
class Memory
{
public:
    typedef T ValueType;
    typedef T* Pointer;
    typedef T& Reference;
    typedef const T* ConstPointer;
    typedef const T& ConstReference;
    typedef size_t SizeType;
    typedef ptrdiff_t DifferenceType;
    typedef std::false_type PropagateOnContainerCopyAssignment;
    typedef std::true_type PropagateOnContainerMoveAssignment;
    typedef std::true_type PropagateOnContainerSwap;

    template <typename U>
    struct Rebind
    {
        typedef Memory<U> Other;
    };

    Memory() noexcept;
    ~Memory() noexcept;
    Memory(const Memory& memory) noexcept;
    Memory(Memory&& memory) noexcept;
    template <class U>
    explicit Memory(const Memory<U>& memory) noexcept;
    Memory& operator=(const Memory& memory) = delete;
    Memory& operator=(Memory&& memory) noexcept;

    inline Pointer address(Reference x) const noexcept;
    inline ConstPointer address(ConstReference x) const noexcept;
    inline Pointer allocate(SizeType n = 1, ConstPointer hint = 0);
    inline void deallocate(Pointer p, SizeType n = 1);
    [[nodiscard]] inline SizeType maxSize() const noexcept;

    template <class U, class... Args>
    inline void construct(U* p, Args&&... args);
    template <class U>
    inline void destroy(U* p);

    template <class... Args>
    inline Pointer newElement(Args&&... args);
    inline void deleteElement(Pointer p);

private:
    union Slot
    {
        ValueType element;
        Slot* next;
    };

    typedef char* DataPointer;
    typedef Slot SlotType;
    typedef Slot* SlotPointer;

    SlotPointer currentBlock;
    SlotPointer currentSlot;
    SlotPointer lastSlot;
    SlotPointer freeSlots;

    inline SizeType padPointer(DataPointer p, SizeType align) const noexcept;
    void allocateBlock();
    static_assert(BlockSize >= (2 * sizeof(SlotType)), "memory: BlockSize is too small.");
};
} // namespace utility::memory

#include "utility/source/memory.tpp"
