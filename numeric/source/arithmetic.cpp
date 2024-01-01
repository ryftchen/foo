//! @file arithmetic.cpp
//! @author ryftchen
//! @brief The definitions (arithmetic) in the numeric module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "arithmetic.hpp"

namespace numeric::arithmetic
{
int Arithmetic::addition(const int augend, const int addend)
{
    const int sum = bitAdd(augend, addend);

    return sum;
}

int Arithmetic::subtraction(const int minuend, const int subtrahend)
{
    const int difference = bitAdd(minuend, bitAdd(~subtrahend, 1));

    return difference;
}

int Arithmetic::multiplication(const int multiplier, const int multiplicand)
{
    int product = 0;

    for (int i = (32 - 1); i >= 0; --i)
    {
        product <<= 1;
        if ((multiplicand & (1 << i)) >> i)
        {
            product = bitAdd(product, multiplier);
        }
    }

    return product;
}

int Arithmetic::division(const int dividend, const int divisor)
{
    if (0 == divisor)
    {
        return 0;
    }

    int quotient = 0, remainder = 0, absDividend = bitAbs(dividend), absDivisor = bitAbs(divisor);
    for (int i = (32 - 1); i >= 0; --i)
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

int Arithmetic::bitAdd(const int a, const int b)
{
    const int sum = a ^ b, carry = (a & b) << 1;
    return ((sum & carry) ? bitAdd(sum, carry) : (sum ^ carry));
}

int Arithmetic::bitSub(const int a, const int b)
{
    return bitAdd(a, bitAdd(~b, 1));
}

int Arithmetic::bitAbs(const int a)
{
    const int mask = a >> (sizeof(int) * 8 - 1);
    return ((a ^ mask) - mask);
}
} // namespace numeric::arithmetic
