//! @file register_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (register_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

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

namespace manage
{
//! @brief Get the algorithm choice applier.
//! @return reference of the ApplyAlgorithm object
ApplyAlgorithm& choiceApplier()
{
    static utility::common::NoDestructor<ApplyAlgorithm> applier{};
    return *applier.get();
}

//! @brief Check whether any algorithm choices exist.
//! @return any design pattern choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyAlgorithm>::forEachVarOf(
        choiceApplier(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });
    return isExist;
}
//! @brief Reset bit flags that manage algorithm choices.
void clear()
{
    TypeInfo<ApplyAlgorithm>::forEachVarOf(choiceApplier(), [](const auto /*field*/, auto&& var) { var.reset(); });
}
} // namespace manage

//! @brief Find the position of bit flags to set a particular method.
//! @tparam Meth - type of target method
//! @param stringify - method name
//! @return position of bit flags
template <typename Meth>
static consteval std::size_t mappedPos(const std::string_view stringify)
{
    return static_cast<std::underlying_type_t<Meth>>(TypeInfo<Meth>::fields.template valueOfName<Meth>(stringify));
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
//! @brief Set the match-related choice.
//! @param choice - target choice
template <>
void setChoice<MatchMethod>(const std::string& choice)
{
    constexpr auto category = Category::match;
    auto& specs = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(MatchMethod::rabinKarp):
            specs.set(mappedPos<MatchMethod>(MACRO_STRINGIFY(rabinKarp)));
            break;
        case abbrLitHash(MatchMethod::knuthMorrisPratt):
            specs.set(mappedPos<MatchMethod>(MACRO_STRINGIFY(knuthMorrisPratt)));
            break;
        case abbrLitHash(MatchMethod::boyerMoore):
            specs.set(mappedPos<MatchMethod>(MACRO_STRINGIFY(boyerMoore)));
            break;
        case abbrLitHash(MatchMethod::horspool):
            specs.set(mappedPos<MatchMethod>(MACRO_STRINGIFY(horspool)));
            break;
        case abbrLitHash(MatchMethod::sunday):
            specs.set(mappedPos<MatchMethod>(MACRO_STRINGIFY(sunday)));
            break;
        default:
            specs.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run the match-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<MatchMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set the notation-related choice.
//! @param choice - target choice
template <>
void setChoice<NotationMethod>(const std::string& choice)
{
    constexpr auto category = Category::notation;
    auto& specs = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(NotationMethod::prefix):
            specs.set(mappedPos<NotationMethod>(MACRO_STRINGIFY(prefix)));
            break;
        case abbrLitHash(NotationMethod::postfix):
            specs.set(mappedPos<NotationMethod>(MACRO_STRINGIFY(postfix)));
            break;
        default:
            specs.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run the notation-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<NotationMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set the optimal-related choice.
//! @param choice - target choice
template <>
void setChoice<OptimalMethod>(const std::string& choice)
{
    constexpr auto category = Category::optimal;
    auto& specs = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(OptimalMethod::gradient):
            specs.set(mappedPos<OptimalMethod>(MACRO_STRINGIFY(gradient)));
            break;
        case abbrLitHash(OptimalMethod::tabu):
            specs.set(mappedPos<OptimalMethod>(MACRO_STRINGIFY(tabu)));
            break;
        case abbrLitHash(OptimalMethod::annealing):
            specs.set(mappedPos<OptimalMethod>(MACRO_STRINGIFY(annealing)));
            break;
        case abbrLitHash(OptimalMethod::particle):
            specs.set(mappedPos<OptimalMethod>(MACRO_STRINGIFY(particle)));
            break;
        case abbrLitHash(OptimalMethod::ant):
            specs.set(mappedPos<OptimalMethod>(MACRO_STRINGIFY(ant)));
            break;
        case abbrLitHash(OptimalMethod::genetic):
            specs.set(mappedPos<OptimalMethod>(MACRO_STRINGIFY(genetic)));
            break;
        default:
            specs.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run the optimal-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<OptimalMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set the search-related choice.
//! @param choice - target choice
template <>
void setChoice<SearchMethod>(const std::string& choice)
{
    constexpr auto category = Category::search;
    auto& specs = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(SearchMethod::binary):
            specs.set(mappedPos<SearchMethod>(MACRO_STRINGIFY(binary)));
            break;
        case abbrLitHash(SearchMethod::interpolation):
            specs.set(mappedPos<SearchMethod>(MACRO_STRINGIFY(interpolation)));
            break;
        case abbrLitHash(SearchMethod::fibonacci):
            specs.set(mappedPos<SearchMethod>(MACRO_STRINGIFY(fibonacci)));
            break;
        default:
            specs.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run the search-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<SearchMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set the sort-related choice.
//! @param choice - target choice
template <>
void setChoice<SortMethod>(const std::string& choice)
{
    constexpr auto category = Category::sort;
    auto& specs = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(SortMethod::bubble):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(bubble)));
            break;
        case abbrLitHash(SortMethod::selection):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(selection)));
            break;
        case abbrLitHash(SortMethod::insertion):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(insertion)));
            break;
        case abbrLitHash(SortMethod::shell):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(shell)));
            break;
        case abbrLitHash(SortMethod::merge):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(merge)));
            break;
        case abbrLitHash(SortMethod::quick):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(quick)));
            break;
        case abbrLitHash(SortMethod::heap):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(heap)));
            break;
        case abbrLitHash(SortMethod::counting):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(counting)));
            break;
        case abbrLitHash(SortMethod::bucket):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(bucket)));
            break;
        case abbrLitHash(SortMethod::radix):
            specs.set(mappedPos<SortMethod>(MACRO_STRINGIFY(radix)));
            break;
        default:
            specs.reset();
            throw std::runtime_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run the sort-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<SortMethod>(const std::vector<std::string>& candidates)
{
    app_algo::applyingSort(candidates);
}
} // namespace application::reg_algo
