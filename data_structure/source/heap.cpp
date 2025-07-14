//! @file heap.cpp
//! @author ryftchen
//! @brief The definitions (heap) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "heap.hpp"

#include <cstring>

namespace date_structure::heap
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
namespace binary
{
//! @brief Get the index of the key in the binary heap.
//! @param heap - heap to search in
//! @param key - key of the data
//! @return index of the key if found, otherwise -1
static int getIndex(const BinaryHeap* const heap, const void* const key)
{
    if (!heap || !key)
    {
        return -1;
    }

    for (int i = 0; i < heap->size; ++i)
    {
        if (heap->compare(heap->data[i], key) == 0)
        {
            return i;
        }
    }

    return -1;
}

//! @brief Filter up the element at the specified index in the binary heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterUp(const BinaryHeap* const heap, const int start)
{
    if (!heap || (start < 0))
    {
        return;
    }

    int child = start, parent = (child - 1) / 2;
    void* const temp = heap->data[child];
    while (child > 0)
    {
        if (heap->compare(temp, heap->data[parent]) >= 0)
        {
            break;
        }

        heap->data[child] = heap->data[parent];
        child = parent;
        parent = (parent - 1) / 2;
    }
    heap->data[child] = temp;
}

//! @brief Filter down the element at the specified index in the binary heap.
//! @param heap - heap to filter in
//! @param start - index to start filtering from
static void filterDown(const BinaryHeap* const heap, const int start)
{
    if (!heap || (start < 0))
    {
        return;
    }

    const int end = heap->size - 1;
    int child = start, selected = (2 * child) + 1;
    void* const temp = heap->data[child];
    while (selected <= end)
    {
        if ((selected < end) && (heap->compare(heap->data[selected], heap->data[selected + 1]) > 0))
        {
            ++selected;
        }
        if (heap->compare(temp, heap->data[selected]) <= 0)
        {
            break;
        }

        heap->data[child] = heap->data[selected];
        child = selected;
        selected = 2 * selected + 1;
    }
    heap->data[child] = temp;
}

//! @brief Create the binary heap.
//! @param cap - capacity of the heap
//! @param cmp - compare function to compare keys
//! @return new binary heap
BinaryHeap* create(const int cap, const Compare cmp)
{
    if ((cap <= 0) || !cmp)
    {
        return nullptr;
    }

    auto* const heap = ::new (std::nothrow) BinaryHeap;
    heap->data = ::new (std::nothrow) void*[cap];
    std::memset(static_cast<void*>(heap->data), 0, sizeof(void*) * cap);
    heap->capacity = cap;
    heap->size = 0;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the binary heap.
//! @param heap - heap to destroy
void destroy(const BinaryHeap* heap)
{
    if (!heap)
    {
        return;
    }

    ::delete[] heap->data;
    ::delete heap;
    heap = nullptr;
}

//! @brief Insert the data into the binary heap.
//! @param heap - heap to insert into
//! @param key - key of the data
//! @return success or failure
bool insert(BinaryHeap* const heap, const void* const key)
{
    if (!heap || !key || (heap->size >= heap->capacity))
    {
        return false;
    }

    heap->data[heap->size] = const_cast<void*>(key);
    filterUp(heap, heap->size);
    ++heap->size;

    return true;
}

//! @brief Remove the data from the binary heap.
//! @param heap - heap to remove from
//! @param key - key of the data
//! @return success or failure
bool remove(BinaryHeap* const heap, const void* const key)
{
    if (!heap || !key || (heap->size == 0))
    {
        return false;
    }

    const int index = getIndex(heap, key);
    if (index == -1)
    {
        return false;
    }

    heap->data[index] = heap->data[--heap->size];
    filterDown(heap, index);

    return true;
}

void Traverse::order(const Operation& op) const
{
    if (!heap)
    {
        return;
    }

    for (int i = 0; i < heap->size; ++i)
    {
        op(heap->data[i]);
    }
}
} // namespace binary

namespace leftist
{
//! @brief Create the node of the leftist subheap.
//! @param key - key of the node to be created
//! @return new node after creating
static Node* createNode(const void* const key)
{
    if (!key)
    {
        return nullptr;
    }

    Node* const newNode = ::new (std::nothrow) Node;
    if (!newNode)
    {
        return nullptr;
    }

    newNode->left = newNode->right = nullptr;
    newNode->key = const_cast<void*>(key);
    newNode->npl = 0;

    return newNode;
}

//! @brief Remove the node from the leftist subheap.
//! @param node - target node
static void removeNode(const Node* node)
{
    if (!node)
    {
        return;
    }

    removeNode(node->left);
    removeNode(node->right);
    ::delete node;
    node = nullptr;
}

//! @brief Merge two nodes in the leftist subheap.
//! @param d - destination node
//! @param s - source node
//! @param cmp - compare function to compare keys
//! @return destination node after merging
static Node* mergeNode(Node* d, Node* s, const CompareFunc cmp)
{
    if (!cmp)
    {
        return nullptr;
    }
    if (!d)
    {
        return s;
    }
    if (!s)
    {
        return d;
    }

    if (cmp(d->key, s->key) > 0)
    {
        Node* const temp = d;
        d = s;
        s = temp;
    }

    d->right = mergeNode(d->right, s, cmp);
    if (!d->left || (d->left->npl < d->right->npl))
    {
        Node* const temp = d->left;
        d->left = d->right;
        d->right = temp;
    }

    if (!d->right || !d->left)
    {
        d->npl = 0;
    }
    else
    {
        d->npl = (d->right->npl < d->left->npl) ? (d->right->npl + 1) : (d->left->npl + 1);
    }

    return d;
}

//! @brief Get the node where the minimum key is located in the leftist heap.
//! @param heap - leftist heap
//! @return node where the minimum key is located
void* getMinimum(const LeftistHeap* const heap)
{
    return (heap && heap->root) ? heap->root->key : nullptr;
}

//! @brief Create the leftist heap.
//! @param cmp - compare function to compare keys
//! @return new leftist heap
LeftistHeap* create(const CompareFunc cmp)
{
    if (!cmp)
    {
        return nullptr;
    }

    auto* const heap = ::new (std::nothrow) LeftistHeap;
    heap->root = nullptr;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the leftist heap.
//! @param heap - leftist heap
void destroy(const LeftistHeap* heap)
{
    if (!heap)
    {
        return;
    }

    removeNode(heap->root);
    ::delete heap;
    heap = nullptr;
}

//! @brief Insert the node into the leftist heap.
//! @param heap - leftist heap
//! @param key - key of the target node
void insert(LeftistHeap* const heap, const void* const key)
{
    if (!heap)
    {
        return;
    }

    Node* const node = createNode(key);
    heap->root = mergeNode(heap->root, node, heap->compare);
}

//! @brief Remove the node where the minimum key is located from the leftist heap.
//! @param heap - leftist heap
void remove(LeftistHeap* const heap)
{
    if (!heap || !heap->root)
    {
        return;
    }

    Node *const left = heap->root->left, *const right = heap->root->right;
    ::delete heap->root;

    heap->root = mergeNode(left, right, heap->compare);
}

//! @brief Merge the source leftist heap into the destination heap.
//! @param dst - destination leftist heap
//! @param src - source leftist heap
void merge(LeftistHeap* const dst, LeftistHeap* const src)
{
    if (!dst || !src)
    {
        return;
    }

    dst->root = mergeNode(dst->root, src->root, dst->compare);
    src->root = nullptr;
}
} // namespace leftist

namespace skew
{
//! @brief Create the node of the skew subheap.
//! @param key - key of the node to be created
//! @return new node after creating
static Node* createNode(const void* const key)
{
    if (!key)
    {
        return nullptr;
    }

    Node* const newNode = ::new (std::nothrow) Node;
    if (!newNode)
    {
        return nullptr;
    }

    newNode->left = newNode->right = nullptr;
    newNode->key = const_cast<void*>(key);

    return newNode;
}

//! @brief Remove the node from the skew subheap.
//! @param node - target node
static void removeNode(const Node* node)
{
    if (!node)
    {
        return;
    }

    removeNode(node->left);
    removeNode(node->right);
    ::delete node;
    node = nullptr;
}

//! @brief Merge two nodes in the skew subheap.
//! @param d - destination node
//! @param s - source node
//! @param cmp - compare function to compare keys
//! @return destination node after merging
static Node* mergeNode(Node* d, Node* s, const CompareFunc cmp)
{
    if (!cmp)
    {
        return nullptr;
    }
    if (!d)
    {
        return s;
    }
    if (!s)
    {
        return d;
    }

    if (cmp(d->key, s->key) > 0)
    {
        Node* const temp = d;
        d = s;
        s = temp;
    }

    Node* const merged = mergeNode(d->right, s, cmp);
    d->right = d->left;
    d->left = merged;

    return d;
}

//! @brief Create the skew heap.
//! @param cmp - compare function to compare keys
//! @return new skew heap
SkewHeap* create(const CompareFunc cmp)
{
    if (!cmp)
    {
        return nullptr;
    }

    auto* const heap = ::new (std::nothrow) SkewHeap;
    heap->root = nullptr;
    heap->compare = cmp;

    return heap;
}

//! @brief Destroy the skew heap.
//! @param heap - skew heap
void destroy(const SkewHeap* heap)
{
    if (!heap)
    {
        return;
    }

    removeNode(heap->root);
    ::delete heap;
    heap = nullptr;
}

//! @brief Insert the node into the skew heap.
//! @param heap - skew heap
//! @param key - key of the target node
void insert(SkewHeap* const heap, const void* const key)
{
    if (!heap)
    {
        return;
    }

    Node* const node = createNode(key);
    heap->root = mergeNode(heap->root, node, heap->compare);
}

//! @brief Remove the node with the minimum key from the skew heap.
//! @param heap - target heap
void remove(SkewHeap* const heap)
{
    if (!heap || !heap->root)
    {
        return;
    }

    Node *const left = heap->root->left, *const right = heap->root->right;
    ::delete heap->root;

    heap->root = mergeNode(left, right, heap->compare);
}

//! @brief Get the minimum key from the skew heap.
//! @param heap - target heap
//! @return pointer to the minimum key, or nullptr if empty
void* getMinimum(const SkewHeap* const heap)
{
    return (heap && heap->root) ? heap->root->key : nullptr;
}

//! @brief Merge the source skew heap into the destination heap.
//! @param dst - destination skew heap
//! @param src - source skew heap
void merge(SkewHeap* const dst, SkewHeap* const src)
{
    if (!dst || !src)
    {
        return;
    }

    dst->root = mergeNode(dst->root, src->root, dst->compare);
    src->root = nullptr;
}
} // namespace skew
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::heap
