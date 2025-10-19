//! @file apply_numeric.cpp
//! @author ryftchen
//! @brief The definitions (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "apply_numeric.hpp"
#include "register_numeric.hpp"

#ifndef _PRECOMPILED_HEADER
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
#define APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(title)                                                                  \
    std::osyncstream(std::cout) << "\nAPPLY NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (title) << "BEGIN" << std::resetiosflags(std::ios_base::left)    \
                                << std::setfill(' ') << std::endl;                                                   \
    {
//! @brief Title of printing when numeric tasks are ending.
#define APP_NUM_PRINT_TASK_TITLE_SCOPE_END(title)                                                                    \
    }                                                                                                                \
    std::osyncstream(std::cout) << "\nAPPLY NUMERIC: " << std::setiosflags(std::ios_base::left) << std::setfill('.') \
                                << std::setw(50) << (title) << "END" << std::resetiosflags(std::ios_base::left)      \
                                << std::setfill(' ') << '\n'                                                         \
                                << std::endl;

namespace application::app_num
{
using namespace reg_num; // NOLINT(google-build-using-namespace)

//! @brief Make the title of a particular method in numeric choices.
//! @tparam T - type of target method
//! @param method - target method
//! @return initial capitalized title
template <typename T>
static std::string customTitle(const T method)
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
//! @tparam Cat - target category
//! @return alias of the category name
template <Category Cat>
static consteval std::string_view categoryAlias()
{
    constexpr auto attr =
        TypeInfo<ApplyNumeric>::fields.find(REFLECTION_STR(toString(Cat))).attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}

namespace arithmetic
{
//! @brief Show the contents of the arithmetic result.
//! @param method - used arithmetic method
//! @param result - arithmetic result
//! @param a - first integer for elementary arithmetic
//! @param b - second integer for elementary arithmetic
//! @param op - operator of arithmetic
static void display(
    const ArithmeticMethod method, const std::int32_t result, const std::int32_t a, const std::int32_t b, const char op)
{
    std::printf("\n==> %-14s Method <==\n(%d) %c (%d) = %d\n", customTitle(method).c_str(), a, op, b, result);
}

//! @brief Calculation of arithmetic.
//! @param method - used arithmetic method
//! @param a - first integer for elementary arithmetic
//! @param b - second integer for elementary arithmetic
static void calculation(const ArithmeticMethod method, const std::int32_t a, const std::int32_t b)
try
{
    std::int32_t result = 0;
    char op = ' ';
    switch (method)
    {
        using numeric::arithmetic::Arithmetic;
        case ArithmeticMethod::addition:
            result = Arithmetic().addition(a, b);
            op = '+';
            break;
        case ArithmeticMethod::subtraction:
            result = Arithmetic().subtraction(a, b);
            op = '-';
            break;
        case ArithmeticMethod::multiplication:
            result = Arithmetic().multiplication(a, b);
            op = '*';
            break;
        case ArithmeticMethod::division:
            result = Arithmetic().division(a, b);
            op = '/';
            break;
        default:
            return;
    }
    display(method, result, a, b, op);
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace arithmetic
//! @brief To apply arithmetic-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingArithmetic(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::arithmetic;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = numeric::arithmetic::description();
    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using arithmetic::InputBuilder, arithmetic::input::operandA, arithmetic::input::operandB;
    const auto inputData = std::make_shared<InputBuilder>(operandA, operandB);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const ArithmeticMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask),
            arithmetic::calculation,
            method,
            inputData->getOperands().first,
            inputData->getOperands().second);
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(ArithmeticMethod::addition):
                addTask(choice, ArithmeticMethod::addition);
                break;
            case abbrLitHash(ArithmeticMethod::subtraction):
                addTask(choice, ArithmeticMethod::subtraction);
                break;
            case abbrLitHash(ArithmeticMethod::multiplication):
                addTask(choice, ArithmeticMethod::multiplication);
                break;
            case abbrLitHash(ArithmeticMethod::division):
                addTask(choice, ArithmeticMethod::division);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace divisor
{
//! @brief Show the contents of the divisor result.
//! @param method - used divisor method
//! @param result - divisor result
//! @param interval - time interval
static void display(const DivisorMethod method, const std::set<std::int32_t>& result, const double interval)
{
    const std::uint32_t bufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> fmtBuffer(bufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        customTitle(method).c_str(),
        InputBuilder::template spliceAllIntegers<std::int32_t>(result, fmtBuffer.data(), bufferSize + 1),
        interval);
}

//! @brief Calculation of divisor.
//! @param method - used divisor method
//! @param a - first integer
//! @param b - second integer
static void calculation(const DivisorMethod method, const std::int32_t a, const std::int32_t b)
try
{
    std::set<std::int32_t> result{};
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using numeric::divisor::Divisor;
        case DivisorMethod::euclidean:
            result = Divisor().euclidean(a, b);
            break;
        case DivisorMethod::stein:
            result = Divisor().stein(a, b);
            break;
        default:
            return;
    }
    display(method, result, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace divisor
//! @brief To apply divisor-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingDivisor(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::divisor;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = numeric::divisor::description();
    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using divisor::InputBuilder, divisor::input::numberA, divisor::input::numberB;
    const auto inputData = std::make_shared<InputBuilder>(numberA, numberB);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const DivisorMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask),
            divisor::calculation,
            method,
            inputData->getNumbers().first,
            inputData->getNumbers().second);
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(DivisorMethod::euclidean):
                addTask(choice, DivisorMethod::euclidean);
                break;
            case abbrLitHash(DivisorMethod::stein):
                addTask(choice, DivisorMethod::stein);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace integral
{
//! @brief Show the contents of the integral result.
//! @param method - used integral method
//! @param result - integral result
//! @param interval - time interval
static void display(const IntegralMethod method, const double result, const double interval)
{
    std::printf(
        "\n==> %-11s Method <==\nI(def)=%+.5f, run time: %8.5f ms\n", customTitle(method).c_str(), result, interval);
}

//! @brief Calculation of integral.
//! @param method - used integral method
//! @param expr - target expression
//! @param lower - lower endpoint
//! @param upper - upper endpoint
static void calculation(const IntegralMethod method, const Expression& expr, const double lower, const double upper)
try
{
    double result = 0.0;
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using namespace numeric::integral; // NOLINT(google-build-using-namespace)
        case IntegralMethod::trapezoidal:
            result = Trapezoidal(expr)(lower, upper, epsilon);
            break;
        case IntegralMethod::simpson:
            result = Simpson(expr)(lower, upper, epsilon);
            break;
        case IntegralMethod::romberg:
            result = Romberg(expr)(lower, upper, epsilon);
            break;
        case IntegralMethod::gauss:
            result = Gauss(expr)(lower, upper, epsilon);
            break;
        case IntegralMethod::monteCarlo:
            result = MonteCarlo(expr)(lower, upper, epsilon);
            break;
        default:
            return;
    }
    display(method, result, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace integral
//! @brief To apply integral-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingIntegral(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::integral;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = numeric::integral::description();
    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using integral::InputBuilder, integral::input::CylindricalBessel, integral::Expression;
    static_assert(numeric::integral::epsilon >= std::numeric_limits<double>::epsilon());
    const auto inputData = std::make_shared<InputBuilder>(
        CylindricalBessel{}, CylindricalBessel::range1, CylindricalBessel::range2, CylindricalBessel::exprDescr);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const IntegralMethod method)
    {
        allocatedJob->enqueue(
            taskNamer(subTask),
            integral::calculation,
            method,
            inputData->getExpression(),
            inputData->getRanges().first,
            inputData->getRanges().second);
    };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(IntegralMethod::trapezoidal):
                addTask(choice, IntegralMethod::trapezoidal);
                break;
            case abbrLitHash(IntegralMethod::simpson):
                addTask(choice, IntegralMethod::simpson);
                break;
            case abbrLitHash(IntegralMethod::romberg):
                addTask(choice, IntegralMethod::romberg);
                break;
            case abbrLitHash(IntegralMethod::gauss):
                addTask(choice, IntegralMethod::gauss);
                break;
            case abbrLitHash(IntegralMethod::monteCarlo):
                addTask(choice, IntegralMethod::monteCarlo);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(title);
}

namespace prime
{
//! @brief Show the contents of the prime result.
//! @param method - used prime method
//! @param result - prime result
//! @param interval - time interval
static void display(const PrimeMethod method, const std::vector<std::uint32_t>& result, const double interval)
{
    const std::uint32_t bufferSize = result.size() * maxAlignOfPrint;
    std::vector<char> fmtBuffer(bufferSize + 1);
    std::printf(
        "\n==> %-9s Method <==\n%s\nrun time: %8.5f ms\n",
        customTitle(method).c_str(),
        InputBuilder::template spliceAllIntegers<std::uint32_t>(result, fmtBuffer.data(), bufferSize + 1),
        interval);
}

//! @brief Calculation of prime.
//! @param method - used prime method
//! @param limit - maximum positive integer
static void calculation(const PrimeMethod method, const std::uint32_t limit)
try
{
    std::vector<std::uint32_t> result{};
    const utility::time::Stopwatch timing{};
    switch (method)
    {
        using numeric::prime::Prime;
        case PrimeMethod::eratosthenes:
            result = Prime().eratosthenes(limit);
            break;
        case PrimeMethod::euler:
            result = Prime().euler(limit);
            break;
        default:
            return;
    }
    display(method, result, timing.elapsedTime());
}
catch (const std::exception& err)
{
    LOG_WRN_P("Exception in %s (%s): %s", __func__, customTitle(method).c_str(), err.what());
}
} // namespace prime
//! @brief To apply prime-related methods that are mapped to choices.
//! @param candidates - container for the candidate target choices
void applyingPrime(const std::vector<std::string>& candidates)
{
    constexpr auto category = Category::prime;
    const auto& bits = categoryOpts<category>();
    if (bits.none())
    {
        return;
    }

    const std::string_view title = numeric::prime::description();
    APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN(title);

    auto& pooling = configure::task::resourcePool();
    auto* const allocatedJob = pooling.newEntry(bits.count());
    using prime::InputBuilder, prime::input::upperBound;
    const auto inputData = std::make_shared<InputBuilder>(upperBound);
    const auto taskNamer = utility::currying::curry(curriedTaskName(), categoryAlias<category>());
    const auto addTask =
        [allocatedJob, &inputData, &taskNamer](const std::string_view subTask, const PrimeMethod method)
    { allocatedJob->enqueue(taskNamer(subTask), prime::calculation, method, inputData->getUpperBound()); };
    MACRO_DEFER(utility::common::wrapClosure([&]() { pooling.deleteEntry(allocatedJob); }));

    for (const auto index :
         std::views::iota(0U, bits.size()) | std::views::filter([&bits](const auto i) { return bits.test(i); }))
    {
        const auto& choice = candidates.at(index);
        switch (utility::common::bkdrHash(choice.c_str()))
        {
            case abbrLitHash(PrimeMethod::eratosthenes):
                addTask(choice, PrimeMethod::eratosthenes);
                break;
            case abbrLitHash(PrimeMethod::euler):
                addTask(choice, PrimeMethod::euler);
                break;
            default:
                throw std::logic_error{"Unknown " + std::string{toString(category)} + " choice: " + choice + '.'};
        }
    }

    APP_NUM_PRINT_TASK_TITLE_SCOPE_END(title);
}
} // namespace application::app_num

#undef APP_NUM_PRINT_TASK_TITLE_SCOPE_BEGIN
#undef APP_NUM_PRINT_TASK_TITLE_SCOPE_END
