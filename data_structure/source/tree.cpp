//! @file tree.cpp
//! @author ryftchen
//! @brief The definitions (tree) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "tree.hpp"

namespace date_structure::tree
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

namespace bs
{
//! @brief Get the node where the minimum key is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the minimum key is located
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

//! @brief Get the node where the maximum key is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the maximum key is located
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
//!        The precursor of a node is the node that has the maximum key in that node's left subtree.
//! @param x - current node
//! @return predecessor node
Node* getPredecessor(const Node* x)
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
//!        The precursor of a node is the node that has the minimum key in that node's right subtree.
//! @param x - current node
//! @return successor node
Node* getSuccessor(const Node* x)
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
//! @param key - key of the node to be created
//! @param parent - the parent node of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
Node* createNode(const Type key, Node* const parent, Node* const left, Node* const right)
{
    Node* const node = new (std::nothrow) Node;
    if (nullptr == node)
    {
        return nullptr;
    }

    node->key = key;
    node->left = left;
    node->right = right;
    node->parent = parent;

    return node;
}

//! @brief Insert target node into the binary search tree. Allow inserting node with the same key.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param z - target node
//! @return root node after inserting
Node* insertNode(BSTree tree, Node* const z)
{
    Node *x = tree, *y = nullptr;

    while (nullptr != x)
    {
        y = x;
        x = (z->key < x->key) ? x->left : x->right;
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
Node* deleteNode(BSTree tree, Node* const z)
{
    Node *y = ((nullptr == z->left) || (nullptr == z->right)) ? z : getSuccessor(z),
         *x = (nullptr != y->left) ? y->left : y->right;

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

//! @brief Search the node of binary search tree by key.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - key of the node
//! @return node where the key is located
Node* bsTreeSearch(BSTree tree, const Type key)
{
    if ((nullptr == tree) || (tree->key == key))
    {
        return tree;
    }

    return (key < tree->key) ? bsTreeSearch(tree->left, key) : bsTreeSearch(tree->right, key);
}

//! @brief Insert target node into the binary search tree. Allow inserting node with the same key.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - key of the target node
//! @return root node after inserting
Node* bsTreeInsert(BSTree tree, const Type key)
{
    Node* const z = createNode(key, nullptr, nullptr, nullptr);
    if (nullptr == z)
    {
        return tree;
    }

    return insertNode(tree, z);
}

//! @brief Delete target node into the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - key of the target node
//! @return root node after deleting
Node* bsTreeDelete(BSTree tree, const Type key)
{
    Node* const z = bsTreeSearch(tree, key);
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

std::ostringstream& Output::output()
{
    return process;
}

void Output::preorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ... ";
        preorderBSTree(tree->left);
        preorderBSTree(tree->right);
    }
}

void Output::inorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        inorderBSTree(tree->left);
        output() << tree->key << " ... ";
        inorderBSTree(tree->right);
    }
}

void Output::postorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        postorderBSTree(tree->left);
        postorderBSTree(tree->right);
        output() << tree->key << " ... ";
    }
}

void Output::printBSTree(BSTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        int currInd = indent;
        if (0 == direction)
        {
            indent = 0;
            output() << "+ " << tree->key << " -> root\n";
        }
        else
        {
            output() << "+ " << std::string(currInd, ' ') << tree->key << " -> " << key << "'s "
                     << ((1 == direction) ? "right" : "left") << " child\n";
        }

        indent += 2;
        printBSTree(tree->left, tree->key, -1);
        printBSTree(tree->right, tree->key, 1);
        indent = currInd;
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
    return (nullptr == tree) ? 0 : static_cast<Node*>(tree)->height;
}

//! @brief Get the node where the Minimum key is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the minimum key is located
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

//! @brief Get the node where the maximum key is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the maximum key is located
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
//! @param key - key of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
Node* createNode(const Type key, Node* const left, Node* const right)
{
    Node* const node = new (std::nothrow) Node;
    if (nullptr == node)
    {
        return nullptr;
    }

    node->key = key;
    node->height = 0;
    node->left = left;
    node->right = right;

    return node;
}

//! @brief Delete target node from the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param z - target node
//! @return root node after deleting
Node* deleteNode(AVLTree tree, const Node* const z)
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
            const Node* const r = tree->right;
            tree = (getHeight(r->left) > getHeight(r->right)) ? rightLeftRotation(tree) : rightRightRotation(tree);
        }
    }
    else if (z->key > tree->key)
    {
        tree->right = deleteNode(tree->right, z);
        if (2 == (getHeight(tree->left) - getHeight(tree->right)))
        {
            const Node* const l = tree->left;
            tree = (getHeight(l->right) > getHeight(l->left)) ? leftRightRotation(tree) : leftLeftRotation(tree);
        }
    }
    else if (tree->left && tree->right)
    {
        if (getHeight(tree->left) > getHeight(tree->right))
        {
            const Node* const max = getMaximum(tree->left);
            tree->key = max->key;
            tree->left = deleteNode(tree->left, max);
        }
        else
        {
            const Node* const min = getMaximum(tree->right);
            tree->key = min->key;
            tree->right = deleteNode(tree->right, min);
        }
    }
    else
    {
        const Node* const temp = tree;
        tree = tree->left ? tree->left : tree->right;
        delete temp;
    }

    return tree;
}

//! @brief Search the node of AVL tree by key.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - key of the node
//! @return node where the key is located
Node* avlTreeSearch(AVLTree tree, const Type key)
{
    if ((nullptr == tree) || (tree->key == key))
    {
        return tree;
    }

    return (key < tree->key) ? avlTreeSearch(tree->left, key) : avlTreeSearch(tree->right, key);
}

