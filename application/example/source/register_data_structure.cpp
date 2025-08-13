//! @file register_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (register_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "register_data_structure.hpp"
#include "apply_data_structure.hpp"

namespace application::reg_ds
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Get the data structure choice manager.
//! @return reference of the ApplyDataStructure object
ApplyDataStructure& manager() noexcept
{
    static ApplyDataStructure manager{};
    return manager;
}

//! @brief Check whether any data structure choices exist.
//! @return any data structure choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyDataStructure>::forEachVarOf(
        manager(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });

    return isExist;
}

//! @brief Reset bit flags that manage data structure choices.
void clear()
{
    TypeInfo<ApplyDataStructure>::forEachVarOf(manager(), [](const auto /*field*/, auto&& var) { var.reset(); });
}

//! @brief Find the position of bit flags to set a particular instance.
//! @tparam T - type of target instance
//! @param stringify - instance name
//! @return position of bit flags
template <typename T>
static consteval std::size_t findPosition(const std::string_view stringify)
{
    return static_cast<std::size_t>(TypeInfo<T>::fields.template valueOfName<T>(stringify));
}

namespace cache
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_ds::cache::version;
}
} // namespace cache
//! @brief Update cache-related choice.
//! @param target - target instance
template <>
void updateChoice<CacheInstance>(const std::string& target)
{
    constexpr auto category = Category::cache;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(CacheInstance::firstInFirstOut):
            bits.set(findPosition<CacheInstance>(MACRO_STRINGIFY(firstInFirstOut)));
            break;
        case abbrValue(CacheInstance::leastFrequentlyUsed):
            bits.set(findPosition<CacheInstance>(MACRO_STRINGIFY(leastFrequentlyUsed)));
            break;
        case abbrValue(CacheInstance::leastRecentlyUsed):
            bits.set(findPosition<CacheInstance>(MACRO_STRINGIFY(leastRecentlyUsed)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
    }
}
//! @brief Run cache-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<CacheInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingCache(candidates);
}

namespace filter
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_ds::filter::version;
}
} // namespace filter
//! @brief Update filter-related choice.
//! @param target - target instance
template <>
void updateChoice<FilterInstance>(const std::string& target)
{
    constexpr auto category = Category::filter;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(FilterInstance::bloom):
            bits.set(findPosition<FilterInstance>(MACRO_STRINGIFY(bloom)));
            break;
        case abbrValue(FilterInstance::quotient):
            bits.set(findPosition<FilterInstance>(MACRO_STRINGIFY(quotient)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
    }
}
//! @brief Run filter-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<FilterInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingFilter(candidates);
}

namespace graph
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_ds::graph::version;
}
} // namespace graph
//! @brief Update graph-related choice.
//! @param target - target instance
template <>
void updateChoice<GraphInstance>(const std::string& target)
{
    constexpr auto category = Category::graph;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(GraphInstance::undirected):
            bits.set(findPosition<GraphInstance>(MACRO_STRINGIFY(undirected)));
            break;
        case abbrValue(GraphInstance::directed):
            bits.set(findPosition<GraphInstance>(MACRO_STRINGIFY(directed)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
    }
}
//! @brief Run graph-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<GraphInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingGraph(candidates);
}

namespace heap
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_ds::heap::version;
}
} // namespace heap
//! @brief Update heap-related choice.
//! @param target - target instance
template <>
void updateChoice<HeapInstance>(const std::string& target)
{
    constexpr auto category = Category::heap;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(HeapInstance::binary):
            bits.set(findPosition<HeapInstance>(MACRO_STRINGIFY(binary)));
            break;
        case abbrValue(HeapInstance::leftist):
            bits.set(findPosition<HeapInstance>(MACRO_STRINGIFY(leftist)));
            break;
        case abbrValue(HeapInstance::skew):
            bits.set(findPosition<HeapInstance>(MACRO_STRINGIFY(skew)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
    }
}
//! @brief Run heap-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<HeapInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingHeap(candidates);
}

namespace linear
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_ds::linear::version;
}
} // namespace linear
//! @brief Update linear-related choice.
//! @param target - target instance
template <>
void updateChoice<LinearInstance>(const std::string& target)
{
    constexpr auto category = Category::linear;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(LinearInstance::doublyLinkedList):
            bits.set(findPosition<LinearInstance>(MACRO_STRINGIFY(doublyLinkedList)));
            break;
        case abbrValue(LinearInstance::stack):
            bits.set(findPosition<LinearInstance>(MACRO_STRINGIFY(stack)));
            break;
        case abbrValue(LinearInstance::queue):
            bits.set(findPosition<LinearInstance>(MACRO_STRINGIFY(queue)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
    }
}
//! @brief Run linear-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<LinearInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingLinear(candidates);
}

namespace tree
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_ds::tree::version;
}
} // namespace tree
//! @brief Update tree-related choice.
//! @param target - target instance
template <>
void updateChoice<TreeInstance>(const std::string& target)
{
    constexpr auto category = Category::tree;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(TreeInstance::binarySearch):
            bits.set(findPosition<TreeInstance>(MACRO_STRINGIFY(binarySearch)));
            break;
        case abbrValue(TreeInstance::adelsonVelskyLandis):
            bits.set(findPosition<TreeInstance>(MACRO_STRINGIFY(adelsonVelskyLandis)));
            break;
        case abbrValue(TreeInstance::splay):
            bits.set(findPosition<TreeInstance>(MACRO_STRINGIFY(splay)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " instance: " + target + '.'};
    }
}
//! @brief Run tree-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<TreeInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingTree(candidates);
}
} // namespace application::reg_ds
