//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_numeric.hpp"
#include "register_numeric.hpp"

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

//! @brief Title of printing when numeric tasks are beginning.
#define APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(category)                                                               \
    std::osyncstream(std::cout) << "\nNUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.')  \
                                << std::setw(50) << (category) << "BEGIN" << std::resetiosflags(std::ios_base::left) \
                                << std::setfill(' ') << std::endl;                                                   \
    {
//! @brief Title of printing when numeric tasks are ending.
#define APP_NUM_PRINT_TASK_TITLE_SCOPE_END(category)                                                                \
    }                                                                                                               \
    std::osyncstream(std::cout) << "\nNUMERIC TASK: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (category) << "END" << std::resetiosflags(std::ios_base::left)  \
                                << std::setfill(' ') << '\n'                                                        \
                                << std::endl;

namespace application::app_num
{
using namespace reg_num; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular method in numeric choices.
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
    static const auto curried = utility::currying::curry(configure::task::presetName, TypeInfo<ApplyNumeric>::name);
    return curried;
}

//! @brief Get the alias of the category in numeric choices.
//! @tparam Cat - specific value of Category enum
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyNumeric>::fields.find(REFLECTION_STR(toString<Cat>())).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace arithmetic
{
//! @brief Show the contents of the arithmetic result.
//! @param method - specific value of ArithmeticMethod enum
//! @param result - arithmetic result
//! @param a - first integer for elementary arithmetic
//! @param b - second integer for elementary arithmetic
//! @param op - operator of arithmetic
static void showResult(
    const ArithmeticMethod method, const std::int32_t result, const std::int32_t a, const std::int32_t b, const char op)
{
    std::printf("\n==> %-14s Method <==\n(%d) %c (%d) = %d\n", makeTitle(method).c_str(), a, op, b, result);
}

void ArithmeticCalculation::additionMethod(const std::int32_t augend, const std::int32_t addend)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().addition(augend, addend);
    showResult(ArithmeticMethod::addition, calc, augend, addend, '+');
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void ArithmeticCalculation::subtractionMethod(const std::int32_t minuend, const std::int32_t subtrahend)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().subtraction(minuend, subtrahend);
    showResult(ArithmeticMethod::subtraction, calc, minuend, subtrahend, '-');
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void ArithmeticCalculation::multiplicationMethod(const std::int32_t multiplier, const std::int32_t multiplicand)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().multiplication(multiplier, multiplicand);
    showResult(ArithmeticMethod::multiplication, calc, multiplier, multiplicand, '*');
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void ArithmeticCalculation::divisionMethod(const std::int32_t dividend, const std::int32_t divisor)
try
{
    const auto calc = numeric::arithmetic::Arithmetic().division(dividend, divisor);
    showResult(ArithmeticMethod::division, calc, dividend, divisor, '/');
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}
} // namespace arithmetic
//! @brief To apply arithmetic-related methods.
//! @param candidates - container for the candidate target methods
void applyingArithmetic(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::arithmetic;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using arithmetic::InputBuilder, arithmetic::input::operandA, arithmetic::input::operandB;
    const auto inputData = std::make_shared<InputBuilder>(operandA, operandB);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::int32_t, const std::int32_t))
        {
            allocatedJob->enqueue(
                taskNamer(subTask), targetMethod, inputData->getOperands().first, inputData->getOperands().second);
        });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using arithmetic::ArithmeticCalculation;
            static_assert(utility::common::isStatelessClass<ArithmeticCalculation>());
            case abbrValue(ArithmeticMethod::addition):
                addTask(target, &ArithmeticCalculation::additionMethod);
                break;
            case abbrValue(ArithmeticMethod::subtraction):
                addTask(target, &ArithmeticCalculation::subtractionMethod);
                break;
            case abbrValue(ArithmeticMethod::multiplication):
                addTask(target, &ArithmeticCalculation::multiplicationMethod);
                break;
            case abbrValue(ArithmeticMethod::division):
                addTask(target, &ArithmeticCalculation::divisionMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace divisor
{
//! @brief Show the contents of the divisor result.
//! @param method - specific value of DivisorMethod enum
//! @param result - divisor result
//! @param interval - time interval
static void showResult(const DivisorMethod method, const std::set<std::int32_t>& result, const double interval)
{
    const std::uint32_t bufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> fmtBuffer(bufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        makeTitle(method).c_str(),
        InputBuilder::template spliceAllIntegers<std::int32_t>(result, fmtBuffer.data(), bufferSize + 1),
        interval);
}

void DivisorCalculation::euclideanMethod(const std::int32_t a, const std::int32_t b)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = numeric::divisor::Divisor().euclidean(a, b);
    showResult(DivisorMethod::euclidean, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void DivisorCalculation::steinMethod(const std::int32_t a, const std::int32_t b)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = numeric::divisor::Divisor().stein(a, b);
    showResult(DivisorMethod::stein, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}
} // namespace divisor
//! @brief To apply divisor-related methods.
//! @param candidates - container for the candidate target methods
void applyingDivisor(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::divisor;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using divisor::InputBuilder, divisor::input::numberA, divisor::input::numberB;
    const auto inputData = std::make_shared<InputBuilder>(numberA, numberB);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::int32_t, const std::int32_t))
        {
            allocatedJob->enqueue(
                taskNamer(subTask), targetMethod, inputData->getNumbers().first, inputData->getNumbers().second);
        });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using divisor::DivisorCalculation;
            static_assert(utility::common::isStatelessClass<DivisorCalculation>());
            case abbrValue(DivisorMethod::euclidean):
                addTask(target, &DivisorCalculation::euclideanMethod);
                break;
            case abbrValue(DivisorMethod::stein):
                addTask(target, &DivisorCalculation::steinMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace integral
{
//! @brief Show the contents of the integral result.
//! @param method - specific value of IntegralMethod enum
//! @param result - integral result
//! @param interval - time interval
static void showResult(const IntegralMethod method, const double result, const double interval)
{
    std::printf(
        "\n==> %-11s Method <==\nI(def)=%+.5f, run time: %8.5f ms\n", makeTitle(method).c_str(), result, interval);
}

void IntegralCalculation::trapezoidalMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Stopwatch timing{};
    const auto sum = numeric::integral::Trapezoidal(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::trapezoidal, sum, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void IntegralCalculation::adaptiveSimpsonMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Stopwatch timing{};
    const auto sum = numeric::integral::Simpson(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::simpson, sum, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void IntegralCalculation::rombergMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Stopwatch timing{};
    const auto sum = numeric::integral::Romberg(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::romberg, sum, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void IntegralCalculation::gaussLegendreMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Stopwatch timing{};
    const auto sum = numeric::integral::Gauss(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::gauss, sum, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void IntegralCalculation::monteCarloMethod(const Expression& expr, const double lower, const double upper)
try
{
    const utility::time::Stopwatch timing{};
    const auto sum = numeric::integral::MonteCarlo(expr)(lower, upper, numeric::integral::epsilon);
    showResult(IntegralMethod::monteCarlo, sum, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}
} // namespace integral
//! @brief To apply integral-related methods.
//! @param candidates - container for the candidate target methods
void applyingIntegral(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::integral;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using integral::InputBuilder, integral::input::CylindricalBessel, integral::Expression;
    static_assert(numeric::integral::epsilon >= std::numeric_limits<double>::epsilon());
    const auto inputData = std::make_shared<InputBuilder>(
        CylindricalBessel{}, CylindricalBessel::range1, CylindricalBessel::range2, CylindricalBessel::exprDescr);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const Expression&, const double, const double))
        {
            allocatedJob->enqueue(
                taskNamer(subTask),
                targetMethod,
                inputData->getExpression(),
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
            using integral::IntegralCalculation;
            static_assert(utility::common::isStatelessClass<IntegralCalculation>());
            case abbrValue(IntegralMethod::trapezoidal):
                addTask(target, &IntegralCalculation::trapezoidalMethod);
                break;
            case abbrValue(IntegralMethod::simpson):
                addTask(target, &IntegralCalculation::adaptiveSimpsonMethod);
                break;
            case abbrValue(IntegralMethod::romberg):
                addTask(target, &IntegralCalculation::rombergMethod);
                break;
            case abbrValue(IntegralMethod::gauss):
                addTask(target, &IntegralCalculation::gaussLegendreMethod);
                break;
            case abbrValue(IntegralMethod::monteCarlo):
                addTask(target, &IntegralCalculation::monteCarloMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(category);
}

namespace prime
{
//! @brief Show the contents of the prime result.
//! @param method - specific value of PrimeMethod enum
//! @param result - prime result
//! @param interval - time interval
static void showResult(const PrimeMethod method, const std::vector<std::uint32_t>& result, const double interval)
{
    const std::uint32_t bufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> fmtBuffer(bufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        makeTitle(method).c_str(),
        InputBuilder::template spliceAllIntegers<std::uint32_t>(result, fmtBuffer.data(), bufferSize + 1),
        interval);
}

void PrimeCalculation::eratosthenesMethod(const std::uint32_t max)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = numeric::prime::Prime().eratosthenes(max);
    showResult(PrimeMethod::eratosthenes, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}

void PrimeCalculation::eulerMethod(const std::uint32_t max)
try
{
    const utility::time::Stopwatch timing{};
    const auto coll = numeric::prime::Prime().euler(max);
    showResult(PrimeMethod::euler, coll, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in calculation (%s): %s", __func__, err.what());
}
} // namespace prime
//! @brief To apply prime-related methods.
//! @param candidates - container for the candidate target methods
void applyingPrime(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::prime;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }
    assert(bits.size() == candidates.size());

    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(category);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using prime::InputBuilder, prime::input::upperBound;
    const auto inputData = std::make_shared<InputBuilder>(upperBound);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask = utility::common::wrapClosure(
        [allocatedJob, &inputData, &taskNamer](
            const std::string_view subTask, void (*targetMethod)(const std::uint32_t))
        { allocatedJob->enqueue(taskNamer(subTask), targetMethod, inputData->getUpperBound()); });
    MACRO_DEFER([&]() { pooling.deleteEntry(allocatedJob); });

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& target = candidates.at(index);
        switch (utility::common::bkdrHash(target.c_str()))
        {
            using prime::PrimeCalculation;
            static_assert(utility::common::isStatelessClass<PrimeCalculation>());
            case abbrValue(PrimeMethod::eratosthenes):
                addTask(target, &PrimeCalculation::eratosthenesMethod);
                break;
            case abbrValue(PrimeMethod::euler):
                addTask(target, &PrimeCalculation::eulerMethod);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString<category>()} + " method: " + target + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(category);
}
} // namespace application::app_num

#undef APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_NUM_PRINT_TASK_TITLE_SCOPE_END
