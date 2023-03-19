//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "apply_data_structure.hpp"
#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER
#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/hash.hpp"

//! @brief Title of printing when data structure tasks are beginning.
#define APP_DS_PRINT_TASK_BEGIN_TITLE(taskType)                                                                       \
    std::cout << "\r\n"                                                                                               \
              << "DATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')                \
              << std::setw(50) << taskType << "BEGIN" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << std::endl;                                                                                           \
    {
//! @brief Title of printing when data structure tasks are ending.
#define APP_DS_PRINT_TASK_END_TITLE(taskType)                                                                       \
    }                                                                                                               \
    std::cout << "\r\n"                                                                                             \
              << "DATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')              \
              << std::setw(50) << taskType << "END" << std::resetiosflags(std::ios_base::left) << std::setfill(' ') \
              << "\n"                                                                                               \
              << std::endl;

namespace application::app_ds
{
//! @brief Alias for Type.
using Type = DataStructureTask::Type;
//! @brief Alias for Bottom.
//! @tparam T - type of specific enum
template <class T>
using Bottom = DataStructureTask::Bottom<T>;
//! @brief Alias for LinearInstance.
using LinearInstance = DataStructureTask::LinearInstance;
//! @brief Alias for TreeInstance.
using TreeInstance = DataStructureTask::TreeInstance;

namespace linear
{
//! @brief Display linear result.
#define LINEAR_RESULT "\r\n*%-10s instance:\n%s"
//! @brief Print linear result content.
#define LINEAR_PRINT_RESULT_CONTENT(method) COMMON_PRINT(LINEAR_RESULT, method, output.str().c_str())

LinearStructure::LinearStructure()
{
    std::cout << "\r\nInstances of the linear structure:" << std::endl;
}

void LinearStructure::linkedListInstance()
{
    try
    {
        const auto output = Linear().linkedList();
        LINEAR_PRINT_RESULT_CONTENT("LinkedList");
    }
    catch (const std::exception& error)
    {
        LOG_ERR("<APPLY DATA STRUCTURE> %s", error.what());
    }
}

void LinearStructure::stackInstance()
{
    try
    {
        const auto output = Linear().stack();
        LINEAR_PRINT_RESULT_CONTENT("Stack");
    }
    catch (const std::exception& error)
    {
        LOG_ERR("<APPLY DATA STRUCTURE> %s", error.what());
    }
}

void LinearStructure::queueInstance()
{
    try
    {
        const auto output = Linear().queue();
        LINEAR_PRINT_RESULT_CONTENT("Queue");
    }
    catch (const std::exception& error)
    {
        LOG_ERR("<APPLY DATA STRUCTURE> %s", error.what());
    }
}
} // namespace linear

//! @brief Run linear tasks.
//! @param targets - vector of target instances
void runLinear(const std::vector<std::string>& targets)
{
    if (getBit<LinearInstance>().none())
    {
        return;
    }

    using linear::LinearStructure;
    using utility::hash::operator""_bkdrHash;

    APP_DS_PRINT_TASK_BEGIN_TITLE(Type::linear);
    auto* threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<uint32_t>(getBit<LinearInstance>().count()), static_cast<uint32_t>(Bottom<LinearInstance>::value)));

    const auto linearFunctor = [&](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    for (uint8_t i = 0; i < Bottom<LinearInstance>::value; ++i)
    {
        if (!getBit<LinearInstance>().test(LinearInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "l_" + targetInstance;
        switch (utility::hash::bkdrHash(targetInstance.data()))
        {
            case "lin"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::linkedListInstance);
                break;
            case "sta"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::stackInstance);
                break;
            case "que"_bkdrHash:
                linearFunctor(threadName, &LinearStructure::queueInstance);
                break;
            default:
                LOG_DBG("<APPLY DATA STRUCTURE> Execute to apply an unknown linear instance.");
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(Type::linear);
}

//! @brief Update linear instances in tasks.
//! @param target - target instance
void updateLinearTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "lin"_bkdrHash:
            setBit<LinearInstance>(LinearInstance::linkedList);
            break;
        case "sta"_bkdrHash:
            setBit<LinearInstance>(LinearInstance::stack);
            break;
        case "que"_bkdrHash:
            setBit<LinearInstance>(LinearInstance::queue);
            break;
        default:
            getBit<LinearInstance>().reset();
            throw std::runtime_error("<APPLY DATA STRUCTURE> Unexpected linear instance: " + target + ".");
    }
}

