//! @file linear.cpp
//! @author ryftchen
//! @brief The definitions (linear) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "linear.hpp"

namespace data_structure::linear
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
namespace dll
{
//! @brief Create the node of the doubly linked list.
//! @param value - value of the target node
//! @return new node
static Node* createNode(const void* const value)
{
    Node* const newNode = ::new (std::nothrow) Node;
    if (!newNode)
    {
        return nullptr;
    }

    newNode->prev = newNode->next = newNode;
    newNode->value = const_cast<void*>(value);

    return newNode;
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - target index
//! @return node at the index
static Node* getNode(const DLL head, const int index)
{
    if (!head || (index < 0))
    {
        return nullptr;
    }

    Node* nNext = head->next;
    int counter = 0;
    while (nNext != head)
    {
        ++counter;
        nNext = nNext->next;
    }
    if (index >= counter)
    {
        return nullptr;
    }

    if (index <= (counter / 2))
    {
        nNext = head->next;
        int i = 0;
        while ((i++) < index)
        {
            nNext = nNext->next;
        }

        return nNext;
    }

    Node* pNode = head->prev;
    const int rIndex = counter - index - 1;
    int j = 0;
    while ((j++) < rIndex)
    {
        pNode = pNode->prev;
    }

    return pNode;
}

//! @brief Create the doubly linked list.
//! @param dll - doubly linked list
//! @return success or failure
bool create(DLL* const dll)
{
    return dll ? (*dll = createNode(nullptr)) != nullptr : false;
}

//! @brief Destroy the doubly linked list.
//! @param dll - doubly linked list
//! @return success or failure
bool destroy(DLL* const dll)
{
    if (!dll || !*dll)
    {
        return false;
    }

    const Node *nNode = (*dll)->next, *temp = nullptr;
    while (nNode != *dll)
    {
        temp = nNode;
        nNode = nNode->next;
        ::delete temp;
        temp = nullptr;
    }

    ::delete *dll;
    *dll = nullptr;

    return true;
}

//! @brief Get the size of the doubly linked list.
//! @param head - head of the list
//! @return size of the doubly linked list
int size(const DLL head)
{
    if (!head)
    {
        return 0;
    }

    const Node* nNode = head->next;
    int counter = 0;
    while (nNode != head)
    {
        ++counter;
        nNode = nNode->next;
    }

    return counter;
}

//! @brief Check whether any nodes do not exist in the doubly linked list.
//! @param head - head of the list
//! @return any nodes do not exist or exist
bool empty(const DLL head)
{
    return size(head) == 0;
}

//! @brief Get the node value of the doubly linked list by index.
//! @param head - head of the list
//! @param index - target index
//! @return node value
void* get(const DLL head, const int index)
{
    const Node* const iNode = getNode(head, index);
    return iNode ? iNode->value : nullptr;
}

//! @brief Get the first node value of the doubly linked list.
//! @param head - head of the list
//! @return first node value
void* getFirst(const DLL head)
{
    return get(head, 0);
}

//! @brief Get the last node value of the doubly linked list.
//! @param head - head of the list
//! @return last node value
void* getLast(const DLL head)
{
    return get(head, size(head) - 1);
}

//! @brief Insert the node into the doubly linked list by index.
//! @param head - head of the list
//! @param index - target index
//! @param value - value of the target node
//! @return success or failure
bool insert(const DLL head, const int index, const void* const value)
{
    if (index == 0)
    {
        return insertFirst(head, value);
    }

    Node* const iNode = getNode(head, index);
    if (!iNode)
    {
        return false;
    }

    Node* const newNode = createNode(value);
    if (!newNode)
    {
        return false;
    }

    newNode->prev = iNode->prev;
    newNode->next = iNode;
    iNode->prev->next = newNode;
    iNode->prev = newNode;

    return true;
}

//! @brief Insert the node into the doubly linked list as the first node.
//! @param head - head of the list
//! @param value - value of the target node
//! @return success or failure
bool insertFirst(const DLL head, const void* const value)
{
    if (!head)
    {
        return false;
    }

    Node* const newNode = createNode(value);
    if (!newNode)
    {
        return false;
    }

    newNode->prev = head;
    newNode->next = head->next;
    head->next->prev = newNode;
    head->next = newNode;

    return true;
}

//! @brief Insert the node into the doubly linked list as the last node.
//! @param head - head of the list
//! @param value - value of the target node
//! @return success or failure
bool insertLast(const DLL head, const void* const value)
{
    if (!head)
    {
        return false;
    }

    Node* const newNode = createNode(value);
    if (!newNode)
    {
        return false;
    }

    newNode->next = head;
    newNode->prev = head->prev;
    head->prev->next = newNode;
    head->prev = newNode;

    return true;
}

//! @brief Remove the node from the doubly linked list by index.
//! @param head - head of the list
//! @param index - target index
//! @return success or failure
bool remove(const DLL head, const int index)
{
    const Node* iNode = getNode(head, index);
    if (!iNode)
    {
        return false;
    }

    iNode->next->prev = iNode->prev;
    iNode->prev->next = iNode->next;
    ::delete iNode;
    iNode = nullptr;

    return true;
}

//! @brief Remove the first node from the doubly linked list.
//! @param head - head of the list
//! @return success or failure
bool removeFirst(const DLL head)
{
    return remove(head, 0);
}

//! @brief Remove the last node from the doubly linked list.
//! @param head - head of the list
//! @return success or failure
bool removeLast(const DLL head)
{
    return remove(head, size(head) - 1);
}
} // namespace dll

namespace stack
{
//! @brief Create the stack.
//! @param stk - stack
//! @return success or failure
bool create(Stack* const stk)
{
    return dll::create(stk);
}

//! @brief Destroy the stack.
//! @param stk - stack
//! @return success or failure
bool destroy(Stack* const stk)
{
    return dll::destroy(stk);
}

//! @brief Get the size of the stack.
//! @param head - head of the stack
//! @return size of the stack
int size(const Stack head)
{
    return dll::size(head);
}

//! @brief Check whether any nodes do not exist in the stack.
//! @param head - head of the stack
//! @return any nodes do not exist or exist
bool empty(const Stack head)
{
    return dll::empty(head);
}

//! @brief Push operation of the stack.
//! @param head - head of the stack
//! @param value - value of the target node
//! @return success or failure
bool push(const Stack head, const void* const value)
{
    return dll::insertFirst(head, value);
}

//! @brief Top operation of the stack.
//! @param head - head of the stack
//! @return node value
void* top(const Stack head)
{
    return dll::getFirst(head);
}

//! @brief Pop operation of the stack.
//! @param head - head of the stack
//! @return node value
void* pop(const Stack head)
{
    void* const value = top(head);
    dll::removeFirst(head);

    return value;
}
} // namespace stack

namespace queue
{
//! @brief Create the queue.
//! @param que - queue
//! @return success or failure
bool create(Queue* const que)
{
    return dll::create(que);
}

//! @brief Destroy the queue.
//! @param que - queue
//! @return success or failure
bool destroy(Queue* const que)
{
    return dll::destroy(que);
}

//! @brief Get the size of the queue.
//! @param head - head of the queue
//! @return size of the queue
int size(const Queue head)
{
    return dll::size(head);
}

//! @brief Check whether any nodes do not exist in the queue.
//! @param head - head of the queue
//! @return any nodes do not exist or exist
bool empty(const Queue head)
{
    return dll::empty(head);
}

//! @brief Push operation of the queue.
//! @param head - head of the queue
//! @param value - value of the target node
//! @return success or failure
bool push(const Queue head, const void* const value)
{
    return dll::insertLast(head, value);
}

//! @brief Front operation of the queue.
//! @param head - head of the queue
//! @return node value
void* front(const Queue head)
{
    return dll::getFirst(head);
}

//! @brief Pop operation of the queue.
//! @param head - head of the queue
//! @return node value
void* pop(const Queue head)
{
    void* const value = dll::getFirst(head);
    dll::removeFirst(head);

    return value;
}
} // namespace queue
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)

void Traverse::order(const Operation& op) const
{
    if (!linear || !*linear || !op)
    {
        return;
    }

    for (int i = 0; i < dll::size(*linear); ++i)
    {
        op(dll::get(*linear, i));
    }
}
} // namespace data_structure::linear
