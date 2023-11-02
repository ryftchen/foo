//! @file linear.hpp
//! @author ryftchen
//! @brief The declarations (linear) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <sstream>

//! @brief Linear-related functions in the data structure module.
namespace date_structure::linear
{
//! @brief The doubly linked list structure.
namespace doubly_linked_list
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 8)
    //! @brief The node of the doubly linked list.
    typedef struct TagNode
    {
        //! @brief Pointer to the previous node.
        struct TagNode* prev;
        //! @brief Pointer to the next node.
        struct TagNode* next;
        //! @brief Pointer to node content.
        void* p;
    } Node, *DLL;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern int createDll(DLL* dll);
extern int destroyDll(DLL* dll);
extern int dllSize(DLL head);
extern bool dllIsEmpty(DLL head);
extern void* dllGet(DLL head, const int index);
extern void* dllGetFirst(DLL head);
extern void* dllGetLast(DLL head);
extern int dllInsert(DLL head, const int index, void* const val);
extern int dllInsertFirst(DLL head, void* const val);
extern int dllInsertLast(DLL head, void* const val);
extern int dllDelete(DLL head, const int index);
extern int dllDeleteFirst(DLL head);
extern int dllDeleteLast(DLL head);
} // namespace doubly_linked_list

//! @brief The stack structure.
namespace stack
{
//! @brief Alias for the doubly linked list. Used for the stack.
using Stack = doubly_linked_list::DLL;

extern int createStack(Stack* stack);
extern int destroyStack(Stack* stack);
extern int stackPush(Stack head, void* const val);
extern void* stackTop(Stack head);
extern void* stackPop(Stack head);
extern int stackSize(Stack head);
extern bool stackIsEmpty(Stack head);
} // namespace stack

//! @brief The queue structure.
namespace queue
{
//! @brief Alias for the doubly linked list. Used for the queue.
using Queue = doubly_linked_list::DLL;

extern int createQueue(Queue* queue);
extern int destroyQueue(Queue* queue);
extern int queuePush(Queue head, void* const val);
extern void* queueFront(Queue head);
extern void* queuePop(Queue head);
extern int queueSize(Queue head);
extern bool queueIsEmpty(Queue head);
} // namespace queue

//! @brief Output helper for the data structure.
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& flush();

private:
    //! @brief Output stream of the data structure.
    std::ostringstream stream;
};
} // namespace date_structure::linear
