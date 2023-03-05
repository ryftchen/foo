//! @file tree.cpp
//! @author ryftchen
//! @brief The definitions (tree) in the data structure module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "tree.hpp"
#ifndef __PRECOMPILED_HEADER
#include <iostream>
#endif

namespace date_structure::tree
{
namespace bs
{
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

void preorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ";
        preorderBSTree(tree->left);
        preorderBSTree(tree->right);
    }
}

void inorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        inorderBSTree(tree->left);
        output() << tree->key << " ";
        inorderBSTree(tree->right);
    }
}

void postorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        postorderBSTree(tree->left);
        postorderBSTree(tree->right);
        output() << tree->key << " ";
    }
}

void printBSTree(BSTree tree, const Type key, int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            output() << tree->key << " is root" << std::endl;
        }
        else
        {
            output() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : "left") << " child"
                     << std::endl;
        }

        printBSTree(tree->left, tree->key, -1);
        printBSTree(tree->right, tree->key, 1);
    }
}

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

Node* createNode(const Type key, Node* parent, Node* left, Node* right)
{
    Node* p = nullptr;
    p = new Node();
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

Node* bsTreeInsert(BSTree tree, const Type key)
{
    Node* z = createNode(key, nullptr, nullptr, nullptr);
    if (nullptr == z)
    {
        return tree;
    }

    return insertNode(tree, z);
}

Node* bsTreeDelete(BSTree tree, const Type key)
{
    Node* z = bsTreeSearch(tree, key);
    if (nullptr != z)
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

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
} // namespace bs

namespace avl
{
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

void preorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ";
        preorderAVLTree(tree->left);
        preorderAVLTree(tree->right);
    }
}

void inorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        inorderAVLTree(tree->left);
        output() << tree->key << " ";
        inorderAVLTree(tree->right);
    }
}

void postorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        postorderAVLTree(tree->left);
        postorderAVLTree(tree->right);
        output() << tree->key << " ";
    }
}

void printAVLTree(AVLTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            output() << tree->key << " is root" << std::endl;
        }
        else
        {
            output() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : "left") << " child"
                     << std::endl;
        }

        printAVLTree(tree->left, tree->key, -1);
        printAVLTree(tree->right, tree->key, 1);
    }
}

int getHeight(AVLTree tree)
{
    return ((nullptr == tree) ? 0 : ((Node*)tree)->height);
}

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

Node* leftLeftRotation(AVLTree k2)
{
    AVLTree k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = std::max(getHeight(k2->left), getHeight(k2->right)) + 1;
    k1->height = std::max(getHeight(k1->left), k2->height) + 1;

    return k1;
}

Node* rightRightRotation(AVLTree k1)
{
    AVLTree k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = std::max(getHeight(k1->left), getHeight(k1->right)) + 1;
    k2->height = std::max(getHeight(k2->right), k1->height) + 1;

    return k2;
}

Node* leftRightRotation(AVLTree k3)
{
    k3->left = rightRightRotation(k3->left);

    return leftLeftRotation(k3);
}

Node* rightLeftRotation(AVLTree k1)
{
    k1->right = leftLeftRotation(k1->right);

    return rightRightRotation(k1);
}

Node* createNode(const Type key, Node* left, Node* right)
{
    Node* p = nullptr;
    p = new Node();
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

Node* avlTreeDelete(AVLTree tree, const Type key)
{
    Node* z = avlTreeSearch(tree, key);
    if (nullptr != z)
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

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
} // namespace avl

namespace splay
{
std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

void preorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ";
        preorderSplayTree(tree->left);
        preorderSplayTree(tree->right);
    }
}

void inorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        inorderSplayTree(tree->left);
        output() << tree->key << " ";
        inorderSplayTree(tree->right);
    }
}

void postorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        postorderSplayTree(tree->left);
        postorderSplayTree(tree->right);
        output() << tree->key << " ";
    }
}

void printSplayTree(SplayTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            output() << tree->key << " is root" << std::endl;
        }
        else
        {
            output() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : "left") << " child"
                     << std::endl;
        }

        printSplayTree(tree->left, tree->key, -1);
        printSplayTree(tree->right, tree->key, 1);
    }
}

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

Node* createNode(const Type key, Node* left, Node* right)
{
    Node* p = new Node();
    if (nullptr == p)
    {
        return nullptr;
    }

    p->key = key;
    p->left = left;
    p->right = right;

    return p;
}

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

Node* splayTreeSplay(SplayTree tree, const Type key)
{
    Node n, *l, *r, *c;
    if (tree == nullptr)
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
} // namespace splay
} // namespace date_structure::tree
