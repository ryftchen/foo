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
#pragma pack(8)
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
#pragma pack()
#ifdef __cplusplus
}
#endif // __cplusplus

extern int createDll(DLL* dll);
extern int destroyDll(DLL* dll);
extern int dllSize(DLL pHead);
extern bool dllIsEmpty(DLL pHead);
extern void* dllGet(DLL pHead, const int index);
extern void* dllGetFirst(DLL pHead);
extern void* dllGetLast(DLL pHead);
extern int dllInsert(DLL pHead, const int index, void* const pVal);
extern int dllInsertFirst(DLL pHead, void* const pVal);
extern int dllInsertLast(DLL pHead, void* const pVal);
extern int dllDelete(DLL pHead, const int index);
extern int dllDeleteFirst(DLL pHead);
extern int dllDeleteLast(DLL pHead);
} // namespace doubly_linked_list

//! @brief The stack structure.
namespace stack
{
//! @brief Alias for the doubly linked list. Used for the stack.
using Stack = doubly_linked_list::DLL;

extern int createStack(Stack* stack);
extern int destroyStack(Stack* stack);
extern int stackPush(Stack pHead, void* const pVal);
extern void* stackTop(Stack pHead);
extern void* stackPop(Stack pHead);
extern int stackSize(Stack pHead);
extern bool stackIsEmpty(Stack pHead);
} // namespace stack

//! @brief The queue structure.
namespace queue
{
//! @brief Alias for the doubly linked list. Used for the queue.
using Queue = doubly_linked_list::DLL;

extern int createQueue(Queue* queue);
extern int destroyQueue(Queue* queue);
extern int queuePush(Queue pHead, void* const pVal);
extern void* queueFront(Queue pHead);
extern void* queuePop(Queue pHead);
extern int queueSize(Queue pHead);
extern bool queueIsEmpty(Queue pHead);
} // namespace queue

//! @brief Output helper for the data structure.
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of output stream object, which is on string based
    std::ostringstream& flush();

private:
    //! @brief Output stream of the data structure.
    std::ostringstream stream;
};
} // namespace date_structure::linear
