//! @file divisor.cpp
//! @author ryftchen
//! @brief The definitions (divisor) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "divisor.hpp"

#include <algorithm>
#include <cmath>

namespace numeric::divisor
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version()
{
    static const char* const ver = "0.1.0";
    return ver;
}

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

    return getAllDivisors(greatestCommonDivisor);
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

    return getAllDivisors(greatestCommonDivisor);
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
