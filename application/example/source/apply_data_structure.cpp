//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_data_structure.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <iomanip>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "application/option/include/register_data_structure.hpp"

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
using Category = ApplyDataStructure::Category;
using reg_ds::taskNameCurried, reg_ds::toString, reg_ds::getCategoryOpts, reg_ds::getCategoryAlias, reg_ds::abbrVal;

//! @brief Get the data structure choice manager.
//! @return reference of the ApplyDataStructure object
ApplyDataStructure& manager()
{
    static ApplyDataStructure manager{};
    return manager;
}

//! @brief Get the title of a particular instance in data structure choices.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <typename T>
static std::string getTitle(const T instance)
{
    std::string title(toString(instance));
    title.at(0) = std::toupper(title.at(0));

    return title;
}

// clang-format off
//! @brief Mapping table for enum and string about linear instances. X macro.
#define APP_DS_LINEAR_INSTANCE_TABLE \
    ELEM(linkedList, "linkedList")   \
    ELEM(stack     , "stack"     )   \
    ELEM(queue     , "queue"     )
// clang-format on
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of LinearInstance enum
//! @return instance name
static constexpr std::string_view toString(const LinearInstance instance)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DS_LINEAR_INSTANCE_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<LinearInstance>::value);
    return table[instance];
//! @endcond
#undef ELEM
}
#undef APP_DS_LINEAR_INSTANCE_TABLE

// clang-format off
//! @brief Mapping table for enum and string about tree instances. X macro.
#define APP_DS_TREE_INSTANCE_TABLE                   \
    ELEM(binarySearch       , "binarySearch"       ) \
    ELEM(adelsonVelskyLandis, "adelsonVelskyLandis") \
    ELEM(splay              , "splay"              )
// clang-format on
//! @brief Convert instance enumeration to string.
//! @param instance - the specific value of TreeInstance enum
//! @return instance name
static constexpr std::string_view toString(const TreeInstance instance)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_DS_TREE_INSTANCE_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<TreeInstance>::value);
    return table[instance];
//! @endcond
#undef ELEM
}
#undef APP_DS_TREE_INSTANCE_TABLE

namespace linear
{
//! @brief Show the contents of the linear result.
//! @param instance - the specific value of LinearInstance enum
//! @param result - linear result
static void showResult(const LinearInstance instance, const std::string_view result)
{
    std::printf("\n==> %-10s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void LinearStructure::linkedListInstance()
try
{
    const auto output = Linear().linkedList();
    showResult(LinearInstance::linkedList, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void LinearStructure::stackInstance()
try
{
    const auto output = Linear().stack();
    showResult(LinearInstance::stack, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void LinearStructure::queueInstance()
try
{
    const auto output = Linear().queue();
    showResult(LinearInstance::queue, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace linear

//! @brief Update linear-related choice.
//! @param target - target instance
template <>
void updateChoice<LinearInstance>(const std::string_view target)
{
    constexpr auto category = Category::linear;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(LinearInstance::linkedList):
            bits.set(LinearInstance::linkedList);
            break;
        case abbrVal(LinearInstance::stack):
            bits.set(LinearInstance::stack);
            break;
        case abbrVal(LinearInstance::queue):
            bits.set(LinearInstance::queue);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}

//! @brief Run linear-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<LinearInstance>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::linear;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DS_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto addTask = [threads, &taskNamer](const std::string_view subTask, void (*targetInstance)())
    { threads->enqueue(taskNamer(subTask), targetInstance); };

    std::cout << "\nInstances of the " << toString<category>() << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using linear::LinearStructure;
            case abbrVal(LinearInstance::linkedList):
                addTask(target, &LinearStructure::linkedListInstance);
                break;
            case abbrVal(LinearInstance::stack):
                addTask(target, &LinearStructure::stackInstance);
                break;
            case abbrVal(LinearInstance::queue):
                addTask(target, &LinearStructure::queueInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(category);
}

namespace tree
{
//! @brief Show the contents of the tree result.
//! @param instance - the specific value of TreeInstance enum
//! @param result - tree result
static void showResult(const TreeInstance instance, const std::string_view result)
{
    std::printf("\n==> %-19s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void TreeStructure::bsInstance()
try
{
    const auto output = Tree().bs();
    showResult(TreeInstance::binarySearch, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void TreeStructure::avlInstance()
try
{
    const auto output = Tree().avl();
    showResult(TreeInstance::adelsonVelskyLandis, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void TreeStructure::splayInstance()
try
{
    const auto output = Tree().splay();
    showResult(TreeInstance::splay, output.str());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace tree

//! @brief Update tree-related choice.
//! @param target - target instance
template <>
void updateChoice<TreeInstance>(const std::string_view target)
{
    constexpr auto category = Category::tree;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(TreeInstance::binarySearch):
            bits.set(TreeInstance::binarySearch);
            break;
        case abbrVal(TreeInstance::adelsonVelskyLandis):
            bits.set(TreeInstance::adelsonVelskyLandis);
            break;
        case abbrVal(TreeInstance::splay):
            bits.set(TreeInstance::splay);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " instance: " + target.data() + '.'};
    }
}

//! @brief Run tree-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<TreeInstance>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::tree;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DS_PRINT_TASK_BEGIN_TITLE(category);
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto addTask = [threads, &taskNamer](const std::string_view subTask, void (*targetInstance)())
    { threads->enqueue(taskNamer(subTask), targetInstance); };

    std::cout << "\nInstances of the " << toString<category>() << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using tree::TreeStructure;
            case abbrVal(TreeInstance::binarySearch):
                addTask(target, &TreeStructure::bsInstance);
                break;
            case abbrVal(TreeInstance::adelsonVelskyLandis):
                addTask(target, &TreeStructure::avlInstance);
                break;
            case abbrVal(TreeInstance::splay):
                addTask(target, &TreeStructure::splayInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_ds
