//! @file prime.cpp
//! @author ryftchen
//! @brief The definitions (prime) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "prime.hpp"

namespace numeric::prime
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version()
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::vector<std::uint32_t> Prime::eratosthenes(const std::uint32_t max)
{
    std::vector<std::uint32_t> container(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime[0] = false;
    isPrime[1] = false;
    for (std::uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime[i])
        {
            for (std::uint32_t j = (i * i); j <= max; j += i)
            {
                isPrime[j] = false;
            }
            container.emplace_back(i);
        }
    }

    return container;
}

std::vector<std::uint32_t> Prime::euler(const std::uint32_t max)
{
    std::vector<std::uint32_t> container(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime[0] = false;
    isPrime[1] = false;
    for (std::uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime[i])
        {
            container.emplace_back(i);
        }

        for (std::uint32_t j = 1; (j <= container.size()) && ((i * container[j - 1]) <= max); ++j)
        {
            isPrime[i * container[j - 1]] = false;
            if (0 == (i % container[j - 1]))
            {
                break;
            }
        }
    }

    return container;
}
} // namespace numeric::prime
