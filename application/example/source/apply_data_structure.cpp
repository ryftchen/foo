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

#include "application/core/include/action.hpp"
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
using Category = ApplyDataStructure::Category;

//! @brief Get the data structure choice manager.
//! @return reference of the ApplyDataStructure object
ApplyDataStructure& manager()
{
    static ApplyDataStructure manager{};
    return manager;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried =
        utility::currying::curry(action::presetTaskName, utility::reflection::TypeInfo<ApplyDataStructure>::name);
    return curried;
}

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
consteval std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::linear:
            return utility::reflection::TypeInfo<LinearInstance>::name;
        case Category::tree:
            return utility::reflection::TypeInfo<TreeInstance>::name;
        default:
            break;
    }

    return {};
}

//! @brief Get the bit flags of the category in data structure choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryOpts()
{
    return std::invoke(
        utility::reflection::TypeInfo<ApplyDataStructure>::fields.find(REFLECTION_STR(toString(Cat))).value, manager());
}

//! @brief Get the alias of the category in data structure choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
consteval std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<ApplyDataStructure>::fields.find(REFLECTION_STR(toString(Cat)))
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
        [instance, &value](const auto field)
        {
            if (field.name == toString(instance))
            {
                static_assert(1 == field.attrs.size);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });

    return value;
}

//! @brief Get the title of a particular instance in data structure choices.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <class T>
std::string getTitle(const T instance)
{
    std::string title(toString(instance));
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
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<LinearInstance>::value);
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
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<TreeInstance>::value);
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
static void displayResult(const LinearInstance instance, const std::string_view result)
{
    COMMON_PRINT("\n==> %-10s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void LinearStructure::linkedListInstance()
try
{
    const auto output = Linear().linkedList().str();
    displayResult(LinearInstance::linkedList, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void LinearStructure::stackInstance()
try
{
    const auto output = Linear().stack().str();
    displayResult(LinearInstance::stack, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void LinearStructure::queueInstance()
try
{
    const auto output = Linear().queue().str();
    displayResult(LinearInstance::queue, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
                "Unexpected " + std::string{toString(category)} + " instance: " + target.data() + '.'};
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
    using linear::LinearStructure;
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto functor = [threads](const std::string_view threadName, void (*targetInstance)())
    { threads->enqueue(threadName, targetInstance); };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());
    auto indices =
        std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index : indices)
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            case abbrVal(LinearInstance::linkedList):
                functor(name(target), &LinearStructure::linkedListInstance);
                break;
            case abbrVal(LinearInstance::stack):
                functor(name(target), &LinearStructure::stackInstance);
                break;
            case abbrVal(LinearInstance::queue):
                functor(name(target), &LinearStructure::queueInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(category);
}

namespace tree
{
//! @brief Display the contents of the tree result.
//! @param instance - the specific value of TreeInstance enum
//! @param result - tree result
static void displayResult(const TreeInstance instance, const std::string_view result)
{
    COMMON_PRINT("\n==> %-19s Instance <==\n%s", getTitle(instance).c_str(), result.data());
}

void TreeStructure::bsInstance()
try
{
    const auto output = Tree().bs().str();
    displayResult(TreeInstance::binarySearch, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void TreeStructure::avlInstance()
try
{
    const auto output = Tree().avl().str();
    displayResult(TreeInstance::adelsonVelskyLandis, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void TreeStructure::splayInstance()
try
{
    const auto output = Tree().splay().str();
    displayResult(TreeInstance::splay, output);
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
                "Unexpected " + std::string{toString(category)} + " instance: " + target.data() + '.'};
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
    using tree::TreeStructure;
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto functor = [threads](const std::string_view threadName, void (*targetInstance)())
    { threads->enqueue(threadName, targetInstance); };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());
    auto indices =
        std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index : indices)
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            case abbrVal(TreeInstance::binarySearch):
                functor(name(target), &TreeStructure::bsInstance);
                break;
            case abbrVal(TreeInstance::adelsonVelskyLandis):
                functor(name(target), &TreeStructure::avlInstance);
                break;
            case abbrVal(TreeInstance::splay):
                functor(name(target), &TreeStructure::splayInstance);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " instance: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_DS_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_ds
