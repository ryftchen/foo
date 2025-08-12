//! @file apply_numeric.hpp
//! @author ryftchen
//! @brief The declarations (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <gsl/gsl_sf.h>
#include <iostream>
#include <numeric>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/prime.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Numeric-applying-related functions in the application module.
namespace app_num
{
//! @brief Apply arithmetic.
namespace arithmetic
{
//! @brief The version used to apply.
const char* const version = numeric::arithmetic::version();

//! @brief Set input parameters.
namespace input
{
//! @brief One of operands for arithmetic methods.
constexpr std::int32_t operandA = 46340;
//! @brief One of operands for arithmetic methods.
constexpr std::int32_t operandB = -46340;
} // namespace input

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param operand1 - first operand for elementary arithmetic
    //! @param operand2 - second operand for elementary arithmetic
    InputBuilder(const std::int32_t operand1, const std::int32_t operand2) : operand1{operand1}, operand2{operand2}
    {
#ifdef _RUNTIME_PRINTING
        std::cout << "\nElementary arithmetic of " << operand1 << " and " << operand2 << ':' << std::endl;
#endif // _RUNTIME_PRINTING
    }

    //! @brief Get the pair of operands.
    //! @return pair of operands
    [[nodiscard]] std::pair<std::int32_t, std::int32_t> getOperands() const noexcept
    {
        return std::make_pair(operand1, operand2);
    }

private:
    //! @brief First operand for elementary arithmetic.
    const std::int32_t operand1 : 17 {0};
    //! @brief Second operand for elementary arithmetic.
    const std::int32_t operand2 : 17 {0};
};

//! @brief Calculation of arithmetic.
class ArithmeticCalculation
{
public:
    //! @brief The addition method.
    //! @param augend - augend of addition
    //! @param addend - addend of addition
    static void additionMethod(const std::int32_t augend, const std::int32_t addend);
    //! @brief The subtraction method.
    //! @param minuend - minuend of subtraction
    //! @param subtrahend - subtrahend of subtraction
    static void subtractionMethod(const std::int32_t minuend, const std::int32_t subtrahend);
    //! @brief The multiplication method.
    //! @param multiplier - multiplier of multiplication
    //! @param multiplicand - multiplicand of multiplication
    static void multiplicationMethod(const std::int32_t multiplier, const std::int32_t multiplicand);
    //! @brief The division method.
    //! @param dividend - dividend of division
    //! @param divisor - divisor of division
    static void divisionMethod(const std::int32_t dividend, const std::int32_t divisor);
};
} // namespace arithmetic
extern void applyingArithmetic(const std::vector<std::string>& candidates);

//! @brief Apply divisor.
namespace divisor
{
//! @brief The version used to apply.
const char* const version = numeric::divisor::version();

//! @brief Set input parameters.
namespace input
{
//! @brief One of numbers for divisor methods.
constexpr std::int32_t numberA = 2 * 2 * 3 * 3 * 5 * 5 * 7 * 7;
//! @brief One of numbers for divisor methods.
constexpr std::int32_t numberB = 2 * 3 * 5 * 7 * 11 * 13 * 17;
} // namespace input

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 5;

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param number1 - first number
    //! @param number2 - second number
    InputBuilder(const std::int32_t number1, const std::int32_t number2) : number1{number1}, number2{number2}
    {
#ifdef _RUNTIME_PRINTING
        std::cout << "\nAll common divisors of " << number1 << " and " << number2 << ':' << std::endl;
#endif // _RUNTIME_PRINTING
    }

