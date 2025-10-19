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

std::set<std::int32_t> Divisor::euclidean(const std::int32_t a, const std::int32_t b)
{
    std::int32_t x = std::abs(a);
    std::int32_t y = std::abs(b);
    while (y)
    {
        const std::int32_t temp = x % y;
        x = y;
        y = temp;
    }
    return getAllDivisors(x);
}

std::set<std::int32_t> Divisor::stein(const std::int32_t a, const std::int32_t b)
{
    std::int32_t x = std::abs(a);
    std::int32_t y = std::abs(b);
    std::int32_t gcd = 0;
    std::int32_t c = 0;
    while (isEven(x) && isEven(y))
    {
        x >>= 1;
        y >>= 1;
        ++c;
    }

    if (isEven(x))
    {
        x >>= 1;
        gcd = steinRecursive(x, y) << c;
    }
    else
    {
        gcd = steinRecursive(y, x) << c;
    }
    return getAllDivisors(gcd);
}

bool Divisor::isEven(const std::int32_t n)
{
    return (n & 0b1) == 0;
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

    while (isEven(a))
    {
        a >>= 1;
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
