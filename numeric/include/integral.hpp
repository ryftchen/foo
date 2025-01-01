//! @file integral.hpp
//! @author ryftchen
//! @brief The declarations (integral) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Integral-related functions in the numeric module.
namespace integral
{
extern const char* version() noexcept;

//! @brief Target expressions.
class Expression
{
public:
    //! @brief Destroy the Expression object.
    virtual ~Expression() = default;

    //! @brief The operator (()) overloading of Function class.
    //! @param x - independent variable
    //! @return dependent variable
    virtual inline double operator()(const double x) const = 0;
};

//! @brief The precision of calculation.
inline constexpr double epsilon = 1e-5;

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
    virtual double operator()(double lower, double upper, const double eps) const = 0;

protected:
    //! @brief Construct a new Integral object.
    Integral() = default;

    //! @brief Get the sign.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @return sign
    static std::int8_t getSign(double& lower, double& upper);
    friend double trapezoid(const Expression& expr, const double left, const double height, const std::uint32_t step);
};

//! @brief The trapezoidal method.
class Trapezoidal : public Integral
{
public:
    //! @brief Construct a new Trapezoidal object.
    //! @param expr - target expression
    explicit Trapezoidal(const Expression& expr) : expr(expr) {}

    //! @brief The operator (()) overloading of Trapezoidal class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const Expression& expr;
};

//! @brief The adaptive Simpson's 1/3 method.
class Simpson : public Integral
{
public:
    //! @brief Construct a new Simpson object.
    //! @param expr - target expression
    explicit Simpson(const Expression& expr) : expr(expr) {}

    //! @brief The operator (()) overloading of Simpson class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const Expression& expr;
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
    //! @note I≈1/3*h*{F(X0)+4*[F(X1)+...+F(X2n-1)]+2*[F(X2)+...+F(X2n-2)]+F(Xn)},h=(b-a)/n
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
    explicit Romberg(const Expression& expr) : expr(expr) {}

    //! @brief The operator (()) overloading of Romberg class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const Expression& expr;
};

//! @brief The Gauss-Legendre's 5-points method.
class Gauss : public Integral
{
public:
    //! @brief Construct a new Gauss object.
    //! @param expr - target expression
    explicit Gauss(const Expression& expr) : expr(expr) {}

    //! @brief The operator (()) overloading of Gauss class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    //! @note x=1/2*[(b-a)*t+(a+b)]
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const Expression& expr;
};

//! @brief The Monte-Carlo method.
class MonteCarlo : public Integral
{
public:
    //! @brief Construct a new MonteCarlo object.
    //! @param expr - target expression
    explicit MonteCarlo(const Expression& expr) : expr(expr) {}

    //! @brief The operator (()) overloading of MonteCarlo class.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const Expression& expr;
    //! @brief Sample from the uniform distribution.
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of definite integral
    //! @note I≈(b-a)/N*[F(X1)+F(X2)+...+F(Xn)]
    [[nodiscard]] double sampleFromUniformDistribution(const double lower, const double upper, const double eps) const;
    //! @brief Sample from the normal distribution (Box-Muller transform).
    //! @param lower - lower endpoint
    //! @param upper - upper endpoint
    //! @param eps - precision of calculation
    //! @return result of definite integral
    //! @note I≈1/N*[F(X1)/P(X1)+...+F(Xn)/P(Xn)]
    [[nodiscard]] double sampleFromNormalDistribution(const double lower, const double upper, const double eps) const;
};
} // namespace integral
} // namespace numeric
