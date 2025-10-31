//! @file apply_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (apply_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_data_structure.hpp"
#include "register_data_structure.hpp"

#ifndef _PRECOMPILED_HEADER
#include <iomanip>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when data structure tasks are beginning.
#define APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title)                                                            \
    std::osyncstream(std::cout) << "\nAPPLY DATA STRUCTURE: " << std::setiosflags(std::ios_base::left)        \
                                << std::setfill('.') << std::setw(50) << (title) << "BEGIN"                   \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << std::endl; \
    {
//! @brief Title of printing when data structure tasks are ending.
#define APP_DS_PRINT_TASK_TITLE_SCOPE_END(title)                                                        \
    }                                                                                                   \
    std::osyncstream(std::cout) << "\nAPPLY DATA STRUCTURE: " << std::setiosflags(std::ios_base::left)  \
                                << std::setfill('.') << std::setw(50) << (title) << "END"               \
                                << std::resetiosflags(std::ios_base::left) << std::setfill(' ') << '\n' \
                                << std::endl;

namespace application::app_ds
{
using namespace reg_ds; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular instance in data structure choices.
//! @tparam Inst - type of target instance
//! @param instance - target instance
//! @return initial capitalized title
template <typename Inst>
static std::string customTitle(const Inst instance)
{
    std::string title(TypeInfo<Inst>::fields.nameOfValue(instance));
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
//! @tparam Cat - target category
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyDataStructure>::fields.find(REFLECTION_STR(toString(Cat))).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace cache
{
//! @brief Show the contents of the cache result.
//! @param instance - used cache instance
//! @param result - cache result
static void display(const CacheInstance instance, const std::string& result)
{
    std::printf("\n==> %-19s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Structure of cache.
//! @param instance - used cache instance
static void structure(const CacheInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case CacheInstance::firstInFirstOut:
            result = Showcase().fifo();
            break;
        case CacheInstance::leastFrequentlyUsed:
            result = Showcase().lfu();
            break;
        case CacheInstance::leastRecentlyUsed:
            result = Showcase().lru();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace cache
//! @brief To apply cache-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingCache(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::cache;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = data_structure::cache::description();
    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const CacheInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), cache::structure, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(CacheInstance::firstInFirstOut):
                addTask(choice, CacheInstance::firstInFirstOut);
                break;
            case abbrLitHash(CacheInstance::leastFrequentlyUsed):
                addTask(choice, CacheInstance::leastFrequentlyUsed);
                break;
            case abbrLitHash(CacheInstance::leastRecentlyUsed):
                addTask(choice, CacheInstance::leastRecentlyUsed);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace filter
{
//! @brief Show the contents of the filter result.
//! @param instance - used filter instance
//! @param result - filter result
static void display(const FilterInstance instance, const std::string& result)
{
    std::printf("\n==> %-8s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Structure of filter.
//! @param instance - used filter instance
static void structure(const FilterInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case FilterInstance::bloom:
            result = Showcase().bloom();
            break;
        case FilterInstance::quotient:
            result = Showcase().quotient();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace filter
//! @brief To apply filter-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingFilter(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::filter;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = data_structure::filter::description();
    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const FilterInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), filter::structure, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(FilterInstance::bloom):
                addTask(choice, FilterInstance::bloom);
                break;
            case abbrLitHash(FilterInstance::quotient):
                addTask(choice, FilterInstance::quotient);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace graph
{
//! @brief Show the contents of the graph result.
//! @param instance - used graph instance
//! @param result - graph result
static void display(const GraphInstance instance, const std::string& result)
{
    std::printf("\n==> %-10s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Structure of graph.
//! @param instance - used graph instance
static void structure(const GraphInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case GraphInstance::undirected:
            result = Showcase().undirected();
            break;
        case GraphInstance::directed:
            result = Showcase().directed();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace graph
//! @brief To apply graph-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingGraph(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::graph;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = data_structure::graph::description();
    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const GraphInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), graph::structure, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(GraphInstance::undirected):
                addTask(choice, GraphInstance::undirected);
                break;
            case abbrLitHash(GraphInstance::directed):
                addTask(choice, GraphInstance::directed);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace heap
{
//! @brief Show the contents of the heap result.
//! @param instance - used heap instance
//! @param result - heap result
static void display(const HeapInstance instance, const std::string& result)
{
    std::printf("\n==> %-7s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Structure of heap.
//! @param instance - used heap instance
static void structure(const HeapInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case HeapInstance::binary:
            result = Showcase().binary();
            break;
        case HeapInstance::leftist:
            result = Showcase().leftist();
            break;
        case HeapInstance::skew:
            result = Showcase().skew();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace heap
//! @brief To apply heap-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingHeap(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::heap;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = data_structure::heap::description();
    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const HeapInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), heap::structure, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(HeapInstance::binary):
                addTask(choice, HeapInstance::binary);
                break;
            case abbrLitHash(HeapInstance::leftist):
                addTask(choice, HeapInstance::leftist);
                break;
            case abbrLitHash(HeapInstance::skew):
                addTask(choice, HeapInstance::skew);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace linear
{
//! @brief Show the contents of the linear result.
//! @param instance - used linear instance
//! @param result - linear result
static void display(const LinearInstance instance, const std::string& result)
{
    std::printf("\n==> %-16s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Structure of linear.
//! @param instance - used linear instance
static void structure(const LinearInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case LinearInstance::doublyLinkedList:
            result = Showcase().dll();
            break;
        case LinearInstance::stack:
            result = Showcase().stack();
            break;
        case LinearInstance::queue:
            result = Showcase().queue();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace linear
//! @brief To apply linear-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingLinear(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::linear;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = data_structure::linear::description();
    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const LinearInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), linear::structure, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(LinearInstance::doublyLinkedList):
                addTask(choice, LinearInstance::doublyLinkedList);
                break;
            case abbrLitHash(LinearInstance::stack):
                addTask(choice, LinearInstance::stack);
                break;
            case abbrLitHash(LinearInstance::queue):
                addTask(choice, LinearInstance::queue);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace tree
{
//! @brief Show the contents of the tree result.
//! @param instance - used tree instance
//! @param result - tree result
static void display(const TreeInstance instance, const std::string& result)
{
    std::printf("\n==> %-19s Instance <==\n%s", customTitle(instance).c_str(), result.c_str());
}

//! @brief Structure of tree.
//! @param instance - used tree instance
static void structure(const TreeInstance instance)
try
{
    std::ostringstream result{};
    switch (instance)
    {
        static_assert(utility::common::isStatelessClass<Showcase>());
        case TreeInstance::binarySearch:
            result = Showcase().bs();
            break;
        case TreeInstance::adelsonVelskyLandis:
            result = Showcase().avl();
            break;
        case TreeInstance::splay:
            result = Showcase().splay();
            break;
        default:
            return;
    }
    display(instance, result.str());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(instance).c_str(), err.what());
}
} // namespace tree
//! @brief To apply tree-related instances that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingTree(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::tree;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = data_structure::tree::description();
    APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &taskNamer](const std::string_view subTask, const TreeInstance instance)
    { allocatedJob->enqueue(taskNamer(subTask), tree::structure, instance); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    std::cout << "\nInstances of the " << toString(category) << " structure:" << std::endl;
    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(TreeInstance::binarySearch):
                addTask(choice, TreeInstance::binarySearch);
                break;
            case abbrLitHash(TreeInstance::adelsonVelskyLandis):
                addTask(choice, TreeInstance::adelsonVelskyLandis);
                break;
            case abbrLitHash(TreeInstance::splay):
                addTask(choice, TreeInstance::splay);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_DS_PRINT_TASK_TITLE_SCOPE_END(title);
}
} // namespace application::app_ds

#undef APP_DS_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_DS_PRINT_TASK_TITLE_SCOPE_END
