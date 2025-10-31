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

double Integral::trapezoidalRule(
    const Expression& expr, const double left, const double height, const std::uint32_t step)
{
    double sum = 0.0;
    double x = left;
    const double delta = height / step;
    for (std::uint32_t i = 0; i < step; ++i)
    {
        const double area = (expr(x) + expr(x + delta)) * delta / 2.0;
        sum += area;
        x += delta;
    }
    return sum;
}

double Trapezoidal::operator()(const double lower, const double upper, const double eps) const
{
    if (!expr)
    {
        return 0.0;
    }

    const double height = upper - lower;
    double s1 = 0.0;
    double s2 = 0.0;
    std::uint32_t n = 1;
    do
    {
        const double sum = trapezoidalRule(expr, lower, height, n);
        s1 = s2;
        s2 = sum;
        n *= 2;
    }
    while (std::fabs(s1 - s2) > eps);
    return s2;
}

double Simpson::operator()(const double lower, const double upper, const double eps) const
{
    return expr ? simpsonIntegral(lower, upper, eps) : 0.0;
}

double Simpson::simpsonIntegral(const double left, const double right, const double eps) const
{
    const double mid = (left + right) / 2.0;
    const double sum = simpsonOneThird(left, right);
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

double Romberg::operator()(const double lower, const double upper, const double eps) const
{
    if (!expr)
    {
        return 0.0;
    }

    const double height = upper - lower;
    const auto trapezoid = std::bind(trapezoidalRule, std::ref(expr), lower, height, std::placeholders::_1);
    std::uint32_t k = 0;

    std::vector<std::vector<double>> rombergTbl{};
    rombergTbl.emplace_back(std::vector<double>{trapezoid(1)});
    do
    {
        ++k;
        rombergTbl.emplace_back();
        rombergTbl[k].reserve(k + 1);
        rombergTbl[k].emplace_back(trapezoid(std::pow(2, k)));

        for (std::uint32_t i = 1; i <= k; ++i)
        {
            rombergTbl[k].emplace_back(
                richardsonExtrapolation(rombergTbl[k - 1][i - 1], rombergTbl[k][i - 1], std::pow(4, i)));
        }
    }
    while (std::fabs(rombergTbl[k][k] - rombergTbl[k - 1][k - 1]) > eps);
    return rombergTbl[k][k];
}

double Romberg::richardsonExtrapolation(const double lowPrec, const double highPrec, const double weight)
{
    return (weight * highPrec - lowPrec) / (weight - 1.0);
}

double Gauss::operator()(const double lower, const double upper, const double eps) const
{
    if (!expr)
    {
        return 0.0;
    }

    double s1 = 0.0;
    double s2 = 0.0;
    std::uint32_t n = 1;
    do
    {
        double sum = 0.0;
        const double stepLen = (upper - lower) / n;
        for (std::uint32_t i = 0; i < n; ++i)
        {
            const double left = lower + (i * stepLen);
            const double right = left + stepLen;
            for (const auto& coeff : gaussLegendreTbl)
            {
                const double x = ((right - left) * coeff[0] + (left + right)) / 2.0;
                const double polynomial = expr(x) * coeff[1] * (right - left) / 2.0;
                sum += polynomial;
            }
        }
        s1 = s2;
        s2 = sum;
        n *= 2;
    }
    while (std::fabs(s1 - s2) > eps);
    return s2;
}

double MonteCarlo::operator()(const double lower, const double upper, const double eps) const
{
    return expr ? sampleFromUniformDistribution(lower, upper, eps) : 0.0;
}

double MonteCarlo::sampleFromUniformDistribution(const double lower, const double upper, const double eps) const
{
    const std::uint32_t n = 1.0 / eps;
    std::mt19937_64 engine(std::random_device{}());
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
    const double mu = (lower + upper) / 2.0;
    const double sigma = (upper - lower) / 6.0;
    std::mt19937_64 engine(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double sum = 0.0;
    double x = 0.0;
    for (std::uint32_t i = 0; i < n; ++i)
    {
        do
        {
            const double u1 = dist(engine);
            const double u2 = dist(engine);
            const double mag = sigma * std::sqrt(-2.0 * std::log(u1));
            x = mag * std::sin(2.0 * std::numbers::pi * u2) + mu;
        }
        while ((x < lower) || (x > upper));
        const double probDens = (1.0 / std::sqrt(2.0 * std::numbers::pi * sigma * sigma))
            * std::pow(std::numbers::e, (-(x - mu) * (x - mu)) / (2.0 * sigma * sigma));
        sum += expr(x) / probDens;
    }
    sum /= n;
    return sum;
}
} // namespace numeric::integral
