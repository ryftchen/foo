//! @file linear.hpp
//! @author ryftchen
//! @brief The declarations (linear) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <sstream>

//! @brief The data structure module.
namespace date_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Linear-related functions in the data structure module.
namespace linear
{
extern const char* version() noexcept;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 8)
    //! @brief The node of the linear structure.
    typedef struct TagNode
    {
        //! @brief Pointer to the previous node.
        struct TagNode* prev;
        //! @brief Pointer to the next node.
        struct TagNode* next;
        //! @brief Node data.
        void* p;
    } Node, *Linear;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

//! @brief The doubly linked list structure.
namespace doubly_linked_list
{
//! @brief Alias for the linear structure. Used for the doubly linked list.
using DLL = Linear;

extern bool create(DLL* const dll);
extern bool destroy(DLL* const dll);
extern int size(const DLL head);
extern bool empty(const DLL head);
extern void* get(const DLL head, const int index);
extern void* getFirst(const DLL head);
extern void* getLast(const DLL head);
extern bool insert(const DLL head, const int index, const void* const value);
extern bool insertFirst(const DLL head, const void* const value);
extern bool insertLast(const DLL head, const void* const value);
extern bool remove(const DLL head, const int index);
extern bool removeFirst(const DLL head);
extern bool removeLast(const DLL head);
} // namespace doubly_linked_list

//! @brief The stack structure (FILO/LIFO).
namespace stack
{
//! @brief Alias for the linear structure. Used for the stack.
using Stack = Linear;

extern bool create(Stack* const stack);
extern bool destroy(Stack* const stack);
extern int size(const Stack head);
extern bool empty(const Stack head);
extern bool push(const Stack head, const void* const value);
extern void* top(const Stack head);
extern void* pop(const Stack head);
} // namespace stack

//! @brief The queue structure (FIFO/LILO).
namespace queue
{
//! @brief Alias for the linear structure. Used for the queue.
using Queue = Linear;

extern bool create(Queue* const queue);
extern bool destroy(Queue* const queue);
extern int size(const Queue head);
extern bool empty(const Queue head);
extern bool push(const Queue head, const void* const value);
extern void* front(const Queue head);
extern void* pop(const Queue head);
} // namespace queue

//! @brief Output helper for the data structure.
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;

private:
    //! @brief Output stream of the data structure.
    std::ostringstream process;
};
} // namespace linear
} // namespace date_structure
