//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_data_structure.hpp"
#include "register_data_structure.hpp"

#ifndef _PRECOMPILED_HEADER
#include <cassert>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when data structure tasks are beginning.
#define APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(category)                                                         \
    std::osyncstream(std::cout) << "\nDATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left)         \
                                << std::setfill('.') << std::setw(50) << (category) << "BEGIN"                \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when data structure tasks are ending.
#define APP_DS_PRINT_TASK_TITLE_SCOPE_END(category)                                                     \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\nDATA STRUCTURE TASK: " << std::setiosflags(std::ios_base::left)   \
                                << std::setfill('.') << std::setw(50) << (category) << "END"            \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n' \
                                << std::endl;

namespace application::app_ds
{
using namespace reg_ds; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular instance in data structure choices.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <typename T>
static std::string makeTitle(const T instance)
{
    std::string title(toString(instance));
    title.at(0) = std::toupper(title.at(0));

    return title;
}

//! @brief Get the curried task name.
//! @return curried task name
static const auto& curriedTaskName()
{
    static const auto curried =
        utility::currying::curry(configure::task::presetName, TypeInfo<ApplyDataStructure>::name);
    return curried;
}

//! @brief Get the alias of the category in data structure choices.
//! @tparam Cat - specific value of Category enum
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyDataStructure>::fields.find(REFLECTION_STR(toString<Cat>())).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace linear
{
//! @brief Show the contents of the linear result.
//! @param instance - specific value of LinearInstance enum
//! @param result - linear result
static void showResult(const LinearInstance instance, const std::string_view result)
{
    std::printf("\n==> %-10s Instance <==\n%s", makeTitle(instance).c_str(), result.data());
}

void LinearStructure::linkedListInstance()
try
{
    const auto output = Showcase().linkedList();
    showResult(LinearInstance::linkedList, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in structure (%s): %s", __func__, err.what());
}

void LinearStructure::stackInstance()
try
{
    const auto output = Showcase().stack();
    showResult(LinearInstance::stack, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in structure (%s): %s", __func__, err.what());
}

void LinearStructure::queueInstance()
try
{
    const auto output = Showcase().queue();
    showResult(LinearInstance::queue, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in structure (%s): %s", __func__, err.what());
}
} // namespace linear
//! @brief To apply linear-related instances.
//! @param candidates - container for the candidate target instances
void applyingLinear(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::linear;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &taskNamer](const std::string_view subTask, void (*targetInstance)())
        { allocatedJob->enqueue(taskNamer(subTask), targetInstance); });
    MACRO_DEFER([&]() { pooling.deleteElement(allocatedJob); });

    std::cout << "\nInstances of the " << toString<category>() << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using linear::LinearStructure;
            static_assert(utility::common::isStatelessClass<LinearStructure>());
            case abbrValue(LinearInstance::linkedList):
                addTask(target, &LinearStructure::linkedListInstance);
                break;
            case abbrValue(LinearInstance::stack):
                addTask(target, &LinearStructure::stackInstance);
                break;
            case abbrValue(LinearInstance::queue):
                addTask(target, &LinearStructure::queueInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace tree
{
//! @brief Show the contents of the tree result.
//! @param instance - specific value of TreeInstance enum
//! @param result - tree result
static void showResult(const TreeInstance instance, const std::string_view result)
{
    std::printf("\n==> %-19s Instance <==\n%s", makeTitle(instance).c_str(), result.data());
}

void TreeStructure::bsInstance()
try
{
    const auto output = Showcase().bs();
    showResult(TreeInstance::binarySearch, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in structure (%s): %s", __func__, err.what());
}

void TreeStructure::avlInstance()
try
{
    const auto output = Showcase().avl();
    showResult(TreeInstance::adelsonVelskyLandis, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in structure (%s): %s", __func__, err.what());
}

void TreeStructure::splayInstance()
try
{
    const auto output = Showcase().splay();
    showResult(TreeInstance::splay, output.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in structure (%s): %s", __func__, err.what());
}
} // namespace tree
//! @brief To apply tree-related instances.
//! @param candidates - container for the candidate target instances
void applyingTree(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::tree;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newElement(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &taskNamer](const std::string_view subTask, void (*targetInstance)())
        { allocatedJob->enqueue(taskNamer(subTask), targetInstance); });
    MACRO_DEFER([&]() { pooling.deleteElement(allocatedJob); });

    std::cout << "\nInstances of the " << toString<category>() << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using tree::TreeStructure;
            static_assert(utility::common::isStatelessClass<TreeStructure>());
            case abbrValue(TreeInstance::binarySearch):
                addTask(target, &TreeStructure::bsInstance);
                break;
            case abbrValue(TreeInstance::adelsonVelskyLandis):
                addTask(target, &TreeStructure::avlInstance);
                break;
            case abbrValue(TreeInstance::splay):
                addTask(target, &TreeStructure::splayInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " instance: " + target + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(category);
}
} // namespace application::app_ds

#undef APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_DS_PRINT_TASK_TITLE_SCOPE_END
