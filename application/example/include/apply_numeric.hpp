//! @file apply_numeric.hpp
//! @author ryftchen
//! @brief The declarations (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <bitset>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <variant>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "numeric/include/arithmetic.hpp"
#include "numeric/include/divisor.hpp"
#include "numeric/include/integral.hpp"
#include "numeric/include/prime.hpp"

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Numeric-applying-related functions in the application module.
namespace app_num
{
//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <class T>
struct Bottom;

//! @brief Enumerate specific arithmetic methods.
enum ArithmeticMethod : std::uint8_t
{
    //! @brief Addition.
    addition,
    //! @brief Subtraction.
    subtraction,
    //! @brief Multiplication.
    multiplication,
    //! @brief Division.
    division
};
//! @brief Store the maximum value of the ArithmeticMethod enum.
template <>
struct Bottom<ArithmeticMethod>
{
    //! @brief Maximum value of the ArithmeticMethod enum.
    static constexpr std::uint8_t value{4};
};

//! @brief Enumerate specific divisor methods.
enum DivisorMethod : std::uint8_t
{
    //! @brief Euclidean.
    euclidean,
    //! @brief Stein.
    stein
};
//! @brief Store the maximum value of the DivisorMethod enum.
template <>
struct Bottom<DivisorMethod>
{
    //! @brief Maximum value of the DivisorMethod enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Enumerate specific integral methods.
enum IntegralMethod : std::uint8_t
{
    //! @brief Trapezoidal.
    trapezoidal,
    //! @brief Simpson.
    simpson,
    //! @brief Romberg.
    romberg,
    //! @brief Gauss.
    gauss,
    //! @brief Monte-Carlo.
    monteCarlo
};
//! @brief Store the maximum value of the IntegralMethod enum.
template <>
struct Bottom<IntegralMethod>
{
    //! @brief Maximum value of the IntegralMethod enum.
    static constexpr std::uint8_t value{5};
};

//! @brief Enumerate specific prime methods.
enum PrimeMethod : std::uint8_t
{
    //! @brief Eratosthenes.
    eratosthenes,
    //! @brief Euler.
    euler
};
//! @brief Store the maximum value of the PrimeMethod enum.
template <>
struct Bottom<PrimeMethod>
{
    //! @brief Maximum value of the PrimeMethod enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Manage numeric choices.
class ApplyNumeric
{
public:
    //! @brief Enumerate specific numeric choices.
    enum Category : std::uint8_t
    {
        //! @brief Arithmetic.
        arithmetic,
        //! @brief Divisor.
        divisor,
        //! @brief Integral.
        integral,
        //! @brief Prime.
        prime
    };

    //! @brief Bit flags for managing arithmetic methods.
    std::bitset<Bottom<ArithmeticMethod>::value> arithmeticOpts{};
    //! @brief Bit flags for managing divisor methods.
    std::bitset<Bottom<DivisorMethod>::value> divisorOpts{};
    //! @brief Bit flags for managing integral methods.
    std::bitset<Bottom<IntegralMethod>::value> integralOpts{};
    //! @brief Bit flags for managing prime methods.
    std::bitset<Bottom<PrimeMethod>::value> primeOpts{};

    //! @brief Check whether any numeric choices do not exist.
    //! @return any numeric choices do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return arithmeticOpts.none() && divisorOpts.none() && integralOpts.none() && primeOpts.none();
    }
    //! @brief Reset bit flags that manage numeric choices.
    inline void reset()
    {
        arithmeticOpts.reset();
        divisorOpts.reset();
        integralOpts.reset();
        primeOpts.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - the specific value of Category enum
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::arithmetic:
                os << "ARITHMETIC";
                break;
            case Category::divisor:
                os << "DIVISOR";
                break;
            case Category::integral:
                os << "INTEGRAL";
                break;
            case Category::prime:
                os << "PRIME";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Category>>(cat);
        }
        return os;
    }
};
extern ApplyNumeric& manager();

//! @brief Update choice.
//! @tparam T - type of target method
//! @param target - target method
template <class T>
void updateChoice(const std::string& target);
//! @brief Run choices.
//! @tparam T - type of target method
//! @param candidates - container for the candidate target methods
template <class T>
void runChoices(const std::vector<std::string>& candidates);

//! @brief Apply arithmetic.
namespace arithmetic
{
//! @brief The version used to apply.
const char* const version = numeric::arithmetic::version();
//! @brief Set input parameters.
namespace input
{
//! @brief One of integers for arithmetic methods.
constexpr std::int32_t integerA = 1073741823;
//! @brief One of integers for arithmetic methods.
constexpr std::int32_t integerB = -2;
} // namespace input

//! @brief Solution of arithmetic.
class ArithmeticSolution
{
public:
    //! @brief Destroy the ArithmeticSolution object.
    virtual ~ArithmeticSolution() = default;

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

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param integer1 - first integer for elementary arithmetic
    //! @param integer2 - second integer for elementary arithmetic
    InputBuilder(const std::int32_t integer1, const std::int32_t integer2) : integer1(integer1), integer2(integer2)
    {
#ifdef __RUNTIME_PRINTING
        std::cout << "\nElementary arithmetic of " << integer1 << " and " << integer2 << ':' << std::endl;
#endif
    }
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() = default;

