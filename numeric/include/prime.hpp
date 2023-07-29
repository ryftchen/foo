//! @file prime.hpp
//! @author ryftchen
//! @brief The declarations (prime) in the numeric module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>

//! @brief Prime-related functions in the numeric module.
namespace numeric::prime
{
//! @brief Prime methods.
class Prime
{
public:
    //! @brief Destroy the Prime object.
    virtual ~Prime() = default;

    //! @brief Eratosthenes.
    //! @param max - maximum positive integer
    //! @return all prime numbers that are not greater than the maximum positive integer
    static std::vector<std::uint32_t> eratosthenes(const std::uint32_t max);
    //! @brief Euler.
    //! @param max - maximum positive integer
    //! @return all prime numbers that are not greater than the maximum positive integer
    static std::vector<std::uint32_t> euler(const std::uint32_t max);
};
} // namespace numeric::prime
