//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_algorithm.hpp"
#include "register_algorithm.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <iomanip>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when algorithm tasks are beginning.
#define APP_ALGO_PRINT_TASK_BEGIN_TITLE(category)                                                                     \
    std::osyncstream(std::cout) << "\nALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "BEGIN" << std::resetiosflags(std::ios_base::left)    \
                                << std::setfill(' ') << std::endl;                                                    \
    {
//! @brief Title of printing when algorithm tasks are ending.
#define APP_ALGO_PRINT_TASK_END_TITLE(category)                                                                       \
    }                                                                                                                 \
    std::osyncstream(std::cout) << "\nALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "END" << std::resetiosflags(std::ios_base::left)      \
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
    std::string title(toString(method));
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
//! @tparam Cat - the specific value of Category enum
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
//! @param method - the specific value of MatchMethod enum
//! @param result - match result
//! @param pattern - single pattern
//! @param interval - time interval
static void showResult(
    const MatchMethod method, const std::int64_t result, const unsigned char* const pattern, const double interval)
{
    if (-1 != result)
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
    const utility::time::Time timer{};
    const auto shift = algorithm::match::Match().rk(text, pattern, textLen, patternLen);
    showResult(MatchMethod::rabinKarp, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};
    const auto shift = algorithm::match::Match().kmp(text, pattern, textLen, patternLen);
    showResult(MatchMethod::knuthMorrisPratt, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};
    const auto shift = algorithm::match::Match().bm(text, pattern, textLen, patternLen);
    showResult(MatchMethod::boyerMoore, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};
    const auto shift = algorithm::match::Match().horspool(text, pattern, textLen, patternLen);
    showResult(MatchMethod::horspool, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};
    const auto shift = algorithm::match::Match().sunday(text, pattern, textLen, patternLen);
    showResult(MatchMethod::sunday, shift, pattern, timer.elapsedTime());
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

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using match::InputBuilder, match::input::patternString;
    static_assert(InputBuilder::maxDigit > patternString.length());
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(patternString);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](
            const std::string_view subTask,
            void (*targetMethod)(
                const unsigned char* const, const unsigned char* const, const std::uint32_t, const std::uint32_t))
    {
        threads->enqueue(
            taskNamer(subTask),
            targetMethod,
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength());
    };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using match::MatchSolution;
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

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace notation
{
//! @brief Show the contents of the notation result.
//! @param method - the specific value of NotationMethod enum
//! @param result - notation result
//! @param descr - notation description
static void showResult(const NotationMethod method, const std::string_view result, const char* const descr)
{
    std::printf("\n==> %-7s Method <==\n%s: %s\n", makeTitle(method).c_str(), descr, result.data());
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

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using notation::InputBuilder, notation::input::infixString;
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(infixString);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](const std::string_view subTask, void (*targetMethod)(const std::string_view))
    { threads->enqueue(taskNamer(subTask), targetMethod, inputs->getInfixNotation()); };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using notation::NotationSolution;
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

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace optimal
{
//! @brief Show the contents of the optimal result.
//! @param method - the specific value of OptimalMethod enum
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
    const utility::time::Time timer{};
    const auto tuple = algorithm::optimal::Gradient(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::gradient, tuple, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::tabuMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Time timer{};
    const auto tuple = algorithm::optimal::Tabu(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::tabu, tuple, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::simulatedAnnealingMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Time timer{};
    const auto tuple = algorithm::optimal::Annealing(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::annealing, tuple, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::particleSwarmMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Time timer{};
    const auto tuple = algorithm::optimal::Particle(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::particle, tuple, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::antColonyMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Time timer{};
    const auto tuple = algorithm::optimal::Ant(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::ant, tuple, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void OptimalSolution::geneticMethod(const Function& func, const double left, const double right)
try
{
    const utility::time::Time timer{};
    const auto tuple = algorithm::optimal::Genetic(func)(left, right, algorithm::optimal::epsilon);
    showResult(OptimalMethod::genetic, tuple, timer.elapsedTime());
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

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using optimal::InputBuilder, optimal::input::Rastrigin, optimal::Function;
    static_assert(algorithm::optimal::epsilon >= std::numeric_limits<double>::epsilon());
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs =
        std::make_shared<InputBuilder>(Rastrigin{}, Rastrigin::range1, Rastrigin::range2, Rastrigin::funcDescr);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const Function&, const double, const double))
    {
        threads->enqueue(
            taskNamer(subTask),
            targetMethod,
            inputs->getFunction(),
            inputs->getRanges().first,
            inputs->getRanges().second);
    };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using optimal::OptimalSolution;
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

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace search
{
//! @brief Show the contents of the search result.
//! @param method - the specific value of SearchMethod enum
//! @param result - search result
//! @param key - search key
//! @param interval - time interval
static void showResult(const SearchMethod method, const std::int64_t result, const float key, const double interval)
{
    if (-1 != result)
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
    const utility::time::Time timer{};
    const auto index = algorithm::search::Search<float>().binary(array, length, key);
    showResult(SearchMethod::binary, index, key, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SearchSolution::interpolationMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    const utility::time::Time timer{};
    const auto index = algorithm::search::Search<float>().interpolation(array, length, key);
    showResult(SearchMethod::interpolation, index, key, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SearchSolution::fibonacciMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    const utility::time::Time timer{};
    const auto index = algorithm::search::Search<float>().fibonacci(array, length, key);
    showResult(SearchMethod::fibonacci, index, key, timer.elapsedTime());
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

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using search::InputBuilder, search::input::arrayLength, search::input::arrayRangeMin, search::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder<float>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const float* const, const std::uint32_t, const float))
    {
        threads->enqueue(
            taskNamer(subTask),
            targetMethod,
            inputs->getOrderedArray().get(),
            inputs->getLength(),
            inputs->getSearchKey());
    };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using search::SearchSolution;
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

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace sort
{
//! @brief Show the contents of the sort result.
//! @param method - the specific value of SortMethod enum
//! @param result - sort result
//! @param interval - time interval
static void showResult(const SortMethod method, const std::vector<std::int32_t>& result, const double interval)
{
    const std::uint32_t arrayBufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> arrayBuffer(arrayBufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\n(asc) run time: %8.5f ms\n",
        makeTitle(method).c_str(),
        InputBuilder<std::int32_t>::template spliceAll<std::int32_t>(
            result.data(), result.size(), arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void SortSolution::bubbleMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().bubble(array, length);
    showResult(SortMethod::bubble, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::selectionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().selection(array, length);
    showResult(SortMethod::selection, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::insertionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().insertion(array, length);
    showResult(SortMethod::insertion, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::shellMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().shell(array, length);
    showResult(SortMethod::shell, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::mergeMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().merge(array, length);
    showResult(SortMethod::merge, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::quickMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().quick(array, length);
    showResult(SortMethod::quick, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::heapMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().heap(array, length);
    showResult(SortMethod::heap, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::countingMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().counting(array, length);
    showResult(SortMethod::counting, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::bucketMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().bucket(array, length);
    showResult(SortMethod::bucket, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in solution (%s): %s", __func__, err.what());
}

void SortSolution::radixMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    const utility::time::Time timer{};
    const auto coll = algorithm::sort::Sort<std::int32_t>().radix(array, length);
    showResult(SortMethod::radix, coll, timer.elapsedTime());
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

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using sort::InputBuilder, sort::input::arrayLength, sort::input::arrayRangeMin, sort::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder<std::int32_t>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::int32_t* const, const std::uint32_t))
    { threads->enqueue(taskNamer(subTask), targetMethod, inputs->getRandomArray().get(), inputs->getLength()); };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using sort::SortSolution;
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

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_algo