    //! @brief Get the pair of numbers.
    //! @return pair of numbers
    [[nodiscard]] std::pair<std::int32_t, std::int32_t> getNumbers() const noexcept
    {
        return std::make_pair(number1, number2);
    }
    //! @brief Splice all integers for printing.
    //! @tparam T - type of container
    //! @param container - integer container
    //! @param fmtBuffer - buffer for printing
    //! @param bufferSize - size of the buffer
    //! @return buffer after splicing
    template <typename T>
    requires std::is_integral_v<T>
    static char* spliceAllIntegers(const std::set<T>& container, char* const fmtBuffer, const std::uint32_t bufferSize)
    {
        const std::uint32_t align = std::reduce(
            container.cbegin(),
            container.cend(),
            0,
            [](const auto max, const auto elem)
            { return std::max<std::uint32_t>(std::to_string(elem).length(), max); });
        std::uint32_t offset = 0;
        for (auto iterator = container.cbegin(); iterator != container.cend(); ++iterator)
        {
            const std::uint32_t nextIdx = std::distance(container.cbegin(), iterator) + 1;
            const char sep = ((nextIdx % maxColumnOfPrint == 0) && (nextIdx != container.size())) ? '\n' : ' ';
            const int written =
                std::snprintf(fmtBuffer + offset, bufferSize - offset, "%*d%c", align + 1, *iterator, sep);
            if ((written < 0) || (written >= static_cast<int>(bufferSize - offset)))
            {
                break;
            }
            offset += written;
        }

        return fmtBuffer;
    }

private:
    //! @brief First number.
    const std::int32_t number1{0};
    //! @brief Second number.
    const std::int32_t number2{0};
};

//! @brief Calculation of divisor.
class DivisorCalculation
{
public:
    //! @brief The Euclidean method.
    //! @param a - first integer
    //! @param b - second integer
    static void euclideanMethod(const std::int32_t a, const std::int32_t b);
    //! @brief The Stein method.
    //! @param a - first integer
    //! @param b - second integer
    static void steinMethod(const std::int32_t a, const std::int32_t b);
};
} // namespace divisor
extern void applyingDivisor(const std::vector<std::string>& candidates);

//! @brief Apply integral.
namespace integral
{
//! @brief The version used to apply.
const char* const version = numeric::integral::version();

//! @brief Alias for the target expression.
using Expression = std::function<double(const double)>;
//! @brief Wrapper for the target expression.
class ExprBase
{
public:
    //! @brief Construct a new ExprBase object.
    ExprBase() = default;
    //! @brief Destroy the ExprBase object.
    virtual ~ExprBase() = default;
    //! @brief Construct a new ExprBase object.
    ExprBase(const ExprBase&) = default;
    //! @brief Construct a new ExprBase object.
    ExprBase(ExprBase&&) noexcept = default;
    //! @brief The operator (=) overloading of ExprBase class.
    //! @return reference of the ExprBase object
    ExprBase& operator=(const ExprBase&) = default;
    //! @brief The operator (=) overloading of ExprBase class.
    //! @return reference of the ExprBase object
    ExprBase& operator=(ExprBase&&) noexcept = default;

    //! @brief The operator (()) overloading of ExprBase class.
    //! @param x - independent variable
    //! @return dependent variable
    virtual double operator()(const double x) const = 0;
    //! @brief The operator (Expression) overloading of ExprBase class.
    //! @return Expression object
    virtual explicit operator Expression() const
    {
        return [this](const double x) { return operator()(x); };
    }
};

//! @brief Set input parameters.
namespace input
{
//! @brief Cylindrical Bessel.
class CylindricalBessel : public ExprBase
{
public:
    //! @brief The operator (()) overloading of CylindricalBessel class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override { return ::gsl_sf_bessel_J0(x); }

    //! @brief Left endpoint.
    static constexpr double range1{0.0};
    //! @brief Right endpoint.
    static constexpr double range2{20.0};
    //! @brief Cylindrical Bessel function of the first kind.
    static constexpr std::string_view exprDescr{"f(x)=J₀(x),x∈[0,20] (Cylindrical Bessel function of the first kind)"};
};
} // namespace input

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new Input Builder object.
    //! @param expression - target expression
    //! @param range1 - lower endpoint
    //! @param range2 - upper endpoint
    //! @param exprDescr - expression description
    InputBuilder(
        Expression expression,
        const double range1,
        const double range2,
        [[maybe_unused]] const std::string_view exprDescr) :
        expression{std::move(expression)}, range1{range1}, range2{range2}
    {
#ifdef _RUNTIME_PRINTING
        std::cout << "\nIntegral expression:\n" << exprDescr << std::endl;
#endif // _RUNTIME_PRINTING
    }

