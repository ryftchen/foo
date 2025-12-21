//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_algorithm.hpp"
#include "register_algorithm.hpp"

#ifndef _PRECOMPILED_HEADER
#include <iomanip>
#include <ranges>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/time.hpp"

//! @brief Title of printing when algorithm tasks are beginning.
#define APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(title)                                                                   \
    std::osyncstream(std::cout) << "\nAPPLY ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (title) << "BEGIN" << std::resetiosflags(std::ios_base::left)      \
                                << std::setfill(' ') << std::endl;                                                     \
    {
//! @brief Title of printing when algorithm tasks are ending.
#define APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(title)                                                                     \
    }                                                                                                                  \
    std::osyncstream(std::cout) << "\nAPPLY ALGORITHM: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (title) << "END" << std::resetiosflags(std::ios_base::left)        \
                                << std::setfill(' ') << '\n'                                                           \
                                << std::endl;

namespace application::app_algo
{
using namespace reg_algo; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular method in algorithm choices.
//! @tparam Meth - type of target method
//! @param method - target method
//! @return initial capitalized title
template <typename Meth>
static std::string customTitle(const Meth method)
{
    std::string title(TypeInfo<Meth>::fields.nameOfValue(method));
    title.at(0) = static_cast<char>(std::toupper(title.at(0)));
    return title;
}

//! @brief Get the curried task name.
//! @return curried task name
static const auto& curriedTaskName()
{
    static const auto curried = utility::currying::curry(configure::task::presetName, TypeInfo<ApplyAlgorithm>::name);
    return curried;
}

//! @brief Get the alias of the category in algorithm choices.
//! @tparam Cat - target category
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString(Cat))).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace match
{
//! @brief Show the contents of the match result.
//! @param method - used match method
//! @param result - match result
//! @param pattern - single pattern
//! @param interval - time interval
static void display(
    const MatchMethod method, const std::int64_t result, const unsigned char* const pattern, const double interval)
{
    if (result != -1)
    {
        std::printf(
            "\n==> %-16s Method <==\npattern \"%s\" found starting (1st) at index %ld, run time: %8.5f ms\n",
            customTitle(method).c_str(),
            pattern,
            result,
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-16s Method <==\npattern \"%s\" could not be found, run time: %8.5f ms\n",
            customTitle(method).c_str(),
            pattern,
            interval);
    }
}

//! @brief Solution of match.
//! @param method - used match method
//! @param text - matching text
//! @param pattern - single pattern
//! @param textLen - length of matching text
//! @param patternLen - length of single pattern
static void solution(
    const MatchMethod method,
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    std::int64_t result = 0;
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using algorithm::match::Match;
        case MatchMethod::rabinKarp:
            result = Match().rk(text, pattern, textLen, patternLen);
            break;
        case MatchMethod::knuthMorrisPratt:
            result = Match().kmp(text, pattern, textLen, patternLen);
            break;
        case MatchMethod::boyerMoore:
            result = Match().bm(text, pattern, textLen, patternLen);
            break;
        case MatchMethod::horspool:
            result = Match().horspool(text, pattern, textLen, patternLen);
            break;
        case MatchMethod::sunday:
            result = Match().sunday(text, pattern, textLen, patternLen);
            break;
        default:
            return;
    }
    display(method, result, pattern, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace match
//! @brief To apply match-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingMatch(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::match;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = algorithm::match::description();
    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    using match::InputBuilder, match::input::patternString;
    static_assert(InputBuilder::maxDigit > patternString.length());
    const auto inputData = std::make_shared<InputBuilder>(patternString);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const MatchMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask),
            match::solution,
            method,
            inputData->getMatchingText().get(),
            inputData->getSinglePattern().get(),
            inputData->getTextLength(),
            inputData->getPatternLength());
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(MatchMethod::rabinKarp):
                addTask(choice, MatchMethod::rabinKarp);
                break;
            case abbrLitHash(MatchMethod::knuthMorrisPratt):
                addTask(choice, MatchMethod::knuthMorrisPratt);
                break;
            case abbrLitHash(MatchMethod::boyerMoore):
                addTask(choice, MatchMethod::boyerMoore);
                break;
            case abbrLitHash(MatchMethod::horspool):
                addTask(choice, MatchMethod::horspool);
                break;
            case abbrLitHash(MatchMethod::sunday):
                addTask(choice, MatchMethod::sunday);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace notation
{
//! @brief Show the contents of the notation result.
//! @param method - used notation method
//! @param result - notation result
//! @param descr - notation description
static void display(const NotationMethod method, const std::string& result, const std::string& descr)
{
    std::printf("\n==> %-7s Method <==\n%s: %s\n", customTitle(method).c_str(), descr.c_str(), result.c_str());
}

//! @brief Solution of notation.
//! @param method - used notation method
//! @param infix - infix notation
static void solution(const NotationMethod method, const std::string_view infix)
try
{
    std::string result{};
    std::string descr{};
    switch (method)
    {
        using algorithm::notation::Notation;
        case NotationMethod::prefix:
            result = Notation().prefix(infix);
            descr = "polish notation";
            break;
        case NotationMethod::postfix:
            result = Notation().postfix(infix);
            descr = "reverse polish notation";
            break;
        default:
            return;
    }
    display(method, result, descr);
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace notation
//! @brief To apply notation-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingNotation(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::notation;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = algorithm::notation::description();
    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    using notation::InputBuilder, notation::input::infixString;
    const auto inputData = std::make_shared<InputBuilder>(infixString);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const NotationMethod method)
    { allocatedJob->enqueue(taskNamer(subTask), notation::solution, method, inputData->getInfixNotation()); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(NotationMethod::prefix):
                addTask(choice, NotationMethod::prefix);
                break;
            case abbrLitHash(NotationMethod::postfix):
                addTask(choice, NotationMethod::postfix);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace optimal
{
//! @brief Show the contents of the optimal result.
//! @param method - used optimal method
//! @param result - optimal result
//! @param interval - time interval
static void display(
    const OptimalMethod method, const std::optional<std::tuple<double, double>>& result, const double interval)
{
    if (result.has_value())
    {
        std::printf(
            "\n==> %-9s Method <==\nF(min)=%+.5f X=%+.5f, run time: %8.5f ms\n",
            customTitle(method).c_str(),
            std::get<0>(result.value()),
            std::get<1>(result.value()),
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-9s Method <==\nF(min) could not be found, run time: %8.5f ms\n",
            customTitle(method).c_str(),
            interval);
    }
}

//! @brief Solution of optimal.
//! @param method - used optimal method
//! @param func - target function
//! @param left - left endpoint
//! @param right - right endpoint
static void solution(const OptimalMethod method, const Function& func, const double left, const double right)
try
{
    std::optional<std::tuple<double, double>> result = std::nullopt;
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using namespace algorithm::optimal; // NOLINT(google-build-using-namespace)
        case OptimalMethod::gradient:
            result = Gradient(func)(left, right, epsilon);
            break;
        case OptimalMethod::tabu:
            result = Tabu(func)(left, right, epsilon);
            break;
        case OptimalMethod::annealing:
            result = Annealing(func)(left, right, epsilon);
            break;
        case OptimalMethod::particle:
            result = Particle(func)(left, right, epsilon);
            break;
        case OptimalMethod::ant:
            result = Ant(func)(left, right, epsilon);
            break;
        case OptimalMethod::genetic:
            result = Genetic(func)(left, right, epsilon);
            break;
        default:
            return;
    }
    display(method, result, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace optimal
//! @brief To apply optimal-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingOptimal(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::optimal;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = algorithm::optimal::description();
    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    using optimal::InputBuilder, optimal::input::SphericalBessel, optimal::Function;
    static_assert(algorithm::optimal::epsilon >= std::numeric_limits<double>::epsilon());
    const auto inputData = std::make_shared<InputBuilder>(
        SphericalBessel{}, SphericalBessel::range1, SphericalBessel::range2, SphericalBessel::funcDescr);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const OptimalMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask),
            optimal::solution,
            method,
            inputData->getFunction(),
            inputData->getRanges().first,
            inputData->getRanges().second);
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(OptimalMethod::gradient):
                addTask(choice, OptimalMethod::gradient);
                break;
            case abbrLitHash(OptimalMethod::tabu):
                addTask(choice, OptimalMethod::tabu);
                break;
            case abbrLitHash(OptimalMethod::annealing):
                addTask(choice, OptimalMethod::annealing);
                break;
            case abbrLitHash(OptimalMethod::particle):
                addTask(choice, OptimalMethod::particle);
                break;
            case abbrLitHash(OptimalMethod::ant):
                addTask(choice, OptimalMethod::ant);
                break;
            case abbrLitHash(OptimalMethod::genetic):
                addTask(choice, OptimalMethod::genetic);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace search
{
//! @brief Show the contents of the search result.
//! @param method - used search method
//! @param result - search result
//! @param key - search key
//! @param interval - time interval
static void display(const SearchMethod method, const std::int64_t result, const float key, const double interval)
{
    if (result != -1)
    {
        std::printf(
            "\n==> %-13s Method <==\nfound the key \"%.5f\" that appears in the index %ld, run time: %8.5f ms\n",
            customTitle(method).c_str(),
            key,
            result,
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-13s Method <==\ncould not find the key \"%.5f\", run time: %8.5f ms\n",
            customTitle(method).c_str(),
            key,
            interval);
    }
}

//! @brief Solution of search.
//! @param method - used search method
//! @param array - ordered array to be searched
//! @param length - length of array
//! @param key - search key
static void solution(const SearchMethod method, const float* const array, const std::uint32_t length, const float key)
try
{
    std::int64_t result = 0;
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using algorithm::search::Search;
        case SearchMethod::binary:
            result = Search<float>().binary(array, length, key);
            break;
        case SearchMethod::interpolation:
            result = Search<float>().interpolation(array, length, key);
            break;
        case SearchMethod::fibonacci:
            result = Search<float>().fibonacci(array, length, key);
            break;
        default:
            return;
    }
    display(method, result, key, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace search
//! @brief To apply search-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingSearch(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::search;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = algorithm::search::description();
    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    using search::InputBuilder, search::input::arrayLength, search::input::arrayRangeMin, search::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));
    const auto inputData = std::make_shared<InputBuilder<float>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const SearchMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask),
            search::solution,
            method,
            inputData->getOrderedArray().get(),
            inputData->getLength(),
            inputData->getSearchKey());
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(SearchMethod::binary):
                addTask(choice, SearchMethod::binary);
                break;
            case abbrLitHash(SearchMethod::interpolation):
                addTask(choice, SearchMethod::interpolation);
                break;
            case abbrLitHash(SearchMethod::fibonacci):
                addTask(choice, SearchMethod::fibonacci);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace sort
{
//! @brief Show the contents of the sort result.
//! @param method - used sort method
//! @param result - sort result
//! @param interval - time interval
static void display(const SortMethod method, const std::vector<std::int32_t>& result, const double interval)
{
    const std::uint32_t bufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> fmtBuffer(bufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\n(asc) run time: %8.5f ms\n",
        customTitle(method).c_str(),
        InputBuilder<std::int32_t>::spliceAll(result.data(), result.size(), fmtBuffer.data(), bufferSize + 1),
        interval);
}

//! @brief Solution of sort.
//! @param method - used sort method
//! @param array - array to be sorted
//! @param length - length of array
static void solution(const SortMethod method, const std::int32_t* const array, const std::uint32_t length)
try
{
    std::vector<std::int32_t> result{};
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using algorithm::sort::Sort;
        case SortMethod::bubble:
            result = Sort<std::int32_t>().bubble(array, length);
            break;
        case SortMethod::selection:
            result = Sort<std::int32_t>().selection(array, length);
            break;
        case SortMethod::insertion:
            result = Sort<std::int32_t>().insertion(array, length);
            break;
        case SortMethod::shell:
            result = Sort<std::int32_t>().shell(array, length);
            break;
        case SortMethod::merge:
            result = Sort<std::int32_t>().merge(array, length);
            break;
        case SortMethod::quick:
            result = Sort<std::int32_t>().quick(array, length);
            break;
        case SortMethod::heap:
            result = Sort<std::int32_t>().heap(array, length);
            break;
        case SortMethod::counting:
            result = Sort<std::int32_t>().counting(array, length);
            break;
        case SortMethod::bucket:
            result = Sort<std::int32_t>().bucket(array, length);
            break;
        case SortMethod::radix:
            result = Sort<std::int32_t>().radix(array, length);
            break;
        default:
            return;
    }
    display(method, result, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace sort
//! @brief To apply sort-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingSort(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::sort;
    const auto& spec = categoryOpts<category>();
    if (MACRO_IMPLIES(spec.any(), spec.size() != candidates.size()))
    {
        return;
    }

    const std::string_view title = algorithm::sort::description();
    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(spec.count());
    using sort::InputBuilder, sort::input::arrayLength, sort::input::arrayRangeMin, sort::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));
    const auto inputData = std::make_shared<InputBuilder<std::int32_t>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const SortMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask), sort::solution, method, inputData->getRandomArray().get(), inputData->getLength());
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, spec.size()) | std::views::filter([&spec](const auto i) { return spec.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(SortMethod::bubble):
                addTask(choice, SortMethod::bubble);
                break;
            case abbrLitHash(SortMethod::selection):
                addTask(choice, SortMethod::selection);
                break;
            case abbrLitHash(SortMethod::insertion):
                addTask(choice, SortMethod::insertion);
                break;
            case abbrLitHash(SortMethod::shell):
                addTask(choice, SortMethod::shell);
                break;
            case abbrLitHash(SortMethod::merge):
                addTask(choice, SortMethod::merge);
                break;
            case abbrLitHash(SortMethod::quick):
                addTask(choice, SortMethod::quick);
                break;
            case abbrLitHash(SortMethod::heap):
                addTask(choice, SortMethod::heap);
                break;
            case abbrLitHash(SortMethod::counting):
                addTask(choice, SortMethod::counting);
                break;
            case abbrLitHash(SortMethod::bucket):
                addTask(choice, SortMethod::bucket);
                break;
            case abbrLitHash(SortMethod::radix):
                addTask(choice, SortMethod::radix);
                break;
            default:
                throw std::runtime_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(title);
}
} // namespace application::app_algo

#undef APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_ALGO_PRINT_TASK_TITLE_SCOPE_END
