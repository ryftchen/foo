//! @file prime.cpp
//! @author ryftchen
//! @brief The definitions (prime) in the numeric module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023

#include "prime.hpp"

namespace numeric::prime
{
std::vector<std::uint32_t> Prime::eratosthenes(const std::uint32_t max)
{
    std::vector<std::uint32_t> primeVector(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime.at(0) = false;
    isPrime.at(1) = false;
    for (std::uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime.at(i))
        {
            for (std::uint32_t j = (i * i); j <= max; j += i)
            {
                isPrime.at(j) = false;
            }
            primeVector.emplace_back(i);
        }
    }

    return primeVector;
}

std::vector<std::uint32_t> Prime::euler(const std::uint32_t max)
{
    std::vector<std::uint32_t> primeVector(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime.at(0) = false;
    isPrime.at(1) = false;
    for (std::uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime.at(i))
        {
            primeVector.emplace_back(i);
        }

        for (std::uint32_t j = 1; (j <= primeVector.size()) && ((i * primeVector.at(j - 1)) <= max); ++j)
        {
            isPrime.at(static_cast<std::uint32_t>(i * primeVector.at(j - 1))) = false;
            if (0 == (i % primeVector.at(j - 1)))
            {
                break;
            }
        }
    }

    return primeVector;
}
} // namespace numeric::prime
