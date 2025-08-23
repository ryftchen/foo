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

namespace manage
{
//! @brief Get the data structure choice applier.
//! @return reference of the ApplyDataStructure object
ApplyDataStructure& choiceApplier()
{
    static ApplyDataStructure applier{};
    return applier;
}

//! @brief Check whether any data structure choices exist.
//! @return any data structure choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyDataStructure>::forEachVarOf(
        choiceApplier(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });

    return isExist;
}
//! @brief Reset bit flags that manage data structure choices.
void clear()
{
    TypeInfo<ApplyDataStructure>::forEachVarOf(choiceApplier(), [](const auto /*field*/, auto&& var) { var.reset(); });
}
} // namespace manage

//! @brief Find the position of bit flags to set a particular instance.
//! @tparam T - type of target instance
//! @param stringify - instance name
//! @return position of bit flags
template <typename T>
static consteval std::size_t mappedPos(const std::string_view stringify)
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
        case abbrLitHash(CacheInstance::firstInFirstOut):
            bits.set(mappedPos<CacheInstance>(MACRO_STRINGIFY(firstInFirstOut)));
            break;
        case abbrLitHash(CacheInstance::leastFrequentlyUsed):
            bits.set(mappedPos<CacheInstance>(MACRO_STRINGIFY(leastFrequentlyUsed)));
            break;
        case abbrLitHash(CacheInstance::leastRecentlyUsed):
            bits.set(mappedPos<CacheInstance>(MACRO_STRINGIFY(leastRecentlyUsed)));
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
        case abbrLitHash(FilterInstance::bloom):
            bits.set(mappedPos<FilterInstance>(MACRO_STRINGIFY(bloom)));
            break;
        case abbrLitHash(FilterInstance::quotient):
            bits.set(mappedPos<FilterInstance>(MACRO_STRINGIFY(quotient)));
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
        case abbrLitHash(GraphInstance::undirected):
            bits.set(mappedPos<GraphInstance>(MACRO_STRINGIFY(undirected)));
            break;
        case abbrLitHash(GraphInstance::directed):
            bits.set(mappedPos<GraphInstance>(MACRO_STRINGIFY(directed)));
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
        case abbrLitHash(HeapInstance::binary):
            bits.set(mappedPos<HeapInstance>(MACRO_STRINGIFY(binary)));
            break;
        case abbrLitHash(HeapInstance::leftist):
            bits.set(mappedPos<HeapInstance>(MACRO_STRINGIFY(leftist)));
            break;
        case abbrLitHash(HeapInstance::skew):
            bits.set(mappedPos<HeapInstance>(MACRO_STRINGIFY(skew)));
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
        case abbrLitHash(LinearInstance::doublyLinkedList):
            bits.set(mappedPos<LinearInstance>(MACRO_STRINGIFY(doublyLinkedList)));
            break;
        case abbrLitHash(LinearInstance::stack):
            bits.set(mappedPos<LinearInstance>(MACRO_STRINGIFY(stack)));
            break;
        case abbrLitHash(LinearInstance::queue):
            bits.set(mappedPos<LinearInstance>(MACRO_STRINGIFY(queue)));
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
        case abbrLitHash(TreeInstance::binarySearch):
            bits.set(mappedPos<TreeInstance>(MACRO_STRINGIFY(binarySearch)));
            break;
        case abbrLitHash(TreeInstance::adelsonVelskyLandis):
            bits.set(mappedPos<TreeInstance>(MACRO_STRINGIFY(adelsonVelskyLandis)));
            break;
        case abbrLitHash(TreeInstance::splay):
            bits.set(mappedPos<TreeInstance>(MACRO_STRINGIFY(splay)));
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
