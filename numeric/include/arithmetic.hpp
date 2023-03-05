//! @file arithmetic.hpp
//! @author ryftchen
//! @brief The declarations (arithmetic) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#ifndef __PRECOMPILED_HEADER
#else
#include "pch_numeric.hpp"
#endif

//! @brief Arithmetic-related functions in the numeric module.
namespace numeric::arithmetic
{
//! @brief Arithmetic methods.
class Arithmetic
{
public:
    //! @brief Destroy the Arithmetic object.
    virtual ~Arithmetic() = default;

    //! @brief Addition.
    //! @param augend - augend of addition
    //! @param addend - augend of addition
    //! @return sum
    static int addition(const int augend, const int addend);
    //! @brief Subtraction.
    //! @param minuend - minuend of subtraction
    //! @param subtrahend - subtrahend of subtraction
    //! @return difference
    static int subtraction(const int minuend, const int subtrahend);
    //! @brief Multiplication.
    //! @param multiplier - multiplier of multiplication
    //! @param multiplicand - multiplicand of multiplication
    //! @return product
    static int multiplication(const int multiplier, const int multiplicand);
    //! @brief Division.
    //! @param dividend - dividend of division
    //! @param divisor - divisor of division
    //! @return quotient
    static int division(const int dividend, const int divisor);

private:
    //! @brief Bitwise operation for add.
    //! @param a - augend
    //! @param b - augend
    //! @return sum
    static inline int bitAdd(const int a, const int b);
    //! @brief Bitwise operation for subtract.
    //! @param a - minuend
    //! @param b - subtrahend
    //! @return difference
    static inline int bitSub(const int a, const int b);
    //! @brief Bitwise operation for absolute value.
    //! @param a - value
    //! @return absolute value
    static inline int bitAbs(const int a);
};

inline int Arithmetic::bitAdd(const int a, const int b)
{
    const int sum = a ^ b, carry = (a & b) << 1;
    return ((sum & carry) ? bitAdd(sum, carry) : (sum ^ carry));
}

inline int Arithmetic::bitSub(const int a, const int b)
{
    return bitAdd(a, bitAdd(~b, 1));
}

inline int Arithmetic::bitAbs(const int a)
{
    const int mask = a >> (sizeof(int) * 8 - 1);
    return ((a ^ mask) - mask);
}
} // namespace numeric::arithmetic
