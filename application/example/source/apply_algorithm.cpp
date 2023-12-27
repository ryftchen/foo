//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

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
    const char* const text,
    const char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
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
}

void MatchSolution::kmpMethod(
    const char* const text,
    const char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
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
}

void MatchSolution::bmMethod(
    const char* const text,
    const char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
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
}

void MatchSolution::horspoolMethod(
    const char* const text,
    const char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
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
}

void MatchSolution::sundayMethod(
    const char* const text,
    const char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
{
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
    if (getBit<MatchMethod>().none())
    {
        return;
    }

    using match::MatchSolution;
    using match::TargetBuilder;
    using match::input::patternString;
    using utility::common::operator""_bkdrHash;

    static_assert(TargetBuilder::maxDigit > patternString.length());
    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Category::match);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<MatchMethod>().count()),
        static_cast<std::uint32_t>(Bottom<MatchMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(patternString);
    const auto matchFunctor =
        [threads, builder](
            const std::string& threadName,
            void (*methodPtr)(const char* const, const char* const, const std::uint32_t, const std::uint32_t))
    {
        threads->enqueue(
            threadName,
            methodPtr,
            builder->getMatchingText().get(),
            builder->getSinglePattern().data(),
            std::string_view(builder->getMatchingText().get()).length(),
            builder->getSinglePattern().length());
    };

    for (std::uint8_t i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!getBit<MatchMethod>().test(MatchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "m_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "rab"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::rkMethod);
                break;
            case "knu"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::kmpMethod);
                break;
            case "boy"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::bmMethod);
                break;
            case "hor"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::horspoolMethod);
                break;
            case "sun"_bkdrHash:
                matchFunctor(threadName, &MatchSolution::sundayMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown match method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Category::match);
}

