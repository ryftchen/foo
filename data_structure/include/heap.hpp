//! @file heap.hpp
//! @author ryftchen
//! @brief The declarations (heap) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>

//! @brief The data structure module.
namespace date_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Heap-related functions in the data structure module.
namespace heap
{
extern const char* version() noexcept;

//! @brief The max heap.
namespace max
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The max heap structure.
    struct MaxHeap
    {
        //! @brief Pointer to the data array.
        void** data;
        //! @brief The capacity of the heap.
        int capacity;
        //! @brief The current size of the heap.
        int size;
        //! @brief The key's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern MaxHeap* creation(const int cap, const Compare cmp);
extern void destruction(const MaxHeap* heap);
extern bool insertion(MaxHeap* const heap, const void* const key);
extern bool deletion(MaxHeap* const heap, const void* const key);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param heap - heap to be traversed
    explicit Traverse(const MaxHeap* const heap) : heap{heap} {}
    //! @brief Destroy the Traverse object.
    virtual ~Traverse() = default;

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a order traversal starting from head.
    //! @param op - operation on each node
    void order(const Operation& op) const;

private:
    //! @brief The heap to be traversed.
    const MaxHeap* const heap{nullptr};
};
} // namespace max

//! @brief The min heap.
namespace min
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The min heap structure.
    struct MinHeap
    {
        //! @brief Pointer to the data array.
        void** data;
        //! @brief The capacity of the heap.
        int capacity;
        //! @brief The current size of the heap.
        int size;
        //! @brief The key's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern MinHeap* creation(const int cap, const Compare cmp);
extern void destruction(const MinHeap* heap);
extern bool insertion(MinHeap* const heap, const void* const key);
extern bool deletion(MinHeap* const heap, const void* const key);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param heap - heap to be traversed
    explicit Traverse(const MinHeap* const heap) : heap{heap} {}
    //! @brief Destroy the Traverse object.
    virtual ~Traverse() = default;

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a order traversal starting from head.
    //! @param op - operation on each node
    void order(const Operation& op) const;

private:
    //! @brief The heap to be traversed.
    const MinHeap* const heap{nullptr};
};
} // namespace min
} // namespace heap
} // namespace date_structure
