//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "apply_data_structure.hpp"

#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when data structure tasks are beginning.
#define APP_DS_PRINT_TASK_BEGIN_TITLE(category)                                                               \
    std::osyncstream(std::cout) << "\r\n"                                                                     \
                                << "DATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left)           \
                                << std::setfill('.') << std::setw(50) << category << "BEGIN"                  \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when data structure tasks are ending.
#define APP_DS_PRINT_TASK_END_TITLE(category)                                                           \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\r\n"                                                               \
                                << "DATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left)     \
                                << std::setfill('.') << std::setw(50) << category << "END"              \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n' \
                                << std::endl;

namespace application::app_ds
{
//! @brief Alias for Category.
using Category = DataStructureTask::Category;

//! @brief Get the data structure task.
//! @return reference of the DataStructureTask object
DataStructureTask& getTask()
{
    static DataStructureTask task{};
    return task;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried = utility::currying::curry(command::presetTaskName, "ds");
    return curried;
}

namespace linear
{
//! @brief Display linear result.
#define LINEAR_RESULT "\r\n==> %-10s Instance <==\n%s"
//! @brief Print linear result content.
#define LINEAR_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(LINEAR_RESULT, toString(instance).data(), output.str().c_str())
//! @brief Mapping table for enum and string about linear instances. X macro.
#define LINEAR_INSTANCE_TABLE      \
    ELEM(linkedList, "LinkedList") \
    ELEM(stack, "Stack")           \
    ELEM(queue, "Queue")

//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of LinearInstance enum
//! @return instance name
constexpr std::string_view toString(const LinearInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {LINEAR_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}

void LinearStructure::linkedListInstance()
try
{
    const auto output = Linear().linkedList();
    LINEAR_PRINT_RESULT_CONTENT(LinearInstance::linkedList);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void LinearStructure::stackInstance()
try
{
    const auto output = Linear().stack();
    LINEAR_PRINT_RESULT_CONTENT(LinearInstance::stack);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void LinearStructure::queueInstance()
try
{
    const auto output = Linear().queue();
    LINEAR_PRINT_RESULT_CONTENT(LinearInstance::queue);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef LINEAR_RESULT
#undef LINEAR_PRINT_RESULT_CONTENT
#undef LINEAR_INSTANCE_TABLE
} // namespace linear

//! @brief Run linear tasks.
//! @param targets - container of target instances
void runLinearTasks(const std::vector<std::string>& targets)
{
    if (getBit<LinearInstance>().none())
    {
        return;
    }

    APP_DS_PRINT_TASK_BEGIN_TITLE(Category::linear);
    using linear::LinearStructure;
    using utility::common::operator""_bkdrHash;

    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<LinearInstance>().count()),
        static_cast<std::uint32_t>(Bottom<LinearInstance>::value)));
    const auto linearFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), "l");

    std::cout << "\r\nInstances of the linear structure:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<LinearInstance>::value; ++i)
    {
        if (!getBit<LinearInstance>().test(LinearInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "lin"_bkdrHash:
                linearFunctor(name(targetInstance), &LinearStructure::linkedListInstance);
                break;
            case "sta"_bkdrHash:
                linearFunctor(name(targetInstance), &LinearStructure::stackInstance);
                break;
            case "que"_bkdrHash:
                linearFunctor(name(targetInstance), &LinearStructure::queueInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown linear instance.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(Category::linear);
}

//! @brief Update linear instances in tasks.
//! @param target - target instance
void updateLinearTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected linear instance: " + target + '.');
    }
}

namespace tree
{
//! @brief Display tree result.
#define TREE_RESULT "\r\n==> %-19s Instance <==\n%s"
//! @brief Print tree result content.
#define TREE_PRINT_RESULT_CONTENT(instance) COMMON_PRINT(TREE_RESULT, toString(instance).data(), output.str().c_str());
//! @brief Mapping table for enum and string about tree instances. X macro.
#define TREE_INSTANCE_TABLE                          \
    ELEM(binarySearch, "BinarySearch")               \
    ELEM(adelsonVelskyLandis, "AdelsonVelskyLandis") \
    ELEM(splay, "Splay")

//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of TreeInstance enum
//! @return instance name
constexpr std::string_view toString(const TreeInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {TREE_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}

void TreeStructure::bsInstance()
try
{
    const auto output = Tree().bs();
    TREE_PRINT_RESULT_CONTENT(TreeInstance::binarySearch);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void TreeStructure::avlInstance()
try
{
    const auto output = Tree().avl();
    TREE_PRINT_RESULT_CONTENT(TreeInstance::adelsonVelskyLandis);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void TreeStructure::splayInstance()
try
{
    const auto output = Tree().splay();
    TREE_PRINT_RESULT_CONTENT(TreeInstance::splay);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef TREE_RESULT
#undef TREE_PRINT_RESULT_CONTENT
#undef TREE_INSTANCE_TABLE
} // namespace tree

//! @brief Run tree tasks.
//! @param targets - container of target instances
void runTreeTasks(const std::vector<std::string>& targets)
{
    if (getBit<TreeInstance>().none())
    {
        return;
    }

    APP_DS_PRINT_TASK_BEGIN_TITLE(Category::tree);
    using tree::TreeStructure;
    using utility::common::operator""_bkdrHash;

    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<TreeInstance>().count()),
        static_cast<std::uint32_t>(Bottom<TreeInstance>::value)));
    const auto treeFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), "t");

    std::cout << "\r\nInstances of the tree structure:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<TreeInstance>::value; ++i)
    {
        if (!getBit<TreeInstance>().test(TreeInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = targets.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case "bin"_bkdrHash:
                treeFunctor(name(targetInstance), &TreeStructure::bsInstance);
                break;
            case "ade"_bkdrHash:
                treeFunctor(name(targetInstance), &TreeStructure::avlInstance);
                break;
            case "spl"_bkdrHash:
                treeFunctor(name(targetInstance), &TreeStructure::splayInstance);
                break;
            default:
                LOG_INF << "Execute to apply an unknown tree instance.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(Category::tree);
}

//! @brief Update tree instances in tasks.
//! @param target - target instance
void updateTreeTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
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
            throw std::runtime_error("Unexpected tree instance: " + target + '.');
    }
}
} // namespace application::app_ds
