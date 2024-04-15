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
//! @brief Get the title of a particular method in algorithm tasks.
#define APP_ALGO_GET_METHOD_TITLE(method)                  \
    ({                                                     \
        std::string title = std::string{toString(method)}; \
        title.at(0) = std::toupper(title.at(0));           \
        title;                                             \
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

//! @brief Convert category enumeration to string.
//! @param cat - the specific value of Category enum
//! @return category name
constexpr std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::match:
            return utility::reflection::TypeInfo<MatchMethod>::name;
        case Category::notation:
            return utility::reflection::TypeInfo<NotationMethod>::name;
        case Category::optimal:
            return utility::reflection::TypeInfo<OptimalMethod>::name;
        case Category::search:
            return utility::reflection::TypeInfo<SearchMethod>::name;
        case Category::sort:
            return utility::reflection::TypeInfo<SortMethod>::name;
        default:
            return "";
    }
}

//! @brief Get the bit flags of the category in algorithm tasks.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryBit()
{
    return std::invoke(
        utility::reflection::TypeInfo<AlgorithmTask>::fields.find(REFLECTION_STR(toString(Cat))).value, getTask());
}

//! @brief Get the alias of the category in algorithm tasks.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
constexpr std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<AlgorithmTask>::fields.find(REFLECTION_STR(toString(Cat)))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Case value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return case value
template <class T>
consteval std::size_t caseValue(const T method)
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    static_assert(TypeInfo::fields.size == Bottom<T>::value);

    std::size_t value = 0;
    TypeInfo::fields.forEach(
        [method, &value](auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(1 == field.attrs.size);
                auto attr = field.attrs.find(REFLECTION_STR("task"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });
    return value;
}

//! @brief Mapping table for enum and string about match methods. X macro.
#define APP_ALGO_MATCH_METHOD_TABLE            \
    ELEM(rabinKarp, "rabinKarp")               \
    ELEM(knuthMorrisPratt, "knuthMorrisPratt") \
    ELEM(boyerMoore, "boyerMoore")             \
    ELEM(horspool, "horspool")                 \
    ELEM(sunday, "sunday")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of MatchMethod enum
//! @return method name
constexpr std::string_view toString(const MatchMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_MATCH_METHOD_TABLE};
#undef ELEM
    return table[method];
}
#undef APP_ALGO_MATCH_METHOD_TABLE

//! @brief Mapping table for enum and string about notation methods. X macro.
#define APP_ALGO_NOTATION_METHOD_TABLE \
    ELEM(prefix, "prefix")             \
    ELEM(postfix, "postfix")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of NotationMethod enum
//! @return method name
constexpr std::string_view toString(const NotationMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_NOTATION_METHOD_TABLE};
#undef ELEM
    return table[method];
}
#undef APP_ALGO_NOTATION_METHOD_TABLE

//! @brief Mapping table for enum and string about optimal methods. X macro.
#define APP_ALGO_OPTIMAL_METHOD_TABLE \
    ELEM(gradient, "gradient")        \
    ELEM(annealing, "annealing")      \
    ELEM(particle, "particle")        \
    ELEM(genetic, "genetic")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of OptimalMethod enum
//! @return method name
constexpr std::string_view toString(const OptimalMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_OPTIMAL_METHOD_TABLE};
#undef ELEM
    return table[method];
}
#undef APP_ALGO_OPTIMAL_METHOD_TABLE

//! @brief Mapping table for enum and string about search methods. X macro.
#define APP_ALGO_SEARCH_METHOD_TABLE     \
    ELEM(binary, "binary")               \
    ELEM(interpolation, "interpolation") \
    ELEM(fibonacci, "fibonacci")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of SearchMethod enum
//! @return method name
constexpr std::string_view toString(const SearchMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_SEARCH_METHOD_TABLE};
#undef ELEM
    return table[method];
}
#undef APP_ALGO_SEARCH_METHOD_TABLE

