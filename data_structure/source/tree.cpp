#include "tree.hpp"
#include <array>
#include "utility/include/common.hpp"

#define TREE_RESULT "\r\n*%-19s instance:\r\n%s"

namespace ds_tree
{
TreeStructure::TreeStructure()
{
    std::cout << "\r\nTree structure:" << std::endl;
}

// Binary Search
namespace bs
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

typedef int Type;
typedef struct BSTreeNode
{
    Type key;
    struct BSTreeNode* left;
    struct BSTreeNode* right;
    struct BSTreeNode* parent;
} Node, *BSTree;

static void preorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ";
        preorderBSTree(tree->left);
        preorderBSTree(tree->right);
    }
}

static void inorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        inorderBSTree(tree->left);
        output() << tree->key << " ";
        inorderBSTree(tree->right);
    }
}

static void postorderBSTree(BSTree tree)
{
    if (nullptr != tree)
    {
        postorderBSTree(tree->left);
        postorderBSTree(tree->right);
        output() << tree->key << " ";
    }
}

static Node* bsTreeSearch(BSTree x, const Type key)
{
    if ((nullptr == x) || (x->key == key))
    {
        return x;
    }

    if (key < x->key)
    {
        return bsTreeSearch(x->left, key);
    }
    else
    {
        return bsTreeSearch(x->right, key);
    }
}

static Node* bsTreeMinimum(BSTree tree)
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

static Node* bsTreeMaximum(BSTree tree)
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

static Node* bsTreeSuccessor(Node* x)
{
    if (nullptr != x->right)
    {
        return bsTreeMinimum(x->right);
    }

    Node* y = x->parent;
    while ((nullptr != y) && (y->right == x))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

static Node* bsTreePredecessor(Node* x)
{
    if (nullptr != x->left)
    {
        return bsTreeMaximum(x->left);
    }

    Node* y = x->parent;
    while ((nullptr != y) && (y->left == x))
    {
        x = y;
        y = y->parent;
    }

    return y;
}

static Node* createBSTreeNode(const Type key, Node* parent, Node* left, Node* right)
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

static Node* bsTreeInsert(BSTree tree, Node* z)
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

static Node* insertBSTree(BSTree tree, const Type key)
{
    Node* z = createBSTreeNode(key, nullptr, nullptr, nullptr);
    if (nullptr == z)
    {
        return tree;
    }

    return bsTreeInsert(tree, z);
}

static Node* bsTreeDelete(BSTree tree, Node* z)
{
    Node* x = nullptr;
    Node* y = nullptr;

    if ((nullptr == z->left) || (nullptr == z->right))
    {
        y = z;
    }
    else
    {
        y = bsTreeSuccessor(z);
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

static Node* deleteBSTree(BSTree tree, const Type key)
{
    Node* z = bsTreeSearch(tree, key);
    if (nullptr != z)
    {
        tree = bsTreeDelete(tree, z);
    }

    return tree;
}

static void destroyBSTree(BSTree tree)
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

static void printBSTree(BSTree tree, const Type key, int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            output() << tree->key << " is root" << std::endl;
        }
        else
        {
            output() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : " left") << " child"
                     << std::endl;
        }

        printBSTree(tree->left, tree->key, -1);
        printBSTree(tree->right, tree->key, 1);
    }
}
} // namespace bs

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void TreeStructure::bsInstance() const
{
    using bs::BSTree;
    using bs::bsTreeMaximum;
    using bs::bsTreeMinimum;
    using bs::deleteBSTree;
    using bs::destroyBSTree;
    using bs::inorderBSTree;
    using bs::insertBSTree;
    using bs::postorderBSTree;
    using bs::preorderBSTree;
    using bs::printBSTree;

    BSTree root = nullptr;
    constexpr int arraySize = 6;
    constexpr std::array<int, arraySize> array = {1, 5, 4, 3, 2, 6};

    bs::output() << "Insert: ";
    for (int i = 0; i < arraySize; ++i)
    {
        bs::output() << array.at(i) << " ";
        root = insertBSTree(root, array.at(i));
    }

    bs::output() << "\nPreorder traversal: ";
    preorderBSTree(root);
    bs::output() << "\nInorder traversal: ";
    inorderBSTree(root);
    bs::output() << "\nPostorder traversal: ";
    postorderBSTree(root);

    bs::output() << "\nMinimum: " << bsTreeMinimum(root)->key;
    bs::output() << "\nMaximum: " << bsTreeMaximum(root)->key;
    bs::output() << "\nTree verbose: " << std::endl;
    printBSTree(root, root->key, 0);

    constexpr int deleteNode = 3;
    bs::output() << "Delete root node: " << deleteNode;
    root = deleteBSTree(root, deleteNode);
    bs::output() << "\nInorder traversal: ";
    inorderBSTree(root);
    bs::output() << std::endl;

    destroyBSTree(root);

    COMMON_PRINT(TREE_RESULT, "BinarySearch", bs::output().str().c_str());
}

