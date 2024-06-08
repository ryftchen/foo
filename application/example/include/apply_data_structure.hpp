//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <array>
#include <bitset>
#include <vector>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "data_structure/include/linear.hpp"
#include "data_structure/include/tree.hpp"

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Data-structure-applying-related functions in the application module.
namespace app_ds
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <class T>
struct Bottom;

//! @brief Enumerate specific linear instances.
enum LinearInstance : std::uint8_t
{
    //! @brief Linked list.
    linkedList,
    //! @brief Stack.
    stack,
    //! @brief Queue.
    queue
};
//! @brief Store the maximum value of the LinearInstance enum.
template <>
struct Bottom<LinearInstance>
{
    //! @brief Maximum value of the LinearInstance enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Enumerate specific tree instances.
enum TreeInstance : std::uint8_t
{
    //! @brief Binary search.
    binarySearch,
    //! @brief Adelson-Velsky-Landis.
    adelsonVelskyLandis,
    //! @brief Splay.
    splay
};
//! @brief Store the maximum value of the TreeInstance enum.
template <>
struct Bottom<TreeInstance>
{
    //! @brief Maximum value of the TreeInstance enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Manage data structure tasks.
class DataStructureTask
{
public:
    //! @brief Enumerate specific data structure tasks.
    enum Category : std::uint8_t
    {
        //! @brief Linear.
        linear,
        //! @brief Tree.
        tree
    };

    //! @brief Bit flags for managing linear instances.
    std::bitset<Bottom<LinearInstance>::value> linearBit;
    //! @brief Bit flags for managing tree instances.
    std::bitset<Bottom<TreeInstance>::value> treeBit;

    //! @brief Check whether any data structure tasks do not exist.
    //! @return any data structure tasks do not exist or exist
    [[nodiscard]] inline bool empty() const { return linearBit.none() && treeBit.none(); }
    //! @brief Reset bit flags that manage data structure tasks.
    inline void reset()
    {
        linearBit.reset();
        treeBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - the specific value of Category enum
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::linear:
                os << "LINEAR";
                break;
            case Category::tree:
                os << "TREE";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Category>>(cat);
        }
        return os;
    }
};
extern DataStructureTask& getTask();

//! @brief Apply linear.
namespace linear
{
//! @brief Metadata, which is used in the instance.
struct Meta
{
    //! @brief Id of the metadata.
    int id;
    //! @brief Name of the metadata.
    char name[4];
};

//! @brief Linear instances.
class Linear
{
public:
    //! @brief Destroy the Linear object.
    virtual ~Linear() = default;

