//! @file heap.hpp
//! @author ryftchen
//! @brief The declarations (heap) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <ostream>

//! @brief The data structure module.
namespace data_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Heap-related functions in the data structure module.
namespace heap
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "DS_HEAP";
}
extern const char* version() noexcept;

//! @brief The binary heap (min heap).
namespace binary
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The binary heap structure.
    struct BinaryHeap
    {
        //! @brief Pointer to the data array.
        void** data;
        //! @brief The capacity of the heap.
        int capacity;
        //! @brief The current size of the heap.
        int size;
        //! @brief The key's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern BinaryHeap* create(const int cap, const Compare cmp);
extern void destroy(BinaryHeap* heap);
extern bool insert(BinaryHeap* const heap, const void* const key);
extern bool remove(BinaryHeap* const heap, const void* const key);

//! @brief Do traversing.
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param heap - heap structure to be traversed
    explicit Traverse(const BinaryHeap* const heap) : heap{heap} {}

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a order traversal starting from head.
    //! @param op - operation on each node
    void order(const Operation& op) const;

private:
    //! @brief The heap structure to be traversed.
    const BinaryHeap* const heap{nullptr};
};
} // namespace binary

//! @brief The leftist heap.
namespace leftist
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*CompareFunc)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The node of the leftist heap.
    typedef struct LeftistNode
    {
        //! @brief Pointer to the left child node.
        struct LeftistNode* left;
        //! @brief Pointer to the right child node.
        struct LeftistNode* right;
        //! @brief Key value.
        void* key;
        //! @brief The null path length.
        int npl;
    } Node;

    //! @brief The leftist heap structure.
    struct LeftistHeap
    {
        //! @brief Pointer to the root node.
        LeftistNode* root;
        //! @brief The key's compare function.
        CompareFunc compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

void* getMinimum(const LeftistHeap* const heap);

LeftistHeap* create(const CompareFunc cmp);
void destroy(const LeftistHeap* heap);
void insert(LeftistHeap* const heap, const void* const key);
void remove(LeftistHeap* const heap);
void merge(LeftistHeap* const dst, LeftistHeap* const src);
} // namespace leftist

//! @brief The skew heap.
namespace skew
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*CompareFunc)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The node of the skew heap.
    typedef struct SkewNode
    {
        //! @brief Pointer to the left child node.
        struct SkewNode* left;
        //! @brief Pointer to the right child node.
        struct SkewNode* right;
        //! @brief Key value.
        void* key;
    } Node;

    //! @brief The leftist heap structure.
    struct SkewHeap
    {
        //! @brief Pointer to the root node.
        SkewNode* root;
        //! @brief The key's compare function.
        CompareFunc compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

void* getMinimum(const SkewHeap* const heap);

SkewHeap* create(const CompareFunc cmp);
void destroy(const SkewHeap* heap);
void insert(SkewHeap* const heap, const void* const key);
void remove(SkewHeap* const heap);
void merge(SkewHeap* const dst, SkewHeap* const src);
} // namespace skew

//! @brief Do traversing.
//! @tparam Heap - type of heap node
//! @tparam Node - type of heap node
template <typename Heap, typename Node>
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param heap - heap structure to be traversed
    explicit Traverse(const Heap* const heap) : heap{heap} {}

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a pre-order traversal of the heap.
    //! @param op - operation on each node
    void preOrder(const Operation& op) const;
    //! @brief Perform a in-order traversal of the heap.
    //! @param op - operation on each node
    void inOrder(const Operation& op) const;
    //! @brief Perform a post-order traversal of the heap.
    //! @param op - operation on each node
    void postOrder(const Operation& op) const;

private:
    //! @brief The heap structure to be traversed.
    const Heap* const heap{nullptr};
    //! @brief Perform a pre-order traversal of the subheap.
    //! @param node - root of the subheap
    //! @param op - operation on each node
    static void preOrderTraversal(const Node* const node, const Operation& op);
    //! @brief Perform a in-order traversal of the subheap.
    //! @param node - root of the subheap
    //! @param op - operation on each node
    static void inOrderTraversal(const Node* const node, const Operation& op);
    //! @brief Perform a post-order traversal of the subheap.
    //! @param node - root of the subheap
    //! @param op - operation on each node
    static void postOrderTraversal(const Node* const node, const Operation& op);
};

