#pragma once

#include <cmath>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace algo_expression
{
class Expression
{
public:
    virtual ~Expression() = default;
    virtual double operator()(const double x) const = 0;
};

class Function1 : public Expression
{
public:
    double operator()(const double x) const override;

    static constexpr double range1 = -M_PI / 2.0;
    static constexpr double range2 = 2.0 * M_PI;
    static constexpr std::string_view optimumExpr{"y=x*sin(x)/(1+(cos(x))^2),x∈[-π/2,2π]"};
    static constexpr std::string_view integralExpr{"I=∫(-π/2→2π)x*sin(x)/(1+(cos(x))^2)dx"};
};

class Function2 : public Expression
{
public:
    double operator()(const double x) const override;

    static constexpr double range1 = 0.0;
    static constexpr double range2 = 9.0;
    static constexpr std::string_view optimumExpr{"y=x+10sin(5x)+7cos(4x),x∈[0,9]"};
    static constexpr std::string_view integralExpr{"I=∫(0→9)x+10sin(5x)+7cos(4x)dx"};
};

#ifdef EXPRESSION_NO_MAXIMUM
inline constexpr double positiveScaleFactor = 1.0;
inline constexpr double inverseScaleFactor = -1.0;

class Griewank : public Expression
{
public:
    explicit Griewank(const double rate) : rate(rate){};
    Griewank() = delete;
    double operator()(const double x) const override;

    static constexpr double range1 = -600.0;
    static constexpr double range2 = 600.0;
    static constexpr std::string_view optimumExpr{"one-dimensional Griewank function"};
    static constexpr std::string_view integralExpr{"one-dimensional Griewank function"};

private:
    const double rate;
};

class Rastrigin : public Expression
{
public:
    explicit Rastrigin(const double rate) : rate(rate){};
    Rastrigin() = delete;
    double operator()(const double x) const override;

    static constexpr double range1 = -5.12;
    static constexpr double range2 = 5.12;
    static constexpr std::string_view optimumExpr{"one-dimensional Rastrigin function"};
    static constexpr std::string_view integralExpr{"one-dimensional Rastrigin function"};

private:
    const double rate;
};
#endif

typedef std::variant<Function1, Function2> ExprTarget;
template <class... Ts>
struct ExprOverloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
ExprOverloaded(Ts...) -> ExprOverloaded<Ts...>;

template <typename T1, typename T2>
struct ExprRange
{
    ExprRange(const T1& range1, const T2& range2, const std::string_view exprStr) :
        range1(range1), range2(range2), exprStr(exprStr){};
    ExprRange() = delete;
    T1 range1;
    T2 range2;
    std::string_view exprStr;

    bool operator==(const ExprRange& range) const
    {
        return (
            std::tie(range.range1, range.range2, range.exprStr)
            == std::tie(range1, range2, exprStr));
    }
};
struct ExprMapHash
{
    template <typename T1, typename T2>
    std::size_t operator()(const ExprRange<T1, T2>& range) const
    {
        std::size_t hash1 = std::hash<T1>()(range.range1);
        std::size_t hash2 = std::hash<T2>()(range.range2);
        std::size_t hash3 = std::hash<std::string_view>()(range.exprStr);
        return (hash1 ^ hash2 ^ hash3);
    }
};
} // namespace algo_expression
