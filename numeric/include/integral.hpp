#pragma once

#include <string_view>
#include <tuple>

namespace numeric::integral
{
namespace expression
{
class Expression
{
public:
    virtual ~Expression() = default;

    virtual inline double operator()(const double x) const = 0;
};

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
        return (std::tie(range.range1, range.range2, range.exprStr) == std::tie(range1, range2, exprStr));
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
} // namespace expression

inline constexpr double epsilon = 1e-5;

class IntegralSolution
{
public:
    virtual ~IntegralSolution() = default;

    virtual double operator()(double lower, double upper, const double eps) const = 0;

protected:
    static inline int getSign(double& lower, double& upper);
    friend double trapezoid(
        const expression::Expression& expr,
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
    explicit Trapezoidal(const expression::Expression& expr);

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const expression::Expression& expr;
};

// Adaptive Simpson's 1/3
class Simpson : public IntegralSolution
{
public:
    explicit Simpson(const expression::Expression& expr);

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const expression::Expression& expr;
    [[nodiscard]] double simpsonIntegral(const double left, const double right, const double eps) const;
    [[nodiscard]] double compositeSimpsonOneThird(const double left, const double right, const uint32_t n) const;
    [[nodiscard]] double simpsonOneThird(const double left, const double right) const;
};

// Romberg
class Romberg : public IntegralSolution
{
public:
    explicit Romberg(const expression::Expression& expr);

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const expression::Expression& expr;
};

// Gauss-Legendre's 5-points
class Gauss : public IntegralSolution
{
public:
    explicit Gauss(const expression::Expression& expr);

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const expression::Expression& expr;
};

// Monte-Carlo
class MonteCarlo : public IntegralSolution
{
public:
    explicit MonteCarlo(const expression::Expression& expr);

    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    const expression::Expression& expr;
    [[nodiscard]] double sampleFromUniformDistribution(const double lower, const double upper, const double eps) const;
#ifdef INTEGRAL_MONTE_CARLO_NO_UNIFORM
    [[nodiscard]] double sampleFromNormalDistribution(const double lower, const double upper, const double eps) const;
#endif
};
} // namespace numeric::integral