template <typename Heap, typename Node>
void Traverse<Heap, Node>::preOrder(const Operation& op) const
{
    if (!heap || !op)
    {
        return;
    }

    preOrderTraversal(heap->root, op);
}

template <typename Heap, typename Node>
void Traverse<Heap, Node>::inOrder(const Operation& op) const
{
    if (!heap || !op)
    {
        return;
    }

    inOrderTraversal(heap->root, op);
}

template <typename Heap, typename Node>
void Traverse<Heap, Node>::postOrder(const Operation& op) const
{
    if (!heap || !op)
    {
        return;
    }

    postOrderTraversal(heap->root, op);
}

template <typename Heap, typename Node>
void Traverse<Heap, Node>::preOrderTraversal(const Node* const node, const Operation& op)
{
    if (!node)
    {
        return;
    }

    op(node->key);
    preOrderTraversal(node->left, op);
    preOrderTraversal(node->right, op);
}

template <typename Heap, typename Node>
void Traverse<Heap, Node>::inOrderTraversal(const Node* const node, const Operation& op)
{
    if (!node)
    {
        return;
    }

    inOrderTraversal(node->left, op);
    op(node->key);
    inOrderTraversal(node->right, op);
}

template <typename Heap, typename Node>
void Traverse<Heap, Node>::postOrderTraversal(const Node* const node, const Operation& op)
{
    if (!node)
    {
        return;
    }

    postOrderTraversal(node->left, op);
    postOrderTraversal(node->right, op);
    op(node->key);
}

//! @brief Print the heap structure.
//! @tparam Node - type of heap node
//! @tparam Key - type of node key
template <typename Node, typename Key>
class Printer
{
public:
    //! @brief Construct a new Printer object.
    //! @param root - root of the heap
    explicit Printer(const Node* const root) : root{root} {}

    //! @brief Print the node in the heap.
    //! @param os - output stream object
    //! @param node - root of the subheap
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void printNode(std::ostream& os, const Node* const node, const void* const key, const int direction) const;

private:
    //! @brief The root of the heap.
    const Node* const root{nullptr};
    //! @brief Indentation size.
    mutable int indent{0};

protected:
    template <typename HN, typename NK>
    friend std::ostream& operator<<(std::ostream&, const Printer<HN, NK>&);
};

template <typename Node, typename Key>
void Printer<Node, Key>::printNode(
    std::ostream& os, const Node* const node, const void* const key, const int direction) const
{
    if (!node || !key)
    {
        return;
    }

    const int currInd = indent;
    const Key nodeKey = *static_cast<const Key*>(node->key);
    std::string ext{};
    if constexpr (std::is_same_v<Node, leftist::Node>)
    {
        ext = '(' + std::to_string(node->npl) + ')';
    }
    if (direction == 0)
    {
        indent = 0;
        os << "+ " << nodeKey << ext << " -> root\n";
    }
    else
    {
        os << "+ " << std::string(currInd, ' ') << nodeKey << ext << " -> " << *static_cast<const Key*>(key) << "'s "
           << ((direction == 1) ? "right" : "left") << " child\n";
    }

    indent += 2;
    printNode(os, node->left, node->key, -1);
    printNode(os, node->right, node->key, 1);
    indent = currInd;
}

//! @brief The operator (<<) overloading of the Printer class.
//! @tparam HN - type of heap node
//! @tparam NK - type of node key
//! @param os - output stream object
//! @param printer - specific Printer object
//! @return reference of the output stream object
template <typename HN, typename NK>
std::ostream& operator<<(std::ostream& os, const Printer<HN, NK>& printer)
{
    if (printer.root)
    {
        printer.printNode(os, printer.root, printer.root->key, 0);
    }
    return os;
}
} // namespace heap
} // namespace data_structure