    //! @brief Get the target expression.
    //! @return target expression
    [[nodiscard]] Expression getExpression() const noexcept { return expression; }
    //! @brief Get the pair of ranges.
    //! @return pair of ranges
    [[nodiscard]] std::pair<double, double> getRanges() const noexcept { return std::make_pair(range1, range2); }

private:
    //! @brief Target expression.
    const Expression expression;
    //! @brief Lower endpoint.
    const double range1{0.0};
    //! @brief Upper endpoint.
    const double range2{0.0};
};

//! @brief Calculation of integral.
class IntegralCalculation
{
public:
    //! @brief The trapezoidal method.
    //! @param expr - target expression
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    static void trapezoidalMethod(const Expression& expr, const double lower, const double upper);
    //! @brief The adaptive Simpson's 1/3 method.
    //! @param expr - target expression
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    static void adaptiveSimpsonMethod(const Expression& expr, const double lower, const double upper);
    //! @brief The Romberg method.
    //! @param expr - target expression
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    static void rombergMethod(const Expression& expr, const double lower, const double upper);
    //! @brief The Gauss-Legendre's 5-points method.
    //! @param expr - target expression
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    static void gaussLegendreMethod(const Expression& expr, const double lower, const double upper);
    //! @brief The Monte-Carlo method.
    //! @param expr - target expression
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    static void monteCarloMethod(const Expression& expr, const double lower, const double upper);
};
} // namespace integral
extern void applyingIntegral(const std::vector<std::string>& candidates);

//! @brief Apply prime.
namespace prime
{
//! @brief The version used to apply.
const char* const version = numeric::prime::version();

//! @brief Set input parameters.
namespace input
{
//! @brief Upper bound for prime methods.
constexpr std::uint32_t upperBound = 997;
} // namespace input

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 10;

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param upperBound - upper bound
    explicit InputBuilder(const std::uint32_t upperBound) : upperBound{upperBound}
    {
#ifdef _RUNTIME_PRINTING
        std::cout << "\nAll prime numbers smaller than " << upperBound << ':' << std::endl;
#endif // _RUNTIME_PRINTING
    }

    //! @brief Get the upper bound.
    //! @return upper bound
    [[nodiscard]] std::uint32_t getUpperBound() const noexcept { return upperBound; }
    //! @brief Splice all integers for printing.
    //! @tparam T - type of container
    //! @param container - integer container
    //! @param fmtBuffer - buffer for printing
    //! @param bufferSize - size of the buffer
    //! @return buffer after splicing
    template <typename T>
    requires std::is_integral_v<T>
    static char* spliceAllIntegers(
        const std::vector<T>& container, char* const fmtBuffer, const std::uint32_t bufferSize)
    {
        const std::uint32_t align = std::reduce(
            container.cbegin(),
            container.cend(),
            0,
            [](const auto max, const auto elem)
            { return std::max<std::uint32_t>(std::to_string(elem).length(), max); });
        for (std::uint32_t i = 0, offset = 0; i < container.size(); ++i)
        {
            const char sep = (((i + 1) % maxColumnOfPrint == 0) && ((i + 1) != container.size())) ? '\n' : ' ';
            const int written =
                std::snprintf(fmtBuffer + offset, bufferSize - offset, "%*d%c", align + 1, container.at(i), sep);
            if ((written < 0) || (written >= static_cast<int>(bufferSize - offset)))
            {
                break;
            }
            offset += written;
        }

        return fmtBuffer;
    }

private:
    //! @brief Upper bound.
    const std::uint32_t upperBound{0};
};

//! @brief Calculation of prime.
class PrimeCalculation
{
public:
    //! @brief The Eratosthenes method.
    //! @param max - maximum positive integer
    static void eratosthenesMethod(const std::uint32_t max);
    //! @brief The Euler method.
    //! @param max - maximum positive integer
    static void eulerMethod(const std::uint32_t max);
};
} // namespace prime
extern void applyingPrime(const std::vector<std::string>& candidates);
} // namespace app_num
} // namespace application
