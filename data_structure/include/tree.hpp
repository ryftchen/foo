//! @file tree.hpp
//! @author ryftchen
//! @brief The declarations (tree) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <sstream>

//! @brief The data structure module.
namespace date_structure // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Tree-related functions in the data structure module.
namespace tree
{
extern const char* version() noexcept;

//! @brief The binary search tree structure.
namespace bs
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#pragma pack(push, 8)
    //! @brief Alias for the compare function type.
    typedef int (*Compare)(const void* const, const void* const);
    //! @brief The node of the BS tree.
    typedef struct BSTreeNode
    {
        //! @brief Key value.
        void* key;
        //! @brief Pointer to the left child node.
        struct BSTreeNode* left;
        //! @brief Pointer to the right child node.
        struct BSTreeNode* right;
        //! @brief Pointer to the parent node.
        struct BSTreeNode* parent;
    } Node;

    //! @brief The BS tree structure.
    typedef struct BSTree
    {
        //! @brief Pointer to the root node.
        struct BSTreeNode* root;
        //! @brief The key's compare function.
        Compare compare;
    } Tree;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern Node* getMinimum(const Tree* const tree);
extern Node* getMaximum(const Tree* const tree);
extern Node* getPredecessor(const Node* x);
extern Node* getSuccessor(const Node* x);

extern Node* search(const Tree* const, const void* const key);
extern void insertion(Tree* const tree, const void* const key);
extern void deletion(Tree* const tree, const void* const key);
extern void destruction(const Tree* const tree);

//! @brief Output helper for the BS tree structure.
//! @tparam T - type of key
template <typename BSKey>
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;
    //! @brief The pre-order traversal of the BS subtree.
    //! @param node - root of the subtree
    void preOrderTraversal(const Node* const node);
    //! @brief The in-order traversal of the BS subtree.
    //! @param node - root of the subtree
    void inOrderTraversal(const Node* const node);
    //! @brief The post-order traversal of the BS subtree.
    //! @param node - root of the subtree
    void postOrderTraversal(const Node* const node);
    //! @brief Traverse the BS subtree.
    //! @param node - root of the subtree
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void traverse(const Node* const node, void* const key, const int direction);

private:
    //! @brief Output stream of the BS tree structure.
    std::ostringstream process;
    //! @brief Indentation size.
    int indent{0};
};

template <typename BSKey>
std::ostringstream& Output<BSKey>::output() noexcept
{
    return process;
}

template <typename BSKey>
void Output<BSKey>::preOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    output() << *static_cast<BSKey*>(node->key) << " ... ";
    preOrderTraversal(node->left);
    preOrderTraversal(node->right);
}

template <typename BSKey>
void Output<BSKey>::inOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    inOrderTraversal(node->left);
    output() << *static_cast<BSKey*>(node->key) << " ... ";
    inOrderTraversal(node->right);
}

template <typename BSKey>
void Output<BSKey>::postOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    postOrderTraversal(node->left);
    postOrderTraversal(node->right);
    output() << *static_cast<BSKey*>(node->key) << " ... ";
}

template <typename BSKey>
void Output<BSKey>::traverse(const Node* const node, void* const key, const int direction)
{
    if (!node || !key)
    {
        return;
    }

    const int currInd = indent;
    if (direction == 0)
    {
        indent = 0;
        output() << "+ " << *static_cast<BSKey*>(node->key) << " -> root\n";
    }
    else
    {
        output() << "+ " << std::string(currInd, ' ') << *static_cast<BSKey*>(node->key) << " -> "
                 << *static_cast<BSKey*>(key) << "'s " << ((direction == 1) ? "right" : "left") << " child\n";
    }

    indent += 2;
    traverse(node->left, node->key, -1);
    traverse(node->right, node->key, 1);
    indent = currInd;
}
} // namespace bs

//! @brief The Adelson-Velsky-Landis tree structure.
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
        //! @brief Key value.
        void* key;
        //! @brief The height of an empty tree is 0, and the height of a non-empty tree is equal to its maximum level.
        int height;
        //! @brief Pointer to the left child node.
        struct AVLTreeNode* left;
        //! @brief Pointer to the right child node.
        struct AVLTreeNode* right;
    } Node;

    //! @brief The AVL tree structure.
    typedef struct AVLTree
    {
        //! @brief Pointer to the root node.
        struct AVLTreeNode* root;
        //! @brief The key's compare function.
        Compare compare;
    } Tree;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern int getHeight(const Tree* const tree);
extern Node* getMinimum(const Tree* const tree);
extern Node* getMaximum(const Tree* const tree);

extern Node* search(const Tree* const tree, const void* const key);
extern void insertion(Tree* const tree, const void* const key);
extern void deletion(Tree* const tree, const void* const key);
extern void destruction(const Tree* const tree);

//! @brief Output helper for the AVL tree structure.
//! @tparam AVLKey - type of key
template <typename AVLKey>
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;
    //! @brief The pre-order traversal of the AVL subtree.
    //! @param node - root of the subtree
    void preOrderTraversal(const Node* const node);
    //! @brief The in-order traversal of the AVL subtree.
    //! @param node - root of the subtree
    void inOrderTraversal(const Node* const node);
    //! @brief The post-order traversal of the AVL subtree.
    //! @param node - root of the subtree
    void postOrderTraversal(const Node* const node);
    //! @brief Traverse the AVL subtree.
    //! @param node - root of the subtree
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void traverse(const Node* const node, void* const key, const int direction);

