//! @file apply_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (apply_data_structure) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

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

//! @brief Data-structure-applying-related functions in the application module.
namespace application::app_ds
{
//! @brief Manage data structure tasks.
class DataStructureTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T - type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific data structure tasks.
    enum Type : uint8_t
    {
        linear,
        tree
    };

    //! @brief Enumerate specific linear instances.
    enum LinearInstance : uint8_t
    {
        linkedList,
        stack,
        queue
    };
    //! @brief Store the maximum value of the LinearInstance enum.
    template <>
    struct Bottom<LinearInstance>
    {
        //! @brief Maximum value of the LinearInstance enum.
        static constexpr uint8_t value = 3;
    };

    //! @brief Enumerate specific tree instances.
    enum TreeInstance : uint8_t
    {
        binarySearch,
        adelsonVelskyLandis,
        splay
    };
    //! @brief Store the maximum value of the TreeInstance enum.
    template <>
    struct Bottom<TreeInstance>
    {
        //! @brief Maximum value of the TreeInstance enum.
        static constexpr uint8_t value = 3;
    };

    //! @brief Bit flags for managing linear instances.
    std::bitset<Bottom<LinearInstance>::value> linearBit;
    //! @brief Bit flags for managing tree instances.
    std::bitset<Bottom<TreeInstance>::value> treeBit;

    //! @brief Check whether any data structure tasks do not exist.
    //! @return any data structure tasks do not exist or exist
    [[nodiscard]] inline bool empty() const { return (linearBit.none() && treeBit.none()); }
    //! @brief Reset bit flags that manage data structure tasks.
    inline void reset()
    {
        linearBit.reset();
        treeBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Type enum.
    //! @param os - output stream object
    //! @param type - the specific value of Type enum
    //! @return reference of output stream object
    friend std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        switch (type)
        {
            case Type::linear:
                os << "LINEAR";
                break;
            case Type::tree:
                os << "TREE";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
        }
        return os;
    }
};

//! @brief Get the data structure task.
//! @return reference of DataStructureTask object
inline DataStructureTask& getTask()
{
    static DataStructureTask task;
    return task;
}

//! @brief Get the bit flags of the instance in data structure tasks.
//! @tparam T - type of the instance
//! @return bit flags of the instance
template <typename T>
auto getBit()
{
    if constexpr (std::is_same_v<T, DataStructureTask::LinearInstance>)
    {
        return getTask().linearBit;
    }
    else if constexpr (std::is_same_v<T, DataStructureTask::TreeInstance>)
    {
        return getTask().treeBit;
    }
}

//! @brief Set the bit flags of the instance in data structure tasks.
//! @tparam T - type of the instance
//! @param index - instance index
template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, DataStructureTask::LinearInstance>)
    {
        getTask().linearBit.set(DataStructureTask::LinearInstance(index));
    }
    else if constexpr (std::is_same_v<T, DataStructureTask::TreeInstance>)
    {
        getTask().treeBit.set(DataStructureTask::TreeInstance(index));
    }
}

