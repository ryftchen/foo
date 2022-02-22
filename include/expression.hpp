#pragma once
#include <cmath>

//#define PI    acos(-1.0)

class Expression
{
public:
    virtual ~Expression() {};
    virtual double operator()(const double x) const = 0;
};

#define EXPRESS_FUN_1_OPTIMUM    "\r\nOptimum Expression: y=x*sin(x)/(1+(cos(x))^2),x∈[-π/2,2π]"
#define EXPRESS_FUN_1_INTEGRAL    "\r\nIntegral Expression: I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"
#define EXPRESS_FUN_1_RANGE_1    (- M_PI / 2.0)
#define EXPRESS_FUN_1_RANGE_2    (2.0 * M_PI)
class Function1: public Expression
{
public:
    double operator()(const double x) const override;
};

#define EXPRESS_FUN_2_OPTIMUM    "\r\nOptimum Expression: y=x+10sin(5x)+7cos(4x),x∈[0,9]"
#define EXPRESS_FUN_2_INTEGRAL    "\r\nIntegral Expression: I=∫(0→9)x+10sin(5x)+7cos(4x)dx"
#define EXPRESS_FUN_2_RANGE_1    0.0
#define EXPRESS_FUN_2_RANGE_2    9.0
class Function2: public Expression
{
public:
    double operator()(const double x) const override;
};

#ifdef NO_MAXIMUM
#define EXPRESS_RATE_KEEP    1.0
#define EXPRESS_RATE_FLIP    -1.0

#define EXPRESS_GRI_OPTIMUM    "\r\nOptimum Expression: one-dimensional Griewank function"
#define EXPRESS_GRI_INTEGRAL    "\r\nIntegral Expression: one-dimensional Griewank function"
#define EXPRESS_GRI_RANGE_1    -600.0
#define EXPRESS_GRI_RANGE_2    600.0
class Griewank: public Expression
{
public:
    explicit Griewank(const double rate): rate(rate) {};
    double operator()(const double x) const override;

    Griewank() = delete;
private:
    const double rate;
};

#define EXPRESS_SCH_OPTIMUM    "\r\nOptimum Expression: one-dimensional Schwefel function"
#define EXPRESS_SCH_INTEGRAL    "\r\nIntegral Expression: one-dimensional Schwefel function"
#define EXPRESS_SCH_RANGE_1    -500.0
#define EXPRESS_SCH_RANGE_2    500.0
class Schwefel: public Expression
{
public:
    explicit Schwefel(const double rate): rate(rate) {};
    double operator()(const double x) const override;

    Schwefel() = delete;
private:
    const double rate;
};

#define EXPRESS_RAS_OPTIMUM    "\r\nOptimum Expression: one-dimensional Rastrigin function"
#define EXPRESS_RAS_INTEGRAL    "\r\nIntegral Expression: one-dimensional Rastrigin function"
#define EXPRESS_RAS_RANGE_1    -5.12
#define EXPRESS_RAS_RANGE_2    5.12
class Rastrigin: public Expression
{
public:
    explicit Rastrigin(const double rate): rate(rate) {};
    double operator()(const double x) const override;

    Rastrigin() = delete;
private:
    const double rate;
};
#endif