//! @brief Mapping table for enum and string about sort methods. X macro.
#define APP_ALGO_SORT_METHOD_TABLE \
    ELEM(bubble, "bubble")         \
    ELEM(selection, "selection")   \
    ELEM(insertion, "insertion")   \
    ELEM(shell, "shell")           \
    ELEM(merge, "merge")           \
    ELEM(quick, "quick")           \
    ELEM(heap, "heap")             \
    ELEM(counting, "counting")     \
    ELEM(bucket, "bucket")         \
    ELEM(radix, "radix")
//! @brief Convert method enumeration to string.
//! @param method - the specific value of SortMethod enum
//! @return method name
constexpr std::string_view toString(const SortMethod method)
{
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_SORT_METHOD_TABLE};
#undef ELEM
    return table[method];
}
#undef APP_ALGO_SORT_METHOD_TABLE

namespace match
{
//! @brief Display match result.
#define MATCH_RESULT(opt) \
    "\r\n==> %-16s Method <==\npattern \"%s\" found starting (" #opt ") at index %d, run time: %8.5f ms\n"
//! @brief Display none match result.
#define MATCH_NONE_RESULT "\r\n==> %-16s Method <==\npattern \"%s\" could not be found, run time: %8.5f ms\n"
//! @brief Print match result content.
#define MATCH_PRINT_RESULT_CONTENT(method)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if (-1 != shift)                                                                                               \
        {                                                                                                              \
            COMMON_PRINT(                                                                                              \
                MATCH_RESULT(1st), APP_ALGO_GET_METHOD_TITLE(method).data(), pattern, shift, TIME_INTERVAL(timing));   \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            COMMON_PRINT(MATCH_NONE_RESULT, APP_ALGO_GET_METHOD_TITLE(method).data(), pattern, TIME_INTERVAL(timing)); \
        }                                                                                                              \
    }                                                                                                                  \
    while (0)

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
} // namespace match

