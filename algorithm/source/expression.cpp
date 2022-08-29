#include "expression.hpp"

namespace alg_expression
{
double Function1::operator()(const double x) const
{
    return ((x * std::sin(x)) / (1.0 + std::cos(x) * std::cos(x)));
}

double Function2::operator()(const double x) const
{
    return (x + 10.0 * std::sin(5.0 * x) + 7.0 * std::cos(4.0 * x));
}

#ifdef EXPRESSION_NO_MAXIMUM
// f(X)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
double Griewank::operator()(const double x) const
{
    return (1.0 + 1.0 / 4000.0 * x * x - std::cos(x)) * rate;
}

// f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
double Rastrigin::operator()(const double x) const
{
    return (x * x - 10.0 * std::cos(2.0 * M_PI * x) + 10.0) * rate;
}
#endif
} // namespace alg_expression
