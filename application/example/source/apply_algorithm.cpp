//! @file apply_algorithm.cpp
//! @author ryftchen
//! @brief The definitions (apply_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_algorithm.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <iomanip>
#include <ranges>
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
consteval std::string_view toString(const Category cat)
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
            break;
    }

    return {};
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
consteval std::string_view getCategoryAlias()
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
        [method, &value](const auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(1 == field.attrs.size);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
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
    std::string title(toString(method));
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
            getTitle(method).c_str(),
            pattern,
            result,
            interval);
    }
    else
    {
        COMMON_PRINT(
            "\n==> %-16s Method <==\npattern \"%s\" could not be found, run time: %8.5f ms\n",
            getTitle(method).c_str(),
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
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto shift = algorithm::match::Match().rk(text, pattern, textLen, patternLen);
    timer.setEndTime();
    displayResult(MatchMethod::rabinKarp, shift, pattern, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void MatchSolution::kmpMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto shift = algorithm::match::Match().kmp(text, pattern, textLen, patternLen);
    timer.setEndTime();
    displayResult(MatchMethod::knuthMorrisPratt, shift, pattern, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void MatchSolution::bmMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto shift = algorithm::match::Match().bm(text, pattern, textLen, patternLen);
    timer.setEndTime();
    displayResult(MatchMethod::boyerMoore, shift, pattern, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void MatchSolution::horspoolMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto shift = algorithm::match::Match().horspool(text, pattern, textLen, patternLen);
    timer.setEndTime();
    displayResult(MatchMethod::horspool, shift, pattern, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void MatchSolution::sundayMethod(
    const unsigned char* const text,
    const unsigned char* const pattern,
    const std::uint32_t textLen,
    const std::uint32_t patternLen)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto shift = algorithm::match::Match().sunday(text, pattern, textLen, patternLen);
    timer.setEndTime();
    displayResult(MatchMethod::sunday, shift, pattern, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target.data() + '.');
    }
}

//! @brief Run match-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<MatchMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::match;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using match::MatchSolution, match::InputBuilder, match::input::patternString;
    static_assert(InputBuilder::maxDigit > patternString.length());
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(patternString);
    const auto functor =
        [threads, &inputs](
            const std::string_view threadName,
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
    auto indices =
        std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

    for (const auto index : indices)
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            case abbrVal(MatchMethod::rabinKarp):
                functor(name(target), &MatchSolution::rkMethod);
                break;
            case abbrVal(MatchMethod::knuthMorrisPratt):
                functor(name(target), &MatchSolution::kmpMethod);
                break;
            case abbrVal(MatchMethod::boyerMoore):
                functor(name(target), &MatchSolution::bmMethod);
                break;
            case abbrVal(MatchMethod::horspool):
                functor(name(target), &MatchSolution::horspoolMethod);
                break;
            case abbrVal(MatchMethod::sunday):
                functor(name(target), &MatchSolution::sundayMethod);
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
static void displayResult(const NotationMethod method, const std::string_view result, const char* const descr)
{
    COMMON_PRINT("\n==> %-7s Method <==\n%s: %s\n", getTitle(method).c_str(), descr, result.data());
}

void NotationSolution::prefixMethod(const std::string_view infix)
try
{
    const auto expr = algorithm::notation::Notation().prefix(infix);
    displayResult(NotationMethod::prefix, expr, "polish notation");
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void NotationSolution::postfixMethod(const std::string_view infix)
try
{
    const auto expr = algorithm::notation::Notation().postfix(infix);
    displayResult(NotationMethod::postfix, expr, "reverse polish notation");
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target.data() + '.');
    }
}

//! @brief Run notation-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<NotationMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::notation;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using notation::NotationSolution, notation::InputBuilder, notation::input::infixString;
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(infixString);
    const auto functor =
        [threads, &inputs](const std::string_view threadName, void (*targetMethod)(const std::string_view))
    { threads->enqueue(threadName, targetMethod, inputs->getInfixNotation()); };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());
    auto indices =
        std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

    for (const auto index : indices)
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            case abbrVal(NotationMethod::prefix):
                functor(name(target), &NotationSolution::prefixMethod);
                break;
            case abbrVal(NotationMethod::postfix):
                functor(name(target), &NotationSolution::postfixMethod);
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
            getTitle(method).c_str(),
            std::get<0>(result.value()),
            std::get<1>(result.value()),
            interval);
    }
    else
    {
        COMMON_PRINT(
            "\n==> %-9s Method <==\nF(min) could not be found, run time: %8.5f ms\n",
            getTitle(method).c_str(),
            interval);
    }
}

