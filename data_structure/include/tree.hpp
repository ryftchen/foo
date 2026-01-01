//! @file tree.hpp
//! @author ryftchen
//! @brief The declarations (tree) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <ostream>

//! @brief The data structure module.
namespace data_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Tree-related functions in the data structure module.
namespace tree
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "DS_TREE";
}
extern const char* version() noexcept;

//! @brief The binary search tree.
namespace bs
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The node of the BS tree.
    typedef struct BSTreeNode
    {
        //! @brief Pointer to the left child node.
        struct BSTreeNode* left;
        //! @brief Pointer to the right child node.
        struct BSTreeNode* right;
        //! @brief Pointer to the parent node.
        struct BSTreeNode* parent;
        //! @brief Key value.
        void* key;
    } Node;

    //! @brief The BS tree structure.
    struct BSTree
    {
        //! @brief Pointer to the root node.
        struct BSTreeNode* root;
        //! @brief The key's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern Node* getMinimum(const BSTree* const tree);
extern Node* getMaximum(const BSTree* const tree);
extern Node* getPredecessor(const Node* x);
extern Node* getSuccessor(const Node* x);

extern BSTree* create(const Compare cmp);
extern void destroy(const BSTree* tree);
extern Node* search(const BSTree* const tree, const void* const key);
extern void insert(BSTree* const tree, const void* const key);
extern void remove(BSTree* const tree, const void* const key);
} // namespace bs

//! @brief The Adelson-Velsky-Landis tree.
namespace avl
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The node of the AVL tree.
    typedef struct AVLTreeNode
    {
        //! @brief Pointer to the left child node.
        struct AVLTreeNode* left;
        //! @brief Pointer to the right child node.
        struct AVLTreeNode* right;
        //! @brief Key value.
        void* key;
        //! @brief The height of an empty tree is 0, and the height of a non-empty tree is equal to its maximum level.
        int height;
    } Node;

    //! @brief The AVL tree structure.
    struct AVLTree
    {
        //! @brief Pointer to the root node.
        struct AVLTreeNode* root;
        //! @brief The key's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern int getHeight(const AVLTree* const tree);
extern Node* getMinimum(const AVLTree* const tree);
extern Node* getMaximum(const AVLTree* const tree);

extern AVLTree* create(const Compare cmp);
extern void destroy(const AVLTree* tree);
extern Node* search(const AVLTree* const tree, const void* const key);
extern void insert(AVLTree* const tree, const void* const key);
extern void remove(AVLTree* const tree, const void* const key);
} // namespace avl

//! @brief The splay tree.
namespace splay
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);

#pragma pack(push, 8)
    //! @brief The node of the splay tree.
    typedef struct SplayTreeNode
    {
        //! @brief Pointer to the left child node.
        struct SplayTreeNode* left;
        //! @brief Pointer to the right child node.
        struct SplayTreeNode* right;
        //! @brief Key value.
        void* key;
    } Node;

    //! @brief The splay tree structure.
    struct SplayTree
    {
        //! @brief Pointer to the root node.
        struct SplayTreeNode* root;
        //! @brief The key's compare function.
        Compare compare;
    };
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern Node* getMinimum(const SplayTree* const tree);
extern Node* getMaximum(const SplayTree* const tree);

extern SplayTree* create(const Compare cmp);
extern void destroy(const SplayTree* tree);
extern Node* search(const SplayTree* const tree, const void* const key);
extern void insert(SplayTree* const tree, const void* const key);
extern void remove(SplayTree* const tree, const void* const key);
extern void splay(SplayTree* const tree, const void* const key);
} // namespace splay

//! @brief Do traversing.
//! @tparam Tree - type of tree
//! @tparam Node - type of tree node
template <typename Tree, typename Node>
class Traverse
{
public:
    //! @brief Construct a new Traverse object.
    //! @param tree - tree structure to be traversed
    explicit Traverse(const Tree* const tree) : tree{tree} {}