    //! @brief Linked list.
    //! @return procedure output
    static std::ostringstream linkedList()
    {
        namespace doubly_linked_list = date_structure::linear::doubly_linked_list;
        using doubly_linked_list::createDll, doubly_linked_list::destroyDll, doubly_linked_list::DLL,
            doubly_linked_list::dllDelete, doubly_linked_list::dllDeleteFirst, doubly_linked_list::dllDeleteLast,
            doubly_linked_list::dllGet, doubly_linked_list::dllGetFirst, doubly_linked_list::dllGetLast,
            doubly_linked_list::dllInsert, doubly_linked_list::dllInsertFirst, doubly_linked_list::dllInsertLast,
            doubly_linked_list::dllIsEmpty, doubly_linked_list::dllSize;

        date_structure::linear::Output output;
        output.flush() << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::uint16_t metaSize = sizeof(meta) / sizeof(meta[0]);

        const Meta* val = nullptr;
        DLL dll = nullptr;
        createDll(&dll);
        dllInsert(dll, 0, &meta[0]);
        output.flush() << "insert (0): {" << meta[0].id << ", " << meta[0].name << "}\n";
        dllInsert(dll, 0, &meta[1]);
        output.flush() << "insert (0): {" << meta[1].id << ", " << meta[1].name << "}\n";
        dllInsert(dll, 1, &meta[2]);
        output.flush() << "insert (1): {" << meta[2].id << ", " << meta[2].name << "}\n";
        dllDelete(dll, 2);
        output.flush() << "delete (2)\n";

        dllInsertFirst(dll, &meta[0]);
        output.flush() << "insert first: {" << meta[0].id << ", " << meta[0].name << "}\n";
        dllInsertLast(dll, &meta[metaSize - 1]);
        output.flush() << "insert last: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}\n";
        val = static_cast<Meta*>(dllGetFirst(dll));
        output.flush() << "get first: {" << val->id << ", " << val->name << "}\n";
        val = static_cast<Meta*>(dllGetLast(dll));
        output.flush() << "get last: {" << val->id << ", " << val->name << "}\n";
        dllDeleteFirst(dll);
        output.flush() << "delete first\n";
        dllDeleteLast(dll);
        output.flush() << "delete last\n";

        output.flush() << "whether it is empty: " << dllIsEmpty(dll) << '\n';
        output.flush() << "size: " << dllSize(dll) << '\n';
        for (int i = 0; i < dllSize(dll); ++i)
        {
            val = static_cast<Meta*>(dllGet(dll, i));
            output.flush() << "get (" << i << "): {" << val->id << ", " << val->name << "}\n";
        }
        destroyDll(&dll);

        return std::ostringstream{output.flush().str()};
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        namespace stack = date_structure::linear::stack;
        using stack::createStack, stack::destroyStack, stack::Stack, stack::stackIsEmpty, stack::stackPop,
            stack::stackPush, stack::stackSize, stack::stackTop;

        date_structure::linear::Output output;
        output.flush() << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::uint16_t metaSize = sizeof(meta) / sizeof(meta[0]);

        const Meta* val = nullptr;
        Stack stacks = nullptr;
        createStack(&stacks);
        for (std::uint16_t i = 0; i < (metaSize - 1); ++i)
        {
            stackPush(stacks, &meta[i]);
            output.flush() << "push: {" << meta[i].id << ", " << meta[i].name << "}\n";
        }

        val = static_cast<Meta*>(stackPop(stacks));
        output.flush() << "pop: {" << val->id << ", " << val->name << "}\n";
        val = static_cast<Meta*>(stackTop(stacks));
        output.flush() << "top: {" << val->id << ", " << val->name << "}\n";
        stackPush(stacks, &meta[metaSize - 1]);
        output.flush() << "push: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}\n";

        output.flush() << "whether it is empty: " << stackIsEmpty(stacks) << '\n';
        output.flush() << "size: " << stackSize(stacks) << '\n';
        while (!stackIsEmpty(stacks))
        {
            val = static_cast<Meta*>(stackPop(stacks));
            output.flush() << "pop: {" << val->id << ", " << val->name << "}\n";
        }
        destroyStack(&stacks);

        return std::ostringstream{output.flush().str()};
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        namespace queue = date_structure::linear::queue;
        using queue::createQueue, queue::destroyQueue, queue::Queue, queue::queueFront, queue::queueIsEmpty,
            queue::queuePop, queue::queuePush, queue::queueSize;

        date_structure::linear::Output output;
        output.flush() << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::uint16_t metaSize = sizeof(meta) / sizeof(meta[0]);

        const Meta* val = nullptr;
        Queue queues = nullptr;
        createQueue(&queues);
        for (std::uint16_t i = 0; i < (metaSize - 1); ++i)
        {
            queuePush(queues, &meta[i]);
            output.flush() << "push: {" << meta[i].id << ", " << meta[i].name << "}\n";
        }

        val = static_cast<Meta*>(queuePop(queues));
        output.flush() << "pop: {" << val->id << ", " << val->name << "}\n";
        val = static_cast<Meta*>(queueFront(queues));
        output.flush() << "front: {" << val->id << ", " << val->name << "}\n";
        queuePush(queues, &meta[metaSize - 1]);
        output.flush() << "push: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}\n";

        output.flush() << "whether it is empty: " << queueIsEmpty(queues) << '\n';
        output.flush() << "size: " << queueSize(queues) << '\n';
        while (!queueIsEmpty(queues))
        {
            val = static_cast<Meta*>(queuePop(queues));
            output.flush() << "pop: {" << val->id << ", " << val->name << "}\n";
        }
        destroyQueue(&queues);

        return std::ostringstream{output.flush().str()};
    }
};

//! @brief Structure of linear.
class LinearStructure
{
public:
    //! @brief Destroy the LinearStructure object.
    virtual ~LinearStructure() = default;

    //! @brief The linked list instance.
    static void linkedListInstance();
    //! @brief The stack instance.
    static void stackInstance();
    //! @brief The queue instance.
    static void queueInstance();
};
} // namespace linear
extern void runLinearTasks(const std::vector<std::string>& candidates);
extern void updateLinearTask(const std::string& target);

//! @brief Apply tree.
namespace tree
{
//! @brief Tree instances.
class Tree
{
public:
    //! @brief Destroy the Tree object.
    virtual ~Tree() = default;