void OptimalSolution::gradientDescentMethod(const Function& func, const double left, const double right)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto tuple = algorithm::optimal::Gradient(func)(left, right, algorithm::optimal::epsilon);
    timer.setEndTime();
    displayResult(OptimalMethod::gradient, tuple, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void OptimalSolution::simulatedAnnealingMethod(const Function& func, const double left, const double right)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto tuple = algorithm::optimal::Annealing(func)(left, right, algorithm::optimal::epsilon);
    timer.setEndTime();
    displayResult(OptimalMethod::annealing, tuple, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void OptimalSolution::particleSwarmMethod(const Function& func, const double left, const double right)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto tuple = algorithm::optimal::Particle(func)(left, right, algorithm::optimal::epsilon);
    timer.setEndTime();
    displayResult(OptimalMethod::particle, tuple, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void OptimalSolution::geneticMethod(const Function& func, const double left, const double right)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto tuple = algorithm::optimal::Genetic(func)(left, right, algorithm::optimal::epsilon);
    timer.setEndTime();
    displayResult(OptimalMethod::genetic, tuple, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
        case abbrVal(OptimalMethod::annealing):
            bits.set(OptimalMethod::annealing);
            break;
        case abbrVal(OptimalMethod::particle):
            bits.set(OptimalMethod::particle);
            break;
        case abbrVal(OptimalMethod::genetic):
            bits.set(OptimalMethod::genetic);
            break;
        default:
            bits.reset();
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target.data() + '.');
    }
}

//! @brief Run optimal-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<OptimalMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::optimal;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    using optimal::InputBuilder, optimal::input::Rastrigin;
    const auto calcFunc =
        [&candidates, &bits](const optimal::Function& function, const optimal::FuncRange<double, double>& range)
    {
        auto& pooling = action::resourcePool();
        auto* const threads = pooling.newElement(bits.count());
        const auto functor = [threads, &function, &range](
                                 const std::string_view threadName,
                                 void (*targetMethod)(const optimal::Function&, const double, const double))
        { threads->enqueue(threadName, targetMethod, std::ref(function), range.range1, range.range2); };
        const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());
        auto indices =
            std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

        using optimal::OptimalSolution;
        for (const auto index : indices)
        {
            const auto& target = candidates.at(index);
            switch (utility::common::bkdrHash(target.c_str()))
            {
                case abbrVal(OptimalMethod::gradient):
                    functor(name(target), &OptimalSolution::gradientDescentMethod);
                    break;
                case abbrVal(OptimalMethod::annealing):
                    functor(name(target), &OptimalSolution::simulatedAnnealingMethod);
                    break;
                case abbrVal(OptimalMethod::particle):
                    functor(name(target), &OptimalSolution::particleSwarmMethod);
                    break;
                case abbrVal(OptimalMethod::genetic):
                    functor(name(target), &OptimalSolution::geneticMethod);
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
            getTitle(method).c_str(),
            key,
            result,
            interval);
    }
    else
    {
        COMMON_PRINT(
            "\n==> %-13s Method <==\ncould not find the key \"%.5f\", run time: %8.5f ms\n",
            getTitle(method).c_str(),
            key,
            interval);
    }
}

void SearchSolution::binaryMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto index = algorithm::search::Search<float>().binary(array, length, key);
    timer.setEndTime();
    displayResult(SearchMethod::binary, index, key, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SearchSolution::interpolationMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto index = algorithm::search::Search<float>().interpolation(array, length, key);
    timer.setEndTime();
    displayResult(SearchMethod::interpolation, index, key, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SearchSolution::fibonacciMethod(const float* const array, const std::uint32_t length, const float key)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto index = algorithm::search::Search<float>().fibonacci(array, length, key);
    timer.setEndTime();
    displayResult(SearchMethod::fibonacci, index, key, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target.data() + '.');
    }
}

