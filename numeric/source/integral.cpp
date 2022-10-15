#include "integral.hpp"
#include <functional>
#include "utility/include/file.hpp"
#include "utility/include/time.hpp"

#define INTEGRAL_RESULT(opt) "*%-11s method: I(" #opt ")=%+.5f  ==>Run time: %8.5f ms\n"

namespace num_integral
{
double trapezoid(const num_expression::Expression& express, const double left, const double height, const uint32_t step)
{
    double sum = 0.0, x = left;
    const double delta = height / step;
    for (uint32_t i = 0; i < step; ++i)
    {
        const double area = (express(x) + express(x + delta)) * delta / 2.0; // S=(a+b)*h/2
        sum += area;
        x += delta;
    }
    return sum;
}

// Trapezoidal method
double Trapezoidal::operator()(double lower, double upper, const double eps) const
{
    TIME_BEGIN(timing);
    const int sign = Integral::getSign(lower, upper);
    const uint32_t minStep = std::pow(2, 3);
    const double height = upper - lower;
    double sum = 0.0, s1 = 0.0, s2 = 0.0;
    uint32_t n = 1;

    do
    {
        sum = trapezoid(func, lower, height, n);
        s1 = s2;
        s2 = sum;
        n *= 2;
    }
    while ((std::fabs(s1 - s2) > eps) || (n < minStep));
    sum = s2 * sign;

    TIME_END(timing);
    FORMAT_PRINT(INTEGRAL_RESULT(def), "Trapezoidal", sum, TIME_INTERVAL(timing));
    return sum;
}

// Adaptive Simpson's 1/3 method
double Simpson::operator()(double lower, double upper, const double eps) const
{
    TIME_BEGIN(timing);
    const int sign = Integral::getSign(lower, upper);

    double sum = simpsonIntegral(lower, upper, eps);
    sum *= sign;

    TIME_END(timing);
    FORMAT_PRINT(INTEGRAL_RESULT(def), "Simpson", sum, TIME_INTERVAL(timing));
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

double Simpson::compositeSimpsonOneThird(const double left, const double right, const uint32_t n) const
{
    const double stepLength = (right - left) / n;
    double sum = 0.0;
    for (uint32_t i = 0; i < n; ++i)
    {
        // I≈(b-a)/6[Y0+Y2n+4(Y1+...+Y2n-1)+6(Y2+...+Y2n-2)]
        sum += simpsonOneThird(left + i * stepLength, left + (i + 1) * stepLength);
    }
    return sum;
}

double Simpson::simpsonOneThird(const double left, const double right) const
{
    return (func(left) + 4.0 * func((left + right) / 2.0) + func(right)) / 6.0 * (right - left);
}

// Romberg method
double Romberg::operator()(double lower, double upper, const double eps) const
{
    TIME_BEGIN(timing);
    const int sign = Integral::getSign(lower, upper);
    uint32_t k = 0;
    double sum = 0.0;
    const double height = upper - lower;
    const auto trapezoidFunctor = std::bind(trapezoid, std::ref(func), lower, height, std::placeholders::_1);
    double t0 = trapezoidFunctor(std::pow(2, k));

    k = 1;
    double t1Zero = trapezoidFunctor(std::pow(2, k));
    double t1 =
        std::pow(4, k) / (std::pow(4, k) - 1) * trapezoidFunctor(std::pow(2, k + 1)) - 1.0 / std::pow(4, k) * t1Zero;

    while (std::fabs(t1 - t0) > eps)
    {
        ++k;
        t0 = t1;
        t1Zero = trapezoidFunctor(std::pow(2, k));
        for (uint32_t i = 1; i <= k; ++i)
        {
            t1 = std::pow(4, i) / (std::pow(4, i) - 1) * trapezoidFunctor(std::pow(2, i + 1))
                - 1.0 / std::pow(4, i) * t1Zero;
        }
    }
    sum = trapezoidFunctor(std::pow(2, k)) * sign;

    TIME_END(timing);
    FORMAT_PRINT(INTEGRAL_RESULT(def), "Romberg", sum, TIME_INTERVAL(timing));
    return sum;
}

// Gauss-Legendre's 5-points method
double Gauss::operator()(double lower, double upper, const double eps) const
{
    TIME_BEGIN(timing);
    const int sign = Integral::getSign(lower, upper);
    constexpr uint32_t gaussNodes = 5, gaussCoefficient = 2;
    constexpr std::array<std::array<double, gaussCoefficient>, gaussNodes> gaussLegendreTable = {
        {{-0.9061798459, +0.2369268851},
         {-0.5384693101, +0.4786286705},
         {+0.0000000000, +0.5688888889},
         {+0.5384693101, +0.4786286705},
         {+0.9061798459, +0.2369268851}}};
    double sum = 0.0, s1 = 0.0, s2 = 0.0;
    uint32_t n = 1;

    do
    {
        sum = 0.0;
        const double stepLength = (upper - lower) / n;
        for (uint32_t i = 0; i < n; ++i)
        {
            const double left = lower + i * stepLength;
            const double right = left + stepLength;
            for (uint32_t j = 0; j < gaussNodes; ++j)
            {
                // x=1/2[(a+b)+(b-a)t]
                const double x = ((right - left) * gaussLegendreTable.at(j).at(0) + (left + right)) / 2.0;
                const double polynomial = func(x) * gaussLegendreTable.at(j).at(1) * (right - left) / 2.0;
                sum += polynomial;
            }
        }
        s1 = s2;
        s2 = sum;
        n *= 2;
    }
    while (std::fabs(s1 - s2) > eps);
    sum = s2 * sign;

    TIME_END(timing);
    FORMAT_PRINT(INTEGRAL_RESULT(def), "Gauss", sum, TIME_INTERVAL(timing));
    return sum;
}

// Monte-Carlo method
double MonteCarlo::operator()(double lower, double upper, const double eps) const
{
    TIME_BEGIN(timing);
    const int sign = Integral::getSign(lower, upper);

    double sum = sampleFromUniformDistribution(lower, upper, eps);
#ifdef INTEGRAL_MONTE_CARLO_NO_UNIFORM
    double sum = sampleFromNormalDistribution(lower, upper, eps);
#endif
    sum *= sign;

    TIME_END(timing);
    FORMAT_PRINT(INTEGRAL_RESULT(def), "MonteCarlo", sum, TIME_INTERVAL(timing));
    return sum;
}

double MonteCarlo::sampleFromUniformDistribution(const double lower, const double upper, const double eps) const
{
    const uint32_t n = (upper - lower) / eps;
    std::mt19937 seed(std::random_device{}());
    std::uniform_real_distribution<double> randomX(lower, upper);
    double sum = 0.0;
    for (uint32_t i = 0; i < n; ++i)
    {
        double x = randomX(seed);
        sum += func(x);
    }
    sum *= (upper - lower) / n; // I≈(b-a)/N*[F(X1)+F(X2)+...+F(Xn)]

    return sum;
}

#ifdef INTEGRAL_MONTE_CARLO_NO_UNIFORM
double MonteCarlo::sampleFromNormalDistribution(const double lower, const double upper, const double eps) const
{
    const uint32_t n = (upper - lower) / eps;
    const double mu = (lower + upper) / 2.0, sigma = (upper - lower) / 6.0;
    std::mt19937 seed(std::random_device{}());
    std::uniform_real_distribution<double> randomU(0.0, 1.0);
    double sum = 0.0, x = 0.0;
    for (uint32_t i = 0; i < n; ++i)
    {
        do
        {
            double u1 = randomU(seed);
            double u2 = randomU(seed);
            double mag = sigma * std::sqrt(-2.0 * std::log(u1));
            x = mag * std::sin(2.0 * M_PI * u2) + mu; // Box-Muller Transform
        }
        while ((x < lower) || (x > upper));
        const double probabilityDensityFunction = (1.0 / std::sqrt(2.0 * M_PI * sigma * sigma))
            * std::pow(M_E, (-(x - mu) * (x - mu)) / (2.0 * sigma * sigma));
        sum += func(x) / probabilityDensityFunction; // I≈1/N*[F(X1)/P(X1)+...+F(Xn)/P(Xn)]
    }
    sum /= n;

    return sum;
}
#endif
} // namespace num_integral
