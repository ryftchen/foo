#include "linear.hpp"
#include <iostream>
#include <sstream>
#include "common.hpp"

#define LINEAR_RESULT "\r\n*%-10s instance:\r\n%s"

namespace ds_linear
{
LinearStructure::LinearStructure()
{
    std::cout << "\r\nLinear structure:" << std::endl;
}

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

Node* getFirstNode(DLL pHead)
{
    return getNode(pHead, 0);
}

Node* getLastNode(DLL pHead)
{
    return getNode(pHead, dllSize(pHead) - 1);
}

int createDll(DLL* pHead)
{
    *pHead = createNode(nullptr);
    if (!*pHead)
    {
        return -1;
    }

    return 0;
}

int destroyDll(DLL* pHead)
{
    if (!*pHead)
    {
        std::cerr << "Destroy doubly linked list failed, doubly linked list is null." << std::endl;
        return -1;
    }

    Node* pNode = (*pHead)->next;
    Node* pTemp = nullptr;
    while (pNode != *pHead)
    {
        pTemp = pNode;
        pNode = pNode->next;
        delete pTemp;
    }

    delete *pHead;
    *pHead = nullptr;

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

int dllIsEmpty(DLL pHead)
{
    return (0 == dllSize(pHead));
}

void* dllGet(DLL pHead, const int index)
{
    Node* pIndex = getNode(pHead, index);
    if (!pIndex)
    {
        std::cerr << "Doubly linked list get failed." << std::endl;
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
        std::cerr << "Doubly linked list delete failed, the index in out of bound." << std::endl;
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

// Linked List
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void LinearStructure::linkedListInstance() const
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
    typedef struct TagFoo
    {
        int id;
        char name[4];
    } Meta;
    Meta meta[] = {{-1, "foo"}, {0, "bar"}, {1, "baz"}, {2, "qux"}};
    const int metaSize = sizeof(meta) / sizeof(meta[0]);

    Meta* pVal = nullptr;
    DLL dll = nullptr;
    createDll(&dll);
    dllInsert(dll, 0, &meta[0]);
    output << "doubly linked list insert(0): [" << meta[0].id << ", " << meta[0].name << "]" << std::endl;
    dllInsert(dll, 0, &meta[1]);
    output << "doubly linked list insert(0): [" << meta[1].id << ", " << meta[1].name << "]" << std::endl;
    dllInsert(dll, 1, &meta[2]);
    output << "doubly linked list insert(1): [" << meta[2].id << ", " << meta[2].name << "]" << std::endl;
    dllDelete(dll, 2);
    output << "doubly linked list delete(2)" << std::endl;

    dllInsertFirst(dll, &meta[0]);
    output << "doubly linked list insert first: [" << meta[0].id << ", " << meta[0].name << "]" << std::endl;
    dllInsertLast(dll, &meta[metaSize - 1]);
    output << "doubly linked list insert last: [" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "]"
           << std::endl;
    pVal = static_cast<Meta*>(dllGetFirst(dll));
    output << "doubly linked list get first: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    pVal = static_cast<Meta*>(dllGetLast(dll));
    output << "doubly linked list get last: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    dllDeleteFirst(dll);
    output << "doubly linked list delete first" << std::endl;
    dllDeleteLast(dll);
    output << "doubly linked list delete last" << std::endl;

    output << "doubly linked list is empty: " << dllIsEmpty(dll) << std::endl;
    output << "doubly linked list size: " << dllSize(dll) << std::endl;
    for (int i = 0; i < dllSize(dll); ++i)
    {
        pVal = static_cast<Meta*>(dllGet(dll, i));
        output << "doubly linked list get(" << i << "): [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    }
    destroyDll(&dll);

    COMMON_PRINT(LINEAR_RESULT, "LinkedList", output.str().c_str());
}

// Stack
namespace stack
{
using doubly_linked_list::createDll;
using doubly_linked_list::destroyDll;
using doubly_linked_list::dllDeleteFirst;
using doubly_linked_list::dllGetFirst;
using doubly_linked_list::dllInsertFirst;
using doubly_linked_list::dllIsEmpty;
using doubly_linked_list::dllSize;
using Stack = doubly_linked_list::DLL;

static int createStack(Stack* stack)
{
    return createDll(stack);
}

static int destroyStack(Stack* stack)
{
    return destroyDll(stack);
}

static int stackPush(Stack stack, void* p)
{
    return dllInsertFirst(stack, p);
}

static void* stackTop(Stack stack)
{
    return dllGetFirst(stack);
}

static void* stackPop(Stack stack)
{
    void* p = stackTop(stack);
    dllDeleteFirst(stack);
    return p;
}

static int stackSize(Stack stack)
{
    return dllSize(stack);
}

static int stackIsEmpty(Stack stack)
{
    return dllIsEmpty(stack);
}
} // namespace stack

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void LinearStructure::stackInstance() const
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
    typedef struct TagFoo
    {
        int id;
        char name[4];
    } Meta;
    Meta meta[] = {{-1, "foo"}, {0, "bar"}, {1, "baz"}, {2, "qux"}};
    const int metaSize = sizeof(meta) / sizeof(meta[0]);

    Meta* pVal = nullptr;
    Stack stack = nullptr;
    createStack(&stack);
    for (int i = 0; i < (metaSize - 1); ++i)
    {
        stackPush(stack, &meta[i]);
        output << "stack push: [" << meta[i].id << ", " << meta[i].name << "]" << std::endl;
    }

    pVal = static_cast<Meta*>(stackPop(stack));
    output << "stack pop: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    pVal = static_cast<Meta*>(stackTop(stack));
    output << "stack top: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    stackPush(stack, &meta[metaSize - 1]);
    output << "stack push: [" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "]" << std::endl;

    output << "stack is empty: " << stackIsEmpty(stack) << std::endl;
    output << "stack size: " << stackSize(stack) << std::endl;
    while (!stackIsEmpty(stack))
    {
        pVal = static_cast<Meta*>(stackPop(stack));
        output << "stack pop: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    }
    destroyStack(&stack);

    COMMON_PRINT(LINEAR_RESULT, "Stack", output.str().c_str());
}

// Queue
namespace queue
{
using doubly_linked_list::createDll;
using doubly_linked_list::destroyDll;
using doubly_linked_list::dllDeleteFirst;
using doubly_linked_list::dllGetFirst;
using doubly_linked_list::dllInsertLast;
using doubly_linked_list::dllIsEmpty;
using doubly_linked_list::dllSize;
using Queue = doubly_linked_list::DLL;

static int createQueue(Queue* queue)
{
    return createDll(queue);
}

static int destroyQueue(Queue* queue)
{
    return destroyDll(queue);
}

static int queuePush(Queue queue, void* p)
{
    return dllInsertLast(queue, p);
}

static void* queueFront(Queue queue)
{
    return dllGetFirst(queue);
}

static void* queuePop(Queue queue)
{
    void* p = dllGetFirst(queue);
    dllDeleteFirst(queue);
    return p;
}

static int queueSize(Queue queue)
{
    return dllSize(queue);
}

static int queueIsEmpty(Queue queue)
{
    return dllIsEmpty(queue);
}
} // namespace queue

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void LinearStructure::queueInstance() const
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
    typedef struct TagFoo
    {
        int id;
        char name[4];
    } Meta;
    Meta meta[] = {{-1, "foo"}, {0, "bar"}, {1, "baz"}, {2, "qux"}};
    const int metaSize = sizeof(meta) / sizeof(meta[0]);

    Meta* pVal = nullptr;
    Queue queue = nullptr;
    createQueue(&queue);
    for (int i = 0; i < (metaSize - 1); ++i)
    {
        queuePush(queue, &meta[i]);
        output << "queue push: [" << meta[i].id << ", " << meta[i].name << "]" << std::endl;
    }

    pVal = static_cast<Meta*>(queuePop(queue));
    output << "queue pop: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    pVal = static_cast<Meta*>(queueFront(queue));
    output << "queue front: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    queuePush(queue, &meta[metaSize - 1]);
    output << "queue push: [" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "]" << std::endl;

    output << "queue is empty: " << queueIsEmpty(queue) << std::endl;
    output << "queue size: " << queueSize(queue) << std::endl;
    while (!queueIsEmpty(queue))
    {
        pVal = static_cast<Meta*>(queuePop(queue));
        output << "queue pop: [" << pVal->id << ", " << pVal->name << "]" << std::endl;
    }
    destroyQueue(&queue);

    COMMON_PRINT(LINEAR_RESULT, "Queue", output.str().c_str());
}
} // namespace ds_linear
