//! @file divisor.cpp
//! @author ryftchen
//! @brief The definitions (divisor) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "divisor.hpp"

#include <cmath>

namespace numeric::divisor
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::set<std::int32_t> Divisor::euclidean(std::int32_t a, std::int32_t b)
{
    a = std::abs(a);
    b = std::abs(b);

    while (b)
    {
        const std::int32_t temp = a % b;
        a = b;
        b = temp;
    }

    return getAllDivisors(a);
}

std::set<std::int32_t> Divisor::stein(std::int32_t a, std::int32_t b)
{
    std::int32_t gcd = 0, c = 0;
    a = std::abs(a);
    b = std::abs(b);

    while (((a & 0x1) == 0) && ((b & 0x1) == 0))
    {
        a = a >> 1;
        b = b >> 1;
        ++c;
    }
    if ((a & 0x1) == 0)
    {
        a = a >> 1;
        gcd = steinRecursive(a, b) << c;
    }
    else
    {
        gcd = steinRecursive(b, a) << c;
    }

    return getAllDivisors(gcd);
}

std::int32_t Divisor::steinRecursive(std::int32_t a, std::int32_t b)
{
    if (a == 0)
    {
        return b;
    }
    if (b == 0)
    {
        return a;
    }

    while ((a & 0x1) == 0)
    {
        a = a >> 1;
    }
    if (a < b)
    {
        b = (b - a) >> 1;
        return steinRecursive(b, a);
    }
    a = (a - b) >> 1;

    return steinRecursive(a, b);
}

std::set<std::int32_t> Divisor::getAllDivisors(const std::int32_t gcd)
{
    std::set<std::int32_t> divisors{};
    for (std::int32_t i = 1; i <= std::sqrt(gcd); ++i)
    {
        if ((gcd % i) == 0)
        {
            divisors.emplace(i);
            if ((gcd / i) != i)
            {
                divisors.emplace(gcd / i);
            }
        }
    }

    return divisors;
}
} // namespace numeric::divisor
