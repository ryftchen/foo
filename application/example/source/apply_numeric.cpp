//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_numeric.hpp"

#ifndef __PRECOMPILED_HEADER
#include <cassert>
#include <iomanip>
#include <ranges>
#include <syncstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "application/core/include/log.hpp"
#include "application/parameter/include/register_numeric.hpp"

//! @brief Title of printing when numeric tasks are beginning.
#define APP_NUM_PRINT_TASK_BEGIN_TITLE(category)                                                                    \
    std::osyncstream(std::cout) << "\nNUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "BEGIN" << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << std::endl;                                                  \
    {
//! @brief Title of printing when numeric tasks are ending.
#define APP_NUM_PRINT_TASK_END_TITLE(category)                                                                      \
    }                                                                                                               \
    std::osyncstream(std::cout) << "\nNUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << category << "END" << std::resetiosflags(std::ios_base::left)    \
                                << std::setfill(' ') << '\n'                                                        \
                                << std::endl;

namespace application::app_num
{
//! @brief Alias for Category.
using Category = ApplyNumeric::Category;
using reg_num::taskNameCurried, reg_num::toString, reg_num::getCategoryOpts, reg_num::getCategoryAlias,
    reg_num::abbrVal;

//! @brief Get the numeric choice manager.
//! @return reference of the ApplyNumeric object
ApplyNumeric& manager()
{
    static ApplyNumeric manager{};
    return manager;
}

//! @brief Get the title of a particular method in numeric choices.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <typename T>
static std::string getTitle(const T method)
{
    std::string title(toString(method));
    title.at(0) = std::toupper(title.at(0));

    return title;
}

// clang-format off
//! @brief Mapping table for enum and string about arithmetic methods. X macro.
#define APP_NUM_ARITHMETIC_METHOD_TABLE    \
    ELEM(addition      , "addition"      ) \
    ELEM(subtraction   , "subtraction"   ) \
    ELEM(multiplication, "multiplication") \
    ELEM(division      , "division"      )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of ArithmeticMethod enum
//! @return method name
static constexpr std::string_view toString(const ArithmeticMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_ARITHMETIC_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<ArithmeticMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_ARITHMETIC_METHOD_TABLE

// clang-format off
//! @brief Mapping table for enum and string about divisor methods. X macro.
#define APP_NUM_DIVISOR_METHOD_TABLE \
    ELEM(euclidean, "euclidean")     \
    ELEM(stein    , "stein"    )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of DivisorMethod enum
//! @return method name
static constexpr std::string_view toString(const DivisorMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_DIVISOR_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<DivisorMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_DIVISOR_METHOD_TABLE

// clang-format off
//! @brief Mapping table for enum and string about integral methods. X macro.
#define APP_NUM_INTEGRAL_METHOD_TABLE \
    ELEM(trapezoidal, "trapezoidal")  \
    ELEM(simpson    , "simpson"    )  \
    ELEM(romberg    , "romberg"    )  \
    ELEM(gauss      , "gauss"      )  \
    ELEM(monteCarlo , "monteCarlo" )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of IntegralMethod enum
//! @return method name
static constexpr std::string_view toString(const IntegralMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_INTEGRAL_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<IntegralMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_INTEGRAL_METHOD_TABLE

// clang-format off
//! @brief Mapping table for enum and string about prime methods. X macro.
#define APP_NUM_PRIME_METHOD_TABLE     \
    ELEM(eratosthenes, "eratosthenes") \
    ELEM(euler       , "euler"       )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of PrimeMethod enum
//! @return method name
static constexpr std::string_view toString(const PrimeMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {APP_NUM_PRIME_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<PrimeMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef APP_NUM_PRIME_METHOD_TABLE

namespace arithmetic
{
//! @brief Show the contents of the arithmetic result.
//! @param method - the specific value of ArithmeticMethod enum
//! @param result - arithmetic result
//! @param a - first integer for elementary arithmetic
//! @param b - second integer for elementary arithmetic
//! @param op - operator of arithmetic
static void showResult(
    const ArithmeticMethod method, const std::int32_t result, const std::int32_t a, const std::int32_t b, const char op)
{
    std::printf("\n==> %-14s Method <==\n(%d) %c (%d) = %d\n", getTitle(method).c_str(), a, op, b, result);
}

void ArithmeticSolution::additionMethod(const std::int32_t augend, const std::int32_t addend)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().addition(augend, addend);
    showResult(ArithmeticMethod::addition, calc, augend, addend, '+');
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void ArithmeticSolution::subtractionMethod(const std::int32_t minuend, const std::int32_t subtrahend)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().subtraction(minuend, subtrahend);
    showResult(ArithmeticMethod::subtraction, calc, minuend, subtrahend, '-');
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void ArithmeticSolution::multiplicationMethod(const std::int32_t multiplier, const std::int32_t multiplicand)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().multiplication(multiplier, multiplicand);
    showResult(ArithmeticMethod::multiplication, calc, multiplier, multiplicand, '*');
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void ArithmeticSolution::divisionMethod(const std::int32_t dividend, const std::int32_t divisor)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().division(dividend, divisor);
    showResult(ArithmeticMethod::division, calc, dividend, divisor, '/');
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace arithmetic

//! @brief Update arithmetic-related choice.
//! @param target - target method
template <>
void updateChoice<ArithmeticMethod>(const std::string_view target)
{
    constexpr auto category = Category::arithmetic;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(ArithmeticMethod::addition):
            bits.set(ArithmeticMethod::addition);
            break;
        case abbrVal(ArithmeticMethod::subtraction):
            bits.set(ArithmeticMethod::subtraction);
            break;
        case abbrVal(ArithmeticMethod::multiplication):
            bits.set(ArithmeticMethod::multiplication);
            break;
        case abbrVal(ArithmeticMethod::division):
            bits.set(ArithmeticMethod::division);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}

//! @brief Run arithmetic-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<ArithmeticMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::arithmetic;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using arithmetic::InputBuilder, arithmetic::input::integerA, arithmetic::input::integerB;
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(integerA, integerB);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::int32_t, const std::int32_t))
    { threads->enqueue(taskNamer(subTask), targetMethod, inputs->getIntegers().first, inputs->getIntegers().second); };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using arithmetic::ArithmeticSolution;
            case abbrVal(ArithmeticMethod::addition):
                addTask(target, &ArithmeticSolution::additionMethod);
                break;
            case abbrVal(ArithmeticMethod::subtraction):
                addTask(target, &ArithmeticSolution::subtractionMethod);
                break;
            case abbrVal(ArithmeticMethod::multiplication):
                addTask(target, &ArithmeticSolution::multiplicationMethod);
                break;
            case abbrVal(ArithmeticMethod::division):
                addTask(target, &ArithmeticSolution::divisionMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

namespace divisor
{
//! @brief Show the contents of the divisor result.
//! @param method - the specific value of DivisorMethod enum
//! @param result - divisor result
//! @param interval - time interval
static void showResult(const DivisorMethod method, const std::set<std::int32_t>& result, const double interval)
{
    const std::uint32_t arrayBufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> arrayBuffer(arrayBufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        getTitle(method).c_str(),
        InputBuilder::template spliceAllIntegers<std::int32_t>(result, arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void DivisorSolution::euclideanMethod(std::int32_t a, std::int32_t b)
try
{
    const utility::time::Time timer{};
    const auto coll = numeric::divisor::Divisor().euclidean(a, b);
    showResult(DivisorMethod::euclidean, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void DivisorSolution::steinMethod(std::int32_t a, std::int32_t b)
try
{
    const utility::time::Time timer{};
    const auto coll = numeric::divisor::Divisor().stein(a, b);
    showResult(DivisorMethod::stein, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace divisor

//! @brief Update divisor-related choice.
//! @param target - target method
template <>
void updateChoice<DivisorMethod>(const std::string_view target)
{
    constexpr auto category = Category::divisor;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(DivisorMethod::euclidean):
            bits.set(DivisorMethod::euclidean);
            break;
        case abbrVal(DivisorMethod::stein):
            bits.set(DivisorMethod::stein);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}

//! @brief Run divisor-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<DivisorMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::divisor;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using divisor::InputBuilder, divisor::input::integerA, divisor::input::integerB;
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(integerA, integerB);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](const std::string_view subTask, void (*targetMethod)(std::int32_t, std::int32_t))
    { threads->enqueue(taskNamer(subTask), targetMethod, inputs->getIntegers().first, inputs->getIntegers().second); };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using divisor::DivisorSolution;
            case abbrVal(DivisorMethod::euclidean):
                addTask(target, &DivisorSolution::euclideanMethod);
                break;
            case abbrVal(DivisorMethod::stein):
                addTask(target, &DivisorSolution::steinMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}

namespace integral
{
//! @brief Show the contents of the integral result.
//! @param method - the specific value of IntegralMethod enum
//! @param result - integral result
//! @param interval - time interval
static void showResult(const IntegralMethod method, const double result, const double interval)
{
    std::printf(
        "\n==> %-11s Method <==\nI(def)=%+.5f, run time: %8.5f ms\n", getTitle(method).c_str(), result, interval);
}

void IntegralSolution::trapezoidalMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Time timer{};
    const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::trapezoidal, sum, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void IntegralSolution::adaptiveSimpsonMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Time timer{};
    const auto sum = numeric::integral::Simpson(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::simpson, sum, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void IntegralSolution::rombergMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Time timer{};
    const auto sum = numeric::integral::Romberg(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::romberg, sum, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void IntegralSolution::gaussLegendreMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Time timer{};
    const auto sum = numeric::integral::Gauss(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::gauss, sum, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void IntegralSolution::monteCarloMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Time timer{};
    const auto sum = numeric::integral::MonteCarlo(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::monteCarlo, sum, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace integral

//! @brief Update integral-related choice.
//! @param target - target method
template <>
void updateChoice<IntegralMethod>(const std::string_view target)
{
    constexpr auto category = Category::integral;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(IntegralMethod::trapezoidal):
            bits.set(IntegralMethod::trapezoidal);
            break;
        case abbrVal(IntegralMethod::simpson):
            bits.set(IntegralMethod::simpson);
            break;
        case abbrVal(IntegralMethod::romberg):
            bits.set(IntegralMethod::romberg);
            break;
        case abbrVal(IntegralMethod::gauss):
            bits.set(IntegralMethod::gauss);
            break;
        case abbrVal(IntegralMethod::monteCarlo):
            bits.set(IntegralMethod::monteCarlo);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}

//! @brief Run integral-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<IntegralMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::integral;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto calcExpr = [&candidates, &bits, &taskNamer](
                              const integral::Expression& expression, const integral::ExprRange<double, double>& range)
    {
        auto& pooling = configure::task::resourcePool();
        auto* const threads = pooling.newElement(bits.count());
        const auto addTask = [threads, &expression, &range, &taskNamer](
                                 const std::string_view subTask,
                                 void (*targetMethod)(const integral::Expression&, const double, const double))
        { threads->enqueue(taskNamer(subTask), targetMethod, std::ref(expression), range.range1, range.range2); };

        for (const auto index :
             std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
        {
            const auto& target = candidates.at(index);
            switch (utility::common::bkdrHash(target.c_str()))
            {
                using integral::IntegralSolution;
                case abbrVal(IntegralMethod::trapezoidal):
                    addTask(target, &IntegralSolution::trapezoidalMethod);
                    break;
                case abbrVal(IntegralMethod::simpson):
                    addTask(target, &IntegralSolution::adaptiveSimpsonMethod);
                    break;
                case abbrVal(IntegralMethod::romberg):
                    addTask(target, &IntegralSolution::rombergMethod);
                    break;
                case abbrVal(IntegralMethod::gauss):
                    addTask(target, &IntegralSolution::gaussLegendreMethod);
                    break;
                case abbrVal(IntegralMethod::monteCarlo):
                    addTask(target, &IntegralSolution::monteCarloMethod);
                    break;
                default:
                    throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
            }
        }
        pooling.deleteElement(threads);
    };

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);

    using integral::InputBuilder, integral::input::Griewank;
    static_assert(numeric::integral::epsilon >= std::numeric_limits<double>::epsilon());
    const auto inputs = std::make_shared<InputBuilder<Griewank>>(
        integral::IntegralExprMap<Griewank>{{{Griewank::range1, Griewank::range2, Griewank::exprDescr}, Griewank{}}});
    for ([[maybe_unused]] const auto& [range, expression] : inputs->getExpressionMap())
    {
        inputs->printExpression(expression);
        switch (expression.index())
        {
            case 0:
                calcExpr(std::get<0>(expression), range);
                break;
            [[unlikely]] default:
                break;
        }
    }

    APP_NUM_PRINT_TASK_END_TITLE(category);
}

namespace prime
{
//! @brief Show the contents of the prime result.
//! @param method - the specific value of PrimeMethod enum
//! @param result - prime result
//! @param interval - time interval
static void showResult(const PrimeMethod method, const std::vector<std::uint32_t>& result, const double interval)
{
    const std::uint32_t arrayBufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> arrayBuffer(arrayBufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        getTitle(method).c_str(),
        InputBuilder::template spliceAllIntegers<std::uint32_t>(result, arrayBuffer.data(), arrayBufferSize + 1),
        interval);
}

void PrimeSolution::eratosthenesMethod(const std::uint32_t max)
try
{
    const utility::time::Time timer{};
    const auto coll = numeric::prime::Prime().eratosthenes(max);
    showResult(PrimeMethod::eratosthenes, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}

void PrimeSolution::eulerMethod(const std::uint32_t max)
try
{
    const utility::time::Time timer{};
    const auto coll = numeric::prime::Prime().euler(max);
    showResult(PrimeMethod::euler, coll, timer.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_ERR_P("Exception in solution (%s): %s", __func__, err.what());
}
} // namespace prime

//! @brief Update prime-related choice.
//! @param target - target method
template <>
void updateChoice<PrimeMethod>(const std::string_view target)
{
    constexpr auto category = Category::prime;
    auto& bits = getCategoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrVal(PrimeMethod::eratosthenes):
            bits.set(PrimeMethod::eratosthenes);
            break;
        case abbrVal(PrimeMethod::euler):
            bits.set(PrimeMethod::euler);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}

//! @brief Run prime-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<PrimeMethod>(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::prime;
    const auto& bits = getCategoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_BEGIN_TITLE(category);
    using prime::InputBuilder, prime::input::maxPositiveInteger;
    auto& pooling = configure::task::resourcePool();
    auto* const threads = pooling.newElement(bits.count());
    const auto inputs = std::make_shared<InputBuilder>(maxPositiveInteger);
    const auto taskNamer = utility::currying::curry(taskNameCurried(), getCategoryAlias<category>());
    const auto addTask =
        [threads, &inputs, &taskNamer](const std::string_view subTask, void (*targetMethod)(const std::uint32_t))
    { threads->enqueue(taskNamer(subTask), targetMethod, inputs->getMaxPositiveInteger()); };

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using prime::PrimeSolution;
            case abbrVal(PrimeMethod::eratosthenes):
                addTask(target, &PrimeSolution::eratosthenesMethod);
                break;
            case abbrVal(PrimeMethod::euler):
                addTask(target, &PrimeSolution::eulerMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    pooling.deleteElement(threads);
    APP_NUM_PRINT_TASK_END_TITLE(category);
}
} // namespace application::app_num
