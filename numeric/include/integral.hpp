//! @file integral.hpp
//! @author ryftchen
//! @brief The declarations (integral) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#pragma once

#include <string_view>
#include <tuple>

//! @brief Integral-related functions in the numeric module.
namespace numeric::integral
{
//! @brief Target expressions of integration.
namespace expression
{
//! @brief Target expressions.
class Expression
{
public:
    //! @brief Destroy the Expression object.
    virtual ~Expression() = default;

    //! @brief The operator (()) overloading of Function class.
    //! @param x independent variable
    //! @return dependent variable
    virtual inline double operator()(const double x) const = 0;
};

//! @brief Expression object's helper type for the visitor.
//! @tparam Ts type of visitor
template <class... Ts>
struct ExprOverloaded : Ts...
{
    using Ts::operator()...;
};

//! @brief Explicit deduction guide for ExprOverloaded.
//! @tparam Ts type of visitor
template <class... Ts>
ExprOverloaded(Ts...) -> ExprOverloaded<Ts...>;

//! @brief Range properties of the expression.
//! @tparam T1 type of lower endpoint
//! @tparam T2 type of upper endpoint
template <typename T1, typename T2>
struct ExprRange
{
    //! @brief Construct a new ExprRange object.
    //! @param range1 lower endpoint
    //! @param range2 upper endpoint
    //! @param exprDescr expression description
    ExprRange(const T1& range1, const T2& range2, const std::string_view exprDescr) :
        range1(range1), range2(range2), exprDescr(exprDescr){};
    //! @brief Construct a new ExprRange object.
    ExprRange() = delete;
    //! @brief Lower endpoint.
    T1 range1;
    //! @brief Upper endpoint.
    T2 range2;
    //! @brief Expression description.
    std::string_view exprDescr;
    //! @brief The operator (==) overloading of ExprRange class.
    //! @param rhs right-hand side
    //! @return be equal or not equal
    bool operator==(const ExprRange& rhs) const
    {
        return (std::tie(rhs.range1, rhs.range2, rhs.exprDescr) == std::tie(range1, range2, exprDescr));
    }
};
//! @brief Mapping hash value for the expression.
struct ExprMapHash
{
    //! @brief The operator (()) overloading of ExprMapHash class.
    //! @tparam T1 type of lower endpoint
    //! @tparam T2 type of upper endpoint
    //! @param range range properties of the expression
    //! @return hash value
    template <typename T1, typename T2>
    std::size_t operator()(const ExprRange<T1, T2>& range) const
    {
        std::size_t hash1 = std::hash<T1>()(range.range1);
        std::size_t hash2 = std::hash<T2>()(range.range2);
        std::size_t hash3 = std::hash<std::string_view>()(range.exprDescr);
        return (hash1 ^ hash2 ^ hash3);
    }
};
} // namespace expression

//! @brief The precision of calculation.
inline constexpr double epsilon = 1e-5;

//! @brief Solution of integral.
class IntegralSolution
{
public:
    //! @brief Destroy the IntegralSolution object.
    virtual ~IntegralSolution() = default;

    //! @brief The operator (()) overloading of IntegralSolution class.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of integral
    virtual double operator()(double lower, double upper, const double eps) const = 0;

protected:
    //! @brief Get the sign.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @return sign
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

//! @brief The trapezoidal method.
class Trapezoidal : public IntegralSolution
{
public:
    //! @brief Construct a new Trapezoidal object.
    //! @param expr target expression
    explicit Trapezoidal(const expression::Expression& expr);

    //! @brief The operator (()) overloading of Trapezoidal class.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const expression::Expression& expr;
};

//! @brief The adaptive Simpson's 1/3 method.
class Simpson : public IntegralSolution
{
public:
    //! @brief Construct a new Simpson object.
    //! @param expr target expression
    explicit Simpson(const expression::Expression& expr);

    //! @brief The operator (()) overloading of Simpson class.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const expression::Expression& expr;
    //! @brief Calculate the value of the definite integral with the Simpson's rule.
    //! @param left left endpoint
    //! @param right right endpoint
    //! @param eps precision of calculation
    //! @return result of definite integral
    [[nodiscard]] double simpsonIntegral(const double left, const double right, const double eps) const;
    //! @brief Composite Simpson's 1/3 formula.
    //! @param left left endpoint
    //! @param right right endpoint
    //! @param step number of steps
    //! @return result of definite integral
    [[nodiscard]] double compositeSimpsonOneThird(const double left, const double right, const uint32_t step) const;
    //! @brief Simpson's 1/3 formula.
    //! @param left left endpoint
    //! @param right right endpoint
    //! @return result of definite integral
    [[nodiscard]] double simpsonOneThird(const double left, const double right) const;
};

//! @brief The Romberg method.
class Romberg : public IntegralSolution
{
public:
    //! @brief Construct a new Romberg object.
    //! @param expr target expression
    explicit Romberg(const expression::Expression& expr);

    //! @brief The operator (()) overloading of Romberg class.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const expression::Expression& expr;
};

//! @brief The Gauss-Legendre's 5-points method.
class Gauss : public IntegralSolution
{
public:
    //! @brief Construct a new Gauss object.
    //! @param expr target expression
    explicit Gauss(const expression::Expression& expr);

    //! @brief The operator (()) overloading of Gauss class.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const expression::Expression& expr;
};

//! @brief The Monte-Carlo method.
class MonteCarlo : public IntegralSolution
{
public:
    //! @brief Construct a new MonteCarlo object.
    //! @param expr target expression
    explicit MonteCarlo(const expression::Expression& expr);

    //! @brief The operator (()) overloading of MonteCarlo class.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of integral
    [[nodiscard]] double operator()(double lower, double upper, const double eps) const override;

private:
    //! @brief Target expression.
    const expression::Expression& expr;
    //! @brief Sample from the uniform distribution.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of definite integral
    [[nodiscard]] double sampleFromUniformDistribution(const double lower, const double upper, const double eps) const;
#ifdef INTEGRAL_MONTE_CARLO_NO_UNIFORM
    //! @brief Sample from the normal distribution.
    //! @param lower lower endpoint
    //! @param upper upper endpoint
    //! @param eps precision of calculation
    //! @return result of definite integral
    [[nodiscard]] double sampleFromNormalDistribution(const double lower, const double upper, const double eps) const;
#endif
};
} // namespace numeric::integral
