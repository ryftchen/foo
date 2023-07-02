//! @file linear.cpp
//! @author ryftchen
//! @brief The definitions (linear) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen.

#include "linear.hpp"
#include <iostream>

namespace date_structure::linear
{
namespace doubly_linked_list
{
//! @brief Create a node of the doubly linked list.
//! @param pVal - value of the target node
//! @return node
static Node* createNode(void* const pVal)
{
    Node* pNode = new (std::nothrow) Node;
    if (nullptr == pNode)
    {
        std::cerr << "Create node failed." << std::endl;
        return nullptr;
    }

    pNode->prev = pNode->next = pNode;
    pNode->p = pVal;

    return pNode;
}

//! @brief Get the node of the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @return node
static Node* getNode(DLL pHead, const int index)
{
    int count = 0;
    Node* pNode = pHead->next;
    while (pNode != pHead)
    {
        ++count;
        pNode = pNode->next;
    }

    if ((index < 0) || (index >= count))
    {
        std::cerr << "Get node failed, the index in out of bound." << std::endl;
        return nullptr;
    }

    if (index <= (count / 2))
    {
        int i = 0;
        Node* pNode = pHead->next;
        while ((i++) < index)
        {
            pNode = pNode->next;
        }

        return pNode;
    }

    int j = 0;
    const int rIndex = count - index - 1;
    Node* rNode = pHead->prev;
    while ((j++) < rIndex)
    {
        rNode = rNode->prev;
    }

    return rNode;
}

//! @brief Create a doubly linked list.
//! @param dll - doubly linked list
//! @return the value is 0 if successful, otherwise -1
int createDll(DLL* dll)
{
    *dll = createNode(nullptr);
    if (nullptr == *dll)
    {
        return -1;
    }

    return 0;
}

//! @brief Destroy a doubly linked list.
//! @param dll - doubly linked list
//! @return the value is 0 if successful, otherwise -1
int destroyDll(DLL* dll)
{
    if (nullptr == *dll)
    {
        std::cerr << "Destroy failed, doubly linked list is null." << std::endl;
        return -1;
    }

    Node* pNode = (*dll)->next;
    Node* pTemp = nullptr;
    while (pNode != *dll)
    {
        pTemp = pNode;
        pNode = pNode->next;
        delete pTemp;
    }

    delete *dll;
    *dll = nullptr;

    return 0;
}

//! @brief Get the size of the doubly linked list.
//! @param pHead - head of the list
//! @return size of the doubly linked list
int dllSize(DLL pHead)
{
    int count = 0;
    Node* pNode = pHead->next;
    while (pNode != pHead)
    {
        ++count;
        pNode = pNode->next;
    }

    return count;
}

//! @brief Check whether any nodes do not exist in the doubly linked list.
//! @param pHead - head of the list
//! @return any nodes do not exist or exist
bool dllIsEmpty(DLL pHead)
{
    return (0 == dllSize(pHead));
}

//! @brief Get the node of the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @return node of the doubly linked list
void* dllGet(DLL pHead, const int index)
{
    Node* pIndex = getNode(pHead, index);
    if (nullptr == pIndex)
    {
        std::cerr << "Get node failed." << std::endl;
        return nullptr;
    }

    return pIndex->p;
}

//! @brief Get the first node of the doubly linked list.
//! @param pHead - head of the list
//! @return first node of the doubly linked list
void* dllGetFirst(DLL pHead)
{
    return dllGet(pHead, 0);
}

//! @brief Get the last node of the doubly linked list.
//! @param pHead - head of the list
//! @return last node of the doubly linked list
void* dllGetLast(DLL pHead)
{
    return dllGet(pHead, dllSize(pHead) - 1);
}

//! @brief Insert the target node into the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
int dllInsert(DLL pHead, const int index, void* const pVal)
{
    if (0 == index)
    {
        return dllInsertFirst(pHead, pVal);
    }

    Node* pIndex = getNode(pHead, index);
    if (nullptr == pIndex)
    {
        return -1;
    }

    Node* pNode = createNode(pVal);
    if (nullptr == pNode)
    {
        return -1;
    }

    pNode->prev = pIndex->prev;
    pNode->next = pIndex;
    pIndex->prev->next = pNode;
    pIndex->prev = pNode;

    return 0;
}

//! @brief Insert the target node into the doubly linked list as the first node.
//! @param pHead - head of the list
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
int dllInsertFirst(DLL pHead, void* const pVal)
{
    Node* pNode = createNode(pVal);
    if (nullptr == pNode)
    {
        return -1;
    }

    pNode->prev = pHead;
    pNode->next = pHead->next;
    pHead->next->prev = pNode;
    pHead->next = pNode;

    return 0;
}

//! @brief Insert the target node into the doubly linked list as the last node.
//! @param pHead - head of the list
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
int dllInsertLast(DLL pHead, void* const pVal)
{
    Node* pNode = createNode(pVal);
    if (nullptr == pNode)
    {
        return -1;
    }

    pNode->next = pHead;
    pNode->prev = pHead->prev;
    pHead->prev->next = pNode;
    pHead->prev = pNode;

    return 0;
}

//! @brief Delete the target node from the doubly linked list by index.
//! @param pHead - head of the list
//! @param index - node index
//! @return the value is 0 if successful, otherwise -1
int dllDelete(DLL pHead, const int index)
{
    Node* pIndex = getNode(pHead, index);
    if (nullptr == pIndex)
    {
        std::cerr << "Delete node failed, the index in out of bound." << std::endl;
        return -1;
    }

    pIndex->next->prev = pIndex->prev;
    pIndex->prev->next = pIndex->next;
    delete pIndex;

    return 0;
}

//! @brief Delete the first node from the doubly linked list.
//! @param pHead - head of the list
//! @return the value is 0 if successful, otherwise -1
int dllDeleteFirst(DLL pHead)
{
    return dllDelete(pHead, 0);
}

//! @brief Delete the last node from the doubly linked list.
//! @param pHead - head of the list
//! @return the value is 0 if successful, otherwise -1
int dllDeleteLast(DLL pHead)
{
    return dllDelete(pHead, dllSize(pHead) - 1);
}
} // namespace doubly_linked_list

namespace stack
{
//! @brief Create a stack.
//! @param stack - stack
//! @return the value is 0 if successful, otherwise -1
int createStack(Stack* stack)
{
    return createDll(stack);
}

//! @brief Destroy a stack.
//! @param stack - stack
//! @return the value is 0 if successful, otherwise -1
int destroyStack(Stack* stack)
{
    return destroyDll(stack);
}

//! @brief Push operation of the stack.
//! @param pHead - head of the stack
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
int stackPush(Stack pHead, void* const pVal)
{
    return dllInsertFirst(pHead, pVal);
}

//! @brief Top operation of the stack.
//! @param pHead - head of the stack
//! @return target node
void* stackTop(Stack pHead)
{
    return dllGetFirst(pHead);
}

//! @brief Pop operation of the stack.
//! @param pHead - head of the stack
//! @return target node
void* stackPop(Stack pHead)
{
    void* p = stackTop(pHead);
    dllDeleteFirst(pHead);
    return p;
}

//! @brief Get the size of the stack.
//! @param pHead - head of the stack
//! @return size of the stack
int stackSize(Stack pHead)
{
    return dllSize(pHead);
}

//! @brief Check whether any nodes do not exist in the stack.
//! @param pHead - head of the stack
//! @return any nodes do not exist or exist
bool stackIsEmpty(Stack pHead)
{
    return dllIsEmpty(pHead);
}
} // namespace stack

namespace queue
{
//! @brief Create a queue.
//! @param queue - queue
//! @return the value is 0 if successful, otherwise -1
int createQueue(Queue* queue)
{
    return createDll(queue);
}

//! @brief Destroy a queue.
//! @param queue - queue
//! @return the value is 0 if successful, otherwise -1
int destroyQueue(Queue* queue)
{
    return destroyDll(queue);
}

//! @brief Push operation of the queue.
//! @param pHead - head of the queue
//! @param pVal - value of the target node
//! @return the value is 0 if successful, otherwise -1
int queuePush(Queue pHead, void* const pVal)
{
    return dllInsertLast(pHead, pVal);
}

//! @brief Front operation of the queue.
//! @param pHead - head of the queue
//! @return target node
void* queueFront(Queue pHead)
{
    return dllGetFirst(pHead);
}

//! @brief Pop operation of the queue.
//! @param pHead - head of the queue
//! @return target node
void* queuePop(Queue pHead)
{
    void* p = dllGetFirst(pHead);
    dllDeleteFirst(pHead);
    return p;
}

//! @brief Get the size of the queue.
//! @param pHead - head of the queue
//! @return size of the queue
int queueSize(Queue pHead)
{
    return dllSize(pHead);
}

//! @brief Check whether any nodes do not exist in the queue.
//! @param pHead - head of the queue
//! @return any nodes do not exist or exist
bool queueIsEmpty(Queue pHead)
{
    return dllIsEmpty(pHead);
}
} // namespace queue
} // namespace date_structure::linear
