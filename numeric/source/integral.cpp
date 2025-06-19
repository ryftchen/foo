//! @file integral.cpp
//! @author ryftchen
//! @brief The definitions (integral) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "integral.hpp"

#include <random>

namespace numeric::integral
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::int8_t Integral::getSign(double& lower, double& upper)
{
    return (lower < upper) ? 1 : ((lower > upper) ? (std::swap(lower, upper), -1) : 0);
}

double Integral::trapezoidalRule(
    const Expression& expr, const double left, const double height, const std::uint32_t step)
{
    double sum = 0.0, x = left;
    const double delta = height / step;
    for (std::uint32_t i = 0; i < step; ++i)
    {
        const double area = (expr(x) + expr(x + delta)) * delta / 2.0;
        sum += area;
        x += delta;
    }

    return sum;
}

double Trapezoidal::operator()(double lower, double upper, const double eps) const
{
    const std::int8_t sign = getSign(lower, upper);
    const std::uint32_t minStep = std::pow(2, 3);
    const double height = upper - lower;
    double sum = 0.0, s1 = 0.0, s2 = 0.0;
    std::uint32_t n = 1;

    do
    {
        sum = trapezoidalRule(expr, lower, height, n);
        s1 = s2;
        s2 = sum;
        n *= 2;
    }
    while ((std::fabs(s1 - s2) > eps) || (n < minStep));
    sum = s2 * sign;

    return sum;
}

double Simpson::operator()(double lower, double upper, const double eps) const
{
    const std::int8_t sign = getSign(lower, upper);
    double sum = simpsonIntegral(lower, upper, eps);
    sum *= sign;

    return sum;
}

double Simpson::simpsonIntegral(const double left, const double right, const double eps) const
{
    const double mid = (left + right) / 2.0, sum = simpsonOneThird(left, right);
    if (std::fabs(sum - (compositeSimpsonOneThird(left, mid, 2) + compositeSimpsonOneThird(mid, right, 2))) > eps)
    {
        return simpsonIntegral(left, mid, eps) + simpsonIntegral(mid, right, eps);
    }

    return sum;
}

double Simpson::compositeSimpsonOneThird(const double left, const double right, const std::uint32_t step) const
{
    const double stepLen = (right - left) / step;
    double sum = 0.0;
    for (std::uint32_t i = 0; i < step; ++i)
    {
        sum += simpsonOneThird(left + (i * stepLen), left + ((i + 1) * stepLen));
    }

    return sum;
}

double Simpson::simpsonOneThird(const double left, const double right) const
{
    return (expr(left) + 4.0 * expr((left + right) / 2.0) + expr(right)) / 6.0 * (right - left);
}

double Romberg::operator()(double lower, double upper, const double eps) const
{
    const std::int8_t sign = getSign(lower, upper);
    std::uint32_t k = 0;
    const double height = upper - lower;
    const auto trapezoid = std::bind(trapezoidalRule, std::ref(expr), lower, height, std::placeholders::_1);

    double t0 = trapezoid(std::pow(2, k));
    k = 1;
    double t1Zero = trapezoid(std::pow(2, k)), t1 = richardsonExtrapolation(t1Zero, trapezoid(std::pow(2, k + 1)), k);
    while (std::fabs(t1 - t0) > eps)
    {
        ++k;
        t0 = t1;
        t1Zero = trapezoid(std::pow(2, k));
        for (std::uint32_t i = 1; i <= k; ++i)
        {
            t1 = richardsonExtrapolation(t1Zero, trapezoid(std::pow(2, i + 1)), i);
        }
    }
    const double sum = trapezoid(std::pow(2, k)) * sign;

    return sum;
}

double Romberg::richardsonExtrapolation(const double lowPrec, const double highPrec, const std::uint32_t division)
{
    const double weight = std::pow(4, division);
    return (weight / (weight - 1) * highPrec) - (1.0 / weight * lowPrec);
}

double Gauss::operator()(double lower, double upper, const double eps) const
{
    const std::int8_t sign = getSign(lower, upper);
    double sum = 0.0, s1 = 0.0, s2 = 0.0;
    std::uint32_t n = 1;

    do
    {
        sum = 0.0;
        const double stepLen = (upper - lower) / n;
        for (std::uint32_t i = 0; i < n; ++i)
        {
            const double left = lower + (i * stepLen), right = left + stepLen;
            for (const auto& coeff : gaussLegendreTable)
            {
                const double x = ((right - left) * coeff[0] + (left + right)) / 2.0,
                             polynomial = expr(x) * coeff[1] * (right - left) / 2.0;
                sum += polynomial;
            }
        }
        s1 = s2;
        s2 = sum;
        n *= 2;
    }
    while (std::fabs(s1 - s2) > eps);
    sum = s2 * sign;

    return sum;
}

double MonteCarlo::operator()(double lower, double upper, const double eps) const
{
    const std::int8_t sign = getSign(lower, upper);
    double sum = sampleFromUniformDistribution(lower, upper, eps);
    sum *= sign;

    return sum;
}

double MonteCarlo::sampleFromUniformDistribution(const double lower, const double upper, const double eps) const
{
    const std::uint32_t n = 1.0 / eps;
    std::ranlux48 engine(std::random_device{}());
    std::uniform_real_distribution<double> dist(lower, upper);
    double sum = 0.0;
    for (std::uint32_t i = 0; i < n; ++i)
    {
        sum += expr(dist(engine));
    }
    sum *= (upper - lower) / n;

    return sum;
}

double MonteCarlo::sampleFromNormalDistribution(const double lower, const double upper, const double eps) const
{
    const std::uint32_t n = 1.0 / eps;
    const double mu = (lower + upper) / 2.0, sigma = (upper - lower) / 6.0;
    std::ranlux48 engine(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double sum = 0.0, x = 0.0;
    for (std::uint32_t i = 0; i < n; ++i)
    {
        do
        {
            const double u1 = dist(engine), u2 = dist(engine), mag = sigma * std::sqrt(-2.0 * std::log(u1));
            x = mag * std::sin(2.0 * std::numbers::pi * u2) + mu;
        }
        while ((x < lower) || (x > upper));
        const double probabilityDensityFunction = (1.0 / std::sqrt(2.0 * std::numbers::pi * sigma * sigma))
            * std::pow(std::numbers::e, (-(x - mu) * (x - mu)) / (2.0 * sigma * sigma));
        sum += expr(x) / probabilityDensityFunction;
    }
    sum /= n;

    return sum;
}
} // namespace numeric::integral