// Adelson-Velsky-Landis
namespace avl
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

typedef int Type;
typedef struct AVLTreeNode
{
    Type key;
    int height;
    struct AVLTreeNode* left;
    struct AVLTreeNode* right;
} Node, *AVLTree;

static int avlTreeHeight(AVLTree tree)
{
    return ((nullptr == tree) ? 0 : ((Node*)tree)->height);
}

static void preorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ";
        preorderAVLTree(tree->left);
        preorderAVLTree(tree->right);
    }
}

static void inorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        inorderAVLTree(tree->left);
        output() << tree->key << " ";
        inorderAVLTree(tree->right);
    }
}

static void postorderAVLTree(AVLTree tree)
{
    if (nullptr != tree)
    {
        postorderAVLTree(tree->left);
        postorderAVLTree(tree->right);
        output() << tree->key << " ";
    }
}

static Node* avlTreeSearch(AVLTree x, const Type key)
{
    if ((nullptr == x) || (x->key == key))
    {
        return x;
    }

    if (key < x->key)
    {
        return avlTreeSearch(x->left, key);
    }
    else
    {
        return avlTreeSearch(x->right, key);
    }
}

static Node* avlTreeMinimum(AVLTree tree)
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

static Node* avlTreeMaximum(AVLTree tree)
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

static Node* leftLeftRotation(AVLTree k2)
{
    AVLTree k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = std::max(avlTreeHeight(k2->left), avlTreeHeight(k2->right)) + 1;
    k1->height = std::max(avlTreeHeight(k1->left), k2->height) + 1;

    return k1;
}

static Node* rightRightRotation(AVLTree k1)
{
    AVLTree k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = std::max(avlTreeHeight(k1->left), avlTreeHeight(k1->right)) + 1;
    k2->height = std::max(avlTreeHeight(k2->right), k1->height) + 1;

    return k2;
}

static Node* leftRightRotation(AVLTree k3)
{
    k3->left = rightRightRotation(k3->left);

    return leftLeftRotation(k3);
}

static Node* rightLeftRotation(AVLTree k1)
{
    k1->right = leftLeftRotation(k1->right);

    return rightRightRotation(k1);
}

static Node* avlTreeCreateNode(const Type key, Node* left, Node* right)
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

static Node* avlTreeInsert(AVLTree tree, const Type key)
{
    if (nullptr == tree)
    {
        tree = avlTreeCreateNode(key, nullptr, nullptr);
        if (nullptr == tree)
        {
            std::cerr << "Create AVL tree node failed." << std::endl;
            return nullptr;
        }
    }
    else if (key < tree->key)
    {
        tree->left = avlTreeInsert(tree->left, key);
        if (2 == (avlTreeHeight(tree->left) - avlTreeHeight(tree->right)))
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
        if (2 == (avlTreeHeight(tree->right) - avlTreeHeight(tree->left)))
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
        std::cerr << "Do not allow to insert the same node." << std::endl;
    }

    tree->height = std::max(avlTreeHeight(tree->left), avlTreeHeight(tree->right)) + 1;

    return tree;
}

