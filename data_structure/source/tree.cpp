//! @file tree.cpp
//! @author ryftchen
//! @brief The definitions (tree) in the data structure module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#include "tree.hpp"
#include <iostream>

namespace date_structure::tree
{
namespace bs
{
//! @brief Get the node where the minimum key value is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the minimum key value is located
Node* getMinimum(BSTree tree)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    while (nullptr != tree->left)
    {
        tree = tree->left;
    }

    return tree;
}

//! @brief Get the node where the maximum key value is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the maximum key value is located
Node* getMaximum(BSTree tree)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    while (nullptr != tree->right)
    {
        tree = tree->right;
    }

    return tree;
}

//! @brief Get the predecessor node of the current node.
//!        The precursor of a node is the node that has the maximum key value in that node's left subtree.
//! @param x - current node
//! @return predecessor node
Node* getPredecessor(Node* x)
{
    if (nullptr != x->left)
    {
        return getMaximum(x->left);
    }

    Node* y = x->parent;
    while ((nullptr != y) && (y->left == x))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

//! @brief Get the successor node of the current node.
//!        The precursor of a node is the node that has the minimum key value in that node's right subtree.
//! @param x - current node
//! @return successor node
Node* getSuccessor(Node* x)
{
    if (nullptr != x->right)
    {
        return getMinimum(x->right);
    }

    Node* y = x->parent;
    while ((nullptr != y) && (y->right == x))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

//! @brief Create a node of the binary search tree.
//! @param key - the key value of the node to be created
//! @param parent - the parent node of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
Node* createNode(const Type key, Node* parent, Node* left, Node* right)
{
    Node* p = new (std::nothrow) Node;
    if (nullptr == p)
    {
        return nullptr;
    }

    p->key = key;
    p->left = left;
    p->right = right;
    p->parent = parent;

    return p;
}

//! @brief Insert target node into the binary search tree. Allow inserting node with the same key value.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param z - target node
//! @return root node after inserting
Node* insertNode(BSTree tree, Node* z)
{
    Node* y = nullptr;
    Node* x = tree;

    while (nullptr != x)
    {
        y = x;
        if (z->key < x->key)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    z->parent = y;
    if (nullptr == y)
    {
        tree = z;
    }
    else if (z->key < y->key)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    return tree;
}

//! @brief Delete target node from the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param z - target node
//! @return root node after deleting
Node* deleteNode(BSTree tree, Node* z)
{
    Node* x = nullptr;
    Node* y = nullptr;

    if ((nullptr == z->left) || (nullptr == z->right))
    {
        y = z;
    }
    else
    {
        y = getSuccessor(z);
    }

    if (nullptr != y->left)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }

    if (nullptr != x)
    {
        x->parent = y->parent;
    }

    if (nullptr == y->parent)
    {
        tree = x;
    }
    else if (y->parent->left == y)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }

    if (y != z)
    {
        z->key = y->key;
    }

    if (tree == y)
    {
        delete y;
        return nullptr;
    }
    delete y;
    return tree;
}

//! @brief Search the node of binary search tree by key value.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the node
//! @return node where the key value is located
Node* bsTreeSearch(BSTree tree, const Type key)
{
    if ((nullptr == tree) || (tree->key == key))
    {
        return tree;
    }

    if (key < tree->key)
    {
        return bsTreeSearch(tree->left, key);
    }
    else
    {
        return bsTreeSearch(tree->right, key);
    }
}

//! @brief Insert target node into the binary search tree. Allow inserting node with the same key value.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after inserting
Node* bsTreeInsert(BSTree tree, const Type key)
{
    Node* z = createNode(key, nullptr, nullptr, nullptr);
    if (nullptr == z)
    {
        return tree;
    }

    return insertNode(tree, z);
}

//! @brief Delete target node into the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after deleting
Node* bsTreeDelete(BSTree tree, const Type key)
{
    Node* z = bsTreeSearch(tree, key);
    if (nullptr != z)
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

//! @brief Destroy the the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
void destroyBSTree(BSTree tree)
{
    if (nullptr == tree)
    {
        return;
    }

    if (nullptr != tree->left)
    {
        destroyBSTree(tree->left);
    }
    if (nullptr != tree->right)
    {
        destroyBSTree(tree->right);
    }

    delete tree;
}

void Output::preorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        flush() << tree->key << ' ';
        preorderBSTree(tree->left);
        preorderBSTree(tree->right);
    }
}

