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

// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
namespace dll
{
//! @brief Create a node of the doubly linked list.
//! @param value - value of the target node
//! @return new node
static Node* createNode(const void* const value)
{
    Node* const node = ::new (std::nothrow) Node;
    if (!node)
    {
        return nullptr;
    }

    node->prev = node->next = node;
    node->p = const_cast<void*>(value);

    return node;
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return node at the index
static Node* getNode(const DLL head, const int index)
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
//! @return success or failure
bool create(DLL* const dll)
{
    return (*dll = createNode(nullptr)) != nullptr;
}

//! @brief Destroy a doubly linked list.
//! @param dll - doubly linked list
//! @return success or failure
bool destroy(DLL* const dll)
{
    if (!*dll)
    {
        return false;
    }

    const Node *node = (*dll)->next, *temp = nullptr;
    while (node != *dll)
    {
        temp = node;
        node = node->next;
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
bool empty(const DLL head)
{
    return size(head) == 0;
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return node of the doubly linked list
void* get(const DLL head, const int index)
{
    const Node* const node = getNode(head, index);
    return node ? node->p : nullptr;
}

//! @brief Get the first node of the doubly linked list.
//! @param head - head of the list
//! @return first node of the doubly linked list
void* getFirst(const DLL head)
{
    return get(head, 0);
}

//! @brief Get the last node of the doubly linked list.
//! @param head - head of the list
//! @return last node of the doubly linked list
void* getLast(const DLL head)
{
    return get(head, size(head) - 1);
}

//! @brief Insert the target node into the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @param value - value of the target node
//! @return success or failure
bool insert(const DLL head, const int index, const void* const value)
{
    if (index == 0)
    {
        return insertFirst(head, value);
    }

    Node* const node = getNode(head, index);
    if (!node)
    {
        return false;
    }

    Node* const newNode = createNode(value);
    if (!newNode)
    {
        return false;
    }

    newNode->prev = node->prev;
    newNode->next = node;
    node->prev->next = newNode;
    node->prev = newNode;

    return true;
}

//! @brief Insert the target node into the doubly linked list as the first node.
//! @param head - head of the list
//! @param value - value of the target node
//! @return success or failure
bool insertFirst(const DLL head, const void* const value)
{
    Node* const node = createNode(value);
    if (!node)
    {
        return false;
    }

    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;

    return true;
}

//! @brief Insert the target node into the doubly linked list as the last node.
//! @param head - head of the list
//! @param value - value of the target node
//! @return success or failure
bool insertLast(const DLL head, const void* const value)
{
    Node* const node = createNode(value);
    if (!node)
    {
        return false;
    }

    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;

    return true;
}

//! @brief Remove the target node from the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return success or failure
bool remove(const DLL head, const int index)
{
    const Node* node = getNode(head, index);
    if (!node)
    {
        return false;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;
    ::delete node;
    node = nullptr;

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
//! @brief Create a stack.
//! @param stack - stack
//! @return success or failure
bool create(Stack* const stack)
{
    return dll::create(stack);
}

//! @brief Destroy a stack.
//! @param stack - stack
//! @return success or failure
bool destroy(Stack* const stack)
{
    return dll::destroy(stack);
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
//! @return target node
void* top(const Stack head)
{
    return dll::getFirst(head);
}

//! @brief Pop operation of the stack.
//! @param head - head of the stack
//! @return target node
void* pop(const Stack head)
{
    void* const p = top(head);
    dll::removeFirst(head);

    return p;
}
} // namespace stack

namespace queue
{
//! @brief Create a queue.
//! @param queue - queue
//! @return success or failure
bool create(Queue* const queue)
{
    return dll::create(queue);
}

//! @brief Destroy a queue.
//! @param queue - queue
//! @return success or failure
bool destroy(Queue* const queue)
{
    return dll::destroy(queue);
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
//! @return target node
void* front(const Queue head)
{
    return dll::getFirst(head);
}

//! @brief Pop operation of the queue.
//! @param head - head of the queue
//! @return target node
void* pop(const Queue head)
{
    void* const p = dll::getFirst(head);
    dll::removeFirst(head);

    return p;
}
} // namespace queue

std::ostringstream& Output::output() noexcept
{
    return process;
}
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::linear
