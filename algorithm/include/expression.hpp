#pragma once

#include <cmath>
#include <unordered_map>
#include <variant>

namespace alg_expression
{
class Expression
{
public:
    virtual ~Expression() = default;
    virtual double operator()(const double x) const = 0;
};

#define EXPRESSION_FUN_1_RANGE_1 (-M_PI / 2.0)
#define EXPRESSION_FUN_1_RANGE_2 (2.0 * M_PI)
#define EXPRESSION_FUN_1_OPTIMUM "\r\nOptimum Expression: y=x*sin(x)/(1+(cos(x))^2),x∈[-π/2,2π]"
#define EXPRESSION_FUN_1_INTEGRAL "\r\nIntegral Expression: I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"
class Function1 : public Expression
{
public:
    double operator()(const double x) const override;
};

#define EXPRESSION_FUN_2_RANGE_1 0.0
#define EXPRESSION_FUN_2_RANGE_2 9.0
#define EXPRESSION_FUN_2_OPTIMUM "\r\nOptimum Expression: y=x+10sin(5x)+7cos(4x),x∈[0,9]"
#define EXPRESSION_FUN_2_INTEGRAL "\r\nIntegral Expression: I=∫(0→9)x+10sin(5x)+7cos(4x)dx"
class Function2 : public Expression
{
public:
    double operator()(const double x) const override;
};

#ifdef EXPRESSION_NO_MAXIMUM
#define EXPRESSION_RATE_KEEP 1.0
#define EXPRESSION_RATE_FLIP -1.0

#define EXPRESSION_GRI_RANGE_1 -600.0
#define EXPRESSION_GRI_RANGE_2 600.0
#define EXPRESSION_GRI_OPTIMUM "\r\nOptimum Expression: one-dimensional Griewank function"
#define EXPRESSION_GRI_INTEGRAL "\r\nIntegral Expression: one-dimensional Griewank function"
class Griewank : public Expression
{
public:
    explicit Griewank(const double rate) : rate(rate){};
    double operator()(const double x) const override;

    Griewank() = delete;

private:
    const double rate;
};

#define EXPRESSION_RAS_RANGE_1 -5.12
#define EXPRESSION_RAS_RANGE_2 5.12
#define EXPRESSION_RAS_OPTIMUM "\r\nOptimum Expression: one-dimensional Rastrigin function"
#define EXPRESSION_RAS_INTEGRAL "\r\nIntegral Expression: one-dimensional Rastrigin function"
class Rastrigin : public Expression
{
public:
    explicit Rastrigin(const double rate) : rate(rate){};
    double operator()(const double x) const override;

    Rastrigin() = delete;

private:
    const double rate;
};
#endif

typedef std::variant<Function1, Function2> TargetExpression;
template <class... Ts>
struct ExpressionOverloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
ExpressionOverloaded(Ts...) -> ExpressionOverloaded<Ts...>;

template <typename T1, typename T2>
struct ExpressionRange
{
    ExpressionRange(const T1& range1, const T2& range2) : range1(range1), range2(range2){};
    T1 range1;
    T2 range2;

    bool operator==(const ExpressionRange& range) const
    {
        return (range.range1 == range1) && (range.range2 == range2);
    }
    ExpressionRange() = delete;
};
struct ExpressionMapHash
{
    template <class T1, class T2>
    std::size_t operator()(const ExpressionRange<T1, T2>& range) const
    {
        std::size_t hash1 = std::hash<T1>()(range.range1);
        std::size_t hash2 = std::hash<T2>()(range.range1);
        return hash1 ^ hash2;
    }
};
} // namespace alg_expression
