//! @file arithmetic.cpp
//! @author ryftchen
//! @brief The definitions (arithmetic) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "arithmetic.hpp"

namespace numeric::arithmetic
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::int32_t Arithmetic::addition(const std::int32_t augend, const std::int32_t addend)
{
    return bitAdd(augend, addend);
}

std::int32_t Arithmetic::subtraction(const std::int32_t minuend, const std::int32_t subtrahend)
{
    return bitAdd(minuend, bitAdd(~subtrahend, 1));
}

std::int32_t Arithmetic::multiplication(const std::int32_t multiplier, const std::int32_t multiplicand)
{
    std::int32_t product = 0;
    for (std::int32_t i = (32 - 1); i >= 0; --i)
    {
        product <<= 1;
        if ((multiplicand & (1 << i)) >> i)
        {
            product = bitAdd(product, multiplier);
        }
    }

    return product;
}

std::int32_t Arithmetic::division(const std::int32_t dividend, const std::int32_t divisor)
{
    if (0 == divisor)
    {
        return 0;
    }

    std::int32_t quotient = 0, remainder = 0, absDividend = bitAbs(dividend), absDivisor = bitAbs(divisor);
    for (std::int32_t i = (32 - 1); i >= 0; --i)
    {
        quotient <<= 1;
        remainder <<= 1;
        remainder |= (absDividend & (1 << i)) >> i;
        if (remainder >= absDivisor)
        {
            remainder = bitSub(remainder, absDivisor);
            quotient |= 1;
        }
    }
    if ((dividend ^ divisor) < 0)
    {
        quotient = bitSub(0, quotient);
    }

    return quotient;
}

std::int32_t Arithmetic::bitAdd(const std::int32_t a, const std::int32_t b)
{
    const std::int32_t sum = a ^ b, carry = (a & b) << 1;
    return (sum & carry) ? bitAdd(sum, carry) : (sum ^ carry);
}

std::int32_t Arithmetic::bitSub(const std::int32_t a, const std::int32_t b)
{
    return bitAdd(a, bitAdd(~b, 1));
}

std::int32_t Arithmetic::bitAbs(const std::int32_t a)
{
    const std::int32_t mask = a >> (sizeof(std::int32_t) * 8 - 1);
    return (a ^ mask) - mask;
}
} // namespace numeric::arithmetic
