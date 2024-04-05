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
//! @brief Get the title of a particular instance in data structure tasks.
#define APP_DS_GET_TITLE(instance)                           \
    ({                                                       \
        std::string title = std::string{toString(instance)}; \
        title.at(0) = std::toupper(title.at(0));             \
        title;                                               \
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

//! @brief Case value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return case value
template <class T>
constexpr std::size_t caseValue(const T instance)
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    static_assert(TypeInfo::fields.size == Bottom<T>::value);

    std::size_t value = 0;
    TypeInfo::fields.forEach(
        [instance, &value](auto field)
        {
            if (field.name == toString(instance))
            {
                static_assert(1 == field.attrs.size);
                auto attr = field.attrs.find(REFLECTION_STR("task"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });
    return value;
}

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::linear:
            return utility::reflection::TypeInfo<LinearInstance>::name;
        case Category::tree:
            return utility::reflection::TypeInfo<TreeInstance>::name;
    }
}

//! @brief Mapping table for enum and string about linear instances. X macro.
#define APP_DS_LINEAR_INSTANCE_TABLE \
    ELEM(linkedList, "linkedList")   \
    ELEM(stack, "stack")             \
    ELEM(queue, "queue")
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of LinearInstance enum
//! @return instance name
constexpr std::string_view toString(const LinearInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DS_LINEAR_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}
#undef APP_DS_LINEAR_INSTANCE_TABLE

//! @brief Mapping table for enum and string about tree instances. X macro.
#define APP_DS_TREE_INSTANCE_TABLE                   \
    ELEM(binarySearch, "binarySearch")               \
    ELEM(adelsonVelskyLandis, "adelsonVelskyLandis") \
    ELEM(splay, "splay")
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of TreeInstance enum
//! @return instance name
constexpr std::string_view toString(const TreeInstance instance)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DS_TREE_INSTANCE_TABLE};
#undef ELEM
    return table[instance];
}
#undef APP_DS_TREE_INSTANCE_TABLE

namespace linear
{
//! @brief Display linear result.
#define LINEAR_RESULT "\r\n==> %-10s Instance <==\n%s"
//! @brief Print linear result content.
#define LINEAR_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(LINEAR_RESULT, APP_DS_GET_TITLE(instance).data(), output.str().c_str())

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
            case caseValue(LinearInstance::linkedList):
                linearFunctor(name(targetInstance), &LinearStructure::linkedListInstance);
                break;
            case caseValue(LinearInstance::stack):
                linearFunctor(name(targetInstance), &LinearStructure::stackInstance);
                break;
            case caseValue(LinearInstance::queue):
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

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(LinearInstance::linkedList):
            bit.set(LinearInstance::linkedList);
            break;
        case caseValue(LinearInstance::stack):
            bit.set(LinearInstance::stack);
            break;
        case caseValue(LinearInstance::queue):
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
#define TREE_PRINT_RESULT_CONTENT(instance) \
    COMMON_PRINT(TREE_RESULT, APP_DS_GET_TITLE(instance).data(), output.str().c_str())

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
            case caseValue(TreeInstance::binarySearch):
                treeFunctor(name(targetInstance), &TreeStructure::bsInstance);
                break;
            case caseValue(TreeInstance::adelsonVelskyLandis):
                treeFunctor(name(targetInstance), &TreeStructure::avlInstance);
                break;
            case caseValue(TreeInstance::splay):
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

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(TreeInstance::binarySearch):
            bit.set(TreeInstance::binarySearch);
            break;
        case caseValue(TreeInstance::adelsonVelskyLandis):
            bit.set(TreeInstance::adelsonVelskyLandis);
            break;
        case caseValue(TreeInstance::splay):
            bit.set(TreeInstance::splay);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}
} // namespace application::app_ds