    //! @brief Get the pair of integers.
    //! @return pair of integers
    [[nodiscard]] inline std::pair<std::int32_t, std::int32_t> getIntegers() const
    {
        return std::make_pair(integer1, integer2);
    }

private:
    //! @brief First integer for elementary arithmetic.
    const std::int32_t integer1{};
    //! @brief Second integer for elementary arithmetic.
    const std::int32_t integer2{};
};
} // namespace arithmetic
template <>
void updateChoice<ArithmeticMethod>(const std::string& target);
template <>
void runChoices<ArithmeticMethod>(const std::vector<std::string>& candidates);

//! @brief Apply divisor.
namespace divisor
{
//! @brief The version used to apply.
const char* const version = numeric::divisor::version();
//! @brief Set input parameters.
namespace input
{
//! @brief One of integers for divisor methods.
constexpr std::int32_t integerA = 2 * 2 * 3 * 3 * 5 * 5 * 7 * 7;
//! @brief One of integers for divisor methods.
constexpr std::int32_t integerB = 2 * 3 * 5 * 7 * 11 * 13 * 17;
} // namespace input

//! @brief Solution of divisor.
class DivisorSolution
{
public:
    //! @brief Destroy the DivisorSolution object.
    virtual ~DivisorSolution() = default;

    //! @brief The Euclidean method.
    //! @param a - first integer
    //! @param b - second integer
    static void euclideanMethod(std::int32_t a, std::int32_t b);
    //! @brief The Stein method.
    //! @param a - first integer
    //! @param b - second integer
    static void steinMethod(std::int32_t a, std::int32_t b);
};

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 10;

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param integer1 - first integer
    //! @param integer2 - second integer
    InputBuilder(const std::int32_t integer1, const std::int32_t integer2) : integer1(integer1), integer2(integer2)
    {
#ifdef __RUNTIME_PRINTING
        std::cout << "\nAll common divisors of " << integer1 << " and " << integer2 << ':' << std::endl;
#endif
    }
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() = default;

    //! @brief Get the pair of integers.
    //! @return pair of integers
    [[nodiscard]] inline std::pair<std::int32_t, std::int32_t> getIntegers() const
    {
        return std::make_pair(integer1, integer2);
    }
    //! @brief Splice all integers for printing.
    //! @tparam T - type of container
    //! @param container - integer container
    //! @param buffer - buffer for printing
    //! @param bufferSize - size of buffer
    //! @return buffer after splicing
    template <typename T>
    requires std::is_integral<T>::value
    static char* spliceAllIntegers(const std::vector<T>& container, char* const buffer, const std::uint32_t bufferSize)
    {
        std::uint32_t align = 0;
        for (std::uint32_t i = 0; i < container.size(); ++i)
        {
            align = std::max(static_cast<std::uint32_t>(std::to_string(container.at(i)).length()), align);
        }

        int formatSize = 0;
        std::uint32_t completeSize = 0;
        for (std::uint32_t i = 0; i < container.size(); ++i)
        {
            formatSize =
                std::snprintf(buffer + completeSize, bufferSize - completeSize, "%*d ", align + 1, container.at(i));
            if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
            {
                break;
            }
            completeSize += formatSize;

            if ((0 == ((i + 1) % maxColumnOfPrint)) && ((i + 1) != container.size()))
            {
                formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
                if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
                {
                    break;
                }
                completeSize += formatSize;
            }
        }

        return buffer;
    }

private:
    //! @brief First integer.
    const std::int32_t integer1{};
    //! @brief Second integer.
    const std::int32_t integer2{};
};
} // namespace divisor
template <>
void updateChoice<DivisorMethod>(const std::string& target);
template <>
void runChoices<DivisorMethod>(const std::vector<std::string>& candidates);

//! @brief Apply integral.
namespace integral
{
//! @brief The version used to apply.
const char* const version = numeric::integral::version();
//! @brief Set input parameters.
namespace input
{
//! @brief Expression example 1.
class Expression1 : public numeric::integral::Expression
{
public:
    //! @brief The operator (()) overloading of Expression1 class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override { return (x * std::sin(x)) / (1.0 + std::cos(x) * std::cos(x)); }