namespace tree
{
//! @brief Display tree result.
#define TREE_RESULT "\r\n*%-19s instance:\n%s"
//! @brief Print tree result content.
#define TREE_PRINT_RESULT_CONTENT(method) COMMON_PRINT(TREE_RESULT, method, output.str().c_str());

TreeStructure::TreeStructure()
{
    std::cout << "\r\nInstances of the tree structure:" << std::endl;
}

void TreeStructure::bsInstance()
{
    try
    {
        const auto output = Tree().bs();
        TREE_PRINT_RESULT_CONTENT("BinarySearch");
    }
    catch (const std::exception& error)
    {
        LOG_ERR("<APPLY DATA STRUCTURE> %s", error.what());
    }
}

void TreeStructure::avlInstance()
{
    try
    {
        const auto output = Tree().avl();
        TREE_PRINT_RESULT_CONTENT("AdelsonVelskyLandis");
    }
    catch (const std::exception& error)
    {
        LOG_ERR("<APPLY DATA STRUCTURE> %s", error.what());
    }
}

void TreeStructure::splayInstance()
{
    try
    {
        const auto output = Tree().splay();
        TREE_PRINT_RESULT_CONTENT("Splay");
    }
    catch (const std::exception& error)
    {
        LOG_ERR("<APPLY DATA STRUCTURE> %s", error.what());
    }
}
} // namespace tree

//! @brief Run tree tasks.
//! @param targets - vector of target instances
void runTree(const std::vector<std::string>& targets)
{
    if (getBit<TreeInstance>().none())
    {
        return;
    }

    using tree::TreeStructure;
    using utility::hash::operator""_bkdrHash;

    APP_DS_PRINT_TASK_BEGIN_TITLE(Type::tree);
    auto* threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<uint32_t>(getBit<TreeInstance>().count()), static_cast<uint32_t>(Bottom<TreeInstance>::value)));

    const auto treeFunctor = [&](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };

    for (uint8_t i = 0; i < Bottom<TreeInstance>::value; ++i)
    {
        if (!getBit<TreeInstance>().test(TreeInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i), threadName = "t_" + targetInstance;
        switch (utility::hash::bkdrHash(targetInstance.data()))
        {
            case "bin"_bkdrHash:
                treeFunctor(threadName, &TreeStructure::bsInstance);
                break;
            case "ade"_bkdrHash:
                treeFunctor(threadName, &TreeStructure::avlInstance);
                break;
            case "spl"_bkdrHash:
                treeFunctor(threadName, &TreeStructure::splayInstance);
                break;
            default:
                LOG_DBG("<APPLY DATA STRUCTURE> Execute to apply an unknown tree instance.");
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(Type::tree);
}

//! @brief Update tree instances in tasks.
//! @param target - target instance
void updateTreeTask(const std::string& target)
{
    using utility::hash::operator""_bkdrHash;
    switch (utility::hash::bkdrHash(target.c_str()))
    {
        case "bin"_bkdrHash:
            setBit<TreeInstance>(TreeInstance::binarySearch);
            break;
        case "ade"_bkdrHash:
            setBit<TreeInstance>(TreeInstance::adelsonVelskyLandis);
            break;
        case "spl"_bkdrHash:
            setBit<TreeInstance>(TreeInstance::splay);
            break;
        default:
            getBit<TreeInstance>().reset();
            throw std::runtime_error("<APPLY DATA STRUCTURE> Unexpected tree instance: " + target + ".");
    }
}
} // namespace application::app_ds
