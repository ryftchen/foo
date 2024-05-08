//! @file divisor.hpp
//! @author ryftchen
//! @brief The declarations (divisor) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <vector>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Divisor-related functions in the numeric module.
namespace divisor
{
extern const char* version();

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
    static std::vector<int> euclidean(int a, int b);
    //! @brief Stein.
    //! @param a - first integer
    //! @param b - second integer
    //! @return all common divisors of two integers
    static std::vector<int> stein(int a, int b);

private:
    //! @brief Recursive for the Stein method.
    //! @param a - first integer
    //! @param b - second integer
    //! @return greatest common divisor
    static int steinRecursive(int a, int b);
    //! @brief Get all common divisors by the greatest common divisor.
    //! @param greatestCommonDivisor - greatest common divisor
    //! @return all common divisors
    static std::vector<int> getAllDivisors(const int greatestCommonDivisor);
};
} // namespace divisor
} // namespace numeric
