#include <cmath>
#include "../include/expression.hpp"

double Function1::operator()(const double x) const
{
    return ((x * sin(x)) / (1.0 + cos(x) * cos(x)));
}

double Function2::operator()(const double x) const
{
    return (x + 10 * sin(5 * x) + 7 * cos(4 * x));
}

#ifdef NO_MAXIMUM
// f(X)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
double Griewank::operator()(const double x) const
{
    return (1.0 + 1.0 / 4000.0 * x * x - cos(x)) * rate;
}

// f(x)=418.9829d-Σ(1→d)[Xi*sin((|Xi|)^(1/2))],x∈[-500,500],f(min)=0
double Schwefel::operator()(const double x) const
{
    return (418.9829 - x * sin(sqrt(fabs(x)))) * rate;
}

// f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
double Rastrigin::operator()(const double x) const
{
    return (x * x - 10.0 * cos(2.0 * M_PI * x) + 10.0) * rate;
}
#endif
