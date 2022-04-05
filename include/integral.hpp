#pragma once
#include <bits/stdint-uintn.h>
#include "expression.hpp"

#define INTEGRAL_EPSILON 1e-5
#define INTEGRAL_RUN_BEGIN "\r\n---------- BEGIN INTEGRAL ----------"
#define INTEGRAL_RUN_END "\r\n----------  END INTEGRAL  ----------"
#define INTEGRAL_TRAPEZOIDAL "*Trapezoidal method: I=%+.5f  ==>Run time: %8.5fms\n"
#define INTEGRAL_SIMPSON "*Simpson     method: I=%+.5f  ==>Run time: %8.5fms\n"
#define INTEGRAL_ROMBERG "*Romberg     method: I=%+.5f  ==>Run time: %8.5fms\n"
#define INTEGRAL_GAUSS "*Gauss       method: I=%+.5f  ==>Run time: %8.5fms\n"
#define INTEGRAL_MONTE_CARLO "*MonteCarlo  method: I=%+.5f  ==>Run time: %8.5fms\n"

class Integral
{
public:
    virtual ~Integral() = default;
    virtual double operator()(double lower, double upper, const double eps) const = 0;

protected:
    static int inline getSign(double& lower, double& upper);
};
int inline Integral::getSign(double& lower, double& upper)
{
    return (lower < upper) ? 1 : (lower > upper ? (std::swap(lower, upper), -1) : 0);
}

// Trapezoidal method
#define INTEGRAL_TRAPEZOIDAL_MIN_STEP pow(2, 3)
class Trapezoidal : public Integral
{
public:
    explicit Trapezoidal(const Expression& express) : fun(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const Expression& fun;
    friend double trapezoid(
        const Expression& express, const double left, const double height, const uint32_t step);
};

// Adaptive Simpson's 1/3 method
#define INTEGRAL_SIMPSON_ONE_THIRDS_FORMULAS \
    ((fun(left) + 4.0 * fun((left + right) / 2.0) + fun(right)) / 6.0 * (right - left))
class Simpson : public Integral
{
public:
    explicit Simpson(const Expression& express) : fun(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const Expression& fun;
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
    explicit Romberg(const Expression& express) : fun(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const Expression& fun;
    friend double trapezoid(
        const Expression& express, const double left, const double height, const uint32_t step);
};

// Gauss-Legendre's 5-points method
#define INTEGRAL_GAUSS_NODE 5
#define INTEGRAL_GAUSS_COEFFICIENT 2
class Gauss : public Integral
{
public:
    explicit Gauss(const Expression& express) : fun(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const Expression& fun;
};

// Monte-Carlo method
class MonteCarlo : public Integral
{
public:
    explicit MonteCarlo(const Expression& express) : fun(express){};
    double operator()(double lower, double upper, const double eps) const override;

private:
    const Expression& fun;
    [[nodiscard]] double sampleFromUniformDistribution(
        const double lower, const double upper, const double eps) const;
#ifdef NO_UNIFORM
    double sampleFromNormalDistribution(
        const double lower, const double upper, const double eps) const;
#endif
};
