//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <algorithm>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <span>
#include <vector>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "data_structure/include/linear.hpp"
#include "data_structure/include/tree.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
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

//! @brief Manage data structure choices.
class ApplyDataStructure
{
public:
    //! @brief Enumerate specific data structure choices.
    enum Category : std::uint8_t
    {
        //! @brief Linear.
        linear,
        //! @brief Tree.
        tree
    };

    //! @brief Bit flags for managing linear instances.
    std::bitset<Bottom<LinearInstance>::value> linearOpts{};
    //! @brief Bit flags for managing tree instances.
    std::bitset<Bottom<TreeInstance>::value> treeOpts{};

    //! @brief Check whether any data structure choices do not exist.
    //! @return any data structure choices do not exist or exist
    [[nodiscard]] inline bool empty() const { return linearOpts.none() && treeOpts.none(); }
    //! @brief Reset bit flags that manage data structure choices.
    inline void reset()
    {
        linearOpts.reset();
        treeOpts.reset();
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
                os << "UNKNOWN (" << static_cast<std::underlying_type_t<Category>>(cat) << ')';
                break;
        }
        return os;
    }
};
extern ApplyDataStructure& manager();

//! @brief Update choice.
//! @tparam T - type of target instance
//! @param target - target instance
template <class T>
void updateChoice(const std::string_view target);
//! @brief Run choices.
//! @tparam T - type of target instance
//! @param candidates - container for the candidate target instances
template <class T>
void runChoices(const std::vector<std::string>& candidates);

//! @brief Apply linear.
namespace linear
{
//! @brief The version used to apply.
const char* const version = date_structure::linear::version();
//! @brief Metadata, which is used in the instance.
struct Meta
{
    //! @brief Id of the metadata.
    int id{0};
    //! @brief Name of the metadata.
    char name[4]{'\0'};
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

        date_structure::linear::Output output{};
        auto& flush = output.flush();
        flush << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::span<Meta> nodes(meta);

        const Meta* val = nullptr;
        DLL dll = nullptr;
        createDll(&dll);
        dllInsert(dll, 0, &nodes[0]);
        flush << "insert (0): {" << nodes[0].id << ", " << nodes[0].name << "}\n";
        dllInsert(dll, 0, &nodes[1]);
        flush << "insert (0): {" << nodes[1].id << ", " << nodes[1].name << "}\n";
        dllInsert(dll, 1, &nodes[2]);
        flush << "insert (1): {" << nodes[2].id << ", " << nodes[2].name << "}\n";
        dllDelete(dll, 2);
        flush << "delete (2)\n";

        dllInsertFirst(dll, &nodes.front());
        flush << "insert first: {" << nodes.front().id << ", " << nodes.front().name << "}\n";
        dllInsertLast(dll, &nodes.back());
        flush << "insert last: {" << nodes.back().id << ", " << nodes.back().name << "}\n";
        val = static_cast<Meta*>(dllGetFirst(dll));
        flush << "get first: {" << val->id << ", " << val->name << "}\n";
        val = static_cast<Meta*>(dllGetLast(dll));
        flush << "get last: {" << val->id << ", " << val->name << "}\n";
        dllDeleteFirst(dll);
        flush << "delete first\n";
        dllDeleteLast(dll);
        flush << "delete last\n";
        dllInsert(dll, 1, &nodes[0]);
        flush << "insert (1): {" << nodes[0].id << ", " << nodes[0].name << "}\n";

        flush << "whether it is empty: " << dllIsEmpty(dll) << '\n';
        flush << "size: " << dllSize(dll) << '\n';
        flush << "linear details: HEAD -> ";
        for (int i = 0; i < dllSize(dll); ++i)
        {
            val = static_cast<Meta*>(dllGet(dll, i));
            flush << '{' << val->id << ", " << val->name << "} <-> ";
        }
        flush.seekp(flush.str().length() - std::strlen(" <-> "));
        flush << " -> NULL\n";
        destroyDll(&dll);

