//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "apply_algorithm.hpp"

#ifndef __PRECOMPILED_HEADER
#include <iomanip>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/action.hpp"
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
//! @brief Alias for Category.
using Category = ApplyAlgorithm::Category;

//! @brief Get the algorithm choice manager.
//! @return reference of the ApplyAlgorithm object
ApplyAlgorithm& manager()
{
    static ApplyAlgorithm manager{};
    return manager;
}

//! @brief Get the task name curried.
//! @return task name curried
static const auto& getTaskNameCurried()
{
    static const auto curried =
        utility::currying::curry(action::presetTaskName, utility::reflection::TypeInfo<ApplyAlgorithm>::name);
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

//! @brief Get the bit flags of the category in algorithm choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& getCategoryOpts()
{
    return std::invoke(
        utility::reflection::TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString(Cat))).value, manager());
}

//! @brief Get the alias of the category in algorithm choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
constexpr std::string_view getCategoryAlias()
{
    constexpr auto attr = utility::reflection::TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString(Cat)))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Abbreviation value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return abbreviation value
template <class T>
consteval std::size_t abbrVal(const T method)
{
    using TypeInfo = utility::reflection::TypeInfo<T>;
    static_assert(Bottom<T>::value == TypeInfo::fields.size);

    std::size_t value = 0;
    TypeInfo::fields.forEach(
        [method, &value](auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(1 == field.attrs.size);
                auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value, 0);
            }
        });
    return value;
}

