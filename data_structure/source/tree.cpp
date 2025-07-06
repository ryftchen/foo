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

// NOLINTBEGIN(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
namespace bs
{
//! @brief Get the node where the minimum key is located in the BS subtree.
//! @param node - root of the subtree
//! @return node where the minimum key is located
static Node* getMinimum(Node* node)
{
    if (!node)
    {
        return nullptr;
    }

    while (node->left)
    {
        node = node->left;
    }

    return node;
}

//! @brief Get the node where the maximum key is located in the BS subtree.
//! @param node - root of the subtree
//! @return node where the maximum key is located
static Node* getMaximum(Node* node)
{
    if (!node)
    {
        return nullptr;
    }

    while (node->right)
    {
        node = node->right;
    }

    return node;
}

//! @brief Create a node of the BS tree.
//! @param key - key of the node to be created
//! @param parent - parent node of the node to be created
//! @param left - left child node of the node to be created
//! @param right - right child node of the node to be created
//! @return new node after creating
static Node* createNode(const void* const key, Node* const parent, Node* const left, Node* const right)
{
    Node* const node = ::new (std::nothrow) Node;
    if (!node)
    {
        return nullptr;
    }

    node->key = const_cast<void*>(key);
    node->left = left;
    node->right = right;
    node->parent = parent;

    return node;
}

//! @brief Insert target node into the BS subtree. Allow inserting node with the same key.
//! @param node - root of the subtree
//! @param z - target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insertNode(Node* node, Node* z, const Compare cmp)
{
    if (!z || !cmp)
    {
        ::delete z;
        z = nullptr;
        return node;
    }

    Node *x = node, *y = nullptr;
    while (x)
    {
        y = x;
        x = (cmp(z->key, x->key) < 0) ? x->left : x->right;
    }

    z->parent = y;
    if (!y)
    {
        node = z;
    }
    else if (cmp(z->key, y->key) < 0)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    return node;
}

//! @brief Delete target node from the BS subtree.
//! @param node - root of the subtree
//! @param z - target node
//! @return root node after deleting
static Node* deleteNode(Node* node, Node* const z)
{
    if (!z)
    {
        return node;
    }

    Node* y = (!z->left || !z->right) ? z : getSuccessor(z);
    if (!y)
    {
        return node;
    }
    Node* const x = y->left ? y->left : y->right;
    if (x)
    {
        x->parent = y->parent;
    }

    if (!y->parent)
    {
        node = x;
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

    const bool yIsRoot = (node == y);
    ::delete y;
    y = nullptr;

    return !yIsRoot ? node : nullptr;
}

//! @brief Search the node of BS subtree by key.
//! @param node - root of the subtree
//! @param key - key of the node
//! @param cmp - compare function to compare keys
//! @return node where the key is located
static Node* search(Node* const node, const void* const key, const Compare cmp)
{
    if (!node || !key || !cmp)
    {
        return nullptr;
    }

    return (cmp(node->key, key) == 0) ? node : search((cmp(key, node->key) < 0) ? node->left : node->right, key, cmp);
}

//! @brief Destroy the the BS subtree.
//! @param node - root of the subtree
static void destruction(const Node* node)
{
    if (!node)
    {
        return;
    }

    if (node->left)
    {
        destruction(node->left);
    }
    if (node->right)
    {
        destruction(node->right);
    }

    ::delete node;
    node = nullptr;
}

//! @brief Get the node where the minimum key is located in the BS tree.
//! @param tree - BS tree
//! @return node where the minimum key is located
Node* getMinimum(const Tree* const tree)
{
    return tree ? getMinimum(tree->root) : nullptr;
}

//! @brief Get the node where the maximum key is located in the BS tree.
//! @param tree - BS tree
//! @return node where the maximum key is located
Node* getMaximum(const Tree* const tree)
{
    return tree ? getMaximum(tree->root) : nullptr;
}

//! @brief Get the predecessor node of the current node.
//!        The precursor of a node is the node that has the maximum key in that node's left subtree.
//! @param x - current node
//! @return predecessor node
Node* getPredecessor(const Node* x)
{
    if (!x)
    {
        return nullptr;
    }

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
    if (!x)
    {
        return nullptr;
    }

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

//! @brief Search the node of BS tree by key.
//! @param tree - BS tree
//! @param key - key of the node
//! @return node where the key is located
Node* search(const Tree* const tree, const void* const key)
{
    return tree ? search(tree->root, key, tree->compare) : nullptr;
}

//! @brief Insert target node into the BS tree. Allow inserting node with the same key.
//! @param tree - BS tree
//! @param key - key of the target node
void insertion(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    if (Node* const z = createNode(key, nullptr, nullptr, nullptr))
    {
        tree->root = insertNode(tree->root, z, tree->compare);
    }
}

//! @brief Delete target node from the BS tree.
//! @param tree - BS tree
//! @param key - key of the target node
void deletion(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    if (Node* const z = search(tree, key))
    {
        tree->root = deleteNode(tree->root, z);
    }
}

//! @brief Destroy the the BS tree.
//! @param tree - BS tree
void destruction(const Tree* const tree)
{
    if (!tree)
    {
        return;
    }

    destruction(tree->root);
}
} // namespace bs

namespace avl
{
//! @brief Get the height of the AVL subtree.
//! @param node - root of the subtree
//! @return height of the AVL subtree
static int getHeight(const Node* const node)
{
    return node ? node->height : 0;
}

//! @brief Get the node where the minimum key is located in the AVL subtree.
//! @param node - root of the subtree
//! @return node where the minimum key is located
static Node* getMinimum(Node* node)
{
    if (!node)
    {
        return nullptr;
    }

    while (node->left)
    {
        node = node->left;
    }

    return node;
}

//! @brief Get the node where the maximum key is located in the AVL subtree.
//! @param node - root of the subtree
//! @return node where the maximum key is located
static Node* getMaximum(Node* node)
{
    if (!node)
    {
        return nullptr;
    }

    while (node->right)
    {
        node = node->right;
    }

    return node;
}

//! @brief LL rotation. A single left rotation.
//! @param k2 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* leftLeftRotation(Node* const k2)
{
    if (!k2 || !k2->left)
    {
        return nullptr;
    }

    Node* const k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = std::max(getHeight(k2->left), getHeight(k2->right)) + 1;
    k1->height = std::max(getHeight(k1->left), k2->height) + 1;

    return k1;
}

//! @brief RR rotation. A single right rotation.
//! @param k1 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* rightRightRotation(Node* const k1)
{
    if (!k1 || !k1->right)
    {
        return nullptr;
    }

    Node* const k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = std::max(getHeight(k1->left), getHeight(k1->right)) + 1;
    k2->height = std::max(getHeight(k2->right), k1->height) + 1;

    return k2;
}

//! @brief LR rotation. A double left rotation.
//! @param k3 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* leftRightRotation(Node* const k3)
{
    if (!k3)
    {
        return nullptr;
    }

    k3->left = rightRightRotation(k3->left);

    return leftLeftRotation(k3);
}

//! @brief RL rotation. A double right rotation.
//! @param k1 - root node of the unbalanced AVL tree
//! @return root node after rotation
static Node* rightLeftRotation(Node* const k1)
{
    if (!k1)
    {
        return nullptr;
    }

    k1->right = leftLeftRotation(k1->right);

    return rightRightRotation(k1);
}

//! @brief Create a node of the AVL tree.
//! @param key - key of the node to be created
//! @param left - left child node of the node to be created
//! @param right - right child node of the node to be created
//! @return new node after creating
static Node* createNode(const void* const key, Node* const left, Node* const right)
{
    Node* const node = ::new (std::nothrow) Node;
    if (!node)
    {
        return nullptr;
    }

    node->key = const_cast<void*>(key);
    node->height = 0;
    node->left = left;
    node->right = right;

    return node;
}

//! @brief Delete target node from the AVL subtree.
//! @param node - root of the subtree
//! @param z - target node
//! @param cmp - compare function to compare keys
//! @return root node after deleting
static Node* deleteNode(Node* node, const Node* const z, const Compare cmp)
{
    if (!node || !z || !cmp)
    {
        return node;
    }

    if (cmp(z->key, node->key) < 0)
    {
        node->left = deleteNode(node->left, z, cmp);
        if ((getHeight(node->right) - getHeight(node->left)) != 2)
        {
            return node;
        }

        const Node* const r = node->right;
        node = (getHeight(r->left) > getHeight(r->right)) ? rightLeftRotation(node) : rightRightRotation(node);
    }
    else if (cmp(z->key, node->key) > 0)
    {
        node->right = deleteNode(node->right, z, cmp);
        if ((getHeight(node->left) - getHeight(node->right)) != 2)
        {
            return node;
        }

        const Node* const l = node->left;
        node = (getHeight(l->right) > getHeight(l->left)) ? leftRightRotation(node) : leftLeftRotation(node);
    }
    else if (node->left && node->right)
    {
        if (getHeight(node->left) > getHeight(node->right))
        {
            const Node* const max = getMaximum(node->left);
            node->key = max->key;
            node->left = deleteNode(node->left, max, cmp);
        }
        else
        {
            const Node* const min = getMaximum(node->right);
            node->key = min->key;
            node->right = deleteNode(node->right, min, cmp);
        }
    }
    else
    {
        const Node* temp = node;
        node = node->left ? node->left : node->right;
        ::delete temp;
        temp = nullptr;
    }

    return node;
}

//! @brief Search the node of AVL subtree by key.
//! @param node - root of the subtree
//! @param key - key of the node
//! @param cmp - compare function to compare keys
//! @return node where the key is located
static Node* search(Node* const node, const void* const key, const Compare cmp)
{
    if (!node || !key || !cmp)
    {
        return nullptr;
    }

    return (cmp(node->key, key) == 0) ? node : search((cmp(key, node->key) < 0) ? node->left : node->right, key, cmp);
}

//! @brief Insert target node into the AVL subtree. Not allow inserting node with the same key.
//! @param node - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insertion(Node* node, const void* const key, const Compare cmp)
{
    if (!key || !cmp)
    {
        return node;
    }

    if (!node)
    {
        node = createNode(key, nullptr, nullptr);
        if (!node)
        {
            return nullptr;
        }
    }
    else if (cmp(key, node->key) < 0)
    {
        node->left = insertion(node->left, key, cmp);
        if ((getHeight(node->left) - getHeight(node->right)) == 2)
        {
            node = (cmp(key, node->left->key) < 0) ? leftLeftRotation(node) : leftRightRotation(node);
        }
    }
    else if (cmp(key, node->key) > 0)
    {
        node->right = insertion(node->right, key, cmp);
        if ((getHeight(node->right) - getHeight(node->left)) == 2)
        {
            node = (cmp(key, node->right->key) > 0) ? rightRightRotation(node) : rightLeftRotation(node);
        }
    }

    node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;

    return node;
}

//! @brief Delete target node from the AVL subtree.
//! @param node - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after deleting
static Node* deletion(Node* node, const void* const key, const Compare cmp)
{
    if (const Node* const z = search(node, key, cmp))
    {
        node = deleteNode(node, z, cmp);
    }

    return node;
}

//! @brief Destroy the the AVL subtree.
//! @param node - root of the subtree
static void destruction(const Node* node)
{
    if (!node)
    {
        return;
    }

    if (node->left)
    {
        destruction(node->left);
    }
    if (node->right)
    {
        destruction(node->right);
    }

    ::delete node;
    node = nullptr;
}

//! @brief Get the height of the AVL tree.
//! @param tree - AVL tree
//! @return height of the AVL tree
int getHeight(const Tree* const tree)
{
    return (tree && tree->root) ? tree->root->height : 0;
}

//! @brief Get the node where the minimum key is located in the AVL tree.
//! @param tree - AVL tree
//! @return node where the minimum key is located
Node* getMinimum(const Tree* const tree)
{
    return tree ? getMinimum(tree->root) : nullptr;
}

//! @brief Get the node where the maximum key is located in the AVL tree.
//! @param tree - AVL tree
//! @return node where the maximum key is located
Node* getMaximum(const Tree* const tree)
{
    return tree ? getMaximum(tree->root) : nullptr;
}

//! @brief Search the node of AVL tree by key.
//! @param tree - AVL tree
//! @param key - key of the node
//! @return node where the key is located
Node* search(const Tree* const tree, const void* const key)
{
    return tree ? search(tree->root, key, tree->compare) : nullptr;
}

//! @brief Insert target node into the AVL tree. Not allow inserting node with the same key.
//! @param tree - AVL tree
//! @param key - key of the target node
void insertion(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = insertion(tree->root, key, tree->compare);
}

//! @brief Delete target node from the AVL tree.
//! @param tree - AVL tree
//! @param key - key of the target node
void deletion(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = deletion(tree->root, key, tree->compare);
}

//! @brief Destroy the the AVL tree.
//! @param tree - AVL tree
void destruction(const Tree* const tree)
{
    if (!tree)
    {
        return;
    }

    destruction(tree->root);
}
} // namespace avl

namespace splay
{
//! @brief Get the node where the minimum key is located in the splay subtree.
//! @param node - root of the subtree
//! @return node where the minimum key is located
static Node* getMinimum(Node* node)
{
    if (!node)
    {
        return nullptr;
    }

    while (node->left)
    {
        node = node->left;
    }

    return node;
}

//! @brief Get the node where the maximum key is located in the splay subtree.
//! @param node - root of the subtree
//! @return node where the maximum key is located
static Node* getMaximum(Node* node)
{
    if (!node)
    {
        return nullptr;
    }

    while (node->right)
    {
        node = node->right;
    }

    return node;
}

//! @brief Create a node of the splay tree.
//! @param key - key of the node to be created
//! @param left - left child node of the node to be created
//! @param right - right child node of the node to be created
//! @return new node after creating
static Node* createNode(const void* const key, Node* const left, Node* const right)
{
    Node* const node = ::new (std::nothrow) Node;
    if (!node)
    {
        return nullptr;
    }

    node->key = const_cast<void*>(key);
    node->left = left;
    node->right = right;

    return node;
}

//! @brief Insert target node into the splay subtree. Not splay. Not allow inserting node with the same key.
//! @param node - root of the subtree
//! @param z - target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insertNode(Node* node, Node* z, const Compare cmp)
{
    if (!z || !cmp)
    {
        ::delete z;
        z = nullptr;
        return node;
    }

    Node *x = node, *y = nullptr;
    while (x)
    {
        y = x;
        if (cmp(z->key, x->key) < 0)
        {
            x = x->left;
        }
        else if (cmp(z->key, x->key) > 0)
        {
            x = x->right;
        }
        else
        {
            ::delete z;
            z = nullptr;
            return node;
        }
    }

    if (!y)
    {
        node = z;
    }
    else if (cmp(z->key, y->key) < 0)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    return node;
}

//! @brief Search the node of splay subtree by key.
//! @param node - root of the subtree
//! @param key - key of the node
//! @param cmp - compare function to compare keys
//! @return node where the key is located
static Node* search(Node* const node, const void* const key, const Compare cmp)
{
    if (!node || !key || !cmp)
    {
        return nullptr;
    }

    return (cmp(node->key, key) == 0) ? node : search((cmp(key, node->key) < 0) ? node->left : node->right, key, cmp);
}

//! @brief Splay target node in the splay subtree. Make to be the root node.
//! @param node - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after splaying
static Node* splaying(Node* node, const void* const key, const Compare cmp)
{
    if (!node || !key || !cmp)
    {
        return node;
    }

    Node n{}, *l = nullptr, *r = nullptr;
    n.left = n.right = nullptr;
    l = r = &n;
    while (cmp(key, node->key) != 0)
    {
        if (cmp(key, node->key) < 0)
        {
            if (node->left && (cmp(key, node->left->key) < 0))
            {
                Node* c = node->left;
                node->left = c->right;
                c->right = node;
                node = c;
            }

            if (!node->left)
            {
                break;
            }
            r->left = node;
            r = node;
            node = node->left;
        }
        else if (cmp(key, node->key) > 0)
        {
            if (node->right && (cmp(key, node->right->key) > 0))
            {
                Node* c = node->right;
                node->right = c->left;
                c->left = node;
                node = c;
            }

            if (!node->right)
            {
                break;
            }
            l->right = node;
            l = node;
            node = node->right;
        }
    }

    l->right = node->left;
    r->left = node->right;
    node->left = n.right;
    node->right = n.left;

    return node;
}

//! @brief Insert target node into the splay subtree. Not allow inserting node with the same key.
//! @param node - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insertion(Node* node, const void* const key, const Compare cmp)
{
    Node* const z = createNode(key, nullptr, nullptr);
    if (!z)
    {
        return node;
    }

    node = insertNode(node, z, cmp);
    node = splaying(node, key, cmp);

    return node;
}

//! @brief Delete target node from the splay subtree.
//! @param node - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after deleting
static Node* deletion(Node* node, const void* const key, const Compare cmp)
{
    if (!node || !search(node, key, cmp))
    {
        return node;
    }

    Node* x = nullptr;
    node = splaying(node, key, cmp);
    if (node->left)
    {
        x = splaying(node->left, key, cmp);
        x->right = node->right;
    }
    else
    {
        x = node->right;
    }

    ::delete node;
    node = nullptr;

    return x;
}

//! @brief Destroy the the splay subtree.
//! @param node - root of the subtree
static void destruction(const Node* node)
{
    if (!node)
    {
        return;
    }

    if (node->left)
    {
        destruction(node->left);
    }
    if (node->right)
    {
        destruction(node->right);
    }

    ::delete node;
    node = nullptr;
}

//! @brief Get the node where the minimum key is located in the splay tree.
//! @param tree - splay tree
//! @return node where the minimum key is located
Node* getMinimum(const Tree* const tree)
{
    return tree ? getMinimum(tree->root) : nullptr;
}

//! @brief Get the node where the maximum key is located in the splay tree.
//! @param tree - splay tree
//! @return node where the maximum key is located
Node* getMaximum(const Tree* const tree)
{
    return tree ? getMaximum(tree->root) : nullptr;
}

//! @brief Search the node of splay tree by key.
//! @param tree - splay tree
//! @param key - key of the node
//! @return node where the key is located
Node* search(const Tree* const tree, const void* const key)
{
    return tree ? search(tree->root, key, tree->compare) : nullptr;
}

//! @brief Splay target node in the splay tree. Make to be the root node.
//! @param tree - splay tree
//! @param key - key of the target node
void splaying(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = splaying(tree->root, key, tree->compare);
}

//! @brief Insert target node into the splay tree. Not allow inserting node with the same key.
//! @param tree - splay tree
//! @param key - key of the target node
void insertion(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = insertion(tree->root, key, tree->compare);
}

//! @brief Delete target node from the splay tree.
//! @param tree - splay tree
//! @param key - key of the target node
void deletion(Tree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = deletion(tree->root, key, tree->compare);
}

//! @brief Destroy the the splay tree.
//! @param tree - splay tree
void destruction(const Tree* const tree)
{
    if (!tree)
    {
        return;
    }

    destruction(tree->root);
}
} // namespace splay
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::tree
