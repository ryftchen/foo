//! @file linear.cpp
//! @author ryftchen
//! @brief The definitions (linear) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "linear.hpp"

namespace date_structure::linear
{
namespace doubly_linked_list
{
//! @brief Create a node of the doubly linked list.
//! @param val - value of the target node
//! @return node
static Node* createNode(void* const val)
{
    Node* const node = new (std::nothrow) Node;
    if (nullptr == node)
    {
        return nullptr;
    }

    node->prev = node->next = node;
    node->p = val;

    return node;
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return node
static Node* getNode(DLL head, const int index)
{
    int count = 0;
    Node* node = head->next;
    while (node != head)
    {
        ++count;
        node = node->next;
    }

    if ((index < 0) || (index >= count))
    {
        return nullptr;
    }

    if (index <= (count / 2))
    {
        int i = 0;
        node = head->next;
        while ((i++) < index)
        {
            node = node->next;
        }

        return node;
    }

    int j = 0;
    const int rIndex = count - index - 1;
    Node* rNode = head->prev;
    while ((j++) < rIndex)
    {
        rNode = rNode->prev;
    }

    return rNode;
}

//! @brief Create a doubly linked list.
//! @param dll - doubly linked list
//! @return the value is 0 if successful, otherwise -1
int createDll(DLL* const dll)
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
int destroyDll(DLL* const dll)
{
    if (nullptr == *dll)
    {
        return -1;
    }

    const Node *node = (*dll)->next, *temp = nullptr;
    while (node != *dll)
    {
        temp = node;
        node = node->next;
        delete temp;
    }

    delete *dll;
    *dll = nullptr;

    return 0;
}

//! @brief Get the size of the doubly linked list.
//! @param head - head of the list
//! @return size of the doubly linked list
int dllSize(DLL head)
{
    int count = 0;
    const Node* node = head->next;
    while (node != head)
    {
        ++count;
        node = node->next;
    }

    return count;
}

//! @brief Check whether any nodes do not exist in the doubly linked list.
//! @param head - head of the list
//! @return any nodes do not exist or exist
bool dllIsEmpty(DLL head)
{
    return (0 == dllSize(head));
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return node of the doubly linked list
void* dllGet(DLL head, const int index)
{
    const Node* const node = getNode(head, index);
    if (nullptr == node)
    {
        return nullptr;
    }

    return node->p;
}

//! @brief Get the first node of the doubly linked list.
//! @param head - head of the list
//! @return first node of the doubly linked list
void* dllGetFirst(DLL head)
{
    return dllGet(head, 0);
}

//! @brief Get the last node of the doubly linked list.
//! @param head - head of the list
//! @return last node of the doubly linked list
void* dllGetLast(DLL head)
{
    return dllGet(head, dllSize(head) - 1);
}

//! @brief Insert the target node into the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @param val - value of the target node
//! @return the value is 0 if successful, otherwise -1
int dllInsert(DLL head, const int index, void* const val)
{
    if (0 == index)
    {
        return dllInsertFirst(head, val);
    }

    Node* const node = getNode(head, index);
    if (nullptr == node)
    {
        return -1;
    }

    Node* const newNode = createNode(val);
    if (nullptr == newNode)
    {
        return -1;
    }

    newNode->prev = node->prev;
    newNode->next = node;
    node->prev->next = newNode;
    node->prev = newNode;

    return 0;
}

//! @brief Insert the target node into the doubly linked list as the first node.
//! @param head - head of the list
//! @param val - value of the target node
//! @return the value is 0 if successful, otherwise -1
int dllInsertFirst(DLL head, void* const val)
{
    Node* const node = createNode(val);
    if (nullptr == node)
    {
        return -1;
    }

    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;

    return 0;
}

//! @brief Insert the target node into the doubly linked list as the last node.
//! @param head - head of the list
//! @param val - value of the target node
//! @return the value is 0 if successful, otherwise -1
int dllInsertLast(DLL head, void* const val)
{
    Node* const node = createNode(val);
    if (nullptr == node)
    {
        return -1;
    }

    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;

    return 0;
}

//! @brief Delete the target node from the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return the value is 0 if successful, otherwise -1
int dllDelete(DLL head, const int index)
{
    const Node* const node = getNode(head, index);
    if (nullptr == node)
    {
        return -1;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;
    delete node;

    return 0;
}

//! @brief Delete the first node from the doubly linked list.
//! @param head - head of the list
//! @return the value is 0 if successful, otherwise -1
int dllDeleteFirst(DLL head)
{
    return dllDelete(head, 0);
}

//! @brief Delete the last node from the doubly linked list.
//! @param head - head of the list
//! @return the value is 0 if successful, otherwise -1
int dllDeleteLast(DLL head)
{
    return dllDelete(head, dllSize(head) - 1);
}
} // namespace doubly_linked_list

namespace stack
{
//! @brief Create a stack.
//! @param stack - stack
//! @return the value is 0 if successful, otherwise -1
int createStack(Stack* const stack)
{
    return createDll(stack);
}

//! @brief Destroy a stack.
//! @param stack - stack
//! @return the value is 0 if successful, otherwise -1
int destroyStack(Stack* const stack)
{
    return destroyDll(stack);
}

//! @brief Push operation of the stack.
//! @param head - head of the stack
//! @param val - value of the target node
//! @return the value is 0 if successful, otherwise -1
int stackPush(Stack head, void* const val)
{
    return dllInsertFirst(head, val);
}

//! @brief Top operation of the stack.
//! @param head - head of the stack
//! @return target node
void* stackTop(Stack head)
{
    return dllGetFirst(head);
}

//! @brief Pop operation of the stack.
//! @param head - head of the stack
//! @return target node
void* stackPop(Stack head)
{
    void* const p = stackTop(head);
    dllDeleteFirst(head);
    return p;
}

//! @brief Get the size of the stack.
//! @param head - head of the stack
//! @return size of the stack
int stackSize(Stack head)
{
    return dllSize(head);
}

//! @brief Check whether any nodes do not exist in the stack.
//! @param head - head of the stack
//! @return any nodes do not exist or exist
bool stackIsEmpty(Stack head)
{
    return dllIsEmpty(head);
}
} // namespace stack

namespace queue
{
//! @brief Create a queue.
//! @param queue - queue
//! @return the value is 0 if successful, otherwise -1
int createQueue(Queue* const queue)
{
    return createDll(queue);
}

//! @brief Destroy a queue.
//! @param queue - queue
//! @return the value is 0 if successful, otherwise -1
int destroyQueue(Queue* const queue)
{
    return destroyDll(queue);
}

//! @brief Push operation of the queue.
//! @param head - head of the queue
//! @param val - value of the target node
//! @return the value is 0 if successful, otherwise -1
int queuePush(Queue head, void* const val)
{
    return dllInsertLast(head, val);
}

//! @brief Front operation of the queue.
//! @param head - head of the queue
//! @return target node
void* queueFront(Queue head)
{
    return dllGetFirst(head);
}

//! @brief Pop operation of the queue.
//! @param head - head of the queue
//! @return target node
void* queuePop(Queue head)
{
    void* const p = dllGetFirst(head);
    dllDeleteFirst(head);
    return p;
}

//! @brief Get the size of the queue.
//! @param head - head of the queue
//! @return size of the queue
int queueSize(Queue head)
{
    return dllSize(head);
}

//! @brief Check whether any nodes do not exist in the queue.
//! @param head - head of the queue
//! @return any nodes do not exist or exist
bool queueIsEmpty(Queue head)
{
    return dllIsEmpty(head);
}
} // namespace queue

std::ostringstream& Output::flush()
{
    return stream;
}
} // namespace date_structure::linear
