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

// NOLINTBEGIN(cppcoreguidelines-owning-memory)
namespace bs
{
//! @brief Get the node where the minimum key is located in the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @return node where the minimum key is located
Node* getMinimum(BSTree tree)
{
    if (!tree)
    {
        return nullptr;
    }

    while (tree->left)
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
    if (!tree)
    {
        return nullptr;
    }

    while (tree->right)
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
    if (x->left)
    {
        return getMaximum(x->left);
    }

    Node* y = x->parent;
    while (y && (y->left == x))
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
    if (x->right)
    {
        return getMinimum(x->right);
    }

    Node* y = x->parent;
    while (y && (y->right == x))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

//! @brief Create a node of the binary search tree.
//! @param key - key of the node to be created
//! @param parent - parent node of the node to be created
//! @param left - left child node of the node to be created
//! @param right - right child node of the node to be created
//! @return new node after creating
static Node* createNode(const Type key, Node* const parent, Node* const left, Node* const right)
{
    auto* const node = ::new (std::nothrow) Node;
    if (!node)
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
static Node* insertNode(BSTree tree, Node* const z)
{
    Node *x = tree, *y = nullptr;
    while (x)
    {
        y = x;
        x = (z->key < x->key) ? x->left : x->right;
    }

    z->parent = y;
    if (!y)
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
static Node* deleteNode(BSTree tree, Node* const z)
{
    Node *y = (!z->left || !z->right) ? z : getSuccessor(z), *x = y->left ? y->left : y->right;
    if (x)
    {
        x->parent = y->parent;
    }

    if (!y->parent)
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
        ::delete y;
        return nullptr;
    }

    ::delete y;

    return tree;
}

//! @brief Search the node of binary search tree by key.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - key of the node
//! @return node where the key is located
Node* search(BSTree tree, const Type key)
{
    if (!tree || (tree->key == key))
    {
        return tree;
    }

    return search((key < tree->key) ? tree->left : tree->right, key);
}

//! @brief Insert target node into the binary search tree. Allow inserting node with the same key.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - key of the target node
//! @return root node after inserting
Node* insertion(BSTree tree, const Type key)
{
    Node* const z = createNode(key, nullptr, nullptr, nullptr);
    return z ? insertNode(tree, z) : tree;
}

//! @brief Delete target node into the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
//! @param key - key of the target node
//! @return root node after deleting
Node* deletion(BSTree tree, const Type key)
{
    if (Node* const z = search(tree, key))
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

//! @brief Destroy the the binary search tree.
//! @param tree - tree root, the target binary search tree has this node as the root node
void destruction(BSTree tree)
{
    if (!tree)
    {
        return;
    }

    if (tree->left)
    {
        destruction(tree->left);
    }
    if (tree->right)
    {
        destruction(tree->right);
    }

    ::delete tree;
}

std::ostringstream& Output::output() noexcept
{
    return process;
}

void Output::preorderTraversal(BSTree tree)
{
    if (tree)
    {
        output() << tree->key << " ... ";
        preorderTraversal(tree->left);
        preorderTraversal(tree->right);
    }
}

void Output::inorderTraversal(BSTree tree)
{
    if (tree)
    {
        inorderTraversal(tree->left);
        output() << tree->key << " ... ";
        inorderTraversal(tree->right);
    }
}

void Output::postorderTraversal(BSTree tree)
{
    if (tree)
    {
        postorderTraversal(tree->left);
        postorderTraversal(tree->right);
        output() << tree->key << " ... ";
    }
}

void Output::traverse(BSTree tree, const Type key, const int direction)
{
    if (tree)
    {
        const int currInd = indent;
        if (direction == 0)
        {
            indent = 0;
            output() << "+ " << tree->key << " -> root\n";
        }
        else
        {
            output() << "+ " << std::string(currInd, ' ') << tree->key << " -> " << key << "'s "
                     << ((direction == 1) ? "right" : "left") << " child\n";
        }

        indent += 2;
        traverse(tree->left, tree->key, -1);
        traverse(tree->right, tree->key, 1);
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
    return tree ? static_cast<Node*>(tree)->height : 0;
}

//! @brief Get the node where the Minimum key is located in the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @return node where the minimum key is located
Node* getMinimum(AVLTree tree)
{
    if (!tree)
    {
        return nullptr;
    }

    while (tree->left)
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
    if (!tree)
    {
        return nullptr;
    }

    while (tree->right)
    {
        tree = tree->right;
    }

    return tree;
}

//! @brief LL rotation. A single left rotation.
//! @param k2 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* leftLeftRotation(AVLTree k2)
{
    AVLTree k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = std::max(getHeight(k2->left), getHeight(k2->right)) + 1;
    k1->height = std::max(getHeight(k1->left), k2->height) + 1;

    return k1;
}

//! @brief RR rotation. A single right rotation.
//! @param k1 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* rightRightRotation(AVLTree k1)
{
    AVLTree k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = std::max(getHeight(k1->left), getHeight(k1->right)) + 1;
    k2->height = std::max(getHeight(k2->right), k1->height) + 1;

    return k2;
}

//! @brief LR rotation. A double left rotation.
//! @param k3 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* leftRightRotation(AVLTree k3)
{
    k3->left = rightRightRotation(k3->left);
    return leftLeftRotation(k3);
}

//! @brief RL rotation. A double right rotation.
//! @param k1 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* rightLeftRotation(AVLTree k1)
{
    k1->right = leftLeftRotation(k1->right);
    return rightRightRotation(k1);
}

//! @brief Create a node of the AVL tree.
//! @param key - key of the node to be created
//! @param left - left child node of the node to be created
//! @param right - right child node of the node to be created
//! @return new node after creating
static Node* createNode(const Type key, Node* const left, Node* const right)
{
    auto* const node = ::new (std::nothrow) Node;
    if (!node)
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
static Node* deleteNode(AVLTree tree, const Node* const z)
{
    if (!tree || !z)
    {
        return nullptr;
    }

    if (z->key < tree->key)
    {
        tree->left = deleteNode(tree->left, z);
        if ((getHeight(tree->right) - getHeight(tree->left)) != 2)
        {
            return tree;
        }

        const Node* const r = tree->right;
        tree = (getHeight(r->left) > getHeight(r->right)) ? rightLeftRotation(tree) : rightRightRotation(tree);
    }
    else if (z->key > tree->key)
    {
        tree->right = deleteNode(tree->right, z);
        if ((getHeight(tree->left) - getHeight(tree->right)) != 2)
        {
            return tree;
        }

        const Node* const l = tree->left;
        tree = (getHeight(l->right) > getHeight(l->left)) ? leftRightRotation(tree) : leftLeftRotation(tree);
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
        ::delete temp;
    }

    return tree;
}

//! @brief Search the node of AVL tree by key.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - key of the node
//! @return node where the key is located
Node* search(AVLTree tree, const Type key)
{
    if (!tree || (tree->key == key))
    {
        return tree;
    }

    return search((key < tree->key) ? tree->left : tree->right, key);
}

//! @brief Insert target node into the AVL tree. Not allow inserting node with the same key.
//! @param tree - tree root, the target AVL tree has this node as the root node
//! @param key - key of the target node
//! @return root node after inserting
Node* insertion(AVLTree tree, const Type key)
{
    if (!tree)
    {
        tree = createNode(key, nullptr, nullptr);
        if (!tree)
        {
            return nullptr;
        }
    }
    else if (key < tree->key)
    {
        tree->left = insertion(tree->left, key);
        if ((getHeight(tree->left) - getHeight(tree->right)) == 2)
        {
            tree = (key < tree->left->key) ? leftLeftRotation(tree) : leftRightRotation(tree);
        }
    }
    else if (key > tree->key)
    {
        tree->right = insertion(tree->right, key);
        if ((getHeight(tree->right) - getHeight(tree->left)) == 2)
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
Node* deletion(AVLTree tree, const Type key)
{
    if (const Node* const z = search(tree, key))
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

//! @brief Destroy the the AVL tree.
//! @param tree - tree root, the target AVL tree has this node as the root node
void destruction(AVLTree tree)
{
    if (!tree)
    {
        return;
    }

    if (tree->left)
    {
        destruction(tree->left);
    }
    if (tree->right)
    {
        destruction(tree->right);
    }

    ::delete tree;
}

std::ostringstream& Output::output() noexcept
{
    return process;
}

void Output::preorderTraversal(AVLTree tree)
{
    if (tree)
    {
        output() << tree->key << " ... ";
        preorderTraversal(tree->left);
        preorderTraversal(tree->right);
    }
}

void Output::inorderTraversal(AVLTree tree)
{
    if (tree)
    {
        inorderTraversal(tree->left);
        output() << tree->key << " ... ";
        inorderTraversal(tree->right);
    }
}

void Output::postorderTraversal(AVLTree tree)
{
    if (tree)
    {
        postorderTraversal(tree->left);
        postorderTraversal(tree->right);
        output() << tree->key << " ... ";
    }
}

void Output::traverse(AVLTree tree, const Type key, const int direction)
{
    if (tree)
    {
        const int currInd = indent;
        if (direction == 0)
        {
            indent = 0;
            output() << "+ " << tree->key << " -> root\n";
        }
        else
        {
            output() << "+ " << std::string(currInd, ' ') << tree->key << " -> " << key << "'s "
                     << ((direction == 1) ? "right" : "left") << " child\n";
        }

        indent += 2;
        traverse(tree->left, tree->key, -1);
        traverse(tree->right, tree->key, 1);
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
    if (!tree)
    {
        return nullptr;
    }

    while (tree->left)
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
    if (!tree)
    {
        return nullptr;
    }

    while (tree->right)
    {
        tree = tree->right;
    }

    return tree;
}

//! @brief Create a node of the splay tree.
//! @param key - key of the node to be created
//! @param left - left child node of the node to be created
//! @param right - right child node of the node to be created
//! @return new node after creating
static Node* createNode(const Type key, Node* const left, Node* const right)
{
    auto* const node = ::new (std::nothrow) Node;
    if (!node)
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
static Node* insertNode(SplayTree tree, Node* const z)
{
    Node *x = tree, *y = nullptr;
    while (x)
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
            ::delete z;
            return tree;
        }
    }

    if (!y)
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
Node* search(SplayTree tree, const Type key)
{
    if (!tree || (key == tree->key))
    {
        return tree;
    }

    return search((key < tree->key) ? tree->left : tree->right, key);
}

//! @brief Splay target node in the splay tree. Make to be the root node.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - key of the target node
//! @return root node after splaying
Node* splaying(SplayTree tree, const Type key)
{
    if (!tree)
    {
        return nullptr;
    }

    Node n{}, *l = nullptr, *r = nullptr;
    n.left = n.right = nullptr;
    l = r = &n;
    while (key != tree->key)
    {
        if (key < tree->key)
        {
            if (tree->left && (key < tree->left->key))
            {
                Node* c = tree->left;
                tree->left = c->right;
                c->right = tree;
                tree = c;
            }

            if (!tree->left)
            {
                break;
            }
            r->left = tree;
            r = tree;
            tree = tree->left;
        }
        else if (key > tree->key)
        {
            if (tree->right && (key > tree->right->key))
            {
                Node* c = tree->right;
                tree->right = c->left;
                c->left = tree;
                tree = c;
            }

            if (!tree->right)
            {
                break;
            }
            l->right = tree;
            l = tree;
            tree = tree->right;
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
Node* insertion(SplayTree tree, const Type key)
{
    Node* const z = createNode(key, nullptr, nullptr);
    if (!z)
    {
        return tree;
    }

    tree = insertNode(tree, z);
    tree = splaying(tree, key);

    return tree;
}

//! @brief Delete target node into the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
//! @param key - key of the target node
//! @return root node after deleting
Node* deletion(SplayTree tree, const Type key)
{
    if (!tree)
    {
        return nullptr;
    }

    if (!search(tree, key))
    {
        return tree;
    }

    Node* x = nullptr;
    tree = splaying(tree, key);
    if (tree->left)
    {
        x = splaying(tree->left, key);
        x->right = tree->right;
    }
    else
    {
        x = tree->right;
    }
    ::delete tree;

    return x;
}

//! @brief Destroy the the splay tree.
//! @param tree - tree root, the target splay tree has this node as the root node
void destruction(SplayTree tree)
{
    if (!tree)
    {
        return;
    }

    if (tree->left)
    {
        destruction(tree->left);
    }
    if (tree->right)
    {
        destruction(tree->right);
    }

    ::delete tree;
}

std::ostringstream& Output::output() noexcept
{
    return process;
}

void Output::preorderTraversal(SplayTree tree)
{
    if (tree)
    {
        output() << tree->key << " ... ";
        preorderTraversal(tree->left);
        preorderTraversal(tree->right);
    }
}

void Output::inorderTraversal(SplayTree tree)
{
    if (tree)
    {
        inorderTraversal(tree->left);
        output() << tree->key << " ... ";
        inorderTraversal(tree->right);
    }
}

void Output::postorderTraversal(SplayTree tree)
{
    if (tree)
    {
        postorderTraversal(tree->left);
        postorderTraversal(tree->right);
        output() << tree->key << " ... ";
    }
}

void Output::traverse(SplayTree tree, const Type key, const int direction)
{
    if (tree)
    {
        const int currInd = indent;
        if (direction == 0)
        {
            indent = 0;
            output() << "+ " << tree->key << " -> root\n";
        }
        else
        {
            output() << "+ " << std::string(currInd, ' ') << tree->key << " -> " << key << "'s "
                     << ((direction == 1) ? "right" : "left") << " child\n";
        }

        indent += 2;
        traverse(tree->left, tree->key, -1);
        traverse(tree->right, tree->key, 1);
        indent = currInd;
    }
}
} // namespace splay
// NOLINTEND(cppcoreguidelines-owning-memory)
} // namespace date_structure::tree