void Output::inorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        inorderBSTree(tree->left);
        flush() << tree->key << ' ';
        inorderBSTree(tree->right);
    }
}

void Output::postorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        postorderBSTree(tree->left);
        postorderBSTree(tree->right);
        flush() << tree->key << ' ';
    }
}

void Output::printBSTree(BSTree tree, const Type key, int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            flush() << tree->key << " is root\n";
        }
        else
        {
            flush() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : "left") << " child\n";
        }

        printBSTree(tree->left, tree->key, -1);
        printBSTree(tree->right, tree->key, 1);
    }
}
} // namespace bs

namespace avl
{
//! @brief Get the height of the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return height of the AVL tree
int getHeight(AVLTree tree)
{
    return ((nullptr == tree) ? 0 : ((Node*)tree)->height);
}

//! @brief Get the node where the Minimum key value is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the Minimum key value is located
Node* getMinimum(AVLTree tree)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    while (nullptr != tree->left)
    {
        tree = tree->left;
    }

    return tree;
}

//! @brief Get the node where the maximum key value is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the maximum key value is located
Node* getMaximum(AVLTree tree)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    while (nullptr != tree->right)
    {
        tree = tree->right;
    }
    return tree;
}

//! @brief LL rotation. A single left rotation.
//! @param k2 - the root node of the unbalanced AVL tree
//! @return root node after rotation
Node* leftLeftRotation(AVLTree k2)
{
    AVLTree k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = std::max(getHeight(k2->left), getHeight(k2->right)) + 1;
    k1->height = std::max(getHeight(k1->left), k2->height) + 1;

    return k1;
}

//! @brief RR rotation. A single right rotation.
//! @param k1 - the root node of the unbalanced AVL tree
//! @return root node after rotation
Node* rightRightRotation(AVLTree k1)
{
    AVLTree k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = std::max(getHeight(k1->left), getHeight(k1->right)) + 1;
    k2->height = std::max(getHeight(k2->right), k1->height) + 1;

    return k2;
}

//! @brief LR rotation. A double left rotation.
//! @param k3 - the root node of the unbalanced AVL tree
//! @return root node after rotation
Node* leftRightRotation(AVLTree k3)
{
    k3->left = rightRightRotation(k3->left);

    return leftLeftRotation(k3);
}

//! @brief RL rotation. A double right rotation.
//! @param k1 - the root node of the unbalanced AVL tree
//! @return root node after rotation
Node* rightLeftRotation(AVLTree k1)
{
    k1->right = leftLeftRotation(k1->right);

    return rightRightRotation(k1);
}

//! @brief Create a node of the AVL tree.
//! @param key - the key value of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
Node* createNode(const Type key, Node* left, Node* right)
{
    Node* p = new (std::nothrow) Node;
    if (nullptr == p)
    {
        return nullptr;
    }

    p->key = key;
    p->height = 0;
    p->left = left;
    p->right = right;

    return p;
}

//! @brief Delete target node from the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param z - target node
//! @return root node after deleting
Node* deleteNode(AVLTree tree, Node* z)
{
    if ((nullptr == tree) || (nullptr == z))
    {
        return nullptr;
    }

    if (z->key < tree->key)
    {
        tree->left = deleteNode(tree->left, z);
        if (2 == (getHeight(tree->right) - getHeight(tree->left)))
        {
            Node* r = tree->right;
            if (getHeight(r->left) > getHeight(r->right))
            {
                tree = rightLeftRotation(tree);
            }
            else
            {
                tree = rightRightRotation(tree);
            }
        }
    }
    else if (z->key > tree->key)
    {
        tree->right = deleteNode(tree->right, z);
        if (2 == (getHeight(tree->left) - getHeight(tree->right)))
        {
            Node* l = tree->left;
            if (getHeight(l->right) > getHeight(l->left))
            {
                tree = leftRightRotation(tree);
            }
            else
            {
                tree = leftLeftRotation(tree);
            }
        }
    }
    else
    {
        if (tree->left && tree->right)
        {
            if (getHeight(tree->left) > getHeight(tree->right))
            {
                Node* max = getMaximum(tree->left);
                tree->key = max->key;
                tree->left = deleteNode(tree->left, max);
            }
            else
            {
                Node* min = getMaximum(tree->right);
                tree->key = min->key;
                tree->right = deleteNode(tree->right, min);
            }
        }
        else
        {
            Node* temp = tree;
            tree = tree->left ? tree->left : tree->right;
            delete temp;
        }
    }

    return tree;
}

