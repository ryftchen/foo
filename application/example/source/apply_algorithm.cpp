//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "apply_algorithm.hpp"

#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "algorithm/include/match.hpp"
#include "algorithm/include/notation.hpp"
#include "algorithm/include/optimal.hpp"
#include "algorithm/include/search.hpp"
#include "algorithm/include/sort.hpp"
#include "application/core/include/command.hpp"
#include "application/core/include/log.hpp"
#include "utility/include/currying.hpp"

//! @brief Title of printing when algorithm tasks are beginning.
#define APP_ALGO_PRINT_TASK_BEGIN_TITLE(category)                                                                   \
    std::osyncstream(std::cout) << "\r\n"                                                                           \
                                << "ALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "BEGIN" << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << std::endl;                                                  \
    {
//! @brief Title of printing when algorithm tasks are ending.
#define APP_ALGO_PRINT_TASK_END_TITLE(category)                                                                     \
    }                                                                                                               \
    std::osyncstream(std::cout) << "\r\n"                                                                           \
                                << "ALGORITHM TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "END" << std::resetiosflags(std::ios_base::left)    \
                                << std::setfill(' ') << '\n'                                                        \
                                << std::endl;
//! @brief Get the bit flags of the method (category) in algorithm tasks.
#define APP_ALGO_GET_BIT(category)                                                                           \
    std::invoke(                                                                                             \
        utility::reflection::TypeInfo<AlgorithmTask>::fields.find(REFLECTION_STR(toString(category))).value, \
        getTask())
//! @brief Get the alias of the method (category) in algorithm tasks.
#define APP_ALGO_GET_ALIAS(category)                                                                      \
    ({                                                                                                    \
        constexpr auto attr =                                                                             \
            utility::reflection::TypeInfo<AlgorithmTask>::fields.find(REFLECTION_STR(toString(category))) \
                .attrs.find(REFLECTION_STR("alias"));                                                     \
        static_assert(attr.hasValue);                                                                     \
        attr.value;                                                                                       \
    })

namespace application::app_algo
{
//! @brief Alias for Category.
using Category = AlgorithmTask::Category;

//! @brief Get the algorithm task.
//! @return reference of the AlgorithmTask object
AlgorithmTask& getTask()
{
    static AlgorithmTask task{};
    return task;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried =
        utility::currying::curry(command::presetTaskName, utility::reflection::TypeInfo<AlgorithmTask>::name);
    return curried;
}

//! @brief Mapping table for enum and string about algorithm tasks. X macro.
#define CATEGORY_TABLE         \
    ELEM(match, "match")       \
    ELEM(notation, "notation") \
    ELEM(optimal, "optimal")   \
    ELEM(search, "search")     \
    ELEM(sort, "sort")

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {CATEGORY_TABLE};
#undef ELEM
    return table[cat];
}

