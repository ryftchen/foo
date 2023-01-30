//! @file linear.cpp
//! @author ryftchen
//! @brief The definitions (linear) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "linear.hpp"
#include <iostream>
#include <sstream>
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

//! @brief Display linear result.
#define LINEAR_RESULT "\r\n*%-10s instance:\r\n%s"
//! @brief Print linear result content.
#define LINEAR_PRINT_RESULT_CONTENT(method) COMMON_PRINT(LINEAR_RESULT, method, output.str().c_str())
#else

//! @brief Print linear result content.
#define LINEAR_PRINT_RESULT_CONTENT(method)
#endif

namespace date_structure::linear
{
namespace doubly_linked_list
{
Node* createNode(void* const pVal)
{
    Node* pNode = nullptr;
    pNode = new Node();
    if (!pNode)
    {
        std::cerr << "Create node failed." << std::endl;
        return nullptr;
    }

    pNode->prev = pNode->next = pNode;
    pNode->p = pVal;

    return pNode;
}

Node* getNode(DLL pHead, const int index)
{
    const int count = dllSize(pHead);
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

int createDll(DLL* dll)
{
    *dll = createNode(nullptr);
    if (!*dll)
    {
        return -1;
    }

    return 0;
}

int destroyDll(DLL* dll)
{
    if (!*dll)
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

bool dllIsEmpty(DLL pHead)
{
    return (0 == dllSize(pHead));
}

void* dllGet(DLL pHead, const int index)
{
    Node* pIndex = getNode(pHead, index);
    if (!pIndex)
    {
        std::cerr << "Get node failed." << std::endl;
        return nullptr;
    }

    return pIndex->p;
}

void* dllGetFirst(DLL pHead)
{
    return dllGet(pHead, 0);
}

void* dllGetLast(DLL pHead)
{
    return dllGet(pHead, dllSize(pHead) - 1);
}

int dllInsert(DLL pHead, const int index, void* const pVal)
{
    if (0 == index)
    {
        return dllInsertFirst(pHead, pVal);
    }

    Node* pIndex = getNode(pHead, index);
    if (!pIndex)
    {
        return -1;
    }

    Node* pNode = createNode(pVal);
    if (!pNode)
    {
        return -1;
    }

    pNode->prev = pIndex->prev;
    pNode->next = pIndex;
    pIndex->prev->next = pNode;
    pIndex->prev = pNode;

    return 0;
}

int dllInsertFirst(DLL pHead, void* const pVal)
{
    Node* pNode = createNode(pVal);
    if (!pNode)
    {
        return -1;
    }

    pNode->prev = pHead;
    pNode->next = pHead->next;
    pHead->next->prev = pNode;
    pHead->next = pNode;

    return 0;
}

int dllInsertLast(DLL pHead, void* const pVal)
{
    Node* pNode = createNode(pVal);
    if (!pNode)
    {
        return -1;
    }

    pNode->next = pHead;
    pNode->prev = pHead->prev;
    pHead->prev->next = pNode;
    pHead->prev = pNode;

    return 0;
}

int dllDelete(DLL pHead, const int index)
{
    Node* pIndex = getNode(pHead, index);
    if (!pIndex)
    {
        std::cerr << "Delete node failed, the index in out of bound." << std::endl;
        return -1;
    }

    pIndex->next->prev = pIndex->prev;
    pIndex->prev->next = pIndex->next;
    delete pIndex;

    return 0;
}

int dllDeleteFirst(DLL pHead)
{
    return dllDelete(pHead, 0);
}

int dllDeleteLast(DLL pHead)
{
    return dllDelete(pHead, dllSize(pHead) - 1);
}
} // namespace doubly_linked_list

namespace stack
{
int createStack(Stack* stack)
{
    return createDll(stack);
}

int destroyStack(Stack* stack)
{
    return destroyDll(stack);
}

int stackPush(Stack pHead, void* const pVal)
{
    return dllInsertFirst(pHead, pVal);
}

void* stackTop(Stack pHead)
{
    return dllGetFirst(pHead);
}

void* stackPop(Stack pHead)
{
    void* p = stackTop(pHead);
    dllDeleteFirst(pHead);
    return p;
}

int stackSize(Stack pHead)
{
    return dllSize(pHead);
}

bool stackIsEmpty(Stack pHead)
{
    return dllIsEmpty(pHead);
}
} // namespace stack

namespace queue
{
int createQueue(Queue* queue)
{
    return createDll(queue);
}

int destroyQueue(Queue* queue)
{
    return destroyDll(queue);
}

int queuePush(Queue pHead, void* const pVal)
{
    return dllInsertLast(pHead, pVal);
}

void* queueFront(Queue pHead)
{
    return dllGetFirst(pHead);
}

void* queuePop(Queue pHead)
{
    void* p = dllGetFirst(pHead);
    dllDeleteFirst(pHead);
    return p;
}

int queueSize(Queue pHead)
{
    return dllSize(pHead);
}

bool queueIsEmpty(Queue pHead)
{
    return dllIsEmpty(pHead);
}
} // namespace queue

//! @brief Metadata, which is used in the instance.
typedef struct TagMeta
{
    //! @brief The ID of metadata.
    int id;
    //! @brief The name of metadata.
    char name[4];
} Meta;

LinearStructure::LinearStructure()
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nInstances of the linear structure:" << std::endl;
#endif
}

void LinearStructure::linkedListInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using doubly_linked_list::createDll;
    using doubly_linked_list::destroyDll;
    using doubly_linked_list::DLL;
    using doubly_linked_list::dllDelete;
    using doubly_linked_list::dllDeleteFirst;
    using doubly_linked_list::dllDeleteLast;
    using doubly_linked_list::dllGet;
    using doubly_linked_list::dllGetFirst;
    using doubly_linked_list::dllGetLast;
    using doubly_linked_list::dllInsert;
    using doubly_linked_list::dllInsertFirst;
    using doubly_linked_list::dllInsertLast;
    using doubly_linked_list::dllIsEmpty;
    using doubly_linked_list::dllSize;

