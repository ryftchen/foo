//! @file divisor.hpp
//! @author ryftchen
//! @brief The declarations (divisor) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Divisor-related functions in the numeric module.
namespace divisor
{
extern const char* version() noexcept;

//! @brief Divisor methods.
class Divisor
{
public:
    //! @brief Destroy the Divisor object.
    virtual ~Divisor() = default;

    //! @brief Euclidean.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::vector<std::int32_t> euclidean(std::int32_t a, std::int32_t b);
    //! @brief Stein.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::vector<std::int32_t> stein(std::int32_t a, std::int32_t b);

private:
    //! @brief Recursive for the Stein method.
    //! @param a - first integer
    //! @param b - second integer
    //! @return greatest common divisor
    static std::int32_t steinRecursive(std::int32_t a, std::int32_t b);
    //! @brief Get all common divisors by the greatest common divisor.
    //! @param gcd - greatest common divisor
    //! @return all common divisors
    static std::vector<std::int32_t> getAllDivisors(const std::int32_t gcd);
};
} // namespace divisor
} // namespace numeric