namespace match
{
//! @brief Display match result.
#define MATCH_RESULT(opt) \
    "\r\n==> %-16s Method <==\npattern \"%s\" found starting (" #opt ") at index %d, run time: %8.5f ms\n"
//! @brief Display none match result.
#define MATCH_NONE_RESULT "\r\n==> %-16s Method <==\npattern \"%s\" could not be found, run time: %8.5f ms\n"
//! @brief Print match result content.
#define MATCH_PRINT_RESULT_CONTENT(method)                                                                   \
    do                                                                                                       \
    {                                                                                                        \
        if (-1 != shift)                                                                                     \
        {                                                                                                    \
            COMMON_PRINT(MATCH_RESULT(1st), toString(method).data(), pattern, shift, TIME_INTERVAL(timing)); \
        }                                                                                                    \
        else                                                                                                 \
        {                                                                                                    \
            COMMON_PRINT(MATCH_NONE_RESULT, toString(method).data(), pattern, TIME_INTERVAL(timing));        \
        }                                                                                                    \
    }                                                                                                        \
    while (0)
//! @brief Mapping table for enum and string about match methods. X macro.
#define MATCH_METHOD_TABLE                     \
    ELEM(rabinKarp, "RabinKarp")               \
    ELEM(knuthMorrisPratt, "KnuthMorrisPratt") \
    ELEM(boyerMoore, "BoyerMoore")             \
    ELEM(horspool, "Horspool")                 \
    ELEM(sunday, "Sunday")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of MatchMethod enum
//! @return method name
constexpr std::string_view toString(const MatchMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {MATCH_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void MatchSolution::rkMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    TIME_BEGIN(timing);
    const auto shift = algorithm::match::Match().rk(text, pattern, textLen, patternLen);
    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT(MatchMethod::rabinKarp);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void MatchSolution::kmpMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    TIME_BEGIN(timing);
    const auto shift = algorithm::match::Match().kmp(text, pattern, textLen, patternLen);
    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT(MatchMethod::knuthMorrisPratt);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void MatchSolution::bmMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    TIME_BEGIN(timing);
    const auto shift = algorithm::match::Match().bm(text, pattern, textLen, patternLen);
    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT(MatchMethod::boyerMoore);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void MatchSolution::horspoolMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    TIME_BEGIN(timing);
    const auto shift = algorithm::match::Match().horspool(text, pattern, textLen, patternLen);
    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT(MatchMethod::horspool);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void MatchSolution::sundayMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    TIME_BEGIN(timing);
    const auto shift = algorithm::match::Match().sunday(text, pattern, textLen, patternLen);
    TIME_END(timing);
    MATCH_PRINT_RESULT_CONTENT(MatchMethod::sunday);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef MATCH_RESULT
#undef MATCH_NONE_RESULT
#undef MATCH_PRINT_RESULT_CONTENT
#undef MATCH_METHOD_TABLE
} // namespace match

//! @brief Run match tasks.
//! @param targets - container of target methods
void runMatchTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::match;
    const auto& bit = APP_ALGO_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using match::MatchSolution;
    using match::TargetBuilder;
    using match::input::patternString;
    using utility::common::operator""_bkdrHash;
    static_assert(TargetBuilder::maxDigit > patternString.length());

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<MatchMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder>(std::string{patternString});
    const auto matchFunctor =
        [threads, builder](
            const std::string& threadName,
            void (*methodPtr)(
                const unsigned char* const, const unsigned char* const, const std::uint32_t, const std::uint32_t))
    {
        threads->enqueue(
            threadName,
            methodPtr,
            builder->getMatchingText().get(),
            builder->getSinglePattern().get(),
            builder->getTextLength(),
            builder->getPatternLength());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_ALGO_GET_ALIAS(category));

    for (std::uint8_t i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!bit.test(MatchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "rab"_bkdrHash:
                matchFunctor(name(targetMethod), &MatchSolution::rkMethod);
                break;
            case "knu"_bkdrHash:
                matchFunctor(name(targetMethod), &MatchSolution::kmpMethod);
                break;
            case "boy"_bkdrHash:
                matchFunctor(name(targetMethod), &MatchSolution::bmMethod);
                break;
            case "hor"_bkdrHash:
                matchFunctor(name(targetMethod), &MatchSolution::horspoolMethod);
                break;
            case "sun"_bkdrHash:
                matchFunctor(name(targetMethod), &MatchSolution::sundayMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

//! @brief Update match methods in tasks.
//! @param target - target method
void updateMatchTask(const std::string& target)
{
    constexpr auto category = Category::match;
    auto& bit = APP_ALGO_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "rab"_bkdrHash:
            bit.set(MatchMethod::rabinKarp);
            break;
        case "knu"_bkdrHash:
            bit.set(MatchMethod::knuthMorrisPratt);
            break;
        case "boy"_bkdrHash:
            bit.set(MatchMethod::boyerMoore);
            break;
        case "hor"_bkdrHash:
            bit.set(MatchMethod::horspool);
            break;
        case "sun"_bkdrHash:
            bit.set(MatchMethod::sunday);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace notation
{
//! @brief Display notation result.
#define NOTATION_RESULT "\r\n==> %-7s Method <==\n%s: %s\n"
//! @brief Print notation result content.
#define NOTATION_PRINT_RESULT_CONTENT(method, describe) \
    COMMON_PRINT(NOTATION_RESULT, toString(method).data(), describe, notationStr.data())
//! @brief Mapping table for enum and string about notation methods. X macro.
#define NOTATION_METHOD_TABLE \
    ELEM(prefix, "Prefix")    \
    ELEM(postfix, "Postfix")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of NotationMethod enum
//! @return method name
constexpr std::string_view toString(const NotationMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {NOTATION_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void NotationSolution::prefixMethod(const std::string& infixNotation)
try
{
    const auto notationStr = algorithm::notation::Notation().prefix(infixNotation);
    NOTATION_PRINT_RESULT_CONTENT(NotationMethod::prefix, "polish notation");
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void NotationSolution::postfixMethod(const std::string& infixNotation)
try
{
    const auto notationStr = algorithm::notation::Notation().postfix(infixNotation);
    NOTATION_PRINT_RESULT_CONTENT(NotationMethod::postfix, "reverse polish notation");
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef NOTATION_RESULT
#undef NOTATION_PRINT_RESULT_CONTENT
#undef NOTATION_METHOD_TABLE
} // namespace notation

//! @brief Run notation tasks.
//! @param targets - container of target methods
void runNotationTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::notation;
    const auto& bit = APP_ALGO_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using notation::NotationSolution;
    using notation::TargetBuilder;
    using notation::input::infixString;
    using utility::common::operator""_bkdrHash;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<NotationMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder>(infixString);
    const auto notationFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const std::string&))
    {
        threads->enqueue(threadName, methodPtr, std::string{builder->getInfixNotation()});
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_ALGO_GET_ALIAS(category));

    for (std::uint8_t i = 0; i < Bottom<NotationMethod>::value; ++i)
    {
        if (!bit.test(NotationMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "pre"_bkdrHash:
                notationFunctor(name(targetMethod), &NotationSolution::prefixMethod);
                break;
            case "pos"_bkdrHash:
                notationFunctor(name(targetMethod), &NotationSolution::postfixMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

//! @brief Update notation methods in tasks.
//! @param target - target method
void updateNotationTask(const std::string& target)
{
    constexpr auto category = Category::notation;
    auto& bit = APP_ALGO_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "pre"_bkdrHash:
            bit.set(NotationMethod::prefix);
            break;
        case "pos"_bkdrHash:
            bit.set(NotationMethod::postfix);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace optimal
{
//! @brief Display optimal result.
#define OPTIMAL_RESULT(opt) "\r\n==> %-9s Method <==\nF(" #opt ")=%+.5f X=%+.5f, run time: %8.5f ms\n"
//! @brief Print optimal result content.
#define OPTIMAL_PRINT_RESULT_CONTENT(method) \
    COMMON_PRINT(OPTIMAL_RESULT(min), toString(method).data(), fx, x, TIME_INTERVAL(timing))
//! @brief Mapping table for enum and string about optimal methods. X macro.
#define OPTIMAL_METHOD_TABLE     \
    ELEM(gradient, "Gradient")   \
    ELEM(annealing, "Annealing") \
    ELEM(particle, "Particle")   \
    ELEM(genetic, "Genetic")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of OptimalMethod enum
//! @return method name
constexpr std::string_view toString(const OptimalMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {OPTIMAL_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void OptimalSolution::gradientDescentMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto [fx, x] = algorithm::optimal::Gradient(func)(left, right, algorithm::optimal::epsilon).value();
    TIME_END(timing);
    OPTIMAL_PRINT_RESULT_CONTENT(OptimalMethod::gradient);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void OptimalSolution::simulatedAnnealingMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto [fx, x] = algorithm::optimal::Annealing(func)(left, right, algorithm::optimal::epsilon).value();
    TIME_END(timing);
    OPTIMAL_PRINT_RESULT_CONTENT(OptimalMethod::annealing);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void OptimalSolution::particleSwarmMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto [fx, x] = algorithm::optimal::Particle(func)(left, right, algorithm::optimal::epsilon).value();
    TIME_END(timing);
    OPTIMAL_PRINT_RESULT_CONTENT(OptimalMethod::particle);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void OptimalSolution::geneticMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto [fx, x] = algorithm::optimal::Genetic(func)(left, right, algorithm::optimal::epsilon).value();
    TIME_END(timing);
    OPTIMAL_PRINT_RESULT_CONTENT(OptimalMethod::genetic);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef OPTIMAL_RESULT
#undef OPTIMAL_PRINT_RESULT_CONTENT
#undef OPTIMAL_METHOD_TABLE
} // namespace optimal

//! @brief Run optimal tasks.
//! @param targets - container of target methods
void runOptimalTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::optimal;
    const auto& bit = APP_ALGO_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    using optimal::input::Rastrigin;
    typedef std::variant<Rastrigin> OptimalFuncTarget;
    const std::unordered_multimap<optimal::FuncRange<double, double>, OptimalFuncTarget, optimal::FuncMapHash>
        optimalFuncMap{
            {{Rastrigin::range1, Rastrigin::range2, Rastrigin::funcDescr}, Rastrigin{}},
        };
    constexpr auto printFunc = [](const OptimalFuncTarget& function)
    {
        constexpr std::string_view prefix = "\r\nOptimal function:\n";
        std::visit(
            optimal::FuncOverloaded{
                [&prefix](const Rastrigin& /*func*/)
                {
                    std::cout << prefix << Rastrigin::funcDescr << std::endl;
                },
            },
            function);
    };
    const auto calcFunc =
        [&targets, bit](const optimal::Function& function, const optimal::FuncRange<double, double>& range)
    {
        assert(range.range1 < range.range2);
        auto& pooling = command::getPublicThreadPool();
        auto* const threads = pooling.newElement(std::min(
            static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<OptimalMethod>::value)));
        const auto optimalFunctor =
            [threads, &function, &range](
                const std::string& threadName, void (*methodPtr)(const optimal::Function&, const double, const double))
        {
            threads->enqueue(threadName, methodPtr, std::ref(function), range.range1, range.range2);
        };
        const auto name = utility::currying::curry(getTaskNameCurried(), APP_ALGO_GET_ALIAS(category));

        using optimal::OptimalSolution;
        using utility::common::operator""_bkdrHash;
        for (std::uint8_t i = 0; i < Bottom<OptimalMethod>::value; ++i)
        {
            if (!bit.test(OptimalMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i);
            switch (utility::common::bkdrHash(targetMethod.data()))
            {
                case "gra"_bkdrHash:
                    optimalFunctor(name(targetMethod), &OptimalSolution::gradientDescentMethod);
                    break;
                case "ann"_bkdrHash:
                    optimalFunctor(name(targetMethod), &OptimalSolution::simulatedAnnealingMethod);
                    break;
                case "par"_bkdrHash:
                    optimalFunctor(name(targetMethod), &OptimalSolution::particleSwarmMethod);
                    break;
                case "gen"_bkdrHash:
                    optimalFunctor(name(targetMethod), &OptimalSolution::geneticMethod);
                    break;
                default:
                    LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                    break;
            }
        }
        pooling.deleteElement(threads);
    };

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);

    for ([[maybe_unused]] const auto& [range, function] : optimalFuncMap)
    {
        printFunc(function);
        switch (function.index())
        {
            case 0:
                calcFunc(std::get<0>(function), range);
                break;
            default:
                break;
        }
    }

    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

//! @brief Update optimal methods in tasks.
//! @param target - target method
void updateOptimalTask(const std::string& target)
{
    constexpr auto category = Category::optimal;
    auto& bit = APP_ALGO_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "gra"_bkdrHash:
            bit.set(OptimalMethod::gradient);
            break;
        case "ann"_bkdrHash:
            bit.set(OptimalMethod::annealing);
            break;
        case "par"_bkdrHash:
            bit.set(OptimalMethod::particle);
            break;
        case "gen"_bkdrHash:
            bit.set(OptimalMethod::genetic);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace search
{
//! @brief Display search result.
#define SEARCH_RESULT \
    "\r\n==> %-13s Method <==\nfound the key \"%.5f\" that appears in the index %d, run time: %8.5f ms\n"
//! @brief Display none search result.
#define SEARCH_NONE_RESULT "\r\n==> %-13s Method <==\ncould not find the key \"%.5f\", run time: %8.5f ms\n"
//! @brief Print search result content.
#define SEARCH_PRINT_RESULT_CONTENT(method)                                                          \
    do                                                                                               \
    {                                                                                                \
        if (-1 != index)                                                                             \
        {                                                                                            \
            COMMON_PRINT(SEARCH_RESULT, toString(method).data(), key, index, TIME_INTERVAL(timing)); \
        }                                                                                            \
        else                                                                                         \
        {                                                                                            \
            COMMON_PRINT(SEARCH_NONE_RESULT, toString(method).data(), key, TIME_INTERVAL(timing));   \
        }                                                                                            \
    }                                                                                                \
    while (0)
//! @brief Mapping table for enum and string about search methods. X macro.
#define SEARCH_METHOD_TABLE              \
    ELEM(binary, "Binary")               \
    ELEM(interpolation, "Interpolation") \
    ELEM(fibonacci, "Fibonacci")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of SearchMethod enum
//! @return method name
constexpr std::string_view toString(const SearchMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {SEARCH_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void SearchSolution::binaryMethod(const double* const array, const std::uint32_t length, const double key)
try
{
    TIME_BEGIN(timing);
    const auto index = algorithm::search::Search<double>().binary(array, length, key);
    TIME_END(timing);
    SEARCH_PRINT_RESULT_CONTENT(SearchMethod::binary);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SearchSolution::interpolationMethod(const double* const array, const std::uint32_t length, const double key)
try
{
    TIME_BEGIN(timing);
    const auto index = algorithm::search::Search<double>().interpolation(array, length, key);
    TIME_END(timing);
    SEARCH_PRINT_RESULT_CONTENT(SearchMethod::interpolation);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SearchSolution::fibonacciMethod(const double* const array, const std::uint32_t length, const double key)
try
{
    TIME_BEGIN(timing);
    const auto index = algorithm::search::Search<double>().fibonacci(array, length, key);
    TIME_END(timing);
    SEARCH_PRINT_RESULT_CONTENT(SearchMethod::fibonacci);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef SEARCH_RESULT
#undef SEARCH_NONE_RESULT
#undef SEARCH_PRINT_RESULT_CONTENT
#undef SEARCH_METHOD_TABLE
} // namespace search

//! @brief Run search tasks.
//! @param targets - container of target methods
void runSearchTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::search;
    const auto& bit = APP_ALGO_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using search::SearchSolution;
    using search::TargetBuilder;
    using search::input::arrayLength;
    using search::input::arrayRange1;
    using search::input::arrayRange2;
    using utility::common::operator""_bkdrHash;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<SearchMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder<double>>(arrayLength, arrayRange1, arrayRange2);
    const auto searchFunctor =
        [threads, builder](
            const std::string& threadName, void (*methodPtr)(const double* const, const std::uint32_t, const double))
    {
        threads->enqueue(
            threadName, methodPtr, builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_ALGO_GET_ALIAS(category));

    for (std::uint8_t i = 0; i < Bottom<SearchMethod>::value; ++i)
    {
        if (!bit.test(SearchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "bin"_bkdrHash:
                searchFunctor(name(targetMethod), &SearchSolution::binaryMethod);
                break;
            case "int"_bkdrHash:
                searchFunctor(name(targetMethod), &SearchSolution::interpolationMethod);
                break;
            case "fib"_bkdrHash:
                searchFunctor(name(targetMethod), &SearchSolution::fibonacciMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

//! @brief Update search methods in tasks.
//! @param target - target method
void updateSearchTask(const std::string& target)
{
    constexpr auto category = Category::search;
    auto& bit = APP_ALGO_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "bin"_bkdrHash:
            bit.set(SearchMethod::binary);
            break;
        case "int"_bkdrHash:
            bit.set(SearchMethod::interpolation);
            break;
        case "fib"_bkdrHash:
            bit.set(SearchMethod::fibonacci);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace sort
{
//! @brief Display sort result.
#define SORT_RESULT(opt) "\r\n==> %-9s Method <==\n%s\n(" #opt ") run time: %8.5f ms\n"
//! @brief Print sort result content.
#define SORT_PRINT_RESULT_CONTENT(method)                                                                       \
    do                                                                                                          \
    {                                                                                                           \
        const std::uint32_t arrayBufferSize = length * maxAlignOfPrint;                                         \
        char arrayBuffer[arrayBufferSize + 1];                                                                  \
        arrayBuffer[0] = '\0';                                                                                  \
        COMMON_PRINT(                                                                                           \
            SORT_RESULT(asc),                                                                                   \
            toString(method).data(),                                                                            \
            TargetBuilder<int>::template spliceAll<int>(&resCntr[0], length, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                             \
    }                                                                                                           \
    while (0)
//! @brief Mapping table for enum and string about sort methods. X macro.
#define SORT_METHOD_TABLE        \
    ELEM(bubble, "Bubble")       \
    ELEM(selection, "Selection") \
    ELEM(insertion, "Insertion") \
    ELEM(shell, "Shell")         \
    ELEM(merge, "Merge")         \
    ELEM(quick, "Quick")         \
    ELEM(heap, "Heap")           \
    ELEM(counting, "Counting")   \
    ELEM(bucket, "Bucket")       \
    ELEM(radix, "Radix")

//! @brief Convert method enumeration to string.
//! @param method - the specific value of SortMethod enum
//! @return method name
constexpr std::string_view toString(const SortMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {SORT_METHOD_TABLE};
#undef ELEM
    return table[method];
}

void SortSolution::bubbleMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().bubble(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::bubble);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::selectionMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().selection(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::selection);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::insertionMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().insertion(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::insertion);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::shellMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().shell(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::shell);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::mergeMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().merge(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::merge);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::quickMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().quick(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::quick);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::heapMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().heap(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::heap);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::countingMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().counting(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::counting);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::bucketMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().bucket(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::bucket);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

void SortSolution::radixMethod(const int* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto resCntr = algorithm::sort::Sort<int>().radix(array, length);
    TIME_END(timing);
    SORT_PRINT_RESULT_CONTENT(SortMethod::radix);
}
catch (const std::exception& error)
{
    LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
}

#undef SORT_RESULT
#undef SORT_PRINT_RESULT_CONTENT
#undef SORT_METHOD_TABLE
} // namespace sort

//! @brief Run sort tasks.
//! @param targets - container of target methods
void runSortTasks(const std::vector<std::string>& targets)
{
    constexpr auto category = Category::sort;
    const auto& bit = APP_ALGO_GET_BIT(category);
    if (bit.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using sort::SortSolution;
    using sort::TargetBuilder;
    using sort::input::arrayLength;
    using sort::input::arrayRange1;
    using sort::input::arrayRange2;
    using utility::common::operator""_bkdrHash;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bit.count()), static_cast<std::uint32_t>(Bottom<SortMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder<int>>(arrayLength, arrayRange1, arrayRange2);
    const auto sortFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const int* const, const std::uint32_t))
    {
        threads->enqueue(threadName, methodPtr, builder->getRandomArray().get(), builder->getLength());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), APP_ALGO_GET_ALIAS(category));

    for (std::uint8_t i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!bit.test(SortMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "bub"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::bubbleMethod);
                break;
            case "sel"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::selectionMethod);
                break;
            case "ins"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::insertionMethod);
                break;
            case "she"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::shellMethod);
                break;
            case "mer"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::mergeMethod);
                break;
            case "qui"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::quickMethod);
                break;
            case "hea"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::heapMethod);
                break;
            case "cou"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::countingMethod);
                break;
            case "buc"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::bucketMethod);
                break;
            case "rad"_bkdrHash:
                sortFunctor(name(targetMethod), &SortSolution::radixMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown " << toString(category) << " method.";
                break;
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

//! @brief Update sort methods in tasks.
//! @param target - target method
void updateSortTask(const std::string& target)
{
    constexpr auto category = Category::sort;
    auto& bit = APP_ALGO_GET_BIT(category);

    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "bub"_bkdrHash:
            bit.set(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            bit.set(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            bit.set(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            bit.set(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            bit.set(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            bit.set(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            bit.set(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            bit.set(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            bit.set(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            bit.set(SortMethod::radix);
            break;
        default:
            bit.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

#undef CATEGORY_TABLE
} // namespace application::app_algo
