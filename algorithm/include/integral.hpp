#pragma once

#include <bits/stdint-uintn.h>
#include "expression.hpp"

namespace algo_integral
{
#define INTEGRAL_EPSILON 1e-5
#define INTEGRAL_RESULT "*%-11s method: I=%+.5f  ==>Run time: %8.5f ms\n"

class Integral
{
public:
    virtual ~Integral() = default;
    virtual double operator()(double lower, double upper, const double eps) const = 0;

protected:
    static int inline getSign(double& lower, double& upper);
    friend double trapezoid(
        const algo_expression::Expression& express, const double left, const double height,
        const uint32_t step);
};

int inline Integral::getSign(double& lower, double& upper)
{
    return (lower < upper) ? 1 : ((lower > upper) ? (std::swap(lower, upper), -1) : 0);
}

// Trapezoidal method
#define INTEGRAL_TRAPEZOIDAL_MIN_STEP std::pow(2, 3)
class Trapezoidal : public Integral
{
public:
    explicit Trapezoidal(const algo_expression::Expression& express) : func(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const algo_expression::Expression& func;
};

// Adaptive Simpson's 1/3 method
class Simpson : public Integral
{
public:
    explicit Simpson(const algo_expression::Expression& express) : func(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const algo_expression::Expression& func;
    [[nodiscard]] double simpsonIntegral(
        const double left, const double right, const double eps) const;
    [[nodiscard]] double compositeSimpsonOneThird(
        const double left, const double right, const uint32_t n) const;
    [[nodiscard]] double simpsonOneThird(const double left, const double right) const;
};

// Romberg method
class Romberg : public Integral
{
public:
    explicit Romberg(const algo_expression::Expression& express) : func(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const algo_expression::Expression& func;
};

// Gauss-Legendre's 5-points method
#define INTEGRAL_GAUSS_NODE 5
#define INTEGRAL_GAUSS_COEFFICIENT 2
class Gauss : public Integral
{
public:
    explicit Gauss(const algo_expression::Expression& express) : func(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const algo_expression::Expression& func;
};

// Monte-Carlo method
class MonteCarlo : public Integral
{
public:
    explicit MonteCarlo(const algo_expression::Expression& express) : func(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const algo_expression::Expression& func;
    [[nodiscard]] double sampleFromUniformDistribution(
        const double lower, const double upper, const double eps) const;
#ifdef INTEGRAL_MONTE_CARLO_NO_UNIFORM
    [[nodiscard]] double sampleFromNormalDistribution(
        const double lower, const double upper, const double eps) const;
#endif
};
} // namespace algo_integral