//! @brief Run search-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<SearchMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::search;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using search::SearchSolution, search::InputBuilder, search::input::arrayLength, search::input::arrayRange1,
        search::input::arrayRange2;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder<float>>(arrayLength, arrayRange1, arrayRange2);
    const auto functor = [threads, &inputs](
                             const std::string_view threadName,
                             void (*targetMethod)(const float* const, const std::uint32_t, const float))
    {
        threads->enqueue(
            threadName, targetMethod, inputs->getOrderedArray().get(), inputs->getLength(), inputs->getSearchKey());
    };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());
    auto indices =
        std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

    for (const auto index : indices)
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            case abbrVal(SearchMethod::binary):
                functor(name(target), &SearchSolution::binaryMethod);
                break;
            case abbrVal(SearchMethod::interpolation):
                functor(name(target), &SearchSolution::interpolationMethod);
                break;
            case abbrVal(SearchMethod::fibonacci):
                functor(name(target), &SearchSolution::fibonacciMethod);
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
        getTitle(method).c_str(),
        InputBuilder<std::int32_t>::template spliceAll<std::int32_t>(
            result.data(), result.size(), arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void SortSolution::bubbleMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().bubble(array, length);
    timer.setEndTime();
    displayResult(SortMethod::bubble, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::selectionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().selection(array, length);
    timer.setEndTime();
    displayResult(SortMethod::selection, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::insertionMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().insertion(array, length);
    timer.setEndTime();
    displayResult(SortMethod::insertion, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::shellMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().shell(array, length);
    timer.setEndTime();
    displayResult(SortMethod::shell, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::mergeMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().merge(array, length);
    timer.setEndTime();
    displayResult(SortMethod::merge, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::quickMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().quick(array, length);
    timer.setEndTime();
    displayResult(SortMethod::quick, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::heapMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().heap(array, length);
    timer.setEndTime();
    displayResult(SortMethod::heap, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::countingMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().counting(array, length);
    timer.setEndTime();
    displayResult(SortMethod::counting, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::bucketMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().bucket(array, length);
    timer.setEndTime();
    displayResult(SortMethod::bucket, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void SortSolution::radixMethod(const std::int32_t* const array, const std::uint32_t length)
try
{
    utility::time::Time timer{};
    timer.setBeginTime();
    const auto coll = algorithm::sort::Sort<std::int32_t>().radix(array, length);
    timer.setEndTime();
    displayResult(SortMethod::radix, coll, timer.calculateInterval());
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error("Unexpected " + std::string{toString(category)} + " method: " + target.data() + '.');
    }
}

//! @brief Run sort-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<SortMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::sort;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);
    using sort::SortSolution, sort::InputBuilder, sort::input::arrayLength, sort::input::arrayRange1,
        sort::input::arrayRange2;
    static_assert((arrayRange1 < arrayRange2) && (arrayLength > 0));

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder<std::int32_t>>(arrayLength, arrayRange1, arrayRange2);
    const auto functor =
        [threads, &inputs](
            const std::string_view threadName, void (*targetMethod)(const std::int32_t* const, const std::uint32_t))
    { threads->enqueue(threadName, targetMethod, inputs->getRandomArray().get(), inputs->getLength()); };
    const auto name = utility::currying::curry(getTaskNameCurried(), getCategoryAlias<category>());
    auto indices =
        std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); });

    for (const auto index : indices)
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            case abbrVal(SortMethod::bubble):
                functor(name(target), &SortSolution::bubbleMethod);
                break;
            case abbrVal(SortMethod::selection):
                functor(name(target), &SortSolution::selectionMethod);
                break;
            case abbrVal(SortMethod::insertion):
                functor(name(target), &SortSolution::insertionMethod);
                break;
            case abbrVal(SortMethod::shell):
                functor(name(target), &SortSolution::shellMethod);
                break;
            case abbrVal(SortMethod::merge):
                functor(name(target), &SortSolution::mergeMethod);
                break;
            case abbrVal(SortMethod::quick):
                functor(name(target), &SortSolution::quickMethod);
                break;
            case abbrVal(SortMethod::heap):
                functor(name(target), &SortSolution::heapMethod);
                break;
            case abbrVal(SortMethod::counting):
                functor(name(target), &SortSolution::countingMethod);
                break;
            case abbrVal(SortMethod::bucket):
                functor(name(target), &SortSolution::bucketMethod);
                break;
            case abbrVal(SortMethod::radix):
                functor(name(target), &SortSolution::radixMethod);
                break;
            default:
                throw std::logic_error("Unknown " + std::string{toString(category)} + " method: " + target + '.');
        }
    }

    pooling.deleteElement(threads);
    APP_ALGO_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_algo
