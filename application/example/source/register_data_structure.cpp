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
            bits.set(CacheInstance::firstInFirstOut);
            break;
        case abbrValue(CacheInstance::leastFrequentlyUsed):
            bits.set(CacheInstance::leastFrequentlyUsed);
            break;
        case abbrValue(CacheInstance::leastRecentlyUsed):
            bits.set(CacheInstance::leastRecentlyUsed);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " instance: " + target + '.'};
    }
}
//! @brief Run cache-related choices.
//! @param candidates - container for the candidate target instances
template <>
void runChoices<CacheInstance>(const std::vector<std::string>& candidates)
{
    app_ds::applyingCache(candidates);
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
        case abbrValue(LinearInstance::linkedList):
            bits.set(LinearInstance::linkedList);
            break;
        case abbrValue(LinearInstance::stack):
            bits.set(LinearInstance::stack);
            break;
        case abbrValue(LinearInstance::queue):
            bits.set(LinearInstance::queue);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " instance: " + target + '.'};
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
            bits.set(TreeInstance::binarySearch);
            break;
        case abbrValue(TreeInstance::adelsonVelskyLandis):
            bits.set(TreeInstance::adelsonVelskyLandis);
            break;
        case abbrValue(TreeInstance::splay):
            bits.set(TreeInstance::splay);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " instance: " + target + '.'};
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
