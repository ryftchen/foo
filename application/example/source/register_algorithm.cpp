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
ApplyAlgorithm& manager()
{
    static ApplyAlgorithm manager{};
    return manager;
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
void updateChoice<MatchMethod>(const std::string_view target)
{
    constexpr auto category = Category::match;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(MatchMethod::rabinKarp):
            bits.set(MatchMethod::rabinKarp);
            break;
        case abbrVal(MatchMethod::knuthMorrisPratt):
            bits.set(MatchMethod::knuthMorrisPratt);
            break;
        case abbrVal(MatchMethod::boyerMoore):
            bits.set(MatchMethod::boyerMoore);
            break;
        case abbrVal(MatchMethod::horspool):
            bits.set(MatchMethod::horspool);
            break;
        case abbrVal(MatchMethod::sunday):
            bits.set(MatchMethod::sunday);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
void updateChoice<NotationMethod>(const std::string_view target)
{
    constexpr auto category = Category::notation;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(NotationMethod::prefix):
            bits.set(NotationMethod::prefix);
            break;
        case abbrVal(NotationMethod::postfix):
            bits.set(NotationMethod::postfix);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
void updateChoice<OptimalMethod>(const std::string_view target)
{
    constexpr auto category = Category::optimal;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(OptimalMethod::gradient):
            bits.set(OptimalMethod::gradient);
            break;
        case abbrVal(OptimalMethod::tabu):
            bits.set(OptimalMethod::tabu);
            break;
        case abbrVal(OptimalMethod::annealing):
            bits.set(OptimalMethod::annealing);
            break;
        case abbrVal(OptimalMethod::particle):
            bits.set(OptimalMethod::particle);
            break;
        case abbrVal(OptimalMethod::ant):
            bits.set(OptimalMethod::ant);
            break;
        case abbrVal(OptimalMethod::genetic):
            bits.set(OptimalMethod::genetic);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
void updateChoice<SearchMethod>(const std::string_view target)
{
    constexpr auto category = Category::search;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(SearchMethod::binary):
            bits.set(SearchMethod::binary);
            break;
        case abbrVal(SearchMethod::interpolation):
            bits.set(SearchMethod::interpolation);
            break;
        case abbrVal(SearchMethod::fibonacci):
            bits.set(SearchMethod::fibonacci);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
void updateChoice<SortMethod>(const std::string_view target)
{
    constexpr auto category = Category::sort;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(SortMethod::bubble):
            bits.set(SortMethod::bubble);
            break;
        case abbrVal(SortMethod::selection):
            bits.set(SortMethod::selection);
            break;
        case abbrVal(SortMethod::insertion):
            bits.set(SortMethod::insertion);
            break;
        case abbrVal(SortMethod::shell):
            bits.set(SortMethod::shell);
            break;
        case abbrVal(SortMethod::merge):
            bits.set(SortMethod::merge);
            break;
        case abbrVal(SortMethod::quick):
            bits.set(SortMethod::quick);
            break;
        case abbrVal(SortMethod::heap):
            bits.set(SortMethod::heap);
            break;
        case abbrVal(SortMethod::counting):
            bits.set(SortMethod::counting);
            break;
        case abbrVal(SortMethod::bucket):
            bits.set(SortMethod::bucket);
            break;
        case abbrVal(SortMethod::radix):
            bits.set(SortMethod::radix);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
