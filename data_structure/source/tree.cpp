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
//! @param root - root of the subtree
//! @return node where the minimum key is located
static Node* getMinimum(Node* const root)
{
    if (!root)
    {
        return nullptr;
    }

    Node* node = root;
    while (node->left)
    {
        node = node->left;
    }

    return node;
}

//! @brief Get the node where the maximum key is located in the BS subtree.
//! @param root - root of the subtree
//! @return node where the maximum key is located
static Node* getMaximum(Node* const root)
{
    if (!root)
    {
        return nullptr;
    }

    Node* node = root;
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
    Node* const newNode = ::new (std::nothrow) Node;
    if (!newNode)
    {
        return nullptr;
    }

    newNode->left = left;
    newNode->right = right;
    newNode->parent = parent;
    newNode->key = const_cast<void*>(key);

    return newNode;
}

//! @brief Insert the node into the BS subtree. Allow inserting node with the same key.
//! @param root - root of the subtree
//! @param node - target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insertNode(Node* root, Node* node, const Compare cmp)
{
    if (!node || !cmp)
    {
        ::delete node;
        node = nullptr;
        return root;
    }

    Node *x = root, *y = nullptr;
    while (x)
    {
        y = x;
        x = (cmp(node->key, x->key) < 0) ? x->left : x->right;
    }

    node->parent = y;
    if (!y)
    {
        root = node;
    }
    else if (cmp(node->key, y->key) < 0)
    {
        y->left = node;
    }
    else
    {
        y->right = node;
    }

    return root;
}

//! @brief Delete the node from the BS subtree.
//! @param root - root of the subtree
//! @param node - target node
//! @return root node after deleting
static Node* deleteNode(Node* root, Node* const node)
{
    if (!node)
    {
        return root;
    }

    Node* y = (!node->left || !node->right) ? node : getSuccessor(node);
    if (!y)
    {
        return root;
    }
    Node* const x = y->left ? y->left : y->right;
    if (x)
    {
        x->parent = y->parent;
    }

    if (!y->parent)
    {
        root = x;
    }
    else if (y->parent->left == y)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }

    if (y != node)
    {
        node->key = y->key;
    }

    const bool yIsRoot = (root == y);
    ::delete y;
    y = nullptr;

    return !yIsRoot ? root : nullptr;
}

//! @brief Destroy the the BS subtree.
//! @param root - root of the subtree
static void destroy(const Node* root)
{
    if (!root)
    {
        return;
    }

    if (root->left)
    {
        destroy(root->left);
    }
    if (root->right)
    {
        destroy(root->right);
    }

    ::delete root;
    root = nullptr;
}

//! @brief Search the node of BS subtree by key.
//! @param root - root of the subtree
//! @param key - key of the node
//! @param cmp - compare function to compare keys
//! @return node where the key is located
static Node* search(Node* const root, const void* const key, const Compare cmp)
{
    if (!root || !key || !cmp)
    {
        return nullptr;
    }

    return (cmp(root->key, key) == 0) ? root : search((cmp(key, root->key) < 0) ? root->left : root->right, key, cmp);
}

//! @brief Get the node where the minimum key is located in the BS tree.
//! @param tree - BS tree
//! @return node where the minimum key is located
Node* getMinimum(const BSTree* const tree)
{
    return tree ? getMinimum(tree->root) : nullptr;
}

//! @brief Get the node where the maximum key is located in the BS tree.
//! @param tree - BS tree
//! @return node where the maximum key is located
Node* getMaximum(const BSTree* const tree)
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

//! @brief Create the the BS tree.
//! @param cmp - compare function to compare keys
//! @return new BS tree
BSTree* create(const Compare cmp)
{
    if (!cmp)
    {
        return nullptr;
    }

    auto* const tree = ::new (std::nothrow) BSTree;
    tree->root = nullptr;
    tree->compare = cmp;

    return tree;
}

//! @brief Destroy the the BS tree.
//! @param tree - BS tree
void destroy(const BSTree* tree)
{
    if (!tree)
    {
        return;
    }

    destroy(tree->root);
    ::delete tree;
    tree = nullptr;
}

//! @brief Search the node of BS tree by key.
//! @param tree - BS tree
//! @param key - key of the node
//! @return node where the key is located
Node* search(const BSTree* const tree, const void* const key)
{
    return tree ? search(tree->root, key, tree->compare) : nullptr;
}

//! @brief Insert the node into the BS tree. Allow inserting node with the same key.
//! @param tree - BS tree
//! @param key - key of the target node
void insert(BSTree* const tree, const void* const key)
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

//! @brief Delete the node from the BS tree.
//! @param tree - BS tree
//! @param key - key of the target node
void remove(BSTree* const tree, const void* const key)
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
} // namespace bs

