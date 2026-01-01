//! @file divisor.hpp
//! @author ryftchen
//! @brief The declarations (divisor) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <set>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Divisor-related functions in the numeric module.
namespace divisor
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "NUM_DIVISOR";
}
extern const char* version() noexcept;

//! @brief Divisor methods.
class Divisor
{
public:
    //! @brief Euclidean.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::set<std::int32_t> euclidean(const std::int32_t a, const std::int32_t b);
    //! @brief Stein.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::set<std::int32_t> stein(const std::int32_t a, const std::int32_t b);

private:
    //! @brief Check whether the integer is even.
    //! @param n - integer to check
    //! @return be even or not
    static bool isEven(const std::int32_t n);
    //! @brief Recursion for the Stein method.
    //! @param a - first integer
    //! @param b - second integer
    //! @return greatest common divisor
    static std::int32_t steinRecursive(std::int32_t a, std::int32_t b);
    //! @brief Get all common divisors by the greatest common divisor.
    //! @param gcd - greatest common divisor
    //! @return all common divisors
    static std::set<std::int32_t> getAllDivisors(const std::int32_t gcd);
};
} // namespace divisor
} // namespace numeric