    //! @brief Alias for the operation when traversing.
    using Operation = std::function<void(const void* const)>;
    //! @brief Perform a pre-order traversal of the tree.
    //! @param op - operation on each node
    void preOrder(const Operation& op) const;
    //! @brief Perform a in-order traversal of the tree.
    //! @param op - operation on each node
    void inOrder(const Operation& op) const;
    //! @brief Perform a post-order traversal of the tree.
    //! @param op - operation on each node
    void postOrder(const Operation& op) const;

private:
    //! @brief The tree structure to be traversed.
    const Tree* const tree{nullptr};
    //! @brief Perform a pre-order traversal of the subtree.
    //! @param node - root of the subtree
    //! @param op - operation on each node
    static void preOrderTraversal(const Node* const node, const Operation& op);
    //! @brief Perform a in-order traversal of the subtree.
    //! @param node - root of the subtree
    //! @param op - operation on each node
    static void inOrderTraversal(const Node* const node, const Operation& op);
    //! @brief Perform a post-order traversal of the subtree.
    //! @param node - root of the subtree
    //! @param op - operation on each node
    static void postOrderTraversal(const Node* const node, const Operation& op);
};

template <typename Tree, typename Node>
void Traverse<Tree, Node>::preOrder(const Operation& op) const
{
    if (!tree || !op)
    {
        return;
    }

    preOrderTraversal(tree->root, op);
}

template <typename Tree, typename Node>
void Traverse<Tree, Node>::inOrder(const Operation& op) const
{
    if (!tree || !op)
    {
        return;
    }

    inOrderTraversal(tree->root, op);
}

template <typename Tree, typename Node>
void Traverse<Tree, Node>::postOrder(const Operation& op) const
{
    if (!tree || !op)
    {
        return;
    }

    postOrderTraversal(tree->root, op);
}

template <typename Tree, typename Node>
void Traverse<Tree, Node>::preOrderTraversal(const Node* const node, const Operation& op)
{
    if (!node)
    {
        return;
    }

    op(node->key);
    preOrderTraversal(node->left, op);
    preOrderTraversal(node->right, op);
}

template <typename Tree, typename Node>
void Traverse<Tree, Node>::inOrderTraversal(const Node* const node, const Operation& op)
{
    if (!node)
    {
        return;
    }

    inOrderTraversal(node->left, op);
    op(node->key);
    inOrderTraversal(node->right, op);
}

template <typename Tree, typename Node>
void Traverse<Tree, Node>::postOrderTraversal(const Node* const node, const Operation& op)
{
    if (!node)
    {
        return;
    }

    postOrderTraversal(node->left, op);
    postOrderTraversal(node->right, op);
    op(node->key);
}

//! @brief Print the tree structure.
//! @tparam Node - type of tree node
//! @tparam Key - type of node key
template <typename Node, typename Key>
class Printer
{
public:
    //! @brief Construct a new Printer object.
    //! @param root - root of the tree
    explicit Printer(const Node* const root) : root{root} {}

    //! @brief Print the node in the tree.
    //! @param os - output stream object
    //! @param node - root of the subtree
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void printNode(std::ostream& os, const Node* const node, const void* const key, const int direction) const;

private:
    //! @brief The root of the tree.
    const Node* const root{nullptr};
    //! @brief Indentation size.
    mutable int indent{0};

protected:
    template <typename TN, typename NK>
    friend std::ostream& operator<<(std::ostream&, const Printer<TN, NK>&);
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
    if (direction == 0)
    {
        indent = 0;
        os << "+ " << *static_cast<const Key*>(node->key) << " -> root\n";
    }
    else
    {
        os << "+ " << std::string(currInd, ' ') << *static_cast<const Key*>(node->key) << " -> "
           << *static_cast<const Key*>(key) << "'s " << ((direction == 1) ? "right" : "left") << " child\n";
    }

    indent += 2;
    printNode(os, node->left, node->key, -1);
    printNode(os, node->right, node->key, 1);
    indent = currInd;
}

//! @brief The operator (<<) overloading of the Printer class.
//! @tparam TN - type of tree node
//! @tparam NK - type of node key
//! @param os - output stream object
//! @param printer - specific Printer object
//! @return reference of the output stream object
template <typename TN, typename NK>
std::ostream& operator<<(std::ostream& os, const Printer<TN, NK>& printer)
{
    if (printer.root)
    {
        printer.printNode(os, printer.root, printer.root->key, 0);
    }
    return os;
}
} // namespace tree
} // namespace data_structure
