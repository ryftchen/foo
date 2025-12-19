//! @file integral.hpp
//! @author ryftchen
//! @brief The declarations (integral) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <functional>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Integral-related functions in the numeric module.
namespace integral
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "NUM_INTEGRAL";
}
extern const char* version() noexcept;

//! @brief The precision of calculation.
inline constexpr double epsilon = 1e-5;
//! @brief Alias for the target expression.
using Expression = std::function<double(const double)>;
//! @brief Alias for the result of integral.
using Result = double;

//! @brief Integral methods.
class Integral
{
public:
    //! @brief Destroy the Integral object.
    virtual ~Integral() = default;

    //! @brief The operator (()) overloading of Integral class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    virtual Result operator()(const double lower, const double upper, const double eps) const = 0;

protected:
    //! @brief Construct a new Integral object.
    //! @param expr - target expression
    explicit Integral(Expression expr) : expr{std::move(expr)} {}

    //! @brief Target expression.
    const Expression expr;
    //! @brief Calculate the value of the definite integral with the trapezoidal rule.
    //! @param expr - target expression
    //! @param left - left endpoint
    //! @param height - height of trapezoidal
    //! @param step - number of steps
    //! @return result of definite integral
    static double trapezoidalRule(
        const Expression& expr, const double left, const double height, const std::uint32_t step);
};

//! @brief The trapezoidal method.
class Trapezoidal : public Integral
{
public:
    //! @brief Construct a new Trapezoidal object.
    //! @param expr - target expression
    explicit Trapezoidal(Expression expr) : Integral(std::move(expr)) {}

    //! @brief The operator (()) overloading of Trapezoidal class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    Result operator()(const double lower, const double upper, const double eps) const override;
};

//! @brief The adaptive Simpson's 1/3 method.
class Simpson : public Integral
{
public:
    //! @brief Construct a new Simpson object.
    //! @param expr - target expression
    explicit Simpson(Expression expr) : Integral(std::move(expr)) {}

    //! @brief The operator (()) overloading of Simpson class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    Result operator()(const double lower, const double upper, const double eps) const override;

private:
    //! @brief Calculate the value of the definite integral with the Simpson's rule.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param eps - precision of calculation
    //! @return result of definite integral
    [[nodiscard]] double simpsonIntegral(const double left, const double right, const double eps) const;
    //! @brief Composite Simpson's 1/3 formula.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @param step - number of steps
    //! @return result of definite integral
    [[nodiscard]] double compositeSimpsonOneThird(
        const double left, const double right, const std::uint32_t step) const;
    //! @brief Simpson's 1/3 formula.
    //! @param left - left endpoint
    //! @param right - right endpoint
    //! @return result of definite integral
    [[nodiscard]] double simpsonOneThird(const double left, const double right) const;
};

//! @brief The Romberg method.
class Romberg : public Integral
{
public:
    //! @brief Construct a new Romberg object.
    //! @param expr - target expression
    explicit Romberg(Expression expr) : Integral(std::move(expr)) {}

    //! @brief The operator (()) overloading of Romberg class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    Result operator()(const double lower, const double upper, const double eps) const override;

private:
    //! @brief The Richardson extrapolation.
    //! @param lowPrec - numerical result obtained with low precision
    //! @param highPrec - numerical result obtained with high precision
    //! @param weight - weight factor
    //! @return extrapolation
    static double richardsonExtrapolation(const double lowPrec, const double highPrec, const double weight);
};

//! @brief The Gauss-Legendre's 5-points method.
class Gauss : public Integral
{
public:
    //! @brief Construct a new Gauss object.
    //! @param expr - target expression
    explicit Gauss(Expression expr) : Integral(std::move(expr)) {}

    //! @brief The operator (()) overloading of Gauss class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    Result operator()(const double lower, const double upper, const double eps) const override;

private:
    //! @brief Number of Gauss nodes.
    static constexpr std::uint8_t nodeSize{5};
    //! @brief Number of Gauss coefficients.
    static constexpr std::uint8_t coeffSize{2};
    //! @brief Table of Gauss-Legendre quadrature.
    static constexpr std::array<std::array<double, coeffSize>, nodeSize> gaussLegendreTbl{
        {{-0.9061798459, +0.2369268851},
         {-0.5384693101, +0.4786286705},
         {+0.0000000000, +0.5688888889},
         {+0.5384693101, +0.4786286705},
         {+0.9061798459, +0.2369268851}}};
};

//! @brief The Monte-Carlo method.
class MonteCarlo : public Integral
{
public:
    //! @brief Construct a new MonteCarlo object.
    //! @param expr - target expression
    explicit MonteCarlo(Expression expr) : Integral(std::move(expr)) {}

    //! @brief The operator (()) overloading of MonteCarlo class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    Result operator()(const double lower, const double upper, const double eps) const override;

private:
    //! @brief Sample from the uniform distribution.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of definite integral
    [[nodiscard]] double sampleFromUniformDistribution(const double lower, const double upper, const double eps) const;
    //! @brief Sample from the normal distribution (Box-Muller transform).
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of definite integral
    [[deprecated, nodiscard]] double sampleFromNormalDistribution(
        const double lower, const double upper, const double eps) const;
};
} // namespace integral
} // namespace numeric
