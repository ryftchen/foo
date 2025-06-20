//! @file linear.cpp
//! @author ryftchen
//! @brief The definitions (linear) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "linear.hpp"

namespace date_structure::linear
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory)
namespace doubly_linked_list
{
//! @brief Create a node of the doubly linked list.
//! @param val - value of the target node
//! @return node
static Node* createNode(void* const val)
{
    auto* const node = ::new (std::nothrow) Node;
    if (!node)
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
    int counter = 0;
    Node* node = head->next;
    while (node != head)
    {
        ++counter;
        node = node->next;
    }

    if ((index < 0) || (index >= counter))
    {
        return nullptr;
    }

    if (index <= (counter / 2))
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
    const int rIndex = counter - index - 1;
    Node* rNode = head->prev;
    while ((j++) < rIndex)
    {
        rNode = rNode->prev;
    }

    return rNode;
}

//! @brief Create a doubly linked list.
//! @param dll - doubly linked list
//! @return 0 if successful, otherwise -1
int createDll(DLL* const dll)
{
    return ((*dll = createNode(nullptr)) != nullptr) ? 0 : -1;
}

//! @brief Destroy a doubly linked list.
//! @param dll - doubly linked list
//! @return 0 if successful, otherwise -1
int destroyDll(DLL* const dll)
{
    if (!*dll)
    {
        return -1;
    }

    const Node *node = (*dll)->next, *temp = nullptr;
    while (node != *dll)
    {
        temp = node;
        node = node->next;
        ::delete temp;
    }

    ::delete *dll;
    *dll = nullptr;

    return 0;
}

//! @brief Get the size of the doubly linked list.
//! @param head - head of the list
//! @return size of the doubly linked list
int dllSize(DLL head)
{
    int counter = 0;
    const Node* node = head->next;
    while (node != head)
    {
        ++counter;
        node = node->next;
    }

    return counter;
}

//! @brief Check whether any nodes do not exist in the doubly linked list.
//! @param head - head of the list
//! @return any nodes do not exist or exist
bool dllIsEmpty(DLL head)
{
    return dllSize(head) == 0;
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return node of the doubly linked list
void* dllGet(DLL head, const int index)
{
    const Node* const node = getNode(head, index);
    return node ? node->p : nullptr;
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
//! @return 0 if successful, otherwise -1
int dllInsert(DLL head, const int index, void* const val)
{
    if (index == 0)
    {
        return dllInsertFirst(head, val);
    }

    Node* const node = getNode(head, index);
    if (!node)
    {
        return -1;
    }

    Node* const newNode = createNode(val);
    if (!newNode)
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
//! @return 0 if successful, otherwise -1
int dllInsertFirst(DLL head, void* const val)
{
    Node* const node = createNode(val);
    if (!node)
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
//! @return 0 if successful, otherwise -1
int dllInsertLast(DLL head, void* const val)
{
    Node* const node = createNode(val);
    if (!node)
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
//! @return 0 if successful, otherwise -1
int dllDelete(DLL head, const int index)
{
    const Node* const node = getNode(head, index);
    if (!node)
    {
        return -1;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;
    ::delete node;

    return 0;
}

//! @brief Delete the first node from the doubly linked list.
//! @param head - head of the list
//! @return 0 if successful, otherwise -1
int dllDeleteFirst(DLL head)
{
    return dllDelete(head, 0);
}

//! @brief Delete the last node from the doubly linked list.
//! @param head - head of the list
//! @return 0 if successful, otherwise -1
int dllDeleteLast(DLL head)
{
    return dllDelete(head, dllSize(head) - 1);
}
} // namespace doubly_linked_list

namespace stack
{
//! @brief Create a stack.
//! @param stack - stack
//! @return 0 if successful, otherwise -1
int createStack(Stack* const stack)
{
    return createDll(stack);
}

//! @brief Destroy a stack.
//! @param stack - stack
//! @return 0 if successful, otherwise -1
int destroyStack(Stack* const stack)
{
    return destroyDll(stack);
}

//! @brief Push operation of the stack.
//! @param head - head of the stack
//! @param val - value of the target node
//! @return 0 if successful, otherwise -1
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
//! @return 0 if successful, otherwise -1
int createQueue(Queue* const queue)
{
    return createDll(queue);
}

//! @brief Destroy a queue.
//! @param queue - queue
//! @return 0 if successful, otherwise -1
int destroyQueue(Queue* const queue)
{
    return destroyDll(queue);
}

//! @brief Push operation of the queue.
//! @param head - head of the queue
//! @param val - value of the target node
//! @return 0 if successful, otherwise -1
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

std::ostringstream& Output::output() noexcept
{
    return process;
}
// NOLINTEND(cppcoreguidelines-owning-memory)
} // namespace date_structure::linear