        return std::ostringstream(flush.str());
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        namespace stack = date_structure::linear::stack;
        using stack::createStack, stack::destroyStack, stack::Stack, stack::stackIsEmpty, stack::stackPop,
            stack::stackPush, stack::stackSize, stack::stackTop;

        date_structure::linear::Output output{};
        auto& flush = output.flush();
        flush << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::span<Meta> nodes(meta);

        const Meta* val = nullptr;
        Stack stacks = nullptr;
        createStack(&stacks);
        std::for_each(
            nodes.begin(),
            nodes.end(),
            [&](auto& node)
            {
                stackPush(stacks, &node);
                flush << "push: {" << node.id << ", " << node.name << "}\n";
            });

        val = static_cast<Meta*>(stackPop(stacks));
        flush << "pop: {" << val->id << ", " << val->name << "}\n";
        val = static_cast<Meta*>(stackTop(stacks));
        flush << "top: {" << val->id << ", " << val->name << "}\n";
        stackPush(stacks, &nodes.back());
        flush << "push: {" << nodes.back().id << ", " << nodes.back().name << "}\n";

        flush << "whether it is empty: " << stackIsEmpty(stacks) << '\n';
        flush << "size: " << stackSize(stacks) << '\n';
        flush << "linear details: TOP [";
        while (!stackIsEmpty(stacks))
        {
            val = static_cast<Meta*>(stackPop(stacks));
            flush << '{' << val->id << ", " << val->name << "}, ";
        }
        flush.seekp(flush.str().length() - std::strlen(", "));
        flush << "] BOTTOM\n";
        destroyStack(&stacks);

        return std::ostringstream(flush.str());
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        namespace queue = date_structure::linear::queue;
        using queue::createQueue, queue::destroyQueue, queue::Queue, queue::queueFront, queue::queueIsEmpty,
            queue::queuePop, queue::queuePush, queue::queueSize;

        date_structure::linear::Output output{};
        auto& flush = output.flush();
        flush << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::span<Meta> nodes(meta);

        const Meta* val = nullptr;
        Queue queues = nullptr;
        createQueue(&queues);
        std::for_each(
            nodes.begin(),
            nodes.end(),
            [&](auto& node)
            {
                queuePush(queues, &node);
                flush << "push: {" << node.id << ", " << node.name << "}\n";
            });

        val = static_cast<Meta*>(queuePop(queues));
        flush << "pop: {" << val->id << ", " << val->name << "}\n";
        val = static_cast<Meta*>(queueFront(queues));
        flush << "front: {" << val->id << ", " << val->name << "}\n";
        queuePush(queues, &nodes.front());
        flush << "push: {" << nodes.front().id << ", " << nodes.front().name << "}\n";

        flush << "whether it is empty: " << queueIsEmpty(queues) << '\n';
        flush << "size: " << queueSize(queues) << '\n';
        flush << "linear details: FRONT [";
        while (!queueIsEmpty(queues))
        {
            val = static_cast<Meta*>(queuePop(queues));
            flush << '{' << val->id << ", " << val->name << "}, ";
        }
        flush.seekp(flush.str().length() - std::strlen(", "));
        flush << "] REAR\n";
        destroyQueue(&queues);

        return std::ostringstream(flush.str());
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
template <>
void updateChoice<LinearInstance>(const std::string_view target);
template <>
void runChoices<LinearInstance>(const std::vector<std::string>& candidates);

//! @brief Apply tree.
namespace tree
{
//! @brief The version used to apply.
const char* const version = date_structure::tree::version();
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

        bs::Output output{};
        auto& flush = output.flush();
        BSTree root = nullptr;
        constexpr std::array<std::int16_t, 6> nodes = {1, 5, 4, 3, 2, 6};

        flush << "insert: ";
        std::for_each(
            nodes.cbegin(),
            nodes.cend(),
            [&](const auto node)
            {
                flush << node << ", ";
                root = bsTreeInsert(root, node);
            });
        flush.seekp(flush.str().length() - 2);

        flush << "\npre-order traversal: ";
        output.preorderBSTree(root);
        flush << "\nin-order traversal: ";
        output.inorderBSTree(root);
        flush << "\npost-order traversal: ";
        output.postorderBSTree(root);

