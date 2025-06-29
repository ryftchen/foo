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
    //! @brief The node of the linear data structure.
    typedef struct TagNode
    {
        //! @brief Pointer to the previous node.
        struct TagNode* prev;
        //! @brief Pointer to the next node.
        struct TagNode* next;
        //! @brief Pointer to node content.
        void* p;
    } Node, *Linear;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

//! @brief The doubly linked list structure.
namespace doubly_linked_list
{
//! @brief Alias for the linear data structure. Used for the doubly linked list.
using DLL = Linear;

extern int create(DLL* const dll);
extern int destroy(DLL* const dll);
extern int size(DLL head);
extern bool empty(DLL head);
extern void* get(DLL head, const int index);
extern void* getFirst(DLL head);
extern void* getLast(DLL head);
extern int insert(DLL head, const int index, void* const val);
extern int insertFirst(DLL head, void* const val);
extern int insertLast(DLL head, void* const val);
extern int remove(DLL head, const int index);
extern int removeFirst(DLL head);
extern int removeLast(DLL head);
} // namespace doubly_linked_list

//! @brief The stack structure (FILO/LIFO).
namespace stack
{
//! @brief Alias for the linked list. Used for the stack.
using Stack = Linear;

extern int create(Stack* const stack);
extern int destroy(Stack* const stack);
extern int push(Stack head, void* const val);
extern void* top(Stack head);
extern void* pop(Stack head);
extern int size(Stack head);
extern bool empty(Stack head);
} // namespace stack

//! @brief The queue structure (FIFO/LILO).
namespace queue
{
//! @brief Alias for the linked list. Used for the queue.
using Queue = Linear;

extern int create(Queue* const queue);
extern int destroy(Queue* const queue);
extern int push(Queue head, void* const val);
extern void* front(Queue head);
extern void* pop(Queue head);
extern int size(Queue head);
extern bool empty(Queue head);
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
