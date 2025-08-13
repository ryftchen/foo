//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_algorithm.hpp"
#include "register_algorithm.hpp"

#ifndef _PRECOMPILED_HEADER
#include <cassert>
#include <iomanip>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/time.hpp"

//! @brief Title of printing when algorithm tasks are beginning.
#define APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(category)                                                               \
    std::osyncstream(std::cout) << "\nALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (category) << "BEGIN" << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << std::endl;                                                    \
    {
//! @brief Title of printing when algorithm tasks are ending.
#define APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(category)                                                                 \
    }                                                                                                                 \
    std::osyncstream(std::cout) << "\nALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (category) << "END" << std::resetiosflags(std::ios_base::left)    \
                                << std::setfill(' ') << '\n'                                                          \
                                << std::endl;

namespace application::app_algo
{
using namespace reg_algo; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular method in algorithm choices.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <typename T>
static std::string makeTitle(const T method)
{
    std::string title(TypeInfo<T>::fields.nameOfValue(method));
    title.at(0) = std::toupper(title.at(0));

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
//! @tparam Cat - specific value of Category enum
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString<Cat>())).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace match
{
//! @brief Show the contents of the match result.
//! @param method - specific value of MatchMethod enum
//! @param result - match result
//! @param pattern - single pattern
//! @param interval - time interval
static void showResult(
    const MatchMethod method, const std::int64_t result, const unsigned char* const pattern, const double interval)
{
    if (result != -1)
    {
        std::printf(
            "\n==> %-16s Method <==\npattern \"%s\" found starting (1st) at index %ld, run time: %8.5f ms\n",
            makeTitle(method).c_str(),
            pattern,
            result,
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-16s Method <==\npattern \"%s\" could not be found, run time: %8.5f ms\n",
            makeTitle(method).c_str(),
            pattern,
            interval);
    }
}

void MatchSolution::rkMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    const utility::time::Stopwatch timing{};
    const auto shift = algorithm::match::Match().rk(text, pattern, textLen, patternLen);
    showResult(MatchMethod::rabinKarp, shift, pattern, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void MatchSolution::kmpMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    const utility::time::Stopwatch timing{};
    const auto shift = algorithm::match::Match().kmp(text, pattern, textLen, patternLen);
    showResult(MatchMethod::knuthMorrisPratt, shift, pattern, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void MatchSolution::bmMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    const utility::time::Stopwatch timing{};
    const auto shift = algorithm::match::Match().bm(text, pattern, textLen, patternLen);
    showResult(MatchMethod::boyerMoore, shift, pattern, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void MatchSolution::horspoolMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    const utility::time::Stopwatch timing{};
    const auto shift = algorithm::match::Match().horspool(text, pattern, textLen, patternLen);
    showResult(MatchMethod::horspool, shift, pattern, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void MatchSolution::sundayMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    const utility::time::Stopwatch timing{};
    const auto shift = algorithm::match::Match().sunday(text, pattern, textLen, patternLen);
    showResult(MatchMethod::sunday, shift, pattern, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace match
//! @brief To apply match-related methods.
//! @param candidates - container for the candidate target methods
void applyingMatch(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::match;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using match::InputBuilder, match::input::patternString;
    static_assert(InputBuilder::maxDigit > patternString.length());
    const auto inputData = std::make_shared<InputBuilder>(patternString);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask,
            void (*targetMethod)(
                const unsigned char* const, const unsigned char* const, const std::uint32_t, const std::uint32_t))
        {
            allocatedJob->enqueue(
                taskNamer(subTask),
                targetMethod,
                inputData->getMatchingText().get(),
                inputData->getSinglePattern().get(),
                inputData->getTextLength(),
                inputData->getPatternLength());
        });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using match::MatchSolution;
            static_assert(utility::common::isStatelessClass<MatchSolution>());
            case abbrValue(MatchMethod::rabinKarp):
                addTask(target, &MatchSolution::rkMethod);
                break;
            case abbrValue(MatchMethod::knuthMorrisPratt):
                addTask(target, &MatchSolution::kmpMethod);
                break;
            case abbrValue(MatchMethod::boyerMoore):
                addTask(target, &MatchSolution::bmMethod);
                break;
            case abbrValue(MatchMethod::horspool):
                addTask(target, &MatchSolution::horspoolMethod);
                break;
            case abbrValue(MatchMethod::sunday):
                addTask(target, &MatchSolution::sundayMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace notation
{
//! @brief Show the contents of the notation result.
//! @param method - specific value of NotationMethod enum
//! @param result - notation result
//! @param descr - notation description
static void showResult(const NotationMethod method, const std::string& result, const std::string& descr)
{
    std::printf("\n==> %-7s Method <==\n%s: %s\n", makeTitle(method).c_str(), descr.c_str(), result.c_str());
}

void NotationSolution::prefixMethod(const std::string_view infix)
try
{
    const auto expr = algorithm::notation::Notation().prefix(infix);
    showResult(NotationMethod::prefix, expr, "polish notation");
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void NotationSolution::postfixMethod(const std::string_view infix)
try
{
    const auto expr = algorithm::notation::Notation().postfix(infix);
    showResult(NotationMethod::postfix, expr, "reverse polish notation");
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace notation
//! @brief To apply notation-related methods.
//! @param candidates - container for the candidate target methods
void applyingNotation(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::notation;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using notation::InputBuilder, notation::input::infixString;
    const auto inputData = std::make_shared<InputBuilder>(infixString);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::string_view))
        { allocatedJob->enqueue(taskNamer(subTask), targetMethod, inputData->getInfixNotation()); });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using notation::NotationSolution;
            static_assert(utility::common::isStatelessClass<NotationSolution>());
            case abbrValue(NotationMethod::prefix):
                addTask(target, &NotationSolution::prefixMethod);
                break;
            case abbrValue(NotationMethod::postfix):
                addTask(target, &NotationSolution::postfixMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace optimal
{
//! @brief Show the contents of the optimal result.
//! @param method - specific value of OptimalMethod enum
//! @param result - optimal result
//! @param interval - time interval
static void showResult(
    const OptimalMethod method, const std::optional<std::tuple<double, double>>& result, const double interval)
{
    if (result.has_value())
    {
        std::printf(
            "\n==> %-9s Method <==\nF(min)=%+.5f X=%+.5f, run time: %8.5f ms\n",
            makeTitle(method).c_str(),
            std::get<0>(result.value()),
            std::get<1>(result.value()),
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-9s Method <==\nF(min) could not be found, run time: %8.5f ms\n",
            makeTitle(method).c_str(),
            interval);
    }
}

void OptimalSolution::gradientDescentMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Stopwatch timing{};
    const auto tuple = algorithm::optimal::Gradient(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::gradient, tuple, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::tabuMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Stopwatch timing{};
    const auto tuple = algorithm::optimal::Tabu(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::tabu, tuple, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::simulatedAnnealingMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Stopwatch timing{};
    const auto tuple = algorithm::optimal::Annealing(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::annealing, tuple, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::particleSwarmMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Stopwatch timing{};
    const auto tuple = algorithm::optimal::Particle(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::particle, tuple, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::antColonyMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Stopwatch timing{};
    const auto tuple = algorithm::optimal::Ant(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::ant, tuple, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::geneticMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Stopwatch timing{};
    const auto tuple = algorithm::optimal::Genetic(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::genetic, tuple, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace optimal
//! @brief To apply optimal-related methods.
//! @param candidates - container for the candidate target methods
void applyingOptimal(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::optimal;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using optimal::InputBuilder, optimal::input::SphericalBessel, optimal::Function;
    static_assert(algorithm::optimal::epsilon >= std::numeric_limits<double>::epsilon());
    const auto inputData = std::make_shared<InputBuilder>(
        SphericalBessel{}, SphericalBessel::range1, SphericalBessel::range2, SphericalBessel::funcDescr);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const Function&, const double, const double))
        {
            allocatedJob->enqueue(
                taskNamer(subTask),
                targetMethod,
                inputData->getFunction(),
                inputData->getRanges().first,
                inputData->getRanges().second);
        });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using optimal::OptimalSolution;
            static_assert(utility::common::isStatelessClass<OptimalSolution>());
            case abbrValue(OptimalMethod::gradient):
                addTask(target, &OptimalSolution::gradientDescentMethod);
                break;
            case abbrValue(OptimalMethod::tabu):
                addTask(target, &OptimalSolution::tabuMethod);
                break;
            case abbrValue(OptimalMethod::annealing):
                addTask(target, &OptimalSolution::simulatedAnnealingMethod);
                break;
            case abbrValue(OptimalMethod::particle):
                addTask(target, &OptimalSolution::particleSwarmMethod);
                break;
            case abbrValue(OptimalMethod::ant):
                addTask(target, &OptimalSolution::antColonyMethod);
                break;
            case abbrValue(OptimalMethod::genetic):
                addTask(target, &OptimalSolution::geneticMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace search
{
//! @brief Show the contents of the search result.
//! @param method - specific value of SearchMethod enum
//! @param result - search result
//! @param key - search key
//! @param interval - time interval
static void showResult(const SearchMethod method, const std::int64_t result, const float key, const double interval)
{
    if (result != -1)
    {
        std::printf(
            "\n==> %-13s Method <==\nfound the key \"%.5f\" that appears in the index %ld, run time: %8.5f ms\n",
            makeTitle(method).c_str(),
            key,
            result,
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-13s Method <==\ncould not find the key \"%.5f\", run time: %8.5f ms\n",
            makeTitle(method).c_str(),
            key,
            interval);
    }
}

void SearchSolution::binaryMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    const utility::time::Stopwatch timing{};
    const auto index = algorithm::search::Search<float>().binary(array, length, key);
    showResult(SearchMethod::binary, index, key, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SearchSolution::interpolationMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    const utility::time::Stopwatch timing{};
    const auto index = algorithm::search::Search<float>().interpolation(array, length, key);
    showResult(SearchMethod::interpolation, index, key, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SearchSolution::fibonacciMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    const utility::time::Stopwatch timing{};
    const auto index = algorithm::search::Search<float>().fibonacci(array, length, key);
    showResult(SearchMethod::fibonacci, index, key, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace search
//! @brief To apply search-related methods.
//! @param candidates - container for the candidate target methods
void applyingSearch(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::search;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using search::InputBuilder, search::input::arrayLength, search::input::arrayRangeMin, search::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));
    const auto inputData = std::make_shared<InputBuilder<float>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const float* const, const std::uint32_t, const float))
        {
            allocatedJob->enqueue(
                taskNamer(subTask),
                targetMethod,
                inputData->getOrderedArray().get(),
                inputData->getLength(),
                inputData->getSearchKey());
        });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using search::SearchSolution;
            static_assert(utility::common::isStatelessClass<SearchSolution>());
            case abbrValue(SearchMethod::binary):
                addTask(target, &SearchSolution::binaryMethod);
                break;
            case abbrValue(SearchMethod::interpolation):
                addTask(target, &SearchSolution::interpolationMethod);
                break;
            case abbrValue(SearchMethod::fibonacci):
                addTask(target, &SearchSolution::fibonacciMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace sort
{
//! @brief Show the contents of the sort result.
//! @param method - specific value of SortMethod enum
//! @param result - sort result
//! @param interval - time interval
static void showResult(const SortMethod method, const std::vector<std::int32_t>& result, const double interval)
{
    const std::uint32_t bufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> fmtBuffer(bufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\n(asc) run time: %8.5f ms\n",
        makeTitle(method).c_str(),
        InputBuilder<std::int32_t>::template spliceAll<std::int32_t>(
            result.data(), result.size(), fmtBuffer.data(), bufferSize + 1),
        interval);
}

void SortSolution::bubbleMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().bubble(array, length);
    showResult(SortMethod::bubble, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::selectionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().selection(array, length);
    showResult(SortMethod::selection, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::insertionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().insertion(array, length);
    showResult(SortMethod::insertion, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::shellMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().shell(array, length);
    showResult(SortMethod::shell, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::mergeMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().merge(array, length);
    showResult(SortMethod::merge, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::quickMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().quick(array, length);
    showResult(SortMethod::quick, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::heapMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().heap(array, length);
    showResult(SortMethod::heap, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::countingMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().counting(array, length);
    showResult(SortMethod::counting, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::bucketMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().bucket(array, length);
    showResult(SortMethod::bucket, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::radixMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().radix(array, length);
    showResult(SortMethod::radix, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace sort
//! @brief To apply sort-related methods.
//! @param candidates - container for the candidate target methods
void applyingSort(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::sort;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using sort::InputBuilder, sort::input::arrayLength, sort::input::arrayRangeMin, sort::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));
    const auto inputData = std::make_shared<InputBuilder<std::int32_t>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::int32_t* const, const std::uint32_t))
        {
            allocatedJob->enqueue(
                taskNamer(subTask), targetMethod, inputData->getRandomArray().get(), inputData->getLength());
        });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using sort::SortSolution;
            static_assert(utility::common::isStatelessClass<SortSolution>());
            case abbrValue(SortMethod::bubble):
                addTask(target, &SortSolution::bubbleMethod);
                break;
            case abbrValue(SortMethod::selection):
                addTask(target, &SortSolution::selectionMethod);
                break;
            case abbrValue(SortMethod::insertion):
                addTask(target, &SortSolution::insertionMethod);
                break;
            case abbrValue(SortMethod::shell):
                addTask(target, &SortSolution::shellMethod);
                break;
            case abbrValue(SortMethod::merge):
                addTask(target, &SortSolution::mergeMethod);
                break;
            case abbrValue(SortMethod::quick):
                addTask(target, &SortSolution::quickMethod);
                break;
            case abbrValue(SortMethod::heap):
                addTask(target, &SortSolution::heapMethod);
                break;
            case abbrValue(SortMethod::counting):
                addTask(target, &SortSolution::countingMethod);
                break;
            case abbrValue(SortMethod::bucket):
                addTask(target, &SortSolution::bucketMethod);
                break;
            case abbrValue(SortMethod::radix):
                addTask(target, &SortSolution::radixMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_ALGO_PRINT_TASK_TITLE_SCOPE_END(category);
}
} // namespace application::app_algo

#undef APP_ALGO_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_ALGO_PRINT_TASK_TITLE_SCOPE_END