namespace avl
{
//! @brief Get the height of the AVL subtree.
//! @param root - root of the subtree
//! @return height of the AVL subtree
static int getHeight(const Node* const root)
{
    return root ? root->height : 0;
}

//! @brief Get the node where the minimum key is located in the AVL subtree.
//! @param root - root of the subtree
//! @return node where the minimum key is located
static Node* getMinimum(Node* const root)
{
    if (!root)
    {
        return nullptr;
    }

    Node* node = root;
    while (node->left)
    {
        node = node->left;
    }

    return node;
}

//! @brief Get the node where the maximum key is located in the AVL subtree.
//! @param root - root of the subtree
//! @return node where the maximum key is located
static Node* getMaximum(Node* const root)
{
    if (!root)
    {
        return nullptr;
    }

    Node* node = root;
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
    Node* const newNode = ::new (std::nothrow) Node;
    if (!newNode)
    {
        return nullptr;
    }

    newNode->left = left;
    newNode->right = right;
    newNode->key = const_cast<void*>(key);
    newNode->height = 0;

    return newNode;
}

//! @brief Delete the node from the AVL subtree.
//! @param root - root of the subtree
//! @param node - target node
//! @param cmp - compare function to compare keys
//! @return root node after deleting
static Node* deleteNode(Node* root, const Node* const node, const Compare cmp)
{
    if (!root || !node || !cmp)
    {
        return root;
    }

    if (cmp(node->key, root->key) < 0)
    {
        root->left = deleteNode(root->left, node, cmp);
    }
    else if (cmp(node->key, root->key) > 0)
    {
        root->right = deleteNode(root->right, node, cmp);
    }
    else if (root->left && root->right)
    {
        if (getHeight(root->left) > getHeight(root->right))
        {
            const Node* const max = getMaximum(root->left);
            root->key = max->key;
            root->left = deleteNode(root->left, max, cmp);
        }
        else
        {
            const Node* const min = getMinimum(root->right);
            root->key = min->key;
            root->right = deleteNode(root->right, min, cmp);
        }
    }
    else
    {
        const Node* temp = root;
        root = root->left ? root->left : root->right;
        ::delete temp;
        temp = nullptr;
    }

    if (!root)
    {
        return nullptr;
    }
    root->height = std::max(getHeight(root->left), getHeight(root->right)) + 1;

    const int balance = getHeight(root->left) - getHeight(root->right);
    if (balance > 1)
    {
        const Node* const l = root->left;
        root = (getHeight(l->right) > getHeight(l->left)) ? leftRightRotation(root) : leftLeftRotation(root);
    }
    else if (balance < -1)
    {
        const Node* const r = root->right;
        root = (getHeight(r->left) > getHeight(r->right)) ? rightLeftRotation(root) : rightRightRotation(root);
    }

    return root;
}

//! @brief Destroy the the AVL subtree.
//! @param root - root of the subtree
static void destroy(const Node* root)
{
    if (!root)
    {
        return;
    }

    if (root->left)
    {
        destroy(root->left);
    }
    if (root->right)
    {
        destroy(root->right);
    }

    ::delete root;
    root = nullptr;
}

//! @brief Search the node of AVL subtree by key.
//! @param root - root of the subtree
//! @param key - key of the node
//! @param cmp - compare function to compare keys
//! @return node where the key is located
static Node* search(Node* const root, const void* const key, const Compare cmp)
{
    if (!root || !key || !cmp)
    {
        return nullptr;
    }

    return (cmp(root->key, key) == 0) ? root : search((cmp(key, root->key) < 0) ? root->left : root->right, key, cmp);
}

//! @brief Insert the node into the AVL subtree. Not allow inserting node with the same key.
//! @param root - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insert(Node* root, const void* const key, const Compare cmp)
{
    if (!key || !cmp)
    {
        return root;
    }

    if (!root)
    {
        root = createNode(key, nullptr, nullptr);
        if (!root)
        {
            return nullptr;
        }
    }
    else if (cmp(key, root->key) < 0)
    {
        root->left = insert(root->left, key, cmp);
        if ((getHeight(root->left) - getHeight(root->right)) > 1)
        {
            root = (cmp(key, root->left->key) < 0) ? leftLeftRotation(root) : leftRightRotation(root);
        }
    }
    else if (cmp(key, root->key) > 0)
    {
        root->right = insert(root->right, key, cmp);
        if ((getHeight(root->left) - getHeight(root->right)) < -1)
        {
            root = (cmp(key, root->right->key) > 0) ? rightRightRotation(root) : rightLeftRotation(root);
        }
    }

    root->height = std::max(getHeight(root->left), getHeight(root->right)) + 1;

    return root;
}

//! @brief Delete the node from the AVL subtree.
//! @param root - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after deleting
static Node* remove(Node* root, const void* const key, const Compare cmp)
{
    if (const Node* const z = search(root, key, cmp))
    {
        root = deleteNode(root, z, cmp);
    }

    return root;
}

//! @brief Get the height of the AVL tree.
//! @param tree - AVL tree
//! @return height of the AVL tree
int getHeight(const AVLTree* const tree)
{
    return (tree && tree->root) ? tree->root->height : 0;
}

//! @brief Get the node where the minimum key is located in the AVL tree.
//! @param tree - AVL tree
//! @return node where the minimum key is located
Node* getMinimum(const AVLTree* const tree)
{
    return tree ? getMinimum(tree->root) : nullptr;
}

//! @brief Get the node where the maximum key is located in the AVL tree.
//! @param tree - AVL tree
//! @return node where the maximum key is located
Node* getMaximum(const AVLTree* const tree)
{
    return tree ? getMaximum(tree->root) : nullptr;
}

//! @brief Create the the AVL tree.
//! @param cmp - compare function to compare keys
//! @return new AVL tree
AVLTree* create(const Compare cmp)
{
    if (!cmp)
    {
        return nullptr;
    }

    auto* const tree = ::new (std::nothrow) AVLTree;
    tree->root = nullptr;
    tree->compare = cmp;

    return tree;
}

//! @brief Destroy the the AVL tree.
//! @param tree - AVL tree
void destroy(const AVLTree* tree)
{
    if (!tree)
    {
        return;
    }

    destroy(tree->root);
    ::delete tree;
    tree = nullptr;
}

//! @brief Search the node of AVL tree by key.
//! @param tree - AVL tree
//! @param key - key of the node
//! @return node where the key is located
Node* search(const AVLTree* const tree, const void* const key)
{
    return tree ? search(tree->root, key, tree->compare) : nullptr;
}

//! @brief Insert the node into the AVL tree. Not allow inserting node with the same key.
//! @param tree - AVL tree
//! @param key - key of the target node
void insert(AVLTree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = insert(tree->root, key, tree->compare);
}

//! @brief Delete the node from the AVL tree.
//! @param tree - AVL tree
//! @param key - key of the target node
void remove(AVLTree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = remove(tree->root, key, tree->compare);
}
} // namespace avl

