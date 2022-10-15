#include "arithmetic.hpp"
#include "utility/include/file.hpp"

#define ARITHMETIC_RESULT "\r\n*%-14s method:\r\n(%d) %s (%d) = %d\n"

namespace num_arithmetic
{
Arithmetic::Arithmetic()
{
    std::cout << "\r\nElementary arithmetic of " << integer1 << " and " << integer2 << ":" << std::endl;
}

// Addition method
int Arithmetic::additionMethod(const int augend, const int addend)
{
    const int sum = bitAdd(augend, addend);

    FORMAT_PRINT(ARITHMETIC_RESULT, "Addition", augend, "+", addend, sum);
    return sum;
}

// Subtraction method
int Arithmetic::subtractionMethod(const int minuend, const int subtrahend)
{
    const int difference = bitAdd(minuend, bitAdd(~subtrahend, 1));

    FORMAT_PRINT(ARITHMETIC_RESULT, "Subtraction", minuend, "-", subtrahend, difference);
    return difference;
}

// Multiplication method
int Arithmetic::multiplicationMethod(const int multiplier, const int multiplicand)
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

    FORMAT_PRINT(ARITHMETIC_RESULT, "Multiplication", multiplier, "*", multiplicand, product);
    return product;
}

// Division method
int Arithmetic::divisionMethod(const int dividend, const int divisor)
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

    FORMAT_PRINT(ARITHMETIC_RESULT, "Division", dividend, "/", divisor, quotient);
    return quotient;
}
} // namespace num_arithmetic