//! @brief Get the title of a particular method in algorithm choices.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <class T>
std::string getTitle(const T method)
{
    std::string title = std::string{toString(method)};
    title.at(0) = std::toupper(title.at(0));
    return title;
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_MATCH_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<MatchMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_NOTATION_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<NotationMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_OPTIMAL_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<OptimalMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_SEARCH_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<SearchMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
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
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_ALGO_SORT_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<SortMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_ALGO_SORT_METHOD_TABLE

namespace match
{
//! @brief Display the contents of the match result.
//! @param method - the specific value of MatchMethod enum
//! @param result - match result
//! @param pattern - single pattern
//! @param interval - time interval
static void displayResult(
    const MatchMethod method, const std::int64_t result, const unsigned char* const pattern, const double interval)
{
    if (-1 != result)
    {
        COMMON_PRINT(
            "\n==> %-16s Method <==\npattern \"%s\" found starting (1st) at index %d, run time: %8.5f ms\n",
            getTitle(method).data(),
            pattern,
            result,
            interval);
    }
    else
    {
        COMMON_PRINT(
            "\n==> %-16s Method <==\npattern \"%s\" could not be found, run time: %8.5f ms\n",
            getTitle(method).data(),
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
    TIME_BEGIN(timing);
    const auto shift = algorithm::match::Match().rk(text, pattern, textLen, patternLen);
    TIME_END(timing);
    displayResult(MatchMethod::rabinKarp, shift, pattern, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
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
    displayResult(MatchMethod::knuthMorrisPratt, shift, pattern, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
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
    displayResult(MatchMethod::boyerMoore, shift, pattern, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
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
    displayResult(MatchMethod::horspool, shift, pattern, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
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
    displayResult(MatchMethod::sunday, shift, pattern, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace match

//! @brief Update match-related choice.
//! @param target - target method
template <>
void updateChoice<MatchMethod>(const std::string& target)
{
    constexpr auto category = Category::match;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(MatchMethod::rabinKarp):
            bitFlag.set(MatchMethod::rabinKarp);
            break;
        case abbrVal(MatchMethod::knuthMorrisPratt):
            bitFlag.set(MatchMethod::knuthMorrisPratt);
            break;
        case abbrVal(MatchMethod::boyerMoore):
            bitFlag.set(MatchMethod::boyerMoore);
            break;
        case abbrVal(MatchMethod::horspool):
            bitFlag.set(MatchMethod::horspool);
            break;
        case abbrVal(MatchMethod::sunday):
            bitFlag.set(MatchMethod::sunday);
            break;
        default:
            bitFlag.reset();
            throw std::invalid_argument("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run match-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<MatchMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::match;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using match::MatchSolution, match::InputBuilder, match::input::patternString;
    static_assert(InputBuilder::maxDigit > patternString.length());

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<MatchMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(std::string{patternString});
    const auto matchFunctor =
        [threads, inputs](
            const std::string& threadName,
            void (*targetMethod)(
                const unsigned char* const, const unsigned char* const, const std::uint32_t, const std::uint32_t))
    {
        threads->enqueue(
            threadName,
            targetMethod,
            inputs->getMatchingText().get(),
            inputs->getSinglePattern().get(),
            inputs->getTextLength(),
            inputs->getPatternLength());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<MatchMethod>::value; ++i)
    {
        if (!bitFlag.test(MatchMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(MatchMethod::rabinKarp):
                matchFunctor(name(target), &MatchSolution::rkMethod);
                break;
            case abbrVal(MatchMethod::knuthMorrisPratt):
                matchFunctor(name(target), &MatchSolution::kmpMethod);
                break;
            case abbrVal(MatchMethod::boyerMoore):
                matchFunctor(name(target), &MatchSolution::bmMethod);
                break;
            case abbrVal(MatchMethod::horspool):
                matchFunctor(name(target), &MatchSolution::horspoolMethod);
                break;
            case abbrVal(MatchMethod::sunday):
                matchFunctor(name(target), &MatchSolution::sundayMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace notation
{
//! @brief Display the contents of the notation result.
//! @param method - the specific value of NotationMethod enum
//! @param result - notation result
//! @param descr - notation description
static void displayResult(const NotationMethod method, const std::string& result, const char* const descr)
{
    COMMON_PRINT("\n==> %-7s Method <==\n%s: %s\n", getTitle(method).data(), descr, result.data());
}

void NotationSolution::prefixMethod(const std::string& infixNotation)
try
{
    const auto& expr = algorithm::notation::Notation().prefix(infixNotation);
    displayResult(NotationMethod::prefix, expr, "polish notation");
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void NotationSolution::postfixMethod(const std::string& infixNotation)
try
{
    const auto& expr = algorithm::notation::Notation().postfix(infixNotation);
    displayResult(NotationMethod::postfix, expr, "reverse polish notation");
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace notation

//! @brief Update notation-related choice.
//! @param target - target method
template <>
void updateChoice<NotationMethod>(const std::string& target)
{
    constexpr auto category = Category::notation;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(NotationMethod::prefix):
            bitFlag.set(NotationMethod::prefix);
            break;
        case abbrVal(NotationMethod::postfix):
            bitFlag.set(NotationMethod::postfix);
            break;
        default:
            bitFlag.reset();
            throw std::invalid_argument("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run notation-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<NotationMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::notation;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using notation::NotationSolution, notation::InputBuilder, notation::input::infixString;

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(std::min(
        static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<NotationMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder>(infixString);
    const auto notationFunctor =
        [threads, inputs](const std::string& threadName, void (*targetMethod)(const std::string&))
    {
        threads->enqueue(threadName, targetMethod, std::string{inputs->getInfixNotation()});
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<NotationMethod>::value; ++i)
    {
        if (!bitFlag.test(NotationMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(NotationMethod::prefix):
                notationFunctor(name(target), &NotationSolution::prefixMethod);
                break;
            case abbrVal(NotationMethod::postfix):
                notationFunctor(name(target), &NotationSolution::postfixMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace optimal
{
//! @brief Display the contents of the optimal result.
//! @param method - the specific value of OptimalMethod enum
//! @param result - optimal result
//! @param interval - time interval
static void displayResult(
    const OptimalMethod method, const std::optional<std::tuple<double, double>>& result, const double interval)
{
    if (result.has_value())
    {
        COMMON_PRINT(
            "\n==> %-9s Method <==\nF(min)=%+.5f X=%+.5f, run time: %8.5f ms\n",
            getTitle(method).data(),
            std::get<0>(result.value()),
            std::get<1>(result.value()),
            interval);
    }
    else
    {
        COMMON_PRINT(
            "\n==> %-9s Method <==\nF(min) could not be found, run time: %8.5f ms\n",
            getTitle(method).data(),
            interval);
    }
}

void OptimalSolution::gradientDescentMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto& tuple = algorithm::optimal::Gradient(func)(left, right, algorithm::optimal::epsilon);
    TIME_END(timing);
    displayResult(OptimalMethod::gradient, tuple, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void OptimalSolution::simulatedAnnealingMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto& tuple = algorithm::optimal::Annealing(func)(left, right, algorithm::optimal::epsilon);
    TIME_END(timing);
    displayResult(OptimalMethod::annealing, tuple, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void OptimalSolution::particleSwarmMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto& tuple = algorithm::optimal::Particle(func)(left, right, algorithm::optimal::epsilon);
    TIME_END(timing);
    displayResult(OptimalMethod::particle, tuple, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void OptimalSolution::geneticMethod(const Function& func, const double left, const double right)
try
{
    TIME_BEGIN(timing);
    const auto& tuple = algorithm::optimal::Genetic(func)(left, right, algorithm::optimal::epsilon);
    TIME_END(timing);
    displayResult(OptimalMethod::genetic, tuple, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace optimal

//! @brief Update optimal-related choice.
//! @param target - target method
template <>
void updateChoice<OptimalMethod>(const std::string& target)
{
    constexpr auto category = Category::optimal;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(OptimalMethod::gradient):
            bitFlag.set(OptimalMethod::gradient);
            break;
        case abbrVal(OptimalMethod::annealing):
            bitFlag.set(OptimalMethod::annealing);
            break;
        case abbrVal(OptimalMethod::particle):
            bitFlag.set(OptimalMethod::particle);
            break;
        case abbrVal(OptimalMethod::genetic):
            bitFlag.set(OptimalMethod::genetic);
            break;
        default:
            bitFlag.reset();
            throw std::invalid_argument("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run optimal-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<OptimalMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::optimal;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    using optimal::InputBuilder, optimal::input::Rastrigin;
    const auto calcFunc =
        [&candidates, bitFlag](const optimal::Function& function, const optimal::FuncRange<double, double>& range)
    {
        auto& pooling = action::resourcePool();
        auto* const threads = pooling.newElement(std::min(
            static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<OptimalMethod>::value)));
        const auto optimalFunctor = [threads, &function, &range](
                                        const std::string& threadName,
                                        void (*targetMethod)(const optimal::Function&, const double, const double))
        {
            threads->enqueue(threadName, targetMethod, std::ref(function), range.range1, range.range2);
        };
        const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

        using optimal::OptimalSolution;
        for (std::uint8_t i = 0; i < Bottom<OptimalMethod>::value; ++i)
        {
            if (!bitFlag.test(OptimalMethod(i)))
            {
                continue;
            }

            const std::string target = candidates.at(i);
            switch (utility::common::bkdrHash(target.data()))
            {
                case abbrVal(OptimalMethod::gradient):
                    optimalFunctor(name(target), &OptimalSolution::gradientDescentMethod);
                    break;
                case abbrVal(OptimalMethod::annealing):
                    optimalFunctor(name(target), &OptimalSolution::simulatedAnnealingMethod);
                    break;
                case abbrVal(OptimalMethod::particle):
                    optimalFunctor(name(target), &OptimalSolution::particleSwarmMethod);
                    break;
                case abbrVal(OptimalMethod::genetic):
                    optimalFunctor(name(target), &OptimalSolution::geneticMethod);
                    break;
                default:
                    throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
            }
        }
        pooling.deleteElement(threads);
    };

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);

    static_assert(Rastrigin::range1 < Rastrigin::range2);
    const auto inputs = std::make_shared<InputBuilder<Rastrigin>>(optimal::OptimalFuncMap<Rastrigin>{
        {{Rastrigin::range1, Rastrigin::range2, Rastrigin::funcDescr}, Rastrigin{}}});
    for ([[maybe_unused]] const auto& [range, function] : inputs->getFunctionMap())
    {
        inputs->printFunction(function);
        switch (function.index())
        {
            case 0:
                calcFunc(std::get<0>(function), range);
                break;
            [[unlikely]] default:
                break;
        }
    }

    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace search
{
//! @brief Display the contents of the search result.
//! @param method - the specific value of SearchMethod enum
//! @param result - search result
//! @param key - search key
//! @param interval - time interval
static void displayResult(const SearchMethod method, const std::int64_t result, const float key, const double interval)
{
    if (-1 != result)
    {
        COMMON_PRINT(
            "\n==> %-13s Method <==\nfound the key \"%.5f\" that appears in the index %d, run time: %8.5f ms\n",
            getTitle(method).data(),
            key,
            result,
            interval);
    }
    else
    {
        COMMON_PRINT(
            "\n==> %-13s Method <==\ncould not find the key \"%.5f\", run time: %8.5f ms\n",
            getTitle(method).data(),
            key,
            interval);
    }
}

void SearchSolution::binaryMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    TIME_BEGIN(timing);
    const auto index = algorithm::search::Search<float>().binary(array, length, key);
    TIME_END(timing);
    displayResult(SearchMethod::binary, index, key, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SearchSolution::interpolationMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    TIME_BEGIN(timing);
    const auto index = algorithm::search::Search<float>().interpolation(array, length, key);
    TIME_END(timing);
    displayResult(SearchMethod::interpolation, index, key, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SearchSolution::fibonacciMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    TIME_BEGIN(timing);
    const auto index = algorithm::search::Search<float>().fibonacci(array, length, key);
    TIME_END(timing);
    displayResult(SearchMethod::fibonacci, index, key, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace search

//! @brief Update search-related choice.
//! @param target - target method
template <>
void updateChoice<SearchMethod>(const std::string& target)
{
    constexpr auto category = Category::search;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(SearchMethod::binary):
            bitFlag.set(SearchMethod::binary);
            break;
        case abbrVal(SearchMethod::interpolation):
            bitFlag.set(SearchMethod::interpolation);
            break;
        case abbrVal(SearchMethod::fibonacci):
            bitFlag.set(SearchMethod::fibonacci);
            break;
        default:
            bitFlag.reset();
            throw std::invalid_argument("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run search-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<SearchMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::search;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using search::SearchSolution, search::InputBuilder, search::input::arrayLength, search::input::arrayRange1,
        search::input::arrayRange2;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<SearchMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder<float>>(arrayLength, arrayRange1, arrayRange2);
    const auto searchFunctor =
        [threads, inputs](
            const std::string& threadName, void (*targetMethod)(const float* const, const std::uint32_t, const float))
    {
        threads->enqueue(
            threadName, targetMethod, inputs->getOrderedArray().get(), inputs->getLength(), inputs->getSearchKey());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<SearchMethod>::value; ++i)
    {
        if (!bitFlag.test(SearchMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(SearchMethod::binary):
                searchFunctor(name(target), &SearchSolution::binaryMethod);
                break;
            case abbrVal(SearchMethod::interpolation):
                searchFunctor(name(target), &SearchSolution::interpolationMethod);
                break;
            case abbrVal(SearchMethod::fibonacci):
                searchFunctor(name(target), &SearchSolution::fibonacciMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}

namespace sort
{
//! @brief Display the contents of the sort result.
//! @param method - the specific value of SortMethod enum
//! @param result - sort result
//! @param interval - time interval
static void displayResult(const SortMethod method, const std::vector<std::int32_t>& result, const double interval)
{
    const std::uint32_t arrayBufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> arrayBuffer(arrayBufferSize + 1);
    COMMON_PRINT(
        "\n==> %-9s Method <==\n%s\n(asc) run time: %8.5f ms\n",
        getTitle(method).data(),
        InputBuilder<std::int32_t>::template spliceAll<std::int32_t>(
            result.data(), result.size(), arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void SortSolution::bubbleMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().bubble(array, length);
    TIME_END(timing);
    displayResult(SortMethod::bubble, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::selectionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().selection(array, length);
    TIME_END(timing);
    displayResult(SortMethod::selection, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::insertionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().insertion(array, length);
    TIME_END(timing);
    displayResult(SortMethod::insertion, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::shellMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().shell(array, length);
    TIME_END(timing);
    displayResult(SortMethod::shell, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::mergeMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().merge(array, length);
    TIME_END(timing);
    displayResult(SortMethod::merge, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::quickMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().quick(array, length);
    TIME_END(timing);
    displayResult(SortMethod::quick, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::heapMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().heap(array, length);
    TIME_END(timing);
    displayResult(SortMethod::heap, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::countingMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().counting(array, length);
    TIME_END(timing);
    displayResult(SortMethod::counting, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::bucketMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().bucket(array, length);
    TIME_END(timing);
    displayResult(SortMethod::bucket, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}

void SortSolution::radixMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    TIME_BEGIN(timing);
    const auto& coll = algorithm::sort::Sort<std::int32_t>().radix(array, length);
    TIME_END(timing);
    displayResult(SortMethod::radix, coll, TIME_INTERVAL(timing));
}
catch (const std::exception& err)
{
    LOG_ERR << "Interrupt " << __func__ << ": " << err.what();
}
} // namespace sort

//! @brief Update sort-related choice.
//! @param target - target method
template <>
void updateChoice<SortMethod>(const std::string& target)
{
    constexpr auto category = Category::sort;
    auto& bitFlag = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrVal(SortMethod::bubble):
            bitFlag.set(SortMethod::bubble);
            break;
        case abbrVal(SortMethod::selection):
            bitFlag.set(SortMethod::selection);
            break;
        case abbrVal(SortMethod::insertion):
            bitFlag.set(SortMethod::insertion);
            break;
        case abbrVal(SortMethod::shell):
            bitFlag.set(SortMethod::shell);
            break;
        case abbrVal(SortMethod::merge):
            bitFlag.set(SortMethod::merge);
            break;
        case abbrVal(SortMethod::quick):
            bitFlag.set(SortMethod::quick);
            break;
        case abbrVal(SortMethod::heap):
            bitFlag.set(SortMethod::heap);
            break;
        case abbrVal(SortMethod::counting):
            bitFlag.set(SortMethod::counting);
            break;
        case abbrVal(SortMethod::bucket):
            bitFlag.set(SortMethod::bucket);
            break;
        case abbrVal(SortMethod::radix):
            bitFlag.set(SortMethod::radix);
            break;
        default:
            bitFlag.reset();
            throw std::invalid_argument("Unexpected " + std::string{toString(category)} + " method: " + target + '.');
    }
}

//! @brief Run sort-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<SortMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::sort;
    const auto& bitFlag = getCategoryOpts<category>();
    if (bitFlag.none())
    {
        return;
    }

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using sort::SortSolution, sort::InputBuilder, sort::input::arrayLength, sort::input::arrayRange1,
        sort::input::arrayRange2;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(
        std::min(static_cast<std::uint32_t>(bitFlag.count()), static_cast<std::uint32_t>(Bottom<SortMethod>::value)));
    const auto inputs = std::make_shared<InputBuilder<std::int32_t>>(arrayLength, arrayRange1, arrayRange2);
    const auto sortFunctor =
        [threads,
         inputs](const std::string& threadName, void (*targetMethod)(const std::int32_t* const, const std::uint32_t))
    {
        threads->enqueue(threadName, targetMethod, inputs->getRandomArray().get(), inputs->getLength());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());

    for (std::uint8_t i = 0; i < Bottom<SortMethod>::value; ++i)
    {
        if (!bitFlag.test(SortMethod(i)))
        {
            continue;
        }

        const std::string target = candidates.at(i);
        switch (utility::common::bkdrHash(target.data()))
        {
            case abbrVal(SortMethod::bubble):
                sortFunctor(name(target), &SortSolution::bubbleMethod);
                break;
            case abbrVal(SortMethod::selection):
                sortFunctor(name(target), &SortSolution::selectionMethod);
                break;
            case abbrVal(SortMethod::insertion):
                sortFunctor(name(target), &SortSolution::insertionMethod);
                break;
            case abbrVal(SortMethod::shell):
                sortFunctor(name(target), &SortSolution::shellMethod);
                break;
            case abbrVal(SortMethod::merge):
                sortFunctor(name(target), &SortSolution::mergeMethod);
                break;
            case abbrVal(SortMethod::quick):
                sortFunctor(name(target), &SortSolution::quickMethod);
                break;
            case abbrVal(SortMethod::heap):
                sortFunctor(name(target), &SortSolution::heapMethod);
                break;
            case abbrVal(SortMethod::counting):
                sortFunctor(name(target), &SortSolution::countingMethod);
                break;
            case abbrVal(SortMethod::bucket):
                sortFunctor(name(target), &SortSolution::bucketMethod);
                break;
            case abbrVal(SortMethod::radix):
                sortFunctor(name(target), &SortSolution::radixMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_algo
