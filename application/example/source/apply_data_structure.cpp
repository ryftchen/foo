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
//! @brief Get the bit flags of the instance (category) in data structure tasks.
#define APP_DS_GET_BIT(category)                                                                                 \
    std::invoke(                                                                                                 \
        utility::reflection::TypeInfo<DataStructureTask>::fields.find(REFLECTION_STR(toString(category))).value, \
        getTask())
//! @brief Get the alias of the instance (category) in data structure tasks.
#define APP_DS_GET_ALIAS(category)                                                                            \
    ({                                                                                                        \
        constexpr auto attr =                                                                                 \
            utility::reflection::TypeInfo<DataStructureTask>::fields.find(REFLECTION_STR(toString(category))) \
                .attrs.find(REFLECTION_STR("alias"));                                                         \
        static_assert(attr.hasValue);                                                                         \
        attr.value;                                                                                           \
    })

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
    static const auto curried =
        utility::currying::curry(command::presetTaskName, utility::reflection::TypeInfo<DataStructureTask>::name);
    return curried;
}

//! @brief Mapping table for enum and string about data structure tasks. X macro.
#define CATEGORY_TABLE     \
    ELEM(linear, "linear") \
    ELEM(tree, "tree")

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {CATEGORY_TABLE};
#undef ELEM
    return table[cat];
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
    constexpr auto category = Category::linear;
    const auto& bit = APP_DS_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_DS_PRINT_TASK_BEGIN_TITLE(category);
    using linear::LinearStructure;
    using utility::common::operator""_bkdrHash;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<LinearInstance>::value)));
    const auto linearFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_DS_GET_ALIAS(category));

    std::cout << "\r\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<LinearInstance>::value; ++i)
    {
        if (!bit.test(LinearInstance(i)))
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
                LOG_INF << "Execute to apply an unknown " << toString(category) << " instance.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(category);
}

//! @brief Update linear instances in tasks.
//! @param target - target instance
void updateLinearTask(const std::string& target)
{
    constexpr auto category = Category::linear;
    auto& bit = APP_DS_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "lin"_bkdrHash:
            bit.set(LinearInstance::linkedList);
            break;
        case "sta"_bkdrHash:
            bit.set(LinearInstance::stack);
            break;
        case "que"_bkdrHash:
            bit.set(LinearInstance::queue);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
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
    constexpr auto category = Category::tree;
    const auto& bit = APP_DS_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_DS_PRINT_TASK_BEGIN_TITLE(category);
    using tree::TreeStructure;
    using utility::common::operator""_bkdrHash;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<TreeInstance>::value)));
    const auto treeFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_DS_GET_ALIAS(category));

    std::cout << "\r\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<TreeInstance>::value; ++i)
    {
        if (!bit.test(TreeInstance(i)))
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
                LOG_INF << "Execute to apply an unknown " << toString(category) << " instance.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(category);
}

//! @brief Update tree instances in tasks.
//! @param target - target instance
void updateTreeTask(const std::string& target)
{
    constexpr auto category = Category::tree;
    auto& bit = APP_DS_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "bin"_bkdrHash:
            bit.set(TreeInstance::binarySearch);
            break;
        case "ade"_bkdrHash:
            bit.set(TreeInstance::adelsonVelskyLandis);
            break;
        case "spl"_bkdrHash:
            bit.set(TreeInstance::splay);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

#undef CATEGORY_TABLE
} // namespace application::app_ds
