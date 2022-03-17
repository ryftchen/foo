#include "expression.hpp"

double Function1::operator()(const double x) const
{
    return EXPRESS_FUN_1;
}

double Function2::operator()(const double x) const
{
    return EXPRESS_FUN_2;
}

#ifdef NO_MAXIMUM
// f(X)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
double Griewank::operator()(const double x) const
{
    return EXPRESS_GRI * rate;
}

// f(x)=418.9829d-Σ(1→d)[Xi*sin((|Xi|)^(1/2))],x∈[-500,500],f(min)=0
double Schwefel::operator()(const double x) const
{
    return EXPRESS_SCH * rate;
}

// f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
double Rastrigin::operator()(const double x) const
{
    return EXPRESS_RAS * rate;
}
#endif