//! @brief Search the node of AVL tree by key value.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the node
//! @return node where the key value is located
Node* avlTreeSearch(AVLTree tree, const Type key)
{
    if ((nullptr == tree) || (tree->key == key))
    {
        return tree;
    }

    if (key < tree->key)
    {
        return avlTreeSearch(tree->left, key);
    }
    else
    {
        return avlTreeSearch(tree->right, key);
    }
}

//! @brief Insert target node into the AVL tree. Not allow inserting node with the same key value.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after inserting
Node* avlTreeInsert(AVLTree tree, const Type key)
{
    if (nullptr == tree)
    {
        tree = createNode(key, nullptr, nullptr);
        if (nullptr == tree)
        {
            std::cerr << "Failed to create AVL tree node." << std::endl;
            return nullptr;
        }
    }
    else if (key < tree->key)
    {
        tree->left = avlTreeInsert(tree->left, key);
        if (2 == (getHeight(tree->left) - getHeight(tree->right)))
        {
            if (key < tree->left->key)
            {
                tree = leftLeftRotation(tree);
            }
            else
            {
                tree = leftRightRotation(tree);
            }
        }
    }
    else if (key > tree->key)
    {
        tree->right = avlTreeInsert(tree->right, key);
        if (2 == (getHeight(tree->right) - getHeight(tree->left)))
        {
            if (key > tree->right->key)
            {
                tree = rightRightRotation(tree);
            }
            else
            {
                tree = rightLeftRotation(tree);
            }
        }
    }
    else
    {
        std::cerr << "Do not allow the same node to be inserted." << std::endl;
    }

    tree->height = std::max(getHeight(tree->left), getHeight(tree->right)) + 1;

    return tree;
}

//! @brief Delete target node into the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after deleting
Node* avlTreeDelete(AVLTree tree, const Type key)
{
    Node* z = avlTreeSearch(tree, key);
    if (nullptr != z)
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

//! @brief Destroy the the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
void destroyAVLTree(AVLTree tree)
{
    if (nullptr == tree)
    {
        return;
    }

    if (nullptr != tree->left)
    {
        destroyAVLTree(tree->left);
    }
    if (nullptr != tree->right)
    {
        destroyAVLTree(tree->right);
    }

    delete tree;
}

void Output::preorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        flush() << tree->key << ' ';
        preorderAVLTree(tree->left);
        preorderAVLTree(tree->right);
    }
}

void Output::inorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        inorderAVLTree(tree->left);
        flush() << tree->key << ' ';
        inorderAVLTree(tree->right);
    }
}

void Output::postorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        postorderAVLTree(tree->left);
        postorderAVLTree(tree->right);
        flush() << tree->key << ' ';
    }
}

void Output::printAVLTree(AVLTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            flush() << tree->key << " is root\n";
        }
        else
        {
            flush() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : "left") << " child\n";
        }

        printAVLTree(tree->left, tree->key, -1);
        printAVLTree(tree->right, tree->key, 1);
    }
}
} // namespace avl

namespace splay
{
//! @brief Get the node where the minimum key value is located in the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @return node where the minimum key value is located
Node* getMinimum(SplayTree tree)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    while (nullptr != tree->left)
    {
        tree = tree->left;
    }

    return tree;
}

//! @brief Get the node where the maximum key value is located in the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @return node where the maximum key value is located
Node* getMaximum(SplayTree tree)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    while (nullptr != tree->right)
    {
        tree = tree->right;
    }

    return tree;
}

//! @brief Create a node of the splay tree.
//! @param key - the key value of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
Node* createNode(const Type key, Node* left, Node* right)
{
    Node* p = new (std::nothrow) Node;
    if (nullptr == p)
    {
        return nullptr;
    }

    p->key = key;
    p->left = left;
    p->right = right;

    return p;
}

