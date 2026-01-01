//! @file prime.cpp
//! @author ryftchen
//! @brief The definitions (prime) in the numeric module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "prime.hpp"

namespace numeric::prime
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

std::vector<std::uint32_t> Prime::eratosthenes(const std::uint32_t limit)
{
    std::vector<std::uint32_t> storage{};
    std::vector<bool> isPrime(limit + 1, true);

    isPrime[0] = false;
    isPrime[1] = false;
    for (std::uint32_t i = 2; i <= limit; ++i)
    {
        if (isPrime[i])
        {
            for (std::uint32_t j = i * i; j <= limit; j += i)
            {
                isPrime[j] = false;
            }
            storage.emplace_back(i);
        }
    }
    return storage;
}

std::vector<std::uint32_t> Prime::euler(const std::uint32_t limit)
{
    std::vector<std::uint32_t> storage{};
    std::vector<bool> isPrime(limit + 1, true);

    isPrime[0] = false;
    isPrime[1] = false;
    for (std::uint32_t i = 2; i <= limit; ++i)
    {
        if (isPrime[i])
        {
            storage.emplace_back(i);
        }

        for (std::uint32_t j = 1; (j <= storage.size()) && ((i * storage[j - 1]) <= limit); ++j)
        {
            const std::uint32_t composite = i * storage[j - 1];
            isPrime[composite] = false;
            if ((i % storage[j - 1]) == 0)
            {
                break;
            }
        }
    }
    return storage;
}
} // namespace numeric::prime
