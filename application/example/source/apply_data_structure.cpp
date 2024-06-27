//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 0.1.0
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
    std::osyncstream(std::cout) << "\nDATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left)         \
                                << std::setfill('.') << std::setw(50) << category << "BEGIN"                  \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when data structure tasks are ending.
#define APP_DS_PRINT_TASK_END_TITLE(category)                                                           \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\nDATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left)   \
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
    static const auto curried =
        utility::currying::curry(command::presetTaskName, utility::reflection::TypeInfo<DataStructureTask>::name);
    return curried;
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
        default:
            return "";
    }
}

//! @brief Get the bit flags of the category in data structure tasks.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryBit()
{
    return std::invoke(
        utility::reflection::TypeInfo<DataStructureTask>::fields.find(REFLECTION_STR(toString(Cat))).value, getTask());
}

//! @brief Get the alias of the category in data structure tasks.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
constexpr std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<DataStructureTask>::fields.find(REFLECTION_STR(toString(Cat)))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <class T>
consteval std::size_t abbrVal(const T instance)
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    static_assert(Bottom<T>::value == TypeInfo::fields.size);

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

//! @brief Get the title of a particular instance in data structure tasks.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <class T>
std::string getTitle(const T instance)
{
    std::string title = std::string{toString(instance)};
    title.at(0) = std::toupper(title.at(0));
    return title;
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DS_LINEAR_INSTANCE_TABLE};
    return table[instance];
//! @endcond
#undef ELEM
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DS_TREE_INSTANCE_TABLE};
    return table[instance];
//! @endcond
#undef ELEM
}
#undef APP_DS_TREE_INSTANCE_TABLE

namespace linear
{
//! @brief Display the contents of the linear result.
//! @param instance - the specific value of LinearInstance enum
//! @param result - linear result
static void displayResult(const LinearInstance instance, const std::string& result)
{
    COMMON_PRINT("\n==> %-10s Instance <==\n%s", getTitle(instance).data(), result.data());
}

void LinearStructure::linkedListInstance()
try
{
    const auto& output = Linear().linkedList().str();
    displayResult(LinearInstance::linkedList, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ": " << err.what();
}

void LinearStructure::stackInstance()
try
{
    const auto& output = Linear().stack().str();
    displayResult(LinearInstance::stack, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ": " << err.what();
}

void LinearStructure::queueInstance()
try
{
    const auto& output = Linear().queue().str();
    displayResult(LinearInstance::queue, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ": " << err.what();
}
} // namespace linear

//! @brief Run linear tasks.
//! @param candidates - container for the candidate target instances
void runLinearTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::linear;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_DS_PRINT_TASK_BEGIN_TITLE(category);
    using linear::LinearStructure;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<LinearInstance>::value)));
    const auto linearFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<LinearInstance>::value; ++i)
    {
        if (!bitFlag.test(LinearInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = candidates.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case abbrVal(LinearInstance::linkedList):
                linearFunctor(name(targetInstance), &LinearStructure::linkedListInstance);
                break;
            case abbrVal(LinearInstance::stack):
                linearFunctor(name(targetInstance), &LinearStructure::stackInstance);
                break;
            case abbrVal(LinearInstance::queue):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(LinearInstance::linkedList):
            bitFlag.set(LinearInstance::linkedList);
            break;
        case abbrVal(LinearInstance::stack):
            bitFlag.set(LinearInstance::stack);
            break;
        case abbrVal(LinearInstance::queue):
            bitFlag.set(LinearInstance::queue);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}

namespace tree
{
//! @brief Display the contents of the tree result.
//! @param instance - the specific value of TreeInstance enum
//! @param result - tree result
static void displayResult(const TreeInstance instance, const std::string& result)
{
    COMMON_PRINT("\n==> %-19s Instance <==\n%s", getTitle(instance).data(), result.data());
}

void TreeStructure::bsInstance()
try
{
    const auto& output = Tree().bs().str();
    displayResult(TreeInstance::binarySearch, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ": " << err.what();
}

void TreeStructure::avlInstance()
try
{
    const auto& output = Tree().avl().str();
    displayResult(TreeInstance::adelsonVelskyLandis, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ": " << err.what();
}

void TreeStructure::splayInstance()
try
{
    const auto& output = Tree().splay().str();
    displayResult(TreeInstance::splay, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ": " << err.what();
}
} // namespace tree

//! @brief Run tree tasks.
//! @param candidates - container for the candidate target instances
void runTreeTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::tree;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_DS_PRINT_TASK_BEGIN_TITLE(category);
    using tree::TreeStructure;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<TreeInstance>::value)));
    const auto treeFunctor = [threads](const std::string& threadName, void (*instancePtr)())
    {
        threads->enqueue(threadName, instancePtr);
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (std::uint8_t i = 0; i < Bottom<TreeInstance>::value; ++i)
    {
        if (!bitFlag.test(TreeInstance(i)))
        {
            continue;
        }

        const std::string targetInstance = candidates.at(i);
        switch (utility::common::bkdrHash(targetInstance.data()))
        {
            case abbrVal(TreeInstance::binarySearch):
                treeFunctor(name(targetInstance), &TreeStructure::bsInstance);
                break;
            case abbrVal(TreeInstance::adelsonVelskyLandis):
                treeFunctor(name(targetInstance), &TreeStructure::avlInstance);
                break;
            case abbrVal(TreeInstance::splay):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(TreeInstance::binarySearch):
            bitFlag.set(TreeInstance::binarySearch);
            break;
        case abbrVal(TreeInstance::adelsonVelskyLandis):
            bitFlag.set(TreeInstance::adelsonVelskyLandis);
            break;
        case abbrVal(TreeInstance::splay):
            bitFlag.set(TreeInstance::splay);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " instance: " + target + '.');
    }
}
} // namespace application::app_ds
