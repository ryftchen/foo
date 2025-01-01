//! @file arithmetic.hpp
//! @author ryftchen
//! @brief The declarations (arithmetic) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Arithmetic-related functions in the numeric module.
namespace arithmetic
{
extern const char* version() noexcept;

//! @brief Arithmetic methods.
class Arithmetic
{
public:
    //! @brief Destroy the Arithmetic object.
    virtual ~Arithmetic() = default;

    //! @brief Addition.
    //! @param augend - augend of addition
    //! @param addend - addend of addition
    //! @return sum
    static std::int32_t addition(const std::int32_t augend, const std::int32_t addend);
    //! @brief Subtraction.
    //! @param minuend - minuend of subtraction
    //! @param subtrahend - subtrahend of subtraction
    //! @return difference
    static std::int32_t subtraction(const std::int32_t minuend, const std::int32_t subtrahend);
    //! @brief Multiplication.
    //! @param multiplier - multiplier of multiplication
    //! @param multiplicand - multiplicand of multiplication
    //! @return product
    static std::int32_t multiplication(const std::int32_t multiplier, const std::int32_t multiplicand);
    //! @brief Division.
    //! @param dividend - dividend of division
    //! @param divisor - divisor of division
    //! @return quotient
    static std::int32_t division(const std::int32_t dividend, const std::int32_t divisor);

private:
    //! @brief Bitwise operation for add.
    //! @param a - augend
    //! @param b - augend
    //! @return sum
    static std::int32_t bitAdd(const std::int32_t a, const std::int32_t b);
    //! @brief Bitwise operation for subtract.
    //! @param a - minuend
    //! @param b - subtrahend
    //! @return difference
    static std::int32_t bitSub(const std::int32_t a, const std::int32_t b);
    //! @brief Bitwise operation for absolute value.
    //! @param a - value
    //! @return absolute value
    static std::int32_t bitAbs(const std::int32_t a);
};
} // namespace arithmetic
} // namespace numeric
