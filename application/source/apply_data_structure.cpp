//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "apply_data_structure.hpp"
#include "application/include/command.hpp"
#include "data_structure/include/linear.hpp"
#include "data_structure/include/tree.hpp"
#include "utility/include/hash.hpp"
#include "utility/include/log.hpp"
#include "utility/include/thread.hpp"

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
              << "\r\n"                                                                                             \
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

//! @brief Get the data structure task.
//! @return reference of DataStructureTask object
DataStructureTask& getTask()
{
    static DataStructureTask task;
    return task;
}

//! @brief Run linear tasks.
//! @param targets - vector of target instances
void runLinear(const std::vector<std::string>& targets)
{
    if (getBit<LinearInstance>().none())
    {
        return;
    }

    using date_structure::linear::LinearStructure;
    using utility::hash::operator""_bkdrHash;

    APP_DS_PRINT_TASK_BEGIN_TITLE(Type::linear);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<LinearInstance>().count()), static_cast<uint32_t>(Bottom<LinearInstance>::value)));

    const std::shared_ptr<LinearStructure> structure = std::make_shared<LinearStructure>();
    const auto linearFunctor = [&](const std::string& threadName, void (LinearStructure::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, structure);
    };

    for (int i = 0; i < Bottom<LinearInstance>::value; ++i)
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
                LOG_DBG("execute to run unknown linear instance.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
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
            throw std::runtime_error("Unexpected task of linear: " + target);
    }
}

//! @brief Run tree tasks.
//! @param targets - vector of target instances
void runTree(const std::vector<std::string>& targets)
{
    if (getBit<TreeInstance>().none())
    {
        return;
    }

    using date_structure::tree::TreeStructure;
    using utility::hash::operator""_bkdrHash;

    APP_DS_PRINT_TASK_BEGIN_TITLE(Type::tree);
    auto* threads = command::getMemoryForMultithreading().newElement(std::min(
        static_cast<uint32_t>(getBit<TreeInstance>().count()), static_cast<uint32_t>(Bottom<TreeInstance>::value)));

    const std::shared_ptr<TreeStructure> structure = std::make_shared<TreeStructure>();
    const auto treeFunctor = [&](const std::string& threadName, void (TreeStructure::*instancePtr)() const)
    {
        threads->enqueue(threadName, instancePtr, structure);
    };

    for (int i = 0; i < Bottom<TreeInstance>::value; ++i)
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
                LOG_DBG("execute to run unknown tree instance.");
                break;
        }
    }

    command::getMemoryForMultithreading().deleteElement(threads);
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
            throw std::runtime_error("Unexpected task of tree: " + target);
    }
}
} // namespace application::app_ds
