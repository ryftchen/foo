//! @file arithmetic.hpp
//! @author ryftchen
//! @brief The declarations (arithmetic) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#pragma once

#include <utility>

//! @brief Arithmetic-related functions in the numeric module.
namespace numeric::arithmetic
{
//! @brief Solution of arithmetic.
class ArithmeticSolution
{
public:
    //! @brief Destroy the ArithmeticSolution object.
    virtual ~ArithmeticSolution() = default;

    //! @brief The addition method.
    //! @param augend - augend of addition
    //! @param addend - augend of addition
    //! @return sum
    static int additionMethod(const int augend, const int addend);
    //! @brief The subtraction method.
    //! @param minuend - minuend of subtraction
    //! @param subtrahend - subtrahend of subtraction
    //! @return difference
    static int subtractionMethod(const int minuend, const int subtrahend);
    //! @brief The multiplication method.
    //! @param multiplier - multiplier of multiplication
    //! @param multiplicand - multiplicand of multiplication
    //! @return product
    static int multiplicationMethod(const int multiplier, const int multiplicand);
    //! @brief The division method.
    //! @param dividend - dividend of division
    //! @param divisor - divisor of division
    //! @return quotient
    static int divisionMethod(const int dividend, const int divisor);

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

inline int ArithmeticSolution::bitAdd(const int a, const int b)
{
    const int sum = a ^ b, carry = (a & b) << 1;
    return ((sum & carry) ? bitAdd(sum, carry) : (sum ^ carry));
}

inline int ArithmeticSolution::bitSub(const int a, const int b)
{
    return bitAdd(a, bitAdd(~b, 1));
}

inline int ArithmeticSolution::bitAbs(const int a)
{
    const int mask = a >> (sizeof(int) * 8 - 1);
    return ((a ^ mask) - mask);
}

//! @brief Builder for the target.
class TargetBuilder
{
public:
    //! @brief Construct a new TargetBuilder object.
    //! @param integer1 - first integer for elementary arithmetic
    //! @param integer2 - second integer for elementary arithmetic
    TargetBuilder(const int integer1, const int integer2);
    //! @brief Destroy the TargetBuilder object.
    virtual ~TargetBuilder() = default;

    //! @brief Get the pair of integers.
    //! @return pair of integers
    [[nodiscard]] inline std::pair<int, int> getIntegers() const;

private:
    //! @brief First integer for elementary arithmetic.
    const int integer1;
    //! @brief Second integer for elementary arithmetic.
    const int integer2;
};

inline std::pair<int, int> TargetBuilder::getIntegers() const
{
    return std::make_pair(integer1, integer2);
}
} // namespace numeric::arithmetic