//! @brief Update match methods in tasks.
//! @param target - target method
void updateMatchTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "rab"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::rabinKarp);
            break;
        case "knu"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::knuthMorrisPratt);
            break;
        case "boy"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::boyerMoore);
            break;
        case "hor"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::horspool);
            break;
        case "sun"_bkdrHash:
            setBit<MatchMethod>(MatchMethod::sunday);
            break;
        default:
            getBit<MatchMethod>().reset();
            throw std::runtime_error("Unexpected match method: " + target + '.');
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
{
    try
    {
        const auto notationStr = algorithm::notation::Notation().prefix(infixNotation);
        NOTATION_PRINT_RESULT_CONTENT(NotationMethod::prefix, "polish notation");
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void NotationSolution::postfixMethod(const std::string& infixNotation)
{
    try
    {
        const auto notationStr = algorithm::notation::Notation().postfix(infixNotation);
        NOTATION_PRINT_RESULT_CONTENT(NotationMethod::postfix, "reverse polish notation");
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

#undef NOTATION_RESULT
#undef NOTATION_PRINT_RESULT_CONTENT
#undef NOTATION_METHOD_TABLE
} // namespace notation

//! @brief Run notation tasks.
//! @param targets - container of target methods
void runNotationTasks(const std::vector<std::string>& targets)
{
    if (getBit<NotationMethod>().none())
    {
        return;
    }

    using notation::NotationSolution;
    using notation::TargetBuilder;
    using notation::input::infixString;
    using utility::common::operator""_bkdrHash;

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Category::notation);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<NotationMethod>().count()),
        static_cast<std::uint32_t>(Bottom<NotationMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder>(infixString);
    const auto notationFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const std::string&))
    {
        threads->enqueue(threadName, methodPtr, std::string{builder->getInfixNotation()});
    };

    for (std::uint8_t i = 0; i < Bottom<NotationMethod>::value; ++i)
    {
        if (!getBit<NotationMethod>().test(NotationMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "n_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "pre"_bkdrHash:
                notationFunctor(threadName, &NotationSolution::prefixMethod);
                break;
            case "pos"_bkdrHash:
                notationFunctor(threadName, &NotationSolution::postfixMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown notation method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Category::notation);
}

//! @brief Update notation methods in tasks.
//! @param target - target method
void updateNotationTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "pre"_bkdrHash:
            setBit<NotationMethod>(NotationMethod::prefix);
            break;
        case "pos"_bkdrHash:
            setBit<NotationMethod>(NotationMethod::postfix);
            break;
        default:
            getBit<NotationMethod>().reset();
            throw std::runtime_error("Unexpected notation method: " + target + '.');
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
{
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
}

void OptimalSolution::simulatedAnnealingMethod(const Function& func, const double left, const double right)
{
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
}

void OptimalSolution::particleSwarmMethod(const Function& func, const double left, const double right)
{
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
}

void OptimalSolution::geneticMethod(const Function& func, const double left, const double right)
{
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
}

#undef OPTIMAL_RESULT
#undef OPTIMAL_PRINT_RESULT_CONTENT
#undef OPTIMAL_METHOD_TABLE
} // namespace optimal

//! @brief Run optimal tasks.
//! @param targets - container of target methods
void runOptimalTasks(const std::vector<std::string>& targets)
{
    if (getBit<OptimalMethod>().none())
    {
        return;
    }

    using optimal::input::Rastrigin;
    typedef std::variant<Rastrigin> OptimalFuncTarget;
    constexpr auto printFunctor = [](const OptimalFuncTarget& function)
    {
        constexpr std::string_view prefix{"\r\nOptimal function:\n"};
        std::visit(
            optimal::FuncOverloaded{
                [&prefix](const Rastrigin& /*func*/)
                {
                    std::cout << prefix << Rastrigin::funcDescr << std::endl;
                },
            },
            function);
    };
    const auto resultFunctor =
        [targets](const optimal::Function& function, const optimal::FuncRange<double, double>& range)
    {
        assert(range.range1 < range.range2);
        auto* const threads = command::getPublicThreadPool().newElement(std::min(
            static_cast<std::uint32_t>(getBit<OptimalMethod>().count()),
            static_cast<std::uint32_t>(Bottom<OptimalMethod>::value)));
        const auto optimalFunctor =
            [threads, &function, range](
                const std::string& threadName, void (*methodPtr)(const optimal::Function&, const double, const double))
        {
            threads->enqueue(threadName, methodPtr, std::ref(function), range.range1, range.range2);
        };

        using optimal::OptimalSolution;
        using utility::common::operator""_bkdrHash;
        for (std::uint8_t i = 0; i < Bottom<OptimalMethod>::value; ++i)
        {
            if (!getBit<OptimalMethod>().test(OptimalMethod(i)))
            {
                continue;
            }

            const std::string targetMethod = targets.at(i), threadName = "o_" + targetMethod;
            switch (utility::common::bkdrHash(targetMethod.data()))
            {
                case "gra"_bkdrHash:
                    optimalFunctor(threadName, &OptimalSolution::gradientDescentMethod);
                    break;
                case "ann"_bkdrHash:
                    optimalFunctor(threadName, &OptimalSolution::simulatedAnnealingMethod);
                    break;
                case "par"_bkdrHash:
                    optimalFunctor(threadName, &OptimalSolution::particleSwarmMethod);
                    break;
                case "gen"_bkdrHash:
                    optimalFunctor(threadName, &OptimalSolution::geneticMethod);
                    break;
                default:
                    LOG_INF << "Execute to apply an unknown optimal method.";
                    break;
            }
        }
        command::getPublicThreadPool().deleteElement(threads);
    };

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Category::optimal);

    const std::unordered_multimap<optimal::FuncRange<double, double>, OptimalFuncTarget, optimal::FuncMapHash>
        optimalFuncMap{
            {{Rastrigin::range1, Rastrigin::range2, Rastrigin::funcDescr}, Rastrigin{}},
        };
    for ([[maybe_unused]] const auto& [range, function] : optimalFuncMap)
    {
        printFunctor(function);
        switch (function.index())
        {
            case 0:
                resultFunctor(std::get<0>(function), range);
                break;
            [[unlikely]] default:
                break;
        }
    }

    APP_ALGO_PRINT_TASK_END_TITLE(Category::optimal);
}

//! @brief Update optimal methods in tasks.
//! @param target - target method
void updateOptimalTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "gra"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::gradient);
            break;
        case "ann"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::annealing);
            break;
        case "par"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::particle);
            break;
        case "gen"_bkdrHash:
            setBit<OptimalMethod>(OptimalMethod::genetic);
            break;
        default:
            getBit<OptimalMethod>().reset();
            throw std::runtime_error("Unexpected optimal method: " + target + '.');
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
{
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
}

