//! @file linear.hpp
//! @author ryftchen
//! @brief The declarations (linear) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

//! @brief Linear-related functions in the data structure module.
namespace date_structure::linear
{
//! @brief The doubly linked list structure.
namespace doubly_linked_list
{
#ifdef __cplusplus
extern "C"
{
#endif
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
#ifdef __cplusplus
}
#endif

//! @brief Create a node of the doubly linked list.
//! @param pVal - value of the target node
//! @return node
static Node* createNode(void* const pVal);
//! @brief Get the node of the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @return node
static Node* getNode(DLL pHead, const int index);
//! @brief Create a doubly linked list.
//! @param dll - doubly linked list
//! @return the value is 0 if successful, otherwise -1

static int createDll(DLL* dll);
//! @brief Destroy a doubly linked list.
//! @param dll - doubly linked list
//! @return the value is 0 if successful, otherwise -1
static int destroyDll(DLL* dll);
//! @brief Get the size of the doubly linked list.
//! @param pHead - head of the list
//! @return size of the doubly linked list
static int dllSize(DLL pHead);
//! @brief Check whether any nodes do not exist in the doubly linked list.
//! @param pHead - head of the list
//! @return any nodes do not exist or exist
static bool dllIsEmpty(DLL pHead);
//! @brief Get the node of the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @return node of the doubly linked list
static void* dllGet(DLL pHead, const int index);
//! @brief Get the first node of the doubly linked list.
//! @param pHead - head of the list
//! @return first node of the doubly linked list
static void* dllGetFirst(DLL pHead);
//! @brief Get the last node of the doubly linked list.
//! @param pHead - head of the list
//! @return last node of the doubly linked list
static void* dllGetLast(DLL pHead);
//! @brief Insert the target node into the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
static int dllInsert(DLL pHead, const int index, void* const pVal);
//! @brief Insert the target node into the doubly linked list as the first node.
//! @param pHead - head of the list
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
static int dllInsertFirst(DLL pHead, void* const pVal);
//! @brief Insert the target node into the doubly linked list as the last node.
//! @param pHead - head of the list
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
static int dllInsertLast(DLL pHead, void* const pVal);
//! @brief Delete the target node from the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @return the value is 0 if successful, otherwise -1
static int dllDelete(DLL pHead, const int index);
//! @brief Delete the first node from the doubly linked list.
//! @param pHead - head of the list
//! @return the value is 0 if successful, otherwise -1
static int dllDeleteFirst(DLL pHead);
//! @brief Delete the last node from the doubly linked list.
//! @param pHead - head of the list
//! @return the value is 0 if successful, otherwise -1
static int dllDeleteLast(DLL pHead);
} // namespace doubly_linked_list

//! @brief The stack structure.
namespace stack
{
//! @brief Alias for the doubly linked list. Used for the stack.
using Stack = doubly_linked_list::DLL;

//! @brief Create a stack.
//! @param stack - stack
//! @return the value is 0 if successful, otherwise -1
static int createStack(Stack* stack);
//! @brief Destroy a stack.
//! @param stack - stack
//! @return the value is 0 if successful, otherwise -1
static int destroyStack(Stack* stack);
//! @brief Push operation of the stack.
//! @param pHead - head of the stack
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
static int stackPush(Stack pHead, void* const pVal);
//! @brief Top operation of the stack.
//! @param pHead - head of the stack
//! @return target node
static void* stackTop(Stack pHead);
//! @brief Pop operation of the stack.
//! @param pHead - head of the stack
//! @return target node
static void* stackPop(Stack pHead);
//! @brief Get the size of the stack.
//! @param pHead - head of the stack
//! @return size of the stack
static int stackSize(Stack pHead);
//! @brief Check whether any nodes do not exist in the stack.
//! @param pHead - head of the stack
//! @return any nodes do not exist or exist
static bool stackIsEmpty(Stack pHead);
} // namespace stack

//! @brief The queue structure.
namespace queue
{
//! @brief Alias for the doubly linked list. Used for the queue.
using Queue = doubly_linked_list::DLL;

//! @brief Create a queue.
//! @param queue - queue
//! @return the value is 0 if successful, otherwise -1
static int createQueue(Queue* queue);
//! @brief Destroy a queue.
//! @param queue - queue
//! @return the value is 0 if successful, otherwise -1
static int destroyQueue(Queue* queue);
//! @brief Push operation of the queue.
//! @param pHead - head of the queue
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
static int queuePush(Queue pHead, void* const pVal);
//! @brief Front operation of the queue.
//! @param pHead - head of the queue
//! @return target node
static void* queueFront(Queue pHead);
//! @brief Pop operation of the queue.
//! @param pHead - head of the queue
//! @return target node
static void* queuePop(Queue pHead);
//! @brief Get the size of the queue.
//! @param pHead - head of the queue
//! @return size of the queue
static int queueSize(Queue pHead);
//! @brief Check whether any nodes do not exist in the queue.
//! @param pHead - head of the queue
//! @return any nodes do not exist or exist
static bool queueIsEmpty(Queue pHead);
} // namespace queue

//! @brief Structure of linear.
class LinearStructure
{
public:
    //! @brief Construct a new LinearStructure object.
    LinearStructure();
    //! @brief Destroy the LinearStructure object.
    virtual ~LinearStructure() = default;

    //! @brief The linked list instance.
    void linkedListInstance() const;
    //! @brief The stack instance.
    void stackInstance() const;
    //! @brief The queue instance.
    void queueInstance() const;
};
} // namespace date_structure::linear
