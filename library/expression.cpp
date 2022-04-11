#include "expression.hpp"

double Function1::operator()(const double x) const
{
    return EXPRESSION_FUN_1;
}

double Function2::operator()(const double x) const
{
    return EXPRESSION_FUN_2;
}

#ifdef EXPRESSION_NO_MAXIMUM
// f(X)=1+1/4000*Σ(1→n)[(Xi)^2]-Π(1→n)[cos(Xi/(i)^(1/2))],x∈[-600,600],f(min)=0
double Griewank::operator()(const double x) const
{
    return EXPRESSION_GRI * rate;
}

// f(x)=An+Σ(1→n)[(Xi)^2-Acos(2π*Xi)],A=10,x∈[-5.12,5.12],f(min)=0
double Rastrigin::operator()(const double x) const
{
    return EXPRESSION_RAS * rate;
}
#endif
