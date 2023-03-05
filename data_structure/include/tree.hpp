//! @file tree.hpp
//! @author ryftchen
//! @brief The declarations (tree) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <sstream>
#else
#include "pch_data_structure.hpp"
#endif

//! @brief Tree-related functions in the data structure module.
namespace date_structure::tree
{
//! @brief The binary search tree structure.
namespace bs
{
//! @brief Alias for the type of key in the node of the binary search tree.
typedef int Type;
#ifdef __cplusplus
extern "C"
{
#endif
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
#ifdef __cplusplus
}
#endif

//! @brief Output stream for the binary search tree structure. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
extern std::ostringstream& output();
//! @brief The pre-order traversal of the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
extern void preorderBSTree(BSTree tree);
//! @brief The in-order traversal of the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
extern void inorderBSTree(BSTree tree);
//! @brief The post-order traversal of the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
extern void postorderBSTree(BSTree tree);
//! @brief Print the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the node
//! @param direction - node type, the left is -1, the root is 0, and the right is 1
extern void printBSTree(BSTree tree, const Type key, int direction);
//! @brief Get the node where the minimum key value is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the minimum key value is located
extern Node* getMinimum(BSTree tree);
//! @brief Get the node where the maximum key value is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the maximum key value is located
extern Node* getMaximum(BSTree tree);
//! @brief Get the predecessor node of the current node.
//!        The precursor of a node is the node that has the maximum key value in that node's left subtree.
//! @param x - current node
//! @return predecessor node
extern Node* getPredecessor(Node* x);
//! @brief Get the successor node of the current node.
//!        The precursor of a node is the node that has the minimum key value in that node's right subtree.
//! @param x - current node
//! @return successor node
extern Node* getSuccessor(Node* x);
//! @brief Create a node of the binary search tree.
//! @param key - the key value of the node to be created
//! @param parent - the parent node of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
extern Node* createNode(const Type key, Node* parent, Node* left, Node* right);
//! @brief Insert target node into the binary search tree. Allow inserting node with the same key value.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param z - target node
//! @return root node after inserting
extern Node* insertNode(BSTree tree, Node* z);
//! @brief Delete target node from the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param z - target node
//! @return root node after deleting
extern Node* deleteNode(BSTree tree, Node* z);

//! @brief Search the node of binary search tree by key value.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the node
//! @return node where the key value is located
extern Node* bsTreeSearch(BSTree tree, const Type key);
//! @brief Insert target node into the binary search tree. Allow inserting node with the same key value.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after inserting
extern Node* bsTreeInsert(BSTree tree, const Type key);
//! @brief Delete target node into the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after deleting
extern Node* bsTreeDelete(BSTree tree, const Type key);
//! @brief Destroy the the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
extern void destroyBSTree(BSTree tree);
} // namespace bs

//! @brief The Adelson-Velsky-Landis tree structure.
namespace avl
{
//! @brief Alias for the type of key in the node of the AVL tree.
typedef int Type;
#ifdef __cplusplus
extern "C"
{
#endif
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
#ifdef __cplusplus
}
#endif

//! @brief Output stream for the AVL tree structure. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
extern std::ostringstream& output();
//! @brief The pre-order traversal of the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
extern void preorderAVLTree(AVLTree tree);
//! @brief The in-order traversal of the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
extern void inorderAVLTree(AVLTree tree);
//! @brief The post-order traversal of the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
extern void postorderAVLTree(AVLTree tree);
//! @brief Print the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the node
//! @param direction - node type, the left is -1, the root is 0, and the right is 1
extern void printAVLTree(AVLTree tree, const Type key, const int direction);
//! @brief Get the height of the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return height of the AVL tree
extern int getHeight(AVLTree tree);
//! @brief Get the node where the Minimum key value is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the Minimum key value is located
extern Node* getMinimum(AVLTree tree);
//! @brief Get the node where the maximum key value is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the maximum key value is located
extern Node* getMaximum(AVLTree tree);
//! @brief LL rotation. A single left rotation.
//! @param k2 - the root node of the unbalanced AVL tree
//! @return root node after rotation
extern Node* leftLeftRotation(AVLTree k2);
//! @brief RR rotation. A single right rotation.
//! @param k1 - the root node of the unbalanced AVL tree
//! @return root node after rotation
extern Node* rightRightRotation(AVLTree k1);
//! @brief LR rotation. A double left rotation.
//! @param k3 - the root node of the unbalanced AVL tree
//! @return root node after rotation
extern Node* leftRightRotation(AVLTree k3);
//! @brief RL rotation. A double right rotation.
//! @param k1 - the root node of the unbalanced AVL tree
//! @return root node after rotation
extern Node* rightLeftRotation(AVLTree k1);
//! @brief Create a node of the AVL tree.
//! @param key - the key value of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
extern Node* createNode(const Type key, Node* left, Node* right);
//! @brief Delete target node from the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param z - target node
//! @return root node after deleting
extern Node* deleteNode(AVLTree tree, Node* z);

//! @brief Search the node of AVL tree by key value.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the node
//! @return node where the key value is located
extern Node* avlTreeSearch(AVLTree tree, const Type key);
//! @brief Insert target node into the AVL tree. Not allow inserting node with the same key value.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after inserting
extern Node* avlTreeInsert(AVLTree tree, const Type key);
//! @brief Delete target node into the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after deleting
extern Node* avlTreeDelete(AVLTree tree, const Type key);
//! @brief Destroy the the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
extern void destroyAVLTree(AVLTree tree);
} // namespace avl

//! @brief The splay tree structure.
namespace splay
{
//! @brief Alias for the type of key in the node of the splay tree.
typedef int Type;
#ifdef __cplusplus
extern "C"
{
#endif
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
#ifdef __cplusplus
}
#endif

//! @brief Output stream for the splay tree structure. Need to be cleared manually.
//! @return reference of output stream object, which is on string based.
extern std::ostringstream& output();
//! @brief The pre-order traversal of the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
extern void preorderSplayTree(SplayTree tree);
//! @brief The in-order traversal of the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
extern void inorderSplayTree(SplayTree tree);
//! @brief The post-order traversal of the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
extern void postorderSplayTree(SplayTree tree);
//! @brief Print the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the node
//! @param direction - node type, the left is -1, the root is 0, and the right is 1
extern void printSplayTree(SplayTree tree, const Type key, const int direction);
//! @brief Get the node where the minimum key value is located in the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @return node where the minimum key value is located
extern Node* getMinimum(SplayTree tree);
//! @brief Get the node where the maximum key value is located in the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @return node where the maximum key value is located
extern Node* getMaximum(SplayTree tree);
//! @brief Create a node of the splay tree.
//! @param key - the key value of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
extern Node* createNode(const Type key, Node* left, Node* right);
//! @brief Insert target node into the splay tree. Not splay. Not allow inserting node with the same key value.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param z - target node
//! @return root node after inserting
extern Node* insertNode(SplayTree tree, Node* z);
//! @brief Search the node of splay tree by key value.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the node
//! @return node where the key value is located
extern Node* splayTreeSearch(SplayTree tree, const Type key);
//! @brief Splay target node in the splay tree. Make to be the root node.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after splaying
extern Node* splayTreeSplay(SplayTree tree, const Type key);
//! @brief Insert target node into the splay tree. Not allow inserting node with the same key value.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after inserting
extern Node* splayTreeInsert(SplayTree tree, const Type key);
//! @brief Delete target node into the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after deleting
extern Node* splayTreeDelete(SplayTree tree, const Type key);
//! @brief Destroy the the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
extern void destroySplayTree(SplayTree tree);
} // namespace splay
} // namespace date_structure::tree