//! @brief Insert target node into the splay tree. Not splay. Not allow inserting node with the same key value.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param z - target node
//! @return root node after inserting
Node* insertNode(SplayTree tree, Node* z)
{
    Node* y = nullptr;
    Node* x = tree;

    while (nullptr != x)
    {
        y = x;
        if (z->key < x->key)
        {
            x = x->left;
        }
        else if (z->key > x->key)
        {
            x = x->right;
        }
        else
        {
            std::cerr << "Do not allow the same node to be inserted." << std::endl;
            delete z;
            return tree;
        }
    }

    if (nullptr == y)
    {
        tree = z;
    }
    else if (z->key < y->key)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    return tree;
}

//! @brief Search the node of splay tree by key value.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the node
//! @return node where the key value is located
Node* splayTreeSearch(SplayTree tree, const Type key)
{
    if ((nullptr == tree) || (key == tree->key))
    {
        return tree;
    }

    if (key < tree->key)
    {
        return splayTreeSearch(tree->left, key);
    }
    else
    {
        return splayTreeSearch(tree->right, key);
    }
}

//! @brief Splay target node in the splay tree. Make to be the root node.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after splaying
Node* splayTreeSplay(SplayTree tree, const Type key)
{
    Node n, *l, *r, *c;
    if (nullptr == tree)
    {
        return tree;
    }

    n.left = n.right = nullptr;
    l = r = &n;
    for (;;)
    {
        if (key < tree->key)
        {
            if (nullptr == tree->left)
            {
                break;
            }
            if (key < tree->left->key)
            {
                c = tree->left;
                tree->left = c->right;
                c->right = tree;
                tree = c;
                if (nullptr == tree->left)
                {
                    break;
                }
            }
            r->left = tree;
            r = tree;
            tree = tree->left;
        }
        else if (key > tree->key)
        {
            if (nullptr == tree->right)
            {
                break;
            }
            if (key > tree->right->key)
            {
                c = tree->right;
                tree->right = c->left;
                c->left = tree;
                tree = c;
                if (nullptr == tree->right)
                {
                    break;
                }
            }
            l->right = tree;
            l = tree;
            tree = tree->right;
        }
        else
        {
            break;
        }
    }

    l->right = tree->left;
    r->left = tree->right;
    tree->left = n.right;
    tree->right = n.left;

    return tree;
}

//! @brief Insert target node into the splay tree. Not allow inserting node with the same key value.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after inserting
Node* splayTreeInsert(SplayTree tree, const Type key)
{
    Node* z = createNode(key, nullptr, nullptr);
    if (nullptr == z)
    {
        return tree;
    }

    tree = insertNode(tree, z);
    tree = splayTreeSplay(tree, key);

    return tree;
}

//! @brief Delete target node into the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - the key value of the target node
//! @return root node after deleting
Node* splayTreeDelete(SplayTree tree, const Type key)
{
    if (nullptr == tree)
    {
        return nullptr;
    }

    if (nullptr == splayTreeSearch(tree, key))
    {
        return tree;
    }

    Node* x;
    tree = splayTreeSplay(tree, key);
    if (nullptr != tree->left)
    {
        x = splayTreeSplay(tree->left, key);
        x->right = tree->right;
    }
    else
    {
        x = tree->right;
    }

    delete tree;
    return x;
}

//! @brief Destroy the the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
void destroySplayTree(SplayTree tree)
{
    if (nullptr == tree)
    {
        return;
    }

    if (nullptr != tree->left)
    {
        destroySplayTree(tree->left);
    }
    if (nullptr != tree->right)
    {
        destroySplayTree(tree->right);
    }

    delete tree;
}

void Output::preorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        flush() << tree->key << ' ';
        preorderSplayTree(tree->left);
        preorderSplayTree(tree->right);
    }
}

void Output::inorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        inorderSplayTree(tree->left);
        flush() << tree->key << ' ';
        inorderSplayTree(tree->right);
    }
}

void Output::postorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        postorderSplayTree(tree->left);
        postorderSplayTree(tree->right);
        flush() << tree->key << ' ';
    }
}

void Output::printSplayTree(SplayTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            flush() << tree->key << " is root\n";
        }
        else
        {
            flush() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : "left") << " child\n";
        }

        printSplayTree(tree->left, tree->key, -1);
        printSplayTree(tree->right, tree->key, 1);
    }
}
} // namespace splay
} // namespace date_structure::tree
