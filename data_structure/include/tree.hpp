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

extern std::ostringstream& output();
extern void preorderBSTree(BSTree tree);
extern void inorderBSTree(BSTree tree);
extern void postorderBSTree(BSTree tree);
extern void printBSTree(BSTree tree, const Type key, int direction);

extern Node* getMinimum(BSTree tree);
extern Node* getMaximum(BSTree tree);
extern Node* getPredecessor(Node* x);
extern Node* getSuccessor(Node* x);
extern Node* createNode(const Type key, Node* parent, Node* left, Node* right);
extern Node* insertNode(BSTree tree, Node* z);
extern Node* deleteNode(BSTree tree, Node* z);

extern Node* bsTreeSearch(BSTree tree, const Type key);
extern Node* bsTreeInsert(BSTree tree, const Type key);
extern Node* bsTreeDelete(BSTree tree, const Type key);
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

extern std::ostringstream& output();
extern void preorderAVLTree(AVLTree tree);
extern void inorderAVLTree(AVLTree tree);
extern void postorderAVLTree(AVLTree tree);
extern void printAVLTree(AVLTree tree, const Type key, const int direction);

extern int getHeight(AVLTree tree);
extern Node* getMinimum(AVLTree tree);
extern Node* getMaximum(AVLTree tree);
extern Node* leftLeftRotation(AVLTree k2);
extern Node* rightRightRotation(AVLTree k1);
extern Node* leftRightRotation(AVLTree k3);
extern Node* rightLeftRotation(AVLTree k1);
extern Node* createNode(const Type key, Node* left, Node* right);
extern Node* deleteNode(AVLTree tree, Node* z);

extern Node* avlTreeSearch(AVLTree tree, const Type key);
extern Node* avlTreeInsert(AVLTree tree, const Type key);
extern Node* avlTreeDelete(AVLTree tree, const Type key);
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

extern std::ostringstream& output();
extern void preorderSplayTree(SplayTree tree);
extern void inorderSplayTree(SplayTree tree);
extern void postorderSplayTree(SplayTree tree);
extern void printSplayTree(SplayTree tree, const Type key, const int direction);

extern Node* getMinimum(SplayTree tree);
extern Node* getMaximum(SplayTree tree);
extern Node* createNode(const Type key, Node* left, Node* right);
extern Node* insertNode(SplayTree tree, Node* z);
extern Node* splayTreeSearch(SplayTree tree, const Type key);
extern Node* splayTreeSplay(SplayTree tree, const Type key);
extern Node* splayTreeInsert(SplayTree tree, const Type key);
extern Node* splayTreeDelete(SplayTree tree, const Type key);
extern void destroySplayTree(SplayTree tree);
} // namespace splay
} // namespace date_structure::tree
