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
int create(DLL* const dll)
{
    return ((*dll = createNode(nullptr)) != nullptr) ? 0 : -1;
}

//! @brief Destroy a doubly linked list.
//! @param dll - doubly linked list
//! @return 0 if successful, otherwise -1
int destroy(DLL* const dll)
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
int size(DLL head)
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
bool empty(DLL head)
{
    return size(head) == 0;
}

//! @brief Get the node of the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return node of the doubly linked list
void* get(DLL head, const int index)
{
    const Node* const node = getNode(head, index);
    return node ? node->p : nullptr;
}

//! @brief Get the first node of the doubly linked list.
//! @param head - head of the list
//! @return first node of the doubly linked list
void* getFirst(DLL head)
{
    return get(head, 0);
}

//! @brief Get the last node of the doubly linked list.
//! @param head - head of the list
//! @return last node of the doubly linked list
void* getLast(DLL head)
{
    return get(head, size(head) - 1);
}

//! @brief Insert the target node into the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @param val - value of the target node
//! @return 0 if successful, otherwise -1
int insert(DLL head, const int index, void* const val)
{
    if (index == 0)
    {
        return insertFirst(head, val);
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
int insertFirst(DLL head, void* const val)
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
int insertLast(DLL head, void* const val)
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

//! @brief Remove the target node from the doubly linked list by index.
//! @param head - head of the list
//! @param index - node index
//! @return 0 if successful, otherwise -1
int remove(DLL head, const int index)
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

//! @brief Remove the first node from the doubly linked list.
//! @param head - head of the list
//! @return 0 if successful, otherwise -1
int removeFirst(DLL head)
{
    return remove(head, 0);
}

//! @brief Remove the last node from the doubly linked list.
//! @param head - head of the list
//! @return 0 if successful, otherwise -1
int removeLast(DLL head)
{
    return remove(head, size(head) - 1);
}
} // namespace doubly_linked_list

namespace stack
{
//! @brief Alias for the namespace of the doubly linked list.
namespace dll = doubly_linked_list;

//! @brief Create a stack.
//! @param stack - stack
//! @return 0 if successful, otherwise -1
int create(Stack* const stack)
{
    return dll::create(stack);
}

//! @brief Destroy a stack.
//! @param stack - stack
//! @return 0 if successful, otherwise -1
int destroy(Stack* const stack)
{
    return dll::destroy(stack);
}

//! @brief Push operation of the stack.
//! @param head - head of the stack
//! @param val - value of the target node
//! @return 0 if successful, otherwise -1
int push(Stack head, void* const val)
{
    return dll::insertFirst(head, val);
}

//! @brief Top operation of the stack.
//! @param head - head of the stack
//! @return target node
void* top(Stack head)
{
    return dll::getFirst(head);
}

//! @brief Pop operation of the stack.
//! @param head - head of the stack
//! @return target node
void* pop(Stack head)
{
    void* const p = top(head);
    dll::removeFirst(head);

    return p;
}

//! @brief Get the size of the stack.
//! @param head - head of the stack
//! @return size of the stack
int size(Stack head)
{
    return dll::size(head);
}

//! @brief Check whether any nodes do not exist in the stack.
//! @param head - head of the stack
//! @return any nodes do not exist or exist
bool empty(Stack head)
{
    return dll::empty(head);
}
} // namespace stack

namespace queue
{
//! @brief Alias for the namespace of the doubly linked list.
namespace dll = doubly_linked_list;

//! @brief Create a queue.
//! @param queue - queue
//! @return 0 if successful, otherwise -1
int create(Queue* const queue)
{
    return dll::create(queue);
}

//! @brief Destroy a queue.
//! @param queue - queue
//! @return 0 if successful, otherwise -1
int destroy(Queue* const queue)
{
    return dll::destroy(queue);
}

//! @brief Push operation of the queue.
//! @param head - head of the queue
//! @param val - value of the target node
//! @return 0 if successful, otherwise -1
int push(Queue head, void* const val)
{
    return dll::insertLast(head, val);
}

//! @brief Front operation of the queue.
//! @param head - head of the queue
//! @return target node
void* front(Queue head)
{
    return dll::getFirst(head);
}

//! @brief Pop operation of the queue.
//! @param head - head of the queue
//! @return target node
void* pop(Queue head)
{
    void* const p = dll::getFirst(head);
    dll::removeFirst(head);

    return p;
}

//! @brief Get the size of the queue.
//! @param head - head of the queue
//! @return size of the queue
int size(Queue head)
{
    return dll::size(head);
}

//! @brief Check whether any nodes do not exist in the queue.
//! @param head - head of the queue
//! @return any nodes do not exist or exist
bool empty(Queue head)
{
    return dll::empty(head);
}
} // namespace queue

std::ostringstream& Output::output() noexcept
{
    return process;
}
// NOLINTEND(cppcoreguidelines-owning-memory)
} // namespace date_structure::linear