//! @brief Run match tasks.
//! @param candidates - container for the candidate target methods
void runMatchTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::match;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using match::MatchSolution;
    using match::TargetBuilder;
    using match::input::patternString;
    static_assert(TargetBuilder::maxDigit > patternString.length());

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<MatchMethod>::value)));
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
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!bitFlag.test(MatchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case caseValue(MatchMethod::rabinKarp):
                matchFunctor(name(targetMethod), &MatchSolution::rkMethod);
                break;
            case caseValue(MatchMethod::knuthMorrisPratt):
                matchFunctor(name(targetMethod), &MatchSolution::kmpMethod);
                break;
            case caseValue(MatchMethod::boyerMoore):
                matchFunctor(name(targetMethod), &MatchSolution::bmMethod);
                break;
            case caseValue(MatchMethod::horspool):
                matchFunctor(name(targetMethod), &MatchSolution::horspoolMethod);
                break;
            case caseValue(MatchMethod::sunday):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(MatchMethod::rabinKarp):
            bitFlag.set(MatchMethod::rabinKarp);
            break;
        case caseValue(MatchMethod::knuthMorrisPratt):
            bitFlag.set(MatchMethod::knuthMorrisPratt);
            break;
        case caseValue(MatchMethod::boyerMoore):
            bitFlag.set(MatchMethod::boyerMoore);
            break;
        case caseValue(MatchMethod::horspool):
            bitFlag.set(MatchMethod::horspool);
            break;
        case caseValue(MatchMethod::sunday):
            bitFlag.set(MatchMethod::sunday);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace notation
{
//! @brief Display notation result.
#define NOTATION_RESULT "\r\n==> %-7s Method <==\n%s: %s\n"
//! @brief Print notation result content.
#define NOTATION_PRINT_RESULT_CONTENT(method, describe) \
    COMMON_PRINT(NOTATION_RESULT, APP_ALGO_GET_METHOD_TITLE(method).data(), describe, notationStr.data())

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
} // namespace notation

//! @brief Run notation tasks.
//! @param candidates - container for the candidate target methods
void runNotationTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::notation;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using notation::NotationSolution;
    using notation::TargetBuilder;
    using notation::input::infixString;

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<NotationMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder>(infixString);
    const auto notationFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const std::string&))
    {
        threads->enqueue(threadName, methodPtr, std::string{builder->getInfixNotation()});
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<NotationMethod>::value; ++i)
    {
        if (!bitFlag.test(NotationMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case caseValue(NotationMethod::prefix):
                notationFunctor(name(targetMethod), &NotationSolution::prefixMethod);
                break;
            case caseValue(NotationMethod::postfix):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(NotationMethod::prefix):
            bitFlag.set(NotationMethod::prefix);
            break;
        case caseValue(NotationMethod::postfix):
            bitFlag.set(NotationMethod::postfix);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

namespace optimal
{
//! @brief Display optimal result.
#define OPTIMAL_RESULT(opt) "\r\n==> %-9s Method <==\nF(" #opt ")=%+.5f X=%+.5f, run time: %8.5f ms\n"
//! @brief Print optimal result content.
#define OPTIMAL_PRINT_RESULT_CONTENT(method) \
    COMMON_PRINT(OPTIMAL_RESULT(min), APP_ALGO_GET_METHOD_TITLE(method).data(), fx, x, TIME_INTERVAL(timing))

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
} // namespace optimal

//! @brief Run optimal tasks.
//! @param candidates - container for the candidate target methods
void runOptimalTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::optimal;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
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
        [&candidates, bitFlag](const optimal::Function& function, const optimal::FuncRange<double, double>& range)
    {
        assert(range.range1 < range.range2);
        auto& pooling = command::getPublicThreadPool();
        auto* const threads = pooling.newElement(std::min(
            static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<OptimalMethod>::value)));
        const auto optimalFunctor =
            [threads, &function, &range](
                const std::string& threadName, void (*methodPtr)(const optimal::Function&, const double, const double))
        {
            threads->enqueue(threadName, methodPtr, std::ref(function), range.range1, range.range2);
        };
        const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

        using optimal::OptimalSolution;
        for (std::uint8_t i = 0; i < Bottom<OptimalMethod>::value; ++i)
        {
            if (!bitFlag.test(OptimalMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = candidates.at(i);
            switch (utility::common::bkdrHash(targetMethod.data()))
            {
                case caseValue(OptimalMethod::gradient):
                    optimalFunctor(name(targetMethod), &OptimalSolution::gradientDescentMethod);
                    break;
                case caseValue(OptimalMethod::annealing):
                    optimalFunctor(name(targetMethod), &OptimalSolution::simulatedAnnealingMethod);
                    break;
                case caseValue(OptimalMethod::particle):
                    optimalFunctor(name(targetMethod), &OptimalSolution::particleSwarmMethod);
                    break;
                case caseValue(OptimalMethod::genetic):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(OptimalMethod::gradient):
            bitFlag.set(OptimalMethod::gradient);
            break;
        case caseValue(OptimalMethod::annealing):
            bitFlag.set(OptimalMethod::annealing);
            break;
        case caseValue(OptimalMethod::particle):
            bitFlag.set(OptimalMethod::particle);
            break;
        case caseValue(OptimalMethod::genetic):
            bitFlag.set(OptimalMethod::genetic);
            break;
        default:
            bitFlag.reset();
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
#define SEARCH_PRINT_RESULT_CONTENT(method)                                                                           \
    do                                                                                                                \
    {                                                                                                                 \
        if (-1 != index)                                                                                              \
        {                                                                                                             \
            COMMON_PRINT(SEARCH_RESULT, APP_ALGO_GET_METHOD_TITLE(method).data(), key, index, TIME_INTERVAL(timing)); \
        }                                                                                                             \
        else                                                                                                          \
        {                                                                                                             \
            COMMON_PRINT(SEARCH_NONE_RESULT, APP_ALGO_GET_METHOD_TITLE(method).data(), key, TIME_INTERVAL(timing));   \
        }                                                                                                             \
    }                                                                                                                 \
    while (0)

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
} // namespace search

//! @brief Run search tasks.
//! @param candidates - container for the candidate target methods
void runSearchTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::search;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using search::SearchSolution;
    using search::TargetBuilder;
    using search::input::arrayLength;
    using search::input::arrayRange1;
    using search::input::arrayRange2;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<SearchMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder<double>>(arrayLength, arrayRange1, arrayRange2);
    const auto searchFunctor =
        [threads, builder](
            const std::string& threadName, void (*methodPtr)(const double* const, const std::uint32_t, const double))
    {
        threads->enqueue(
            threadName, methodPtr, builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<SearchMethod>::value; ++i)
    {
        if (!bitFlag.test(SearchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case caseValue(SearchMethod::binary):
                searchFunctor(name(targetMethod), &SearchSolution::binaryMethod);
                break;
            case caseValue(SearchMethod::interpolation):
                searchFunctor(name(targetMethod), &SearchSolution::interpolationMethod);
                break;
            case caseValue(SearchMethod::fibonacci):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(SearchMethod::binary):
            bitFlag.set(SearchMethod::binary);
            break;
        case caseValue(SearchMethod::interpolation):
            bitFlag.set(SearchMethod::interpolation);
            break;
        case caseValue(SearchMethod::fibonacci):
            bitFlag.set(SearchMethod::fibonacci);
            break;
        default:
            bitFlag.reset();
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
            APP_ALGO_GET_METHOD_TITLE(method).data(),                                                           \
            TargetBuilder<int>::template spliceAll<int>(&resCntr[0], length, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                             \
    }                                                                                                           \
    while (0)

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
} // namespace sort

//! @brief Run sort tasks.
//! @param candidates - container for the candidate target methods
void runSortTasks(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::sort;
    const auto& bitFlag = getCategoryBit<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using sort::SortSolution;
    using sort::TargetBuilder;
    using sort::input::arrayLength;
    using sort::input::arrayRange1;
    using sort::input::arrayRange2;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = command::getPublicThreadPool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<SortMethod>::value)));
    const auto builder = std::make_shared<TargetBuilder<int>>(arrayLength, arrayRange1, arrayRange2);
    const auto sortFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const int* const, const std::uint32_t))
    {
        threads->enqueue(threadName, methodPtr, builder->getRandomArray().get(), builder->getLength());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!bitFlag.test(SortMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = candidates.at(i);
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case caseValue(SortMethod::bubble):
                sortFunctor(name(targetMethod), &SortSolution::bubbleMethod);
                break;
            case caseValue(SortMethod::selection):
                sortFunctor(name(targetMethod), &SortSolution::selectionMethod);
                break;
            case caseValue(SortMethod::insertion):
                sortFunctor(name(targetMethod), &SortSolution::insertionMethod);
                break;
            case caseValue(SortMethod::shell):
                sortFunctor(name(targetMethod), &SortSolution::shellMethod);
                break;
            case caseValue(SortMethod::merge):
                sortFunctor(name(targetMethod), &SortSolution::mergeMethod);
                break;
            case caseValue(SortMethod::quick):
                sortFunctor(name(targetMethod), &SortSolution::quickMethod);
                break;
            case caseValue(SortMethod::heap):
                sortFunctor(name(targetMethod), &SortSolution::heapMethod);
                break;
            case caseValue(SortMethod::counting):
                sortFunctor(name(targetMethod), &SortSolution::countingMethod);
                break;
            case caseValue(SortMethod::bucket):
                sortFunctor(name(targetMethod), &SortSolution::bucketMethod);
                break;
            case caseValue(SortMethod::radix):
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
    auto& bitFlag = getCategoryBit<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case caseValue(SortMethod::bubble):
            bitFlag.set(SortMethod::bubble);
            break;
        case caseValue(SortMethod::selection):
            bitFlag.set(SortMethod::selection);
            break;
        case caseValue(SortMethod::insertion):
            bitFlag.set(SortMethod::insertion);
            break;
        case caseValue(SortMethod::shell):
            bitFlag.set(SortMethod::shell);
            break;
        case caseValue(SortMethod::merge):
            bitFlag.set(SortMethod::merge);
            break;
        case caseValue(SortMethod::quick):
            bitFlag.set(SortMethod::quick);
            break;
        case caseValue(SortMethod::heap):
            bitFlag.set(SortMethod::heap);
            break;
        case caseValue(SortMethod::counting):
            bitFlag.set(SortMethod::counting);
            break;
        case caseValue(SortMethod::bucket):
            bitFlag.set(SortMethod::bucket);
            break;
        case caseValue(SortMethod::radix):
            bitFlag.set(SortMethod::radix);
            break;
        default:
            bitFlag.reset();
            throw std::runtime_error("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}
} // namespace application::app_algo