    std::ostringstream output;
    Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
    const int metaSize = sizeof(meta) / sizeof(meta[0]);

    Meta* pVal = nullptr;
    DLL dll = nullptr;
    createDll(&dll);
    dllInsert(dll, 0, &meta[0]);
    output << "insert (0): {" << meta[0].id << ", " << meta[0].name << "}" << std::endl;
    dllInsert(dll, 0, &meta[1]);
    output << "insert (0): {" << meta[1].id << ", " << meta[1].name << "}" << std::endl;
    dllInsert(dll, 1, &meta[2]);
    output << "insert (1): {" << meta[2].id << ", " << meta[2].name << "}" << std::endl;
    dllDelete(dll, 2);
    output << "delete (2)" << std::endl;

    dllInsertFirst(dll, &meta[0]);
    output << "insert first: {" << meta[0].id << ", " << meta[0].name << "}" << std::endl;
    dllInsertLast(dll, &meta[metaSize - 1]);
    output << "insert last: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}" << std::endl;
    pVal = static_cast<Meta*>(dllGetFirst(dll));
    output << "get first: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    pVal = static_cast<Meta*>(dllGetLast(dll));
    output << "get last: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    dllDeleteFirst(dll);
    output << "delete first" << std::endl;
    dllDeleteLast(dll);
    output << "delete last" << std::endl;

    output << "whether it is empty: " << dllIsEmpty(dll) << std::endl;
    output << "size: " << dllSize(dll) << std::endl;
    for (int i = 0; i < dllSize(dll); ++i)
    {
        pVal = static_cast<Meta*>(dllGet(dll, i));
        output << "get (" << i << "): {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    }
    destroyDll(&dll);

    LINEAR_PRINT_RESULT_CONTENT("LinkedList");
}

void LinearStructure::stackInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using stack::createStack;
    using stack::destroyStack;
    using stack::Stack;
    using stack::stackIsEmpty;
    using stack::stackPop;
    using stack::stackPush;
    using stack::stackSize;
    using stack::stackTop;

    std::ostringstream output;
    Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
    const int metaSize = sizeof(meta) / sizeof(meta[0]);

    Meta* pVal = nullptr;
    Stack stack = nullptr;
    createStack(&stack);
    for (int i = 0; i < (metaSize - 1); ++i)
    {
        stackPush(stack, &meta[i]);
        output << "push: {" << meta[i].id << ", " << meta[i].name << "}" << std::endl;
    }

    pVal = static_cast<Meta*>(stackPop(stack));
    output << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    pVal = static_cast<Meta*>(stackTop(stack));
    output << "top: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    stackPush(stack, &meta[metaSize - 1]);
    output << "push: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}" << std::endl;

    output << "whether it is empty: " << stackIsEmpty(stack) << std::endl;
    output << "size: " << stackSize(stack) << std::endl;
    while (!stackIsEmpty(stack))
    {
        pVal = static_cast<Meta*>(stackPop(stack));
        output << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    }
    destroyStack(&stack);

    LINEAR_PRINT_RESULT_CONTENT("Stack");
}

void LinearStructure::queueInstance() const // NOLINT(readability-convert-member-functions-to-static)
{
    using queue::createQueue;
    using queue::destroyQueue;
    using queue::Queue;
    using queue::queueFront;
    using queue::queueIsEmpty;
    using queue::queuePop;
    using queue::queuePush;
    using queue::queueSize;

    std::ostringstream output;
    Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
    const int metaSize = sizeof(meta) / sizeof(meta[0]);

    Meta* pVal = nullptr;
    Queue queue = nullptr;
    createQueue(&queue);
    for (int i = 0; i < (metaSize - 1); ++i)
    {
        queuePush(queue, &meta[i]);
        output << "push: {" << meta[i].id << ", " << meta[i].name << "}" << std::endl;
    }

    pVal = static_cast<Meta*>(queuePop(queue));
    output << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    pVal = static_cast<Meta*>(queueFront(queue));
    output << "front: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    queuePush(queue, &meta[metaSize - 1]);
    output << "push: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}" << std::endl;

    output << "whether it is empty: " << queueIsEmpty(queue) << std::endl;
    output << "size: " << queueSize(queue) << std::endl;
    while (!queueIsEmpty(queue))
    {
        pVal = static_cast<Meta*>(queuePop(queue));
        output << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
    }
    destroyQueue(&queue);

    LINEAR_PRINT_RESULT_CONTENT("Queue");
}
} // namespace date_structure::linear
