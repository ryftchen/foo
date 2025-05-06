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
    //! @brief Alias for the type of key in the node of the binary search tree.
    typedef int Type;
#pragma pack(push, 8)
    //! @brief The node of the binary search tree.
    typedef struct BSTreeNode
    {
        //! @brief Key value. Sort usage.
        Type key;
        //! @brief Pointer to the left child node.
        struct BSTreeNode* left;
        //! @brief Pointer to the right child node.
        struct BSTreeNode* right;
        //! @brief Pointer to the parent node.
        struct BSTreeNode* parent;
    } Node, *BSTree;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern Node* getMinimum(BSTree tree);
extern Node* getMaximum(BSTree tree);
extern Node* getPredecessor(const Node* x);
extern Node* getSuccessor(const Node* x);
extern Node* createNode(const Type key, Node* const parent, Node* const left, Node* const right);
extern Node* insertNode(BSTree tree, Node* const z);
extern Node* deleteNode(BSTree tree, Node* const z);

extern Node* bsTreeSearch(BSTree tree, const Type key);
extern Node* bsTreeInsert(BSTree tree, const Type key);
extern Node* bsTreeDelete(BSTree tree, const Type key);
extern void destroyBSTree(BSTree tree);

//! @brief Output helper for the binary search tree structure.
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;
    //! @brief The pre-order traversal of the binary search tree.
    //! @param tree - tree root, the target binary search tree has this node as the root node
    void preorderBSTree(BSTree tree);
    //! @brief The in-order traversal of the binary search tree.
    //! @param tree - tree root, the target binary search tree has this node as the root node
    void inorderBSTree(BSTree tree);
    //! @brief The post-order traversal of the binary search tree.
    //! @param tree - tree root, the target binary search tree has this node as the root node
    void postorderBSTree(BSTree tree);
    //! @brief Print the binary search tree.
    //! @param tree - tree root, the target binary search tree has this node as the root node
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void printBSTree(BSTree tree, const Type key, const int direction);

private:
    //! @brief Output stream of the binary search tree structure.
    std::ostringstream process{};
    //! @brief Indentation size.
    int indent{0};
};
} // namespace bs

//! @brief The Adelson-Velsky-Landis tree structure.
namespace avl
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the type of key in the node of the AVL tree.
    typedef int Type;
#pragma pack(push, 8)
    //! @brief The node of the AVL tree.
    typedef struct AVLTreeNode
    {
        //! @brief Key value. Sort usage.
        Type key;
        //! @brief The height of an empty tree is 0, and the height of a non-empty tree is equal to its maximum level.
        int height;
        //! @brief Pointer to the left child node.
        struct AVLTreeNode* left;
        //! @brief Pointer to the right child node.
        struct AVLTreeNode* right;
    } Node, *AVLTree;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern int getHeight(AVLTree tree);
extern Node* getMinimum(AVLTree tree);
extern Node* getMaximum(AVLTree tree);
extern Node* leftLeftRotation(AVLTree k2);
extern Node* rightRightRotation(AVLTree k1);
extern Node* leftRightRotation(AVLTree k3);
extern Node* rightLeftRotation(AVLTree k1);
extern Node* createNode(const Type key, Node* const left, Node* const right);
extern Node* deleteNode(AVLTree tree, const Node* const z);

extern Node* avlTreeSearch(AVLTree tree, const Type key);
extern Node* avlTreeInsert(AVLTree tree, const Type key);
extern Node* avlTreeDelete(AVLTree tree, const Type key);
extern void destroyAVLTree(AVLTree tree);

//! @brief Output helper for the AVL tree structure.
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;
    //! @brief The pre-order traversal of the AVL tree.
    //! @param tree - tree root, the target AVL tree has this node as the root node
    void preorderAVLTree(AVLTree tree);
    //! @brief The in-order traversal of the AVL tree.
    //! @param tree - tree root, the target AVL tree has this node as the root node
    void inorderAVLTree(AVLTree tree);
    //! @brief The post-order traversal of the AVL tree.
    //! @param tree - tree root, the target AVL tree has this node as the root node
    void postorderAVLTree(AVLTree tree);
    //! @brief Print the AVL tree.
    //! @param tree - tree root, the target AVL tree has this node as the root node
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void printAVLTree(AVLTree tree, const Type key, const int direction);

private:
    //! @brief Output stream of the AVL tree structure.
    std::ostringstream process{};
    //! @brief Indentation size.
    int indent{0};
};
} // namespace avl

//! @brief The splay tree structure.
namespace splay
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Alias for the type of key in the node of the splay tree.
    typedef int Type;
#pragma pack(push, 8)
    //! @brief The node of the splay tree.
    typedef struct SplayTreeNode
    {
        //! @brief Key value. Sort usage.
        Type key;
        //! @brief Pointer to the left child node.
        struct SplayTreeNode* left;
        //! @brief Pointer to the right child node.
        struct SplayTreeNode* right;
    } Node, *SplayTree;
#pragma pack(pop)
#ifdef __cplusplus
}
#endif // __cplusplus

extern Node* getMinimum(SplayTree tree);
extern Node* getMaximum(SplayTree tree);
extern Node* createNode(const Type key, Node* const left, Node* const right);
extern Node* insertNode(SplayTree tree, Node* const z);

extern Node* splayTreeSearch(SplayTree tree, const Type key);
extern Node* splayTreeSplay(SplayTree tree, const Type key);
extern Node* splayTreeInsert(SplayTree tree, const Type key);
extern Node* splayTreeDelete(SplayTree tree, const Type key);
extern void destroySplayTree(SplayTree tree);

//! @brief Output helper for the splay tree structure.
class Output
{
public:
    //! @brief Destroy the Output object.
    virtual ~Output() = default;

    //! @brief Flush the output stream.
    //! @return reference of the output stream object, which is on string based
    std::ostringstream& output() noexcept;
    //! @brief The pre-order traversal of the splay tree.
    //! @param tree - tree root, the target splay tree has this node as the root node
    void preorderSplayTree(SplayTree tree);
    //! @brief The in-order traversal of the splay tree.
    //! @param tree - tree root, the target splay tree has this node as the root node
    void inorderSplayTree(SplayTree tree);
    //! @brief The post-order traversal of the splay tree.
    //! @param tree - tree root, the target splay tree has this node as the root node
    void postorderSplayTree(SplayTree tree);
    //! @brief Print the splay tree.
    //! @param tree - tree root, the target splay tree has this node as the root node
    //! @param key - key of the node
    //! @param direction - node type, the left is -1, the root is 0, and the right is 1
    void printSplayTree(SplayTree tree, const Type key, const int direction);

private:
    //! @brief Output stream of the splay tree structure.
    std::ostringstream process{};
    //! @brief Indentation size.
    int indent{0};
};
} // namespace splay
} // namespace tree
} // namespace date_structure
