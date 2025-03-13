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
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
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
static const auto& taskNameCurried()
{
    static const auto curried = utility::currying::curry(action::presetTaskName, TypeInfo<ApplyAlgorithm>::name);
    return curried;
}

//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::match:
            return TypeInfo<MatchMethod>::name;
        case Category::notation:
            return TypeInfo<NotationMethod>::name;
        case Category::optimal:
            return TypeInfo<OptimalMethod>::name;
        case Category::search:
            return TypeInfo<SearchMethod>::name;
        case Category::sort:
            return TypeInfo<SortMethod>::name;
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
    return std::invoke(TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString<Cat>())).value, manager());
}

//! @brief Get the alias of the category in algorithm choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
consteval std::string_view getCategoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString<Cat>())).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

//! @brief Abbreviation value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return abbreviation value
template <typename T>
consteval std::size_t abbrVal(const T method)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    std::size_t value = 0;
    TypeInfo<T>::fields.forEach(
        [method, &value](const auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(1 == field.attrs.size);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
            }
        });

    return value;
}

//! @brief Get the title of a particular method in algorithm choices.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <typename T>
std::string getTitle(const T method)
{
    std::string title(toString(method));
    title.at(0) = std::toupper(title.at(0));

    return title;
}

// clang-format off
//! @brief Mapping table for enum and string about match methods. X macro.
#define APP_ALGO_MATCH_METHOD_TABLE            \
    ELEM(rabinKarp       , "rabinKarp"       ) \
    ELEM(knuthMorrisPratt, "knuthMorrisPratt") \
    ELEM(boyerMoore      , "boyerMoore"      ) \
    ELEM(horspool        , "horspool"        ) \
    ELEM(sunday          , "sunday"          )
// clang-format on
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

// clang-format off
//! @brief Mapping table for enum and string about notation methods. X macro.
#define APP_ALGO_NOTATION_METHOD_TABLE \
    ELEM(prefix , "prefix" )           \
    ELEM(postfix, "postfix")
// clang-format on
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

// clang-format off
//! @brief Mapping table for enum and string about optimal methods. X macro.
#define APP_ALGO_OPTIMAL_METHOD_TABLE \
    ELEM(gradient , "gradient" )      \
    ELEM(annealing, "annealing")      \
    ELEM(particle , "particle" )      \
    ELEM(genetic  , "genetic"  )
// clang-format on
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

// clang-format off
//! @brief Mapping table for enum and string about search methods. X macro.
#define APP_ALGO_SEARCH_METHOD_TABLE     \
    ELEM(binary       , "binary"       ) \
    ELEM(interpolation, "interpolation") \
    ELEM(fibonacci    , "fibonacci"    )
// clang-format on
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

// clang-format off
//! @brief Mapping table for enum and string about sort methods. X macro.
#define APP_ALGO_SORT_METHOD_TABLE \
    ELEM(bubble   , "bubble"   )   \
    ELEM(selection, "selection")   \
    ELEM(insertion, "insertion")   \
    ELEM(shell    , "shell"    )   \
    ELEM(merge    , "merge"    )   \
    ELEM(quick    , "quick"    )   \
    ELEM(heap     , "heap"     )   \
    ELEM(counting , "counting" )   \
    ELEM(bucket   , "bucket"   )   \
    ELEM(radix    , "radix"    )