void SearchSolution::interpolationMethod(const double* const array, const std::uint32_t length, const double key)
{
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
}

void SearchSolution::fibonacciMethod(const double* const array, const std::uint32_t length, const double key)
{
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
    if (getBit<SearchMethod>().none())
    {
        return;
    }

    using search::SearchSolution;
    using search::TargetBuilder;
    using search::input::arrayLength;
    using search::input::arrayRange1;
    using search::input::arrayRange2;
    using utility::common::operator""_bkdrHash;

    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));
    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Category::search);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<SearchMethod>().count()),
        static_cast<std::uint32_t>(Bottom<SearchMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder<double>>(arrayLength, arrayRange1, arrayRange2);
    const auto searchFunctor =
        [threads, builder](
            const std::string& threadName, void (*methodPtr)(const double* const, const std::uint32_t, const double))
    {
        threads->enqueue(
            threadName, methodPtr, builder->getOrderedArray().get(), builder->getLength(), builder->getSearchKey());
    };

    for (std::uint8_t i = 0; i < Bottom<SearchMethod>::value; ++i)
    {
        if (!getBit<SearchMethod>().test(SearchMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "s_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "bin"_bkdrHash:
                searchFunctor(threadName, &SearchSolution::binaryMethod);
                break;
            case "int"_bkdrHash:
                searchFunctor(threadName, &SearchSolution::interpolationMethod);
                break;
            case "fib"_bkdrHash:
                searchFunctor(threadName, &SearchSolution::fibonacciMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown search method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Category::search);
}

//! @brief Update search methods in tasks.
//! @param target - target method
void updateSearchTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "bin"_bkdrHash:
            setBit<SearchMethod>(SearchMethod::binary);
            break;
        case "int"_bkdrHash:
            setBit<SearchMethod>(SearchMethod::interpolation);
            break;
        case "fib"_bkdrHash:
            setBit<SearchMethod>(SearchMethod::fibonacci);
            break;
        default:
            getBit<SearchMethod>().reset();
            throw std::runtime_error("Unexpected search method: " + target + '.');
    }
}

namespace sort
{
//! @brief Display sort result.
#define SORT_RESULT(opt) "\r\n==> %-9s Method <==\n%s\n(" #opt ") run time: %8.5f ms\n"
//! @brief Print sort result content.
#define SORT_PRINT_RESULT_CONTENT(method)                                                                         \
    do                                                                                                            \
    {                                                                                                             \
        const std::uint32_t arrayBufferSize = length * maxAlignOfPrint;                                           \
        char arrayBuffer[arrayBufferSize + 1];                                                                    \
        arrayBuffer[0] = '\0';                                                                                    \
        COMMON_PRINT(                                                                                             \
            SORT_RESULT(asc),                                                                                     \
            toString(method).data(),                                                                              \
            TargetBuilder<int>::template spliceAll<int>(&sortArray[0], length, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                               \
    }                                                                                                             \
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
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().bubble(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::bubble);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::selectionMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().selection(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::selection);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::insertionMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().insertion(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::insertion);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::shellMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().shell(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::shell);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::mergeMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().merge(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::merge);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::quickMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().quick(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::quick);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::heapMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().heap(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::heap);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::countingMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().counting(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::counting);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::bucketMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().bucket(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::bucket);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

void SortSolution::radixMethod(const int* const array, const std::uint32_t length)
{
    try
    {
        TIME_BEGIN(timing);
        const auto sortArray = algorithm::sort::Sort<int>().radix(array, length);
        TIME_END(timing);
        SORT_PRINT_RESULT_CONTENT(SortMethod::radix);
    }
    catch (const std::exception& error)
    {
        LOG_ERR << "Interrupt " << __FUNCTION__ << ". " << error.what();
    }
}

#undef SORT_RESULT
#undef SORT_PRINT_RESULT_CONTENT
#undef SORT_METHOD_TABLE
} // namespace sort

//! @brief Run sort tasks.
//! @param targets - container of target methods
void runSortTasks(const std::vector<std::string>& targets)
{
    if (getBit<SortMethod>().none())
    {
        return;
    }

    using sort::SortSolution;
    using sort::TargetBuilder;
    using sort::input::arrayLength;
    using sort::input::arrayRange1;
    using sort::input::arrayRange2;
    using utility::common::operator""_bkdrHash;

    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));
    APP_ALGO_PRINT_TASK_BEGIN_TITLE(Category::sort);
    auto* const threads = command::getPublicThreadPool().newElement(std::min(
        static_cast<std::uint32_t>(getBit<SortMethod>().count()),
        static_cast<std::uint32_t>(Bottom<SortMethod>::value)));

    const auto builder = std::make_shared<TargetBuilder<int>>(arrayLength, arrayRange1, arrayRange2);
    const auto sortFunctor =
        [threads, builder](const std::string& threadName, void (*methodPtr)(const int* const, const std::uint32_t))
    {
        threads->enqueue(threadName, methodPtr, builder->getRandomArray().get(), builder->getLength());
    };

    for (std::uint8_t i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!getBit<SortMethod>().test(SortMethod(i)))
        {
            continue;
        }

        const std::string targetMethod = targets.at(i), threadName = "s_" + targetMethod;
        switch (utility::common::bkdrHash(targetMethod.data()))
        {
            case "bub"_bkdrHash:
                sortFunctor(threadName, &SortSolution::bubbleMethod);
                break;
            case "sel"_bkdrHash:
                sortFunctor(threadName, &SortSolution::selectionMethod);
                break;
            case "ins"_bkdrHash:
                sortFunctor(threadName, &SortSolution::insertionMethod);
                break;
            case "she"_bkdrHash:
                sortFunctor(threadName, &SortSolution::shellMethod);
                break;
            case "mer"_bkdrHash:
                sortFunctor(threadName, &SortSolution::mergeMethod);
                break;
            case "qui"_bkdrHash:
                sortFunctor(threadName, &SortSolution::quickMethod);
                break;
            case "hea"_bkdrHash:
                sortFunctor(threadName, &SortSolution::heapMethod);
                break;
            case "cou"_bkdrHash:
                sortFunctor(threadName, &SortSolution::countingMethod);
                break;
            case "buc"_bkdrHash:
                sortFunctor(threadName, &SortSolution::bucketMethod);
                break;
            case "rad"_bkdrHash:
                sortFunctor(threadName, &SortSolution::radixMethod);
                break;
            default:
                LOG_INF << "Execute to apply an unknown sort method.";
                break;
        }
    }

    command::getPublicThreadPool().deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(Category::sort);
}