static Node* deleteNode(AVLTree tree, Node* z)
{
    if ((nullptr == tree) || (nullptr == z))
    {
        return nullptr;
    }

    if (z->key < tree->key)
    {
        tree->left = deleteNode(tree->left, z);
        if (2 == (avlTreeHeight(tree->right) - avlTreeHeight(tree->left)))
        {
            Node* r = tree->right;
            if (avlTreeHeight(r->left) > avlTreeHeight(r->right))
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
        if (2 == (avlTreeHeight(tree->left) - avlTreeHeight(tree->right)))
        {
            Node* l = tree->left;
            if (avlTreeHeight(l->right) > avlTreeHeight(l->left))
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
            if (avlTreeHeight(tree->left) > avlTreeHeight(tree->right))
            {
                Node* max = avlTreeMaximum(tree->left);
                tree->key = max->key;
                tree->left = deleteNode(tree->left, max);
            }
            else
            {
                Node* min = avlTreeMaximum(tree->right);
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

static Node* avlTreeDelete(AVLTree tree, const Type key)
{
    Node* z = avlTreeSearch(tree, key);
    if (nullptr != z)
    {
        tree = deleteNode(tree, z);
    }

    return tree;
}

static void destroyAVLTree(AVLTree tree)
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

static void printAVLTree(AVLTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            output() << tree->key << " is root" << std::endl;
        }
        else
        {
            output() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : " left") << " child"
                     << std::endl;
        }

        printAVLTree(tree->left, tree->key, -1);
        printAVLTree(tree->right, tree->key, 1);
    }
}
} // namespace avl

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void TreeStructure::avlInstance() const
{
    using avl::AVLTree;
    using avl::avlTreeDelete;
    using avl::avlTreeHeight;
    using avl::avlTreeInsert;
    using avl::avlTreeMaximum;
    using avl::avlTreeMinimum;
    using avl::destroyAVLTree;
    using avl::inorderAVLTree;
    using avl::postorderAVLTree;
    using avl::preorderAVLTree;
    using avl::printAVLTree;

    AVLTree root = nullptr;
    constexpr int arraySize = 16;
    constexpr std::array<int, arraySize> array = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};

    avl::output() << "Height: " << avlTreeHeight(root);
    avl::output() << "\nInsert: ";
    for (int i = 0; i < arraySize; ++i)
    {
        avl::output() << array.at(i) << " ";
        root = avlTreeInsert(root, array.at(i));
    }

    avl::output() << "\nPreorder traversal: ";
    preorderAVLTree(root);
    avl::output() << "\nInorder traversal: ";
    inorderAVLTree(root);
    avl::output() << "\nPostorder traversal: ";
    postorderAVLTree(root);

    avl::output() << "\nHeight: " << avlTreeHeight(root);
    avl::output() << "\nMinimum: " << avlTreeMinimum(root)->key;
    avl::output() << "\nMaximum: " << avlTreeMaximum(root)->key;
    avl::output() << "\nTree verbose: " << std::endl;
    printAVLTree(root, root->key, 0);

    constexpr int deleteNode = 8;
    avl::output() << "Delete root node: " << deleteNode;
    root = avlTreeDelete(root, deleteNode);

    avl::output() << "\nHeight: " << avlTreeHeight(root);
    avl::output() << "\nInorder traversal: ";
    inorderAVLTree(root);
    avl::output() << "\nTree verbose: " << std::endl;
    printAVLTree(root, root->key, 0);

    destroyAVLTree(root);

    COMMON_PRINT(TREE_RESULT, "AdelsonVelskyLandis", avl::output().str().c_str());
}

// Splay
namespace splay
{
static std::ostringstream& output()
{
    static std::ostringstream stream;
    return stream;
}

typedef int Type;
typedef struct SplayTreeNode
{
    Type key;
    struct SplayTreeNode* left;
    struct SplayTreeNode* right;
} Node, *SplayTree;

static void preorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        output() << tree->key << " ";
        preorderSplayTree(tree->left);
        preorderSplayTree(tree->right);
    }
}

