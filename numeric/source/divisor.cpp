//! @file divisor.cpp
//! @author ryftchen
//! @brief The definitions (divisor) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "divisor.hpp"
#ifndef __PRECOMPILED_HEADER
#include <cmath>
#endif

namespace numeric::divisor
{
std::vector<int> Divisor::euclidean(int a, int b)
{
    a = std::abs(a);
    b = std::abs(b);

    while (b)
    {
        int temp = a % b;
        a = b;
        b = temp;
    }
    const int greatestCommonDivisor = a;
    std::vector<int> divisorVector = getAllDivisors(greatestCommonDivisor);

    return divisorVector;
}

std::vector<int> Divisor::stein(int a, int b)
{
    int greatestCommonDivisor = 0, c = 0;
    a = std::abs(a);
    b = std::abs(b);

    while ((0 == (a & 0x1)) && (0 == (b & 0x1)))
    {
        a = a >> 1;
        b = b >> 1;
        ++c;
    }
    if (0 == (a & 0x1))
    {
        a = a >> 1;
        greatestCommonDivisor = steinRecursive(a, b) << c;
    }
    else
    {
        greatestCommonDivisor = steinRecursive(b, a) << c;
    }
    std::vector<int> divisorVector = getAllDivisors(greatestCommonDivisor);

    return divisorVector;
}

int Divisor::steinRecursive(int a, int b)
{
    if (0 == a)
    {
        return b;
    }
    if (0 == b)
    {
        return a;
    }

    while (0 == (a & 0x1))
    {
        a = a >> 1;
    }
    if (a < b)
    {
        b = (b - a) >> 1;
        return steinRecursive(b, a);
    }
    else
    {
        a = (a - b) >> 1;
        return steinRecursive(a, b);
    }
}

std::vector<int> Divisor::getAllDivisors(const int greatestCommonDivisor)
{
    std::vector<int> divisors(0);
    for (int i = 1; i <= std::sqrt(greatestCommonDivisor); ++i)
    {
        if (0 == (greatestCommonDivisor % i))
        {
            divisors.emplace_back(i);
            if ((greatestCommonDivisor / i) != i)
            {
                divisors.emplace_back(greatestCommonDivisor / i);
            }
        }
    }
    std::sort(divisors.begin(), divisors.end());
    return divisors;
}
} // namespace numeric::divisor
