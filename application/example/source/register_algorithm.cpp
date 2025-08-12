//! @file register_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (register_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "register_algorithm.hpp"
#include "apply_algorithm.hpp"

namespace application::reg_algo
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Get the algorithm choice manager.
//! @return reference of the ApplyAlgorithm object
ApplyAlgorithm& manager() noexcept
{
    static ApplyAlgorithm manager{};
    return manager;
}

//! @brief Check whether any algorithm choices exist.
//! @return any design pattern choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyAlgorithm>::forEachVarOf(
        manager(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });

    return isExist;
}

//! @brief Reset bit flags that manage algorithm choices.
void clear()
{
    TypeInfo<ApplyAlgorithm>::forEachVarOf(manager(), [](const auto /*field*/, auto&& var) { var.reset(); });
}

namespace match
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_algo::match::version;
}
} // namespace match
//! @brief Update match-related choice.
//! @param target - target method
template <>
void updateChoice<MatchMethod>(const std::string& target)
{
    constexpr auto category = Category::match;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(MatchMethod::rabinKarp):
            bits.set(MatchMethod::rabinKarp);
            break;
        case abbrValue(MatchMethod::knuthMorrisPratt):
            bits.set(MatchMethod::knuthMorrisPratt);
            break;
        case abbrValue(MatchMethod::boyerMoore):
            bits.set(MatchMethod::boyerMoore);
            break;
        case abbrValue(MatchMethod::horspool):
            bits.set(MatchMethod::horspool);
            break;
        case abbrValue(MatchMethod::sunday):
            bits.set(MatchMethod::sunday);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
    }
}
//! @brief Run match-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<MatchMethod>(const std::vector<std::string>& candidates)
{
    app_algo::applyingMatch(candidates);
}

namespace notation
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_algo::notation::version;
}
} // namespace notation
//! @brief Update notation-related choice.
//! @param target - target method
template <>
void updateChoice<NotationMethod>(const std::string& target)
{
    constexpr auto category = Category::notation;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(NotationMethod::prefix):
            bits.set(NotationMethod::prefix);
            break;
        case abbrValue(NotationMethod::postfix):
            bits.set(NotationMethod::postfix);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
    }
}
//! @brief Run notation-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<NotationMethod>(const std::vector<std::string>& candidates)
{
    app_algo::applyingNotation(candidates);
}

namespace optimal
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_algo::optimal::version;
}
} // namespace optimal
//! @brief Update optimal-related choice.
//! @param target - target method
template <>
void updateChoice<OptimalMethod>(const std::string& target)
{
    constexpr auto category = Category::optimal;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(OptimalMethod::gradient):
            bits.set(OptimalMethod::gradient);
            break;
        case abbrValue(OptimalMethod::tabu):
            bits.set(OptimalMethod::tabu);
            break;
        case abbrValue(OptimalMethod::annealing):
            bits.set(OptimalMethod::annealing);
            break;
        case abbrValue(OptimalMethod::particle):
            bits.set(OptimalMethod::particle);
            break;
        case abbrValue(OptimalMethod::ant):
            bits.set(OptimalMethod::ant);
            break;
        case abbrValue(OptimalMethod::genetic):
            bits.set(OptimalMethod::genetic);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
    }
}
//! @brief Run optimal-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<OptimalMethod>(const std::vector<std::string>& candidates)
{
    app_algo::applyingOptimal(candidates);
}

namespace search
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_algo::search::version;
}
} // namespace search
//! @brief Update search-related choice.
//! @param target - target method
template <>
void updateChoice<SearchMethod>(const std::string& target)
{
    constexpr auto category = Category::search;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(SearchMethod::binary):
            bits.set(SearchMethod::binary);
            break;
        case abbrValue(SearchMethod::interpolation):
            bits.set(SearchMethod::interpolation);
            break;
        case abbrValue(SearchMethod::fibonacci):
            bits.set(SearchMethod::fibonacci);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
    }
}
//! @brief Run search-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<SearchMethod>(const std::vector<std::string>& candidates)
{
    app_algo::applyingSearch(candidates);
}

namespace sort
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_algo::sort::version;
}
} // namespace sort
//! @brief Update sort-related choice.
//! @param target - target method
template <>
void updateChoice<SortMethod>(const std::string& target)
{
    constexpr auto category = Category::sort;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(SortMethod::bubble):
            bits.set(SortMethod::bubble);
            break;
        case abbrValue(SortMethod::selection):
            bits.set(SortMethod::selection);
            break;
        case abbrValue(SortMethod::insertion):
            bits.set(SortMethod::insertion);
            break;
        case abbrValue(SortMethod::shell):
            bits.set(SortMethod::shell);
            break;
        case abbrValue(SortMethod::merge):
            bits.set(SortMethod::merge);
            break;
        case abbrValue(SortMethod::quick):
            bits.set(SortMethod::quick);
            break;
        case abbrValue(SortMethod::heap):
            bits.set(SortMethod::heap);
            break;
        case abbrValue(SortMethod::counting):
            bits.set(SortMethod::counting);
            break;
        case abbrValue(SortMethod::bucket):
            bits.set(SortMethod::bucket);
            break;
        case abbrValue(SortMethod::radix):
            bits.set(SortMethod::radix);
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
    }
}
//! @brief Run sort-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<SortMethod>(const std::vector<std::string>& candidates)
{
    app_algo::applyingSort(candidates);
}
} // namespace application::reg_algo