//! @brief Update sort methods in tasks.
//! @param target - target method
void updateSortTask(const std::string& target)
{
    using utility::common::operator""_bkdrHash;
    switch (utility::common::bkdrHash(target.c_str()))
    {
        case "bub"_bkdrHash:
            setBit<SortMethod>(SortMethod::bubble);
            break;
        case "sel"_bkdrHash:
            setBit<SortMethod>(SortMethod::selection);
            break;
        case "ins"_bkdrHash:
            setBit<SortMethod>(SortMethod::insertion);
            break;
        case "she"_bkdrHash:
            setBit<SortMethod>(SortMethod::shell);
            break;
        case "mer"_bkdrHash:
            setBit<SortMethod>(SortMethod::merge);
            break;
        case "qui"_bkdrHash:
            setBit<SortMethod>(SortMethod::quick);
            break;
        case "hea"_bkdrHash:
            setBit<SortMethod>(SortMethod::heap);
            break;
        case "cou"_bkdrHash:
            setBit<SortMethod>(SortMethod::counting);
            break;
        case "buc"_bkdrHash:
            setBit<SortMethod>(SortMethod::bucket);
            break;
        case "rad"_bkdrHash:
            setBit<SortMethod>(SortMethod::radix);
            break;
        default:
            getBit<SortMethod>().reset();
            throw std::runtime_error("Unexpected sort method: " + target + '.');
    }
}
} // namespace application::app_algo