namespace splay
{
//! @brief Get the node where the minimum key is located in the splay subtree.
//! @param root - root of the subtree
//! @return node where the minimum key is located
static Node* getMinimum(Node* const root)
{
    if (!root)
    {
        return nullptr;
    }

    Node* node = root;
    while (node->left)
    {
        node = node->left;
    }

    return node;
}

//! @brief Get the node where the maximum key is located in the splay subtree.
//! @param root - root of the subtree
//! @return node where the maximum key is located
static Node* getMaximum(Node* const root)
{
    if (!root)
    {
        return nullptr;
    }

    Node* node = root;
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
    Node* const newNode = ::new (std::nothrow) Node;
    if (!newNode)
    {
        return nullptr;
    }

    newNode->left = left;
    newNode->right = right;
    newNode->key = const_cast<void*>(key);

    return newNode;
}

//! @brief Insert the node into the splay subtree. Not splay. Not allow inserting node with the same key.
//! @param root - root of the subtree
//! @param node - target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insertNode(Node* root, Node* node, const Compare cmp)
{
    if (!node || !cmp)
    {
        ::delete node;
        node = nullptr;
        return root;
    }

    Node *x = root, *y = nullptr;
    while (x)
    {
        y = x;
        if (cmp(node->key, x->key) < 0)
        {
            x = x->left;
        }
        else if (cmp(node->key, x->key) > 0)
        {
            x = x->right;
        }
        else
        {
            ::delete node;
            node = nullptr;
            return root;
        }
    }

    if (!y)
    {
        root = node;
    }
    else if (cmp(node->key, y->key) < 0)
    {
        y->left = node;
    }
    else
    {
        y->right = node;
    }

    return root;
}

//! @brief Destroy the the splay subtree.
//! @param root - root of the subtree
static void destroy(const Node* root)
{
    if (!root)
    {
        return;
    }

    if (root->left)
    {
        destroy(root->left);
    }
    if (root->right)
    {
        destroy(root->right);
    }

    ::delete root;
    root = nullptr;
}

//! @brief Search the node of splay subtree by key.
//! @param root - root of the subtree
//! @param key - key of the node
//! @param cmp - compare function to compare keys
//! @return node where the key is located
static Node* search(Node* const root, const void* const key, const Compare cmp)
{
    if (!root || !key || !cmp)
    {
        return nullptr;
    }

    return (cmp(root->key, key) == 0) ? root : search((cmp(key, root->key) < 0) ? root->left : root->right, key, cmp);
}

//! @brief Splay the node in the splay subtree. Make to be the root node.
//! @param root - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after splaying
static Node* splay(Node* root, const void* const key, const Compare cmp)
{
    if (!root || !key || !cmp)
    {
        return root;
    }

    Node n{}, *l = nullptr, *r = nullptr;
    n.left = n.right = nullptr;
    l = r = &n;
    while (cmp(key, root->key) != 0)
    {
        if (cmp(key, root->key) < 0)
        {
            if (root->left && (cmp(key, root->left->key) < 0))
            {
                Node* c = root->left;
                root->left = c->right;
                c->right = root;
                root = c;
            }

            if (!root->left)
            {
                break;
            }
            r->left = root;
            r = root;
            root = root->left;
        }
        else if (cmp(key, root->key) > 0)
        {
            if (root->right && (cmp(key, root->right->key) > 0))
            {
                Node* c = root->right;
                root->right = c->left;
                c->left = root;
                root = c;
            }

            if (!root->right)
            {
                break;
            }
            l->right = root;
            l = root;
            root = root->right;
        }
    }

    l->right = root->left;
    r->left = root->right;
    root->left = n.right;
    root->right = n.left;

    return root;
}

//! @brief Insert the node into the splay subtree. Not allow inserting node with the same key.
//! @param root - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after inserting
static Node* insert(Node* root, const void* const key, const Compare cmp)
{
    Node* const z = createNode(key, nullptr, nullptr);
    if (!z)
    {
        return root;
    }

    root = insertNode(root, z, cmp);
    root = splay(root, key, cmp);

    return root;
}

//! @brief Delete the node from the splay subtree.
//! @param root - root of the subtree
//! @param key - key of the target node
//! @param cmp - compare function to compare keys
//! @return root node after deleting
static Node* remove(Node* root, const void* const key, const Compare cmp)
{
    if (!root || !search(root, key, cmp))
    {
        return root;
    }

    Node* x = nullptr;
    root = splay(root, key, cmp);
    if (root->left)
    {
        x = splay(root->left, key, cmp);
        x->right = root->right;
    }
    else
    {
        x = root->right;
    }

    ::delete root;
    root = nullptr;

    return x;
}

//! @brief Get the node where the minimum key is located in the splay tree.
//! @param tree - splay tree
//! @return node where the minimum key is located
Node* getMinimum(const SplayTree* const tree)
{
    return tree ? getMinimum(tree->root) : nullptr;
}

//! @brief Get the node where the maximum key is located in the splay tree.
//! @param tree - splay tree
//! @return node where the maximum key is located
Node* getMaximum(const SplayTree* const tree)
{
    return tree ? getMaximum(tree->root) : nullptr;
}

//! @brief Create the the splay tree.
//! @param cmp - compare function to compare keys
//! @return new splay tree
SplayTree* create(const Compare cmp)
{
    if (!cmp)
    {
        return nullptr;
    }

    auto* const tree = ::new (std::nothrow) SplayTree;
    tree->root = nullptr;
    tree->compare = cmp;

    return tree;
}

//! @brief Destroy the the splay tree.
//! @param tree - splay tree
void destroy(const SplayTree* tree)
{
    if (!tree)
    {
        return;
    }

    destroy(tree->root);
    ::delete tree;
    tree = nullptr;
}

//! @brief Search the node of splay tree by key.
//! @param tree - splay tree
//! @param key - key of the node
//! @return node where the key is located
Node* search(const SplayTree* const tree, const void* const key)
{
    return tree ? search(tree->root, key, tree->compare) : nullptr;
}

//! @brief Splay the node in the splay tree. Make to be the root node.
//! @param tree - splay tree
//! @param key - key of the target node
void splay(SplayTree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = splay(tree->root, key, tree->compare);
}

//! @brief Insert the node into the splay tree. Not allow inserting node with the same key.
//! @param tree - splay tree
//! @param key - key of the target node
void insert(SplayTree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = insert(tree->root, key, tree->compare);
}

//! @brief Delete the node from the splay tree.
//! @param tree - splay tree
//! @param key - key of the target node
void remove(SplayTree* const tree, const void* const key)
{
    if (!tree)
    {
        return;
    }

    tree->root = remove(tree->root, key, tree->compare);
}
} // namespace splay
// NOLINTEND(cppcoreguidelines-owning-memory, cppcoreguidelines-pro-type-const-cast)
} // namespace date_structure::tree
