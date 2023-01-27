//! @file arithmetic.cpp
//! @author ryftchen
//! @brief The definitions (arithmetic) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#include "arithmetic.hpp"
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"

//! @brief Display arithmetic result.
#define ARITHMETIC_RESULT "\r\n*%-14s method:\r\n(%d) %s (%d) = %d\n"
//! @brief Print arithmetic result content.
#define ARITHMETIC_PRINT_RESULT_CONTENT(method, a, operator, b, result) \
    COMMON_PRINT(ARITHMETIC_RESULT, method, a, operator, b, result)
#else

//! @brief Print arithmetic result content.
#define ARITHMETIC_PRINT_RESULT_CONTENT(method, a, operator, b, result)
#endif

namespace numeric::arithmetic
{
int ArithmeticSolution::additionMethod(const int augend, const int addend)
{
    const int sum = bitAdd(augend, addend);

    ARITHMETIC_PRINT_RESULT_CONTENT("Addition", augend, "+", addend, sum);
    return sum;
}

int ArithmeticSolution::subtractionMethod(const int minuend, const int subtrahend)
{
    const int difference = bitAdd(minuend, bitAdd(~subtrahend, 1));

    ARITHMETIC_PRINT_RESULT_CONTENT("Subtraction", minuend, "-", subtrahend, difference);
    return difference;
}

int ArithmeticSolution::multiplicationMethod(const int multiplier, const int multiplicand)
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

    ARITHMETIC_PRINT_RESULT_CONTENT("Multiplication", multiplier, "*", multiplicand, product);
    return product;
}

int ArithmeticSolution::divisionMethod(const int dividend, const int divisor)
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

    ARITHMETIC_PRINT_RESULT_CONTENT("Division", dividend, "/", divisor, quotient);
    return quotient;
}

TargetBuilder::TargetBuilder(const int integer1, const int integer2) : integer1(integer1), integer2(integer2)
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nElementary arithmetic of " << integer1 << " and " << integer2 << ":" << std::endl;
#endif
}
} // namespace numeric::arithmetic