//! @brief Apply linear.
namespace linear
{
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    //! @brief Metadata, which is used in the instance.
    typedef struct TagMeta
    {
        //! @brief The ID of metadata.
        int id;
        //! @brief The name of metadata.
        char name[4];
    } Meta;
#ifdef __cplusplus
}
#endif // __cplusplus

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
        using date_structure::linear::doubly_linked_list::createDll;
        using date_structure::linear::doubly_linked_list::destroyDll;
        using date_structure::linear::doubly_linked_list::DLL;
        using date_structure::linear::doubly_linked_list::dllDelete;
        using date_structure::linear::doubly_linked_list::dllDeleteFirst;
        using date_structure::linear::doubly_linked_list::dllDeleteLast;
        using date_structure::linear::doubly_linked_list::dllGet;
        using date_structure::linear::doubly_linked_list::dllGetFirst;
        using date_structure::linear::doubly_linked_list::dllGetLast;
        using date_structure::linear::doubly_linked_list::dllInsert;
        using date_structure::linear::doubly_linked_list::dllInsertFirst;
        using date_structure::linear::doubly_linked_list::dllInsertLast;
        using date_structure::linear::doubly_linked_list::dllIsEmpty;
        using date_structure::linear::doubly_linked_list::dllSize;

        date_structure::linear::Output output;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const int metaSize = sizeof(meta) / sizeof(meta[0]);

        Meta* pVal = nullptr;
        DLL dll = nullptr;
        createDll(&dll);
        dllInsert(dll, 0, &meta[0]);
        output.flush() << "insert (0): {" << meta[0].id << ", " << meta[0].name << "}" << std::endl;
        dllInsert(dll, 0, &meta[1]);
        output.flush() << "insert (0): {" << meta[1].id << ", " << meta[1].name << "}" << std::endl;
        dllInsert(dll, 1, &meta[2]);
        output.flush() << "insert (1): {" << meta[2].id << ", " << meta[2].name << "}" << std::endl;
        dllDelete(dll, 2);
        output.flush() << "delete (2)" << std::endl;

        dllInsertFirst(dll, &meta[0]);
        output.flush() << "insert first: {" << meta[0].id << ", " << meta[0].name << "}" << std::endl;
        dllInsertLast(dll, &meta[metaSize - 1]);
        output.flush() << "insert last: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}"
                       << std::endl;
        pVal = static_cast<Meta*>(dllGetFirst(dll));
        output.flush() << "get first: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        pVal = static_cast<Meta*>(dllGetLast(dll));
        output.flush() << "get last: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        dllDeleteFirst(dll);
        output.flush() << "delete first" << std::endl;
        dllDeleteLast(dll);
        output.flush() << "delete last" << std::endl;

        output.flush() << "whether it is empty: " << dllIsEmpty(dll) << std::endl;
        output.flush() << "size: " << dllSize(dll) << std::endl;
        for (int i = 0; i < dllSize(dll); ++i)
        {
            pVal = static_cast<Meta*>(dllGet(dll, i));
            output.flush() << "get (" << i << "): {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        }
        destroyDll(&dll);

        std::ostringstream ret = std::ostringstream(output.flush().str());
        return ret;
    }
    //! @brief Stack.
    //! @return procedure output
    static std::ostringstream stack()
    {
        using date_structure::linear::stack::createStack;
        using date_structure::linear::stack::destroyStack;
        using date_structure::linear::stack::Stack;
        using date_structure::linear::stack::stackIsEmpty;
        using date_structure::linear::stack::stackPop;
        using date_structure::linear::stack::stackPush;
        using date_structure::linear::stack::stackSize;
        using date_structure::linear::stack::stackTop;

        date_structure::linear::Output output;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const int metaSize = sizeof(meta) / sizeof(meta[0]);

        Meta* pVal = nullptr;
        Stack stack = nullptr;
        createStack(&stack);
        for (int i = 0; i < (metaSize - 1); ++i)
        {
            stackPush(stack, &meta[i]);
            output.flush() << "push: {" << meta[i].id << ", " << meta[i].name << "}" << std::endl;
        }

        pVal = static_cast<Meta*>(stackPop(stack));
        output.flush() << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        pVal = static_cast<Meta*>(stackTop(stack));
        output.flush() << "top: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        stackPush(stack, &meta[metaSize - 1]);
        output.flush() << "push: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}" << std::endl;

        output.flush() << "whether it is empty: " << stackIsEmpty(stack) << std::endl;
        output.flush() << "size: " << stackSize(stack) << std::endl;
        while (!stackIsEmpty(stack))
        {
            pVal = static_cast<Meta*>(stackPop(stack));
            output.flush() << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        }
        destroyStack(&stack);

        std::ostringstream ret = std::ostringstream(output.flush().str());
        return ret;
    }
    //! @brief Queue.
    //! @return procedure output
    static std::ostringstream queue()
    {
        using date_structure::linear::queue::createQueue;
        using date_structure::linear::queue::destroyQueue;
        using date_structure::linear::queue::Queue;
        using date_structure::linear::queue::queueFront;
        using date_structure::linear::queue::queueIsEmpty;
        using date_structure::linear::queue::queuePop;
        using date_structure::linear::queue::queuePush;
        using date_structure::linear::queue::queueSize;

        date_structure::linear::Output output;
        Meta meta[] = {{'A', "foo"}, {'B', "bar"}, {'C', "baz"}, {'D', "qux"}};
        const int metaSize = sizeof(meta) / sizeof(meta[0]);

        Meta* pVal = nullptr;
        Queue queue = nullptr;
        createQueue(&queue);
        for (int i = 0; i < (metaSize - 1); ++i)
        {
            queuePush(queue, &meta[i]);
            output.flush() << "push: {" << meta[i].id << ", " << meta[i].name << "}" << std::endl;
        }

        pVal = static_cast<Meta*>(queuePop(queue));
        output.flush() << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        pVal = static_cast<Meta*>(queueFront(queue));
        output.flush() << "front: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        queuePush(queue, &meta[metaSize - 1]);
        output.flush() << "push: {" << meta[metaSize - 1].id << ", " << meta[metaSize - 1].name << "}" << std::endl;

        output.flush() << "whether it is empty: " << queueIsEmpty(queue) << std::endl;
        output.flush() << "size: " << queueSize(queue) << std::endl;
        while (!queueIsEmpty(queue))
        {
            pVal = static_cast<Meta*>(queuePop(queue));
            output.flush() << "pop: {" << pVal->id << ", " << pVal->name << "}" << std::endl;
        }
        destroyQueue(&queue);

        std::ostringstream ret = std::ostringstream(output.flush().str());
        return ret;
    }
};

