//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

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

    //! @brief The operator (<<) overloading of the Meta struct.
    //! @param os - output stream object
    //! @param meta - the specific value of Meta enum
    //! @return reference of the output stream object
    friend inline std::ostream& operator<<(std::ostream& os, const Meta& meta)
    {
        os << '{' << meta.id << ", " << meta.name << '}';
        return os;
    }
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
        date_structure::linear::Output tracker{};
        auto& process = tracker.output();
        process << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::span<Meta> nodes(meta);
        constexpr std::string_view separator = " <-> ";

        const Meta* val = nullptr;
        DLL dll = nullptr;
        createDll(&dll);
        dllInsert(dll, 0, &nodes[0]);
        process << "insert (0): " << nodes[0] << '\n';
        dllInsert(dll, 0, &nodes[1]);
        process << "insert (0): " << nodes[1] << '\n';
        dllInsert(dll, 1, &nodes[2]);
        process << "insert (1): " << nodes[2] << '\n';
        dllDelete(dll, 2);
        process << "delete (2)\n";

        dllInsertFirst(dll, &nodes.front());
        process << "insert first: " << nodes.front() << '\n';
        dllInsertLast(dll, &nodes.back());
        process << "insert last: " << nodes.back() << '\n';
        val = static_cast<Meta*>(dllGetFirst(dll));
        process << "get first: " << *val << '\n';
        val = static_cast<Meta*>(dllGetLast(dll));
        process << "get last: " << *val << '\n';
        dllDeleteFirst(dll);
        process << "delete first\n";
        dllDeleteLast(dll);
        process << "delete last\n";
        dllInsert(dll, 1, &nodes[0]);
        process << "insert (1): " << nodes[0] << '\n';

        process << "whether it is empty: " << dllIsEmpty(dll) << '\n';
        process << "size: " << dllSize(dll) << '\n';
        process << "linear details: HEAD -> ";
        for (int i = 0; i < dllSize(dll); ++i)
        {
            val = static_cast<Meta*>(dllGet(dll, i));
            process << *val << separator;
        }
        process.seekp(process.str().length() - separator.length());
        process << " -> NULL\n";
        destroyDll(&dll);

        return std::ostringstream(process.str());
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        namespace stack = date_structure::linear::stack;
        using stack::createStack, stack::destroyStack, stack::Stack, stack::stackIsEmpty, stack::stackPop,
            stack::stackPush, stack::stackSize, stack::stackTop;
        date_structure::linear::Output tracker{};
        auto& process = tracker.output();
        process << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::span<Meta> nodes(meta);
        constexpr std::string_view separator = ", ";

        const Meta* val = nullptr;
        Stack stacks = nullptr;
        createStack(&stacks);
        std::for_each(
            nodes.begin(),
            nodes.end(),
            [&](auto& node)
            {
                stackPush(stacks, &node);
                process << "push: " << node << '\n';
            });

        val = static_cast<Meta*>(stackPop(stacks));
        process << "pop: " << *val << '\n';
        val = static_cast<Meta*>(stackTop(stacks));
        process << "top: " << *val << '\n';
        stackPush(stacks, &nodes.back());
        process << "push: " << nodes.back() << '\n';

        process << "whether it is empty: " << stackIsEmpty(stacks) << '\n';
        process << "size: " << stackSize(stacks) << '\n';
        process << "linear details: TOP [";
        while (!stackIsEmpty(stacks))
        {
            val = static_cast<Meta*>(stackPop(stacks));
            process << *val << separator;
        }
        process.seekp(process.str().length() - separator.length());
        process << "] BOTTOM\n";
        destroyStack(&stacks);

        return std::ostringstream(process.str());
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        namespace queue = date_structure::linear::queue;
        using queue::createQueue, queue::destroyQueue, queue::Queue, queue::queueFront, queue::queueIsEmpty,
            queue::queuePop, queue::queuePush, queue::queueSize;
        date_structure::linear::Output tracker{};
        auto& process = tracker.output();
        process << std::boolalpha;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const std::span<Meta> nodes(meta);
        constexpr std::string_view separator = ", ";

        const Meta* val = nullptr;
        Queue queues = nullptr;
        createQueue(&queues);
        std::for_each(
            nodes.begin(),
            nodes.end(),
            [&](auto& node)
            {
                queuePush(queues, &node);
                process << "push: " << node << '\n';
            });

        val = static_cast<Meta*>(queuePop(queues));
        process << "pop: " << *val << '\n';
        val = static_cast<Meta*>(queueFront(queues));
        process << "front: " << *val << '\n';
        queuePush(queues, &nodes.front());
        process << "push: " << nodes.front() << '\n';

        process << "whether it is empty: " << queueIsEmpty(queues) << '\n';
        process << "size: " << queueSize(queues) << '\n';
        process << "linear details: FRONT [";
        while (!queueIsEmpty(queues))
        {
            val = static_cast<Meta*>(queuePop(queues));
            process << *val << separator;
        }
        process.seekp(process.str().length() - separator.length());
        process << "] REAR\n";
        destroyQueue(&queues);

        return std::ostringstream(process.str());
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
        bs::Output tracker{};
        auto& process = tracker.output();
        BSTree root = nullptr;
        constexpr std::array<std::int16_t, 6> nodes = {1, 5, 4, 3, 2, 6};

        process << "insert: ";
        std::for_each(
            nodes.cbegin(),
            nodes.cend(),
            [&](const auto node)
            {
                process << node << ", ";
                root = bsTreeInsert(root, node);
            });
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        tracker.preorderBSTree(root);
        process << "\nin-order traversal: ";
        tracker.inorderBSTree(root);
        process << "\npost-order traversal: ";
        tracker.postorderBSTree(root);

        process << "\nminimum: " << getMinimum(root)->key;
        process << "\nmaximum: " << getMaximum(root)->key;
        process << "\ntree details:\n";
        tracker.printBSTree(root, root->key, 0);

        constexpr std::int16_t deleteNode = 3;
        process << "delete root node: " << deleteNode;
        root = bsTreeDelete(root, deleteNode);

        process << "\nin-order traversal: ";
        tracker.inorderBSTree(root);
        process << "\ntree details:\n";
        tracker.printBSTree(root, root->key, 0);

        destroyBSTree(root);

        return std::ostringstream(process.str());
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        namespace avl = date_structure::tree::avl;
        using avl::AVLTree, avl::avlTreeDelete, avl::avlTreeInsert, avl::destroyAVLTree, avl::getHeight,
            avl::getMaximum, avl::getMinimum;
        avl::Output tracker{};
        auto& process = tracker.output();
        AVLTree root = nullptr;
        constexpr std::array<std::int16_t, 16> nodes = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};

        process << "height: " << getHeight(root);
        process << "\ninsert: ";
        std::for_each(
            nodes.cbegin(),
            nodes.cend(),
            [&](const auto node)
            {
                process << node << ", ";
                root = avlTreeInsert(root, node);
            });
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        tracker.preorderAVLTree(root);
        process << "\nin-order traversal: ";
        tracker.inorderAVLTree(root);
        process << "\npost-order traversal: ";
        tracker.postorderAVLTree(root);

        process << "\nheight: " << getHeight(root);
        process << "\nminimum: " << getMinimum(root)->key;
        process << "\nmaximum: " << getMaximum(root)->key;
        process << "\ntree details:\n";
        tracker.printAVLTree(root, root->key, 0);

        constexpr std::int16_t deleteNode = 8;
        process << "delete root node: " << deleteNode;
        root = avlTreeDelete(root, deleteNode);

        process << "\nheight: " << getHeight(root);
        process << "\nin-order traversal: ";
        tracker.inorderAVLTree(root);
        process << "\ntree details:\n";
        tracker.printAVLTree(root, root->key, 0);

        destroyAVLTree(root);

        return std::ostringstream(process.str());
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        namespace splay = date_structure::tree::splay;
        using splay::destroySplayTree, splay::getMaximum, splay::getMinimum, splay::SplayTree, splay::splayTreeInsert,
            splay::splayTreeSplay;
        splay::Output tracker{};
        auto& process = tracker.output();
        SplayTree root = nullptr;
        constexpr std::array<std::int16_t, 6> nodes = {10, 50, 40, 30, 20, 60};

        process << "insert: ";
        std::for_each(
            nodes.cbegin(),
            nodes.cend(),
            [&](const auto node)
            {
                process << node << ", ";
                root = splayTreeInsert(root, node);
            });
        process.seekp(process.str().length() - 2);

        process << "\npre-order traversal: ";
        tracker.preorderSplayTree(root);
        process << "\nin-order traversal: ";
        tracker.inorderSplayTree(root);
        process << "\npost-order traversal: ";
        tracker.postorderSplayTree(root);

        process << "\nminimum: " << getMinimum(root)->key;
        process << "\nmaximum: " << getMaximum(root)->key;
        process << "\ntree details:\n";
        tracker.printSplayTree(root, root->key, 0);

        constexpr std::int16_t splayNode = 30;
        process << "splay node as root node: " << splayNode;
        root = splayTreeSplay(root, splayNode);

        process << "\ntree details:\n";
        tracker.printSplayTree(root, root->key, 0);

        destroySplayTree(root);

        return std::ostringstream(process.str());
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
