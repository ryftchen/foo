#pragma once

#include <bits/stdint-uintn.h>
#include "expression.hpp"

namespace num_integral
{
inline constexpr double epsilon = 1e-5;

class IntegralSolution
{
public:
    virtual ~IntegralSolution() = default;

    virtual double operator()(double lower, double upper, const double eps) const = 0;

protected:
    static inline int getSign(double& lower, double& upper);
    friend double trapezoid(
        const num_expression::Expression& expr,
        const double left,
        const double height,
        const uint32_t step);
};

inline int IntegralSolution::getSign(double& lower, double& upper)
{
    return (lower < upper) ? 1 : ((lower > upper) ? (std::swap(lower, upper), -1) : 0);
}

// Trapezoidal
class Trapezoidal : public IntegralSolution
{
public:
    explicit Trapezoidal(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const num_expression::Expression& func;
};

// Adaptive Simpson's 1/3
class Simpson : public IntegralSolution
{
public:
    explicit Simpson(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const num_expression::Expression& func;
    [[nodiscard]] double simpsonIntegral(const double left, const double right, const double eps) const;
    [[nodiscard]] double compositeSimpsonOneThird(const double left, const double right, const uint32_t n) const;
    [[nodiscard]] double simpsonOneThird(const double left, const double right) const;
};

// Romberg
class Romberg : public IntegralSolution
{
public:
    explicit Romberg(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const num_expression::Expression& func;
};

// Gauss-Legendre's 5-points
class Gauss : public IntegralSolution
{
public:
    explicit Gauss(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const num_expression::Expression& func;
};

// Monte-Carlo
class MonteCarlo : public IntegralSolution
{
public:
    explicit MonteCarlo(const num_expression::Expression& expr) : func(expr){};

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const num_expression::Expression& func;
    [[nodiscard]] double sampleFromUniformDistribution(const double lower, const double upper, const double eps) const;
#ifdef INTEGRAL_MONTE_CARLO_NO_UNIFORM
    [[nodiscard]] double sampleFromNormalDistribution(const double lower, const double upper, const double eps) const;
#endif
};
} // namespace num_integral