    //! @brief Left endpoint.
    static constexpr double range1{-std::numbers::pi / 2.0};
    //! @brief Right endpoint.
    static constexpr double range2{2.0 * std::numbers::pi};
    //! @brief Expression example 1.
    static constexpr std::string_view exprDescr{"I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"};
};

//! @brief Expression example 2.
class Expression2 : public numeric::integral::Expression
{
public:
    //! @brief The operator (()) overloading of Expression2 class.
    //! @param x - independent variable
    //! @return dependent variable
    double operator()(const double x) const override { return x + 10.0 * std::sin(5.0 * x) + 7.0 * std::cos(4.0 * x); }

    //! @brief Left endpoint.
    static constexpr double range1{0.0};
    //! @brief Right endpoint.
    static constexpr double range2{9.0};
    //! @brief Expression example 2.
    static constexpr std::string_view exprDescr{"I=∫(0→9)x+10sin(5x)+7cos(4x)dx"};
};
} // namespace input

//! @brief Alias for the target expression.
using Expression = numeric::integral::Expression;

//! @brief Solution of integral.
class IntegralSolution
{
public:
    //! @brief Destroy the IntegralSolution object.
    virtual ~IntegralSolution() = default;

    //! @brief The trapezoidal method.
    //! @param expr - target expression
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    static void trapezoidalMethod(const Expression& expr, double lower, double upper);
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

//! @brief Expression object's helper type for the visitor.
//! @tparam Ts - type of visitors
template <class... Ts>
struct ExprOverloaded : Ts...
{
    using Ts::operator()...;
};

//! @brief Explicit deduction guide for ExprOverloaded.
//! @tparam Ts - type of visitors
template <class... Ts>
ExprOverloaded(Ts...) -> ExprOverloaded<Ts...>;

//! @brief Range properties of the expression.
//! @tparam T1 - type of lower endpoint
//! @tparam T2 - type of upper endpoint
template <typename T1, typename T2>
struct ExprRange
{
    //! @brief Construct a new ExprRange object.
    //! @param range1 - lower endpoint
    //! @param range2 - upper endpoint
    //! @param exprDescr - expression description
    ExprRange(const T1 range1, const T2 range2, const std::string_view exprDescr) :
        range1(range1), range2(range2), exprDescr(exprDescr)
    {
    }
    //! @brief Construct a new ExprRange object.
    ExprRange() = delete;
    //! @brief Lower endpoint.
    const T1 range1{};
    //! @brief Upper endpoint.
    const T2 range2{};
    //! @brief Expression description.
    const std::string_view exprDescr{};
    //! @brief The operator (==) overloading of ExprRange struct.
    //! @param rhs - right-hand side
    //! @return be equal or not equal
    bool operator==(const ExprRange& rhs) const
    {
        return std::tie(rhs.range1, rhs.range2, rhs.exprDescr) == std::tie(range1, range2, exprDescr);
    }
};
//! @brief Mapping hash value for the expression.
struct ExprMapHash
{
    //! @brief The operator (()) overloading of ExprMapHash class.
    //! @tparam T1 - type of lower endpoint
    //! @tparam T2 - type of upper endpoint
    //! @param range - range properties of the expression
    //! @return hash value
    template <typename T1, typename T2>
    std::size_t operator()(const ExprRange<T1, T2>& range) const
    {
        const std::size_t hash1 = std::hash<T1>()(range.range1), hash2 = std::hash<T2>()(range.range2),
                          hash3 = std::hash<std::string_view>()(range.exprDescr);
        constexpr std::size_t magicNumber = 0x9e3779b9, leftShift = 6, rightShift = 2;
        std::size_t seed = 0;
        seed ^= hash1 + magicNumber + (seed << leftShift) + (seed >> rightShift);
        seed ^= hash2 + magicNumber + (seed << leftShift) + (seed >> rightShift);
        seed ^= hash3 + magicNumber + (seed << leftShift) + (seed >> rightShift);
        return seed;
    }
};

//! @brief Alias for the integral expression.
//! @tparam Ts - type of expressions
template <class... Ts>
using IntegralExpr = std::variant<Ts...>;
//! @brief Alias for the integral expression map.
//! @tparam Ts - type of expressions
template <class... Ts>
using IntegralExprMap = std::unordered_multimap<ExprRange<double, double>, IntegralExpr<Ts...>, ExprMapHash>;

//! @brief Builder for the input.
//! @tparam Ts - type of expressions
template <class... Ts>
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param expressionMap - collection of integral expressions
    explicit InputBuilder(const IntegralExprMap<Ts...>& expressionMap) : expressionMap(std::move(expressionMap)) {}
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() = default;