//! @brief Structure of linear.
class LinearStructure
{
public:
    //! @brief Construct a new LinearStructure object.
    LinearStructure();
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
extern void runLinear(const std::vector<std::string>& targets);
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
        using date_structure::tree::bs::BSTree;
        using date_structure::tree::bs::bsTreeDelete;
        using date_structure::tree::bs::bsTreeInsert;
        using date_structure::tree::bs::destroyBSTree;
        using date_structure::tree::bs::getMaximum;
        using date_structure::tree::bs::getMinimum;

        date_structure::tree::bs::Output output;
        BSTree root = nullptr;
        constexpr int arraySize = 6;
        constexpr std::array<int, arraySize> array = {1, 5, 4, 3, 2, 6};

        output.flush() << "insert: ";
        for (int i = 0; i < arraySize; ++i)
        {
            output.flush() << array.at(i) << " ";
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
        output.flush() << "\ntree verbose: " << std::endl;
        output.printBSTree(root, root->key, 0);

        constexpr int deleteNode = 3;
        output.flush() << "delete root node: " << deleteNode;
        root = bsTreeDelete(root, deleteNode);
        output.flush() << "\nin-order traversal: ";
        output.inorderBSTree(root);
        output.flush() << std::endl;

        destroyBSTree(root);

        std::ostringstream ret = std::ostringstream(output.flush().str());
        return ret;
    }
    //! @brief Adelson-Velsky-Landis.
    //! @return procedure output
    static std::ostringstream avl()
    {
        using date_structure::tree::avl::AVLTree;
        using date_structure::tree::avl::avlTreeDelete;
        using date_structure::tree::avl::avlTreeInsert;
        using date_structure::tree::avl::destroyAVLTree;
        using date_structure::tree::avl::getHeight;
        using date_structure::tree::avl::getMaximum;
        using date_structure::tree::avl::getMinimum;

        date_structure::tree::avl::Output output;
        AVLTree root = nullptr;
        constexpr int arraySize = 16;
        constexpr std::array<int, arraySize> array = {3, 2, 1, 4, 5, 6, 7, 16, 15, 14, 13, 12, 11, 10, 8, 9};

        output.flush() << "height: " << getHeight(root);
        output.flush() << "\ninsert: ";
        for (int i = 0; i < arraySize; ++i)
        {
            output.flush() << array.at(i) << " ";
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
        output.flush() << "\ntree verbose: " << std::endl;
        output.printAVLTree(root, root->key, 0);

        constexpr int deleteNode = 8;
        output.flush() << "delete root node: " << deleteNode;
        root = avlTreeDelete(root, deleteNode);

        output.flush() << "\nheight: " << getHeight(root);
        output.flush() << "\nin-order traversal: ";
        output.inorderAVLTree(root);
        output.flush() << "\ntree verbose: " << std::endl;
        output.printAVLTree(root, root->key, 0);

        destroyAVLTree(root);

        std::ostringstream ret = std::ostringstream(output.flush().str());
        return ret;
    }
    //! @brief Splay.
    //! @return procedure output
    static std::ostringstream splay()
    {
        using date_structure::tree::splay::destroySplayTree;
        using date_structure::tree::splay::getMaximum;
        using date_structure::tree::splay::getMinimum;
        using date_structure::tree::splay::SplayTree;
        using date_structure::tree::splay::splayTreeInsert;
        using date_structure::tree::splay::splayTreeSplay;

        date_structure::tree::splay::Output output;
        SplayTree root = nullptr;
        constexpr int arraySize = 6;
        constexpr std::array<int, arraySize> array = {10, 50, 40, 30, 20, 60};

        output.flush() << "insert: ";
        for (int i = 0; i < arraySize; ++i)
        {
            output.flush() << array.at(i) << " ";
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
        output.flush() << "\ntree verbose: " << std::endl;
        output.printSplayTree(root, root->key, 0);

        constexpr int splayNode = 30;
        output.flush() << "splay node as root node: " << splayNode;
        output.flush() << "\ntree verbose: " << std::endl;
        root = splayTreeSplay(root, splayNode);
        output.printSplayTree(root, root->key, 0);

        destroySplayTree(root);

        std::ostringstream ret = std::ostringstream(output.flush().str());
        return ret;
    }
};

//! @brief Structure of tree.
class TreeStructure
{
public:
    //! @brief Construct a new TreeStructure object.
    TreeStructure();
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
extern void runTree(const std::vector<std::string>& targets);
extern void updateTreeTask(const std::string& target);
} // namespace application::app_ds