        flush << "\nminimum: " << getMinimum(root)->key;
        flush << "\nmaximum: " << getMaximum(root)->key;
        flush << "\ntree details:\n";
        output.printBSTree(root, root->key, 0);

        constexpr std::int16_t deleteNode = 3;
        flush << "delete root node: " << deleteNode;
        root = bsTreeDelete(root, deleteNode);

        flush << "\nin-order traversal: ";
        output.inorderBSTree(root);
        flush << "\ntree details:\n";
        output.printBSTree(root, root->key, 0);

        destroyBSTree(root);

        return std::ostringstream(flush.str());
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        namespace avl = date_structure::tree::avl;
        using avl::AVLTree, avl::avlTreeDelete, avl::avlTreeInsert, avl::destroyAVLTree, avl::getHeight,
            avl::getMaximum, avl::getMinimum;

        avl::Output output{};
        auto& flush = output.flush();
        AVLTree root = nullptr;
        constexpr std::array<std::int16_t, 16> nodes = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};

        flush << "height: " << getHeight(root);
        flush << "\ninsert: ";
        std::for_each(
            nodes.cbegin(),
            nodes.cend(),
            [&](const auto node)
            {
                flush << node << ", ";
                root = avlTreeInsert(root, node);
            });
        flush.seekp(flush.str().length() - 2);

        flush << "\npre-order traversal: ";
        output.preorderAVLTree(root);
        flush << "\nin-order traversal: ";
        output.inorderAVLTree(root);
        flush << "\npost-order traversal: ";
        output.postorderAVLTree(root);

        flush << "\nheight: " << getHeight(root);
        flush << "\nminimum: " << getMinimum(root)->key;
        flush << "\nmaximum: " << getMaximum(root)->key;
        flush << "\ntree details:\n";
        output.printAVLTree(root, root->key, 0);

        constexpr std::int16_t deleteNode = 8;
        flush << "delete root node: " << deleteNode;
        root = avlTreeDelete(root, deleteNode);

        flush << "\nheight: " << getHeight(root);
        flush << "\nin-order traversal: ";
        output.inorderAVLTree(root);
        flush << "\ntree details:\n";
        output.printAVLTree(root, root->key, 0);

        destroyAVLTree(root);

        return std::ostringstream(flush.str());
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        namespace splay = date_structure::tree::splay;
        using splay::destroySplayTree, splay::getMaximum, splay::getMinimum, splay::SplayTree, splay::splayTreeInsert,
            splay::splayTreeSplay;

        splay::Output output{};
        auto& flush = output.flush();
        SplayTree root = nullptr;
        constexpr std::array<std::int16_t, 6> nodes = {10, 50, 40, 30, 20, 60};

        flush << "insert: ";
        std::for_each(
            nodes.cbegin(),
            nodes.cend(),
            [&](const auto node)
            {
                flush << node << ", ";
                root = splayTreeInsert(root, node);
            });
        flush.seekp(flush.str().length() - 2);

        flush << "\npre-order traversal: ";
        output.preorderSplayTree(root);
        flush << "\nin-order traversal: ";
        output.inorderSplayTree(root);
        flush << "\npost-order traversal: ";
        output.postorderSplayTree(root);

        flush << "\nminimum: " << getMinimum(root)->key;
        flush << "\nmaximum: " << getMaximum(root)->key;
        flush << "\ntree details:\n";
        output.printSplayTree(root, root->key, 0);

        constexpr std::int16_t splayNode = 30;
        flush << "splay node as root node: " << splayNode;
        root = splayTreeSplay(root, splayNode);

        flush << "\ntree details:\n";
        output.printSplayTree(root, root->key, 0);

        destroySplayTree(root);

        return std::ostringstream(flush.str());
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
template <>
void updateChoice<TreeInstance>(const std::string_view target);
template <>
void runChoices<TreeInstance>(const std::vector<std::string>& candidates);
} // namespace app_ds
} // namespace application