    //! @brief Binary search.
    //! @return procedure output
    static std::ostringstream bs()
    {
        namespace bs = date_structure::tree::bs;
        using bs::BSTree, bs::bsTreeDelete, bs::bsTreeInsert, bs::destroyBSTree, bs::getMaximum, bs::getMinimum;

        bs::Output output;
        BSTree root = nullptr;
        constexpr std::uint8_t arraySize = 6;
        constexpr std::array<std::int16_t, arraySize> array = {1, 5, 4, 3, 2, 6};

        output.flush() << "insert: ";
        for (std::uint8_t i = 0; i < arraySize; ++i)
        {
            output.flush() << array.at(i) << ' ';
            root = bsTreeInsert(root, array.at(i));
        }

        output.flush() << "\npre-order traversal: ";
        output.preorderBSTree(root);
        output.flush() << "\nin-order traversal: ";
        output.inorderBSTree(root);
        output.flush() << "\npost-order traversal: ";
        output.postorderBSTree(root);

        output.flush() << "\nminimum: " << getMinimum(root)->key;
        output.flush() << "\nmaximum: " << getMaximum(root)->key;
        output.flush() << "\ntree verbose:\n";
        output.printBSTree(root, root->key, 0);

        constexpr std::int16_t deleteNode = 3;
        output.flush() << "delete root node: " << deleteNode;
        root = bsTreeDelete(root, deleteNode);

        output.flush() << "\nin-order traversal: ";
        output.inorderBSTree(root);
        output.flush() << "\ntree verbose:\n";
        output.printBSTree(root, root->key, 0);

        destroyBSTree(root);

        return std::ostringstream{output.flush().str()};
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        namespace avl = date_structure::tree::avl;
        using avl::AVLTree, avl::avlTreeDelete, avl::avlTreeInsert, avl::destroyAVLTree, avl::getHeight,
            avl::getMaximum, avl::getMinimum;

        avl::Output output;
        AVLTree root = nullptr;
        constexpr std::uint8_t arraySize = 16;
        constexpr std::array<std::int16_t, arraySize> array = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};

        output.flush() << "height: " << getHeight(root);
        output.flush() << "\ninsert: ";
        for (std::uint8_t i = 0; i < arraySize; ++i)
        {
            output.flush() << array.at(i) << ' ';
            root = avlTreeInsert(root, array.at(i));
        }

        output.flush() << "\npre-order traversal: ";
        output.preorderAVLTree(root);
        output.flush() << "\nin-order traversal: ";
        output.inorderAVLTree(root);
        output.flush() << "\npost-order traversal: ";
        output.postorderAVLTree(root);

        output.flush() << "\nheight: " << getHeight(root);
        output.flush() << "\nminimum: " << getMinimum(root)->key;
        output.flush() << "\nmaximum: " << getMaximum(root)->key;
        output.flush() << "\ntree verbose:\n";
        output.printAVLTree(root, root->key, 0);

        constexpr std::int16_t deleteNode = 8;
        output.flush() << "delete root node: " << deleteNode;
        root = avlTreeDelete(root, deleteNode);

        output.flush() << "\nheight: " << getHeight(root);
        output.flush() << "\nin-order traversal: ";
        output.inorderAVLTree(root);
        output.flush() << "\ntree verbose:\n";
        output.printAVLTree(root, root->key, 0);

        destroyAVLTree(root);

        return std::ostringstream{output.flush().str()};
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        namespace splay = date_structure::tree::splay;
        using splay::destroySplayTree, splay::getMaximum, splay::getMinimum, splay::SplayTree, splay::splayTreeInsert,
            splay::splayTreeSplay;

        splay::Output output;
        SplayTree root = nullptr;
        constexpr std::uint8_t arraySize = 6;
        constexpr std::array<std::int16_t, arraySize> array = {10, 50, 40, 30, 20, 60};

        output.flush() << "insert: ";
        for (std::uint8_t i = 0; i < arraySize; ++i)
        {
            output.flush() << array.at(i) << ' ';
            root = splayTreeInsert(root, array.at(i));
        }

        output.flush() << "\npre-order traversal: ";
        output.preorderSplayTree(root);
        output.flush() << "\nin-order traversal: ";
        output.inorderSplayTree(root);
        output.flush() << "\npost-order traversal: ";
        output.postorderSplayTree(root);

        output.flush() << "\nminimum: " << getMinimum(root)->key;
        output.flush() << "\nmaximum: " << getMaximum(root)->key;
        output.flush() << "\ntree verbose:\n";
        output.printSplayTree(root, root->key, 0);

        constexpr std::int16_t splayNode = 30;
        output.flush() << "splay node as root node: " << splayNode;
        root = splayTreeSplay(root, splayNode);

        output.flush() << "\ntree verbose:\n";
        output.printSplayTree(root, root->key, 0);

        destroySplayTree(root);

        return std::ostringstream{output.flush().str()};
    }
};

//! @brief Structure of tree.
class TreeStructure
{
public:
    //! @brief Destroy the TreeStructure object.
    virtual ~TreeStructure() = default;

    //! @brief The binary search instance.
    static void bsInstance();
    //! @brief The Adelson-Velsky-Landis instance.
    static void avlInstance();
    //! @brief The splay instance.
    static void splayInstance();
};
} // namespace tree
extern void runTreeTasks(const std::vector<std::string>& candidates);
extern void updateTreeTask(const std::string& target);
} // namespace app_ds
} // namespace application
