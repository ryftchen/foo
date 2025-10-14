//! @file prime.hpp
//! @author ryftchen
//! @brief The declarations (prime) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

//! @brief The numeric module.
namespace numeric // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Prime-related functions in the numeric module.
namespace prime
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "NUM_PRIME";
}
extern const char* version() noexcept;

//! @brief Prime methods.
class Prime
{
public:
    //! @brief Eratosthenes.
    //! @param limit - maximum positive integer
    //! @return all prime numbers that are not greater than the maximum positive integer
    static std::vector<std::uint32_t> eratosthenes(const std::uint32_t limit);
    //! @brief Euler.
    //! @param limit - maximum positive integer
    //! @return all prime numbers that are not greater than the maximum positive integer
    static std::vector<std::uint32_t> euler(const std::uint32_t limit);
};
} // namespace prime
} // namespace numeric