    //! @brief Get the collection of integral expressions.
    //! @return collection of integral expressions
    inline const IntegralExprMap<Ts...>& getExpressionMap() const { return expressionMap; };
    //! @brief Print expression.
    //! @param expression - target expression
    static void printExpression(const IntegralExpr<Ts...>& expression)
    {
        constexpr std::string_view prefix = "\nIntegral expression:\n";
        std::visit(
            ExprOverloaded{
                [&prefix](const input::Expression1& /*expr*/)
                {
                    std::cout << prefix << input::Expression1::exprDescr << std::endl;
                },
                [&prefix](const input::Expression2& /*expr*/)
                {
                    std::cout << prefix << input::Expression2::exprDescr << std::endl;
                },
                [](const auto& expr)
                {
                    throw std::runtime_error("Unknown expression type: " + std::string{typeid(expr).name()} + '.');
                }},
            expression);
    };

private:
    //! @brief Collection of integral expressions.
    const IntegralExprMap<Ts...> expressionMap{};
};
} // namespace integral
template <>
void updateChoice<IntegralMethod>(const std::string& target);
template <>
void runChoices<IntegralMethod>(const std::vector<std::string>& candidates);

//! @brief Apply prime.
namespace prime
{
//! @brief The version used to apply.
const char* const version = numeric::prime::version();
//! @brief Set input parameters.
namespace input
{
//! @brief Maximum positive integer for prime methods.
constexpr std::uint32_t maxPositiveInteger = 997;
} // namespace input

//! @brief Solution of prime.
class PrimeSolution
{
public:
    //! @brief Destroy the PrimeSolution object.
    virtual ~PrimeSolution() = default;

    //! @brief The Eratosthenes method.
    //! @param max - maximum positive integer
    static void eratosthenesMethod(const std::uint32_t max);
    //! @brief The Euler method.
    //! @param max - maximum positive integer
    static void eulerMethod(const std::uint32_t max);
};

//! @brief Maximum alignment length per element of printing.
constexpr std::uint8_t maxAlignOfPrint = 16;
//! @brief Maximum columns per row of printing.
constexpr std::uint8_t maxColumnOfPrint = 10;

//! @brief Builder for the input.
class InputBuilder
{
public:
    //! @brief Construct a new InputBuilder object.
    //! @param maxPositiveInteger - maximum positive integer
    explicit InputBuilder(const std::uint32_t maxPositiveInteger) : maxPositiveInteger(maxPositiveInteger)
    {
#ifdef __RUNTIME_PRINTING
        std::cout << "\nAll prime numbers smaller than " << maxPositiveInteger << ':' << std::endl;
#endif
    }
    //! @brief Destroy the InputBuilder object.
    virtual ~InputBuilder() = default;

    //! @brief Get the Maximum positive integer.
    //! @return maximum positive integer
    [[nodiscard]] inline std::uint32_t getMaxPositiveInteger() const { return maxPositiveInteger; }
    //! @brief Splice all integers for printing.
    //! @tparam T - type of container
    //! @param container - integer container
    //! @param buffer - buffer for printing
    //! @param bufferSize - size of buffer
    //! @return buffer after splicing
    template <typename T>
    requires std::is_integral<T>::value
    static char* spliceAllIntegers(const std::vector<T>& container, char* const buffer, const std::uint32_t bufferSize)
    {
        std::uint32_t align = 0;
        for (std::uint32_t i = 0; i < container.size(); ++i)
        {
            align = std::max(static_cast<std::uint32_t>(std::to_string(container.at(i)).length()), align);
        }

        int formatSize = 0;
        std::uint32_t completeSize = 0;
        for (std::uint32_t i = 0; i < container.size(); ++i)
        {
            formatSize =
                std::snprintf(buffer + completeSize, bufferSize - completeSize, "%*d ", align + 1, container.at(i));
            if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
            {
                break;
            }
            completeSize += formatSize;

            if ((0 == ((i + 1) % maxColumnOfPrint)) && ((i + 1) != container.size()))
            {
                formatSize = std::snprintf(buffer + completeSize, bufferSize - completeSize, "\n");
                if ((formatSize < 0) || (formatSize >= static_cast<int>(bufferSize - completeSize)))
                {
                    break;
                }
                completeSize += formatSize;
            }
        }

        return buffer;
    }

private:
    //! @brief Maximum positive integer.
    const std::uint32_t maxPositiveInteger{};
};
} // namespace prime
template <>
void updateChoice<PrimeMethod>(const std::string& target);
template <>
void runChoices<PrimeMethod>(const std::vector<std::string>& candidates);
} // namespace app_num
} // namespace application