//! @brief Insert target node into the AVL tree. Not allow inserting node with the same key.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - key of the target node
//! @return root node after inserting
Node* avlTreeInsert(AVLTree tree, const Type key)
{
    if (nullptr == tree)
    {
        tree = createNode(key, nullptr, nullptr);
        if (nullptr == tree)
        {
            return nullptr;
        }
    }
    else if (key < tree->key)
    {
        tree->left = avlTreeInsert(tree->left, key);
        if (2 == (getHeight(tree->left) - getHeight(tree->right)))
        {
            tree = (key < tree->left->key) ? leftLeftRotation(tree) : leftRightRotation(tree);
        }
    }
    else if (key > tree->key)
    {
        tree->right = avlTreeInsert(tree->right, key);
        if (2 == (getHeight(tree->right) - getHeight(tree->left)))
        {
            tree = (key > tree->right->key) ? rightRightRotation(tree) : rightLeftRotation(tree);
        }
    }

    tree->height = std::max(getHeight(tree->left), getHeight(tree->right)) + 1;

    return tree;
}

//! @brief Delete target node into the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - key of the target node
//! @return root node after deleting
Node* avlTreeDelete(AVLTree tree, const Type key)
{
    const Node* const z = avlTreeSearch(tree, key);
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

std::ostringstream& Output::output()
{
    return process;
}

void Output::preorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ... ";
        preorderAVLTree(tree->left);
        preorderAVLTree(tree->right);
    }
}

void Output::inorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        inorderAVLTree(tree->left);
        output() << tree->key << " ... ";
        inorderAVLTree(tree->right);
    }
}

void Output::postorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        postorderAVLTree(tree->left);
        postorderAVLTree(tree->right);
        output() << tree->key << " ... ";
    }
}

void Output::printAVLTree(AVLTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        int currInd = indent;
        if (0 == direction)
        {
            indent = 0;
            output() << "+ " << tree->key << " -> root\n";
        }
        else
        {
            output() << "+ " << std::string(currInd, ' ') << tree->key << " -> " << key << "'s "
                     << ((1 == direction) ? "right" : "left") << " child\n";
        }

        indent += 2;
        printAVLTree(tree->left, tree->key, -1);
        printAVLTree(tree->right, tree->key, 1);
        indent = currInd;
    }
}
} // namespace avl

namespace splay
{
//! @brief Get the node where the minimum key is located in the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @return node where the minimum key is located
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

//! @brief Get the node where the maximum key is located in the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @return node where the maximum key is located
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
//! @param key - key of the node to be created
//! @param left - the left child node of the node to be created
//! @param right - the right child node of the node to be created
//! @return new node after creating
Node* createNode(const Type key, Node* const left, Node* const right)
{
    Node* const node = new (std::nothrow) Node;
    if (nullptr == node)
    {
        return nullptr;
    }

    node->key = key;
    node->left = left;
    node->right = right;

    return node;
}

//! @brief Insert target node into the splay tree. Not splay. Not allow inserting node with the same key.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param z - target node
//! @return root node after inserting
Node* insertNode(SplayTree tree, Node* const z)
{
    Node *x = tree, *y = nullptr;

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

//! @brief Search the node of splay tree by key.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - key of the node
//! @return node where the key is located
Node* splayTreeSearch(SplayTree tree, const Type key)
{
    if ((nullptr == tree) || (key == tree->key))
    {
        return tree;
    }

    return (key < tree->key) ? splayTreeSearch(tree->left, key) : splayTreeSearch(tree->right, key);
}

//! @brief Splay target node in the splay tree. Make to be the root node.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - key of the target node
//! @return root node after splaying
Node* splayTreeSplay(SplayTree tree, const Type key)
{
    if (nullptr == tree)
    {
        return nullptr;
    }
    Node n{}, *l = nullptr, *r = nullptr, *c = nullptr;

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
            else if (key < tree->left->key)
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
            else if (key > tree->right->key)
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

//! @brief Insert target node into the splay tree. Not allow inserting node with the same key.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - key of the target node
//! @return root node after inserting
Node* splayTreeInsert(SplayTree tree, const Type key)
{
    Node* const z = createNode(key, nullptr, nullptr);
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
//! @param key - key of the target node
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

    Node* x = nullptr;
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

std::ostringstream& Output::output()
{
    return process;
}

void Output::preorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ... ";
        preorderSplayTree(tree->left);
        preorderSplayTree(tree->right);
    }
}

void Output::inorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        inorderSplayTree(tree->left);
        output() << tree->key << " ... ";
        inorderSplayTree(tree->right);
    }
}

void Output::postorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        postorderSplayTree(tree->left);
        postorderSplayTree(tree->right);
        output() << tree->key << " ... ";
    }
}

void Output::printSplayTree(SplayTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        int currInd = indent;
        if (0 == direction)
        {
            indent = 0;
            output() << "+ " << tree->key << " -> root\n";
        }
        else
        {
            output() << "+ " << std::string(currInd, ' ') << tree->key << " -> " << key << "'s "
                     << ((1 == direction) ? "right" : "left") << " child\n";
        }

        indent += 2;
        printSplayTree(tree->left, tree->key, -1);
        printSplayTree(tree->right, tree->key, 1);
        indent = currInd;
    }
}
} // namespace splay
} // namespace date_structure::tree