static void inorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        inorderSplayTree(tree->left);
        output() << tree->key << " ";
        inorderSplayTree(tree->right);
    }
}

static void postorderSplayTree(SplayTree tree)
{
    if (nullptr != tree)
    {
        postorderSplayTree(tree->left);
        postorderSplayTree(tree->right);
        output() << tree->key << " ";
    }
}

static Node* splayTreeSearch(SplayTree x, const Type key)
{
    if ((nullptr == x) || (key == x->key))
    {
        return x;
    }

    if (key < x->key)
    {
        return splayTreeSearch(x->left, key);
    }
    else
    {
        return splayTreeSearch(x->right, key);
    }
}

static Node* splayTreeMinimum(SplayTree tree)
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

static Node* splayTreeMaximum(SplayTree tree)
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

static Node* splayTreeSplay(SplayTree tree, const Type key)
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

static Node* splayTreeInsert(SplayTree tree, Node* z)
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
            std::cerr << "Do not allow to insert the same node." << std::endl;
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

static Node* createSplayTreeNode(const Type key, Node* left, Node* right)
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

static Node* insertSplayTree(SplayTree tree, const Type key)
{
    Node* z = createSplayTreeNode(key, nullptr, nullptr);
    if (nullptr == z)
    {
        return tree;
    }

    tree = splayTreeInsert(tree, z);
    tree = splayTreeSplay(tree, key);

    return tree;
}

static Node* deleteSplayTree(SplayTree tree, const Type key)
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

static void destroySplayTree(SplayTree tree)
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

static void printSplayTree(SplayTree tree, const Type key, const int direction)
{
    if (nullptr != tree)
    {
        if (0 == direction)
        {
            output() << tree->key << " is root" << std::endl;
        }
        else
        {
            output() << tree->key << " is " << key << "'s " << ((1 == direction) ? "right" : " left") << " child"
                     << std::endl;
        }

        printSplayTree(tree->left, tree->key, -1);
        printSplayTree(tree->right, tree->key, 1);
    }
}
} // namespace splay

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void TreeStructure::splayInstance() const
{
    using splay::destroySplayTree;
    using splay::inorderSplayTree;
    using splay::insertSplayTree;
    using splay::postorderSplayTree;
    using splay::preorderSplayTree;
    using splay::printSplayTree;
    using splay::SplayTree;
    using splay::splayTreeMaximum;
    using splay::splayTreeMinimum;
    using splay::splayTreeSplay;

    SplayTree root = nullptr;
    constexpr int arraySize = 6;
    constexpr std::array<int, arraySize> array = {10, 50, 40, 30, 20, 60};

    splay::output() << "Insert: ";
    for (int i = 0; i < arraySize; ++i)
    {
        splay::output() << array.at(i) << " ";
        root = insertSplayTree(root, array.at(i));
    }

    splay::output() << "\nPreorder traversal: ";
    preorderSplayTree(root);
    splay::output() << "\nInorder traversal: ";
    inorderSplayTree(root);
    splay::output() << "\nPostorder traversal: ";
    postorderSplayTree(root);

    splay::output() << "\nMinimum: " << splayTreeMinimum(root)->key;
    splay::output() << "\nMaximum: " << splayTreeMaximum(root)->key;
    splay::output() << "\nTree verbose: " << std::endl;
    printSplayTree(root, root->key, 0);

    constexpr int splayNode = 30;
    splay::output() << "Splay node as root node: " << splayNode;
    splay::output() << "\nTree verbose: " << std::endl;
    root = splayTreeSplay(root, splayNode);
    printSplayTree(root, root->key, 0);

    destroySplayTree(root);

    COMMON_PRINT(TREE_RESULT, "Splay", splay::output().str().c_str());
}
} // namespace ds_tree