private:
    //! @brief Output stream of the AVL tree structure.
    std::ostringstream process;
    //! @brief Indentation size.
    int indent{0};
};

template <typename AVLKey>
std::ostringstream& Output<AVLKey>::output() noexcept
{
    return process;
}

template <typename AVLKey>
void Output<AVLKey>::preOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    output() << *static_cast<AVLKey*>(node->key) << " ... ";
    preOrderTraversal(node->left);
    preOrderTraversal(node->right);
}

template <typename AVLKey>
void Output<AVLKey>::inOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    inOrderTraversal(node->left);
    output() << *static_cast<AVLKey*>(node->key) << " ... ";
    inOrderTraversal(node->right);
}

template <typename AVLKey>
void Output<AVLKey>::postOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    postOrderTraversal(node->left);
    postOrderTraversal(node->right);
    output() << *static_cast<AVLKey*>(node->key) << " ... ";
}

template <typename AVLKey>
void Output<AVLKey>::traverse(const Node* const node, void* const key, const int direction)
{
    if (!node || !key)
    {
        return;
    }

    const int currInd = indent;
    if (direction == 0)
    {
        indent = 0;
        output() << "+ " << *static_cast<AVLKey*>(node->key) << " -> root\n";
    }
    else
    {
        output() << "+ " << std::string(currInd, ' ') << *static_cast<AVLKey*>(node->key) << " -> "
                 << *static_cast<AVLKey*>(key) << "'s " << ((direction == 1) ? "right" : "left") << " child\n";
    }

    indent += 2;
    traverse(node->left, node->key, -1);
    traverse(node->right, node->key, 1);
    indent = currInd;
}
} // namespace avl

//! @brief The splay tree structure.
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
        //! @brief Key value.
        void* key;
        //! @brief Pointer to the left child node.
        struct SplayTreeNode* left;
        //! @brief Pointer to the right child node.
        struct SplayTreeNode* right;
    } Node;

    //! @brief The splay tree structure.
    typedef struct SplayTree
    {
        //! @brief Pointer to the root node.
        struct SplayTreeNode* root;
        //! @brief The key's compare function.
        Compare compare;
    } Tree;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern Node* getMinimum(const Tree* const tree);
extern Node* getMaximum(const Tree* const tree);

extern Node* search(const Tree* const tree, const void* const key);
extern void splaying(Tree* const tree, const void* const key);
extern void insertion(Tree* const tree, const void* const key);
extern void deletion(Tree* const tree, const void* const key);
extern void destruction(const Tree* const tree);

//! @brief Output helper for the splay tree structure.
//! @tparam SplayKey - type of key
template <typename SplayKey>
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;
    //! @brief The pre-order traversal of the splay subtree.
    //! @param node - root of the subtree
    void preOrderTraversal(const Node* const node);
    //! @brief The in-order traversal of the splay subtree.
    //! @param node - root of the subtree
    void inOrderTraversal(const Node* const node);
    //! @brief The post-order traversal of the splay subtree.
    //! @param node - root of the subtree
    void postOrderTraversal(const Node* const node);
    //! @brief Traverse the splay subtree.
    //! @param node - root of the subtree
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void traverse(const Node* const node, void* const key, const int direction);

private:
    //! @brief Output stream of the splay tree structure.
    std::ostringstream process;
    //! @brief Indentation size.
    int indent{0};
};

template <typename SplayKey>
std::ostringstream& Output<SplayKey>::output() noexcept
{
    return process;
}

template <typename SplayKey>
void Output<SplayKey>::preOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    output() << *static_cast<SplayKey*>(node->key) << " ... ";
    preOrderTraversal(node->left);
    preOrderTraversal(node->right);
}

template <typename SplayKey>
void Output<SplayKey>::inOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    inOrderTraversal(node->left);
    output() << *static_cast<SplayKey*>(node->key) << " ... ";
    inOrderTraversal(node->right);
}

template <typename SplayKey>
void Output<SplayKey>::postOrderTraversal(const Node* const node)
{
    if (!node)
    {
        return;
    }

    postOrderTraversal(node->left);
    postOrderTraversal(node->right);
    output() << *static_cast<SplayKey*>(node->key) << " ... ";
}

template <typename SplayKey>
void Output<SplayKey>::traverse(const Node* const node, void* const key, const int direction)
{
    if (!node || !key)
    {
        return;
    }

    const int currInd = indent;
    if (direction == 0)
    {
        indent = 0;
        output() << "+ " << *static_cast<SplayKey*>(node->key) << " -> root\n";
    }
    else
    {
        output() << "+ " << std::string(currInd, ' ') << *static_cast<SplayKey*>(node->key) << " -> "
                 << *static_cast<SplayKey*>(key) << "'s " << ((direction == 1) ? "right" : "left") << " child\n";
    }

    indent += 2;
    traverse(node->left, node->key, -1);
    traverse(node->right, node->key, 1);
    indent = currInd;
}
} // namespace splay
} // namespace tree
} // namespace date_structure
