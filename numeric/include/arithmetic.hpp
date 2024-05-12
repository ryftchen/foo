//! @file arithmetic.hpp
//! @author ryftchen
//! @brief The declarations (arithmetic) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

//! @brief The numeric module.
namespace numeric // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Arithmetic-related functions in the numeric module.
namespace arithmetic
{
extern const char* version();

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
    static int bitAdd(const int a, const int b);
    //! @brief Bitwise operation for subtract.
    //! @param a - minuend
    //! @param b - subtrahend
    //! @return difference
    static int bitSub(const int a, const int b);
    //! @brief Bitwise operation for absolute value.
    //! @param a - value
    //! @return absolute value
    static int bitAbs(const int a);
};
} // namespace arithmetic
} // namespace numeric