// clang-format on
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
            getTitle(method).c_str(),
            pattern,
            result,
            interval);
    }
    else
    {
        std::printf(
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
    const utility::time::Time timer{};

    const auto shift = algorithm::match::Match().rk(text, pattern, textLen, patternLen);
    showResult(MatchMethod::rabinKarp, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};

    const auto shift = algorithm::match::Match().kmp(text, pattern, textLen, patternLen);
    showResult(MatchMethod::knuthMorrisPratt, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};

    const auto shift = algorithm::match::Match().bm(text, pattern, textLen, patternLen);
    showResult(MatchMethod::boyerMoore, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};

    const auto shift = algorithm::match::Match().horspool(text, pattern, textLen, patternLen);
    showResult(MatchMethod::horspool, shift, pattern, timer.elapsedTime());
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
    const utility::time::Time timer{};

    const auto shift = algorithm::match::Match().sunday(text, pattern, textLen, patternLen);
    showResult(MatchMethod::sunday, shift, pattern, timer.elapsedTime());
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
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
    using match::InputBuilder, match::input::patternString;
    static_assert(InputBuilder::maxDigit > patternString.length());
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(patternString);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(MatchMethod::rabinKarp):
                addTask(target, &MatchSolution::rkMethod);
                break;
            case abbrVal(MatchMethod::knuthMorrisPratt):
                addTask(target, &MatchSolution::kmpMethod);
                break;
            case abbrVal(MatchMethod::boyerMoore):
                addTask(target, &MatchSolution::bmMethod);
                break;
            case abbrVal(MatchMethod::horspool):
                addTask(target, &MatchSolution::horspoolMethod);
                break;
            case abbrVal(MatchMethod::sunday):
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
    std::printf("\n==> %-7s Method <==\n%s: %s\n", getTitle(method).c_str(), descr, result.data());
}

void NotationSolution::prefixMethod(const std::string_view infix)
try
{
    const auto expr = algorithm::notation::Notation().prefix(infix);
    showResult(NotationMethod::prefix, expr, "polish notation");
}
catch (const std::exception& err)
{
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
}

void NotationSolution::postfixMethod(const std::string_view infix)
try
{
    const auto expr = algorithm::notation::Notation().postfix(infix);
    showResult(NotationMethod::postfix, expr, "reverse polish notation");
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
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
    using notation::InputBuilder, notation::input::infixString;
    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(infixString);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(NotationMethod::prefix):
                addTask(target, &NotationSolution::prefixMethod);
                break;
            case abbrVal(NotationMethod::postfix):
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
            getTitle(method).c_str(),
            std::get<0>(result.value()),
            std::get<1>(result.value()),
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-9s Method <==\nF(min) could not be found, run time: %8.5f ms\n",
            getTitle(method).c_str(),
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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

    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto calcFunc = [&candidates, &bits, &taskNamer](
                              const optimal::Function& function, const optimal::FuncRange<double, double>& range)
    {
        auto& pooling = action::resourcePool();
        auto* const threads = pooling.newElement(bits.count());
        const auto addTask = [threads, &function, &range, &taskNamer](
                                 const std::string_view subTask,
                                 void (*targetMethod)(const optimal::Function&, const double, const double))
        { threads->enqueue(taskNamer(subTask), targetMethod, std::ref(function), range.range1, range.range2); };

        for (const auto index :
             std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
        {
            const auto& target = candidates.at(index);
            switch (utility::common::bkdrHash(target.c_str()))
            {
                using optimal::OptimalSolution;
                case abbrVal(OptimalMethod::gradient):
                    addTask(target, &OptimalSolution::gradientDescentMethod);
                    break;
                case abbrVal(OptimalMethod::annealing):
                    addTask(target, &OptimalSolution::simulatedAnnealingMethod);
                    break;
                case abbrVal(OptimalMethod::particle):
                    addTask(target, &OptimalSolution::particleSwarmMethod);
                    break;
                case abbrVal(OptimalMethod::genetic):
                    addTask(target, &OptimalSolution::geneticMethod);
                    break;
                default:
                    throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
            }
        }
        pooling.deleteElement(threads);
    };

    APP_ALGO_PRINT_TASK_BEGIN_TITLE(category);

    using optimal::InputBuilder, optimal::input::Rastrigin;
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
            getTitle(method).c_str(),
            key,
            result,
            interval);
    }
    else
    {
        std::printf(
            "\n==> %-13s Method <==\ncould not find the key \"%.5f\", run time: %8.5f ms\n",
            getTitle(method).c_str(),
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
    using search::InputBuilder, search::input::arrayLength, search::input::arrayRangeMin, search::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder<float>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(SearchMethod::binary):
                addTask(target, &SearchSolution::binaryMethod);
                break;
            case abbrVal(SearchMethod::interpolation):
                addTask(target, &SearchSolution::interpolationMethod);
                break;
            case abbrVal(SearchMethod::fibonacci):
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
        getTitle(method).c_str(),
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
    LOG_ERR << "Exception in solution (" << __func__ << "): " << err.what();
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
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
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
    using sort::InputBuilder, sort::input::arrayLength, sort::input::arrayRangeMin, sort::input::arrayRangeMax;
    static_assert((arrayRangeMin < arrayRangeMax) && (arrayLength > 0));

    auto& pooling = action::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder<std::int32_t>>(arrayLength, arrayRangeMin, arrayRangeMax);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
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
            case abbrVal(SortMethod::bubble):
                addTask(target, &SortSolution::bubbleMethod);
                break;
            case abbrVal(SortMethod::selection):
                addTask(target, &SortSolution::selectionMethod);
                break;
            case abbrVal(SortMethod::insertion):
                addTask(target, &SortSolution::insertionMethod);
                break;
            case abbrVal(SortMethod::shell):
                addTask(target, &SortSolution::shellMethod);
                break;
            case abbrVal(SortMethod::merge):
                addTask(target, &SortSolution::mergeMethod);
                break;
            case abbrVal(SortMethod::quick):
                addTask(target, &SortSolution::quickMethod);
                break;
            case abbrVal(SortMethod::heap):
                addTask(target, &SortSolution::heapMethod);
                break;
            case abbrVal(SortMethod::counting):
                addTask(target, &SortSolution::countingMethod);
                break;
            case abbrVal(SortMethod::bucket):
                addTask(target, &SortSolution::bucketMethod);
                break;
            case abbrVal(SortMethod::radix):
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
