//! @file prime.cpp
//! @author ryftchen
//! @brief The definitions (prime) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022
#include "prime.hpp"
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

//! @brief Display prime result.
#define PRIME_RESULT "\r\n*%-9s method:\r\n%s\r\n==>Run time: %8.5f ms\n"
//! @brief Print prime result content.
#define PRIME_PRINT_RESULT_CONTENT(method)                                                                        \
    do                                                                                                            \
    {                                                                                                             \
        const uint32_t arrayBufferSize = primeVector.size() * maxAlignOfPrint;                                    \
        char arrayBuffer[arrayBufferSize + 1];                                                                    \
        arrayBuffer[0] = '\0';                                                                                    \
        COMMON_PRINT(                                                                                             \
            PRIME_RESULT,                                                                                         \
            method,                                                                                               \
            TargetBuilder::template formatIntegerVector<uint32_t>(primeVector, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                                                               \
    }                                                                                                             \
    while (0)
//! @brief Store prime beginning runtime.
#define PRIME_RUNTIME_BEGIN TIME_BEGIN(timing)
//! @brief Store prime ending runtime.
#define PRIME_RUNTIME_END TIME_END(timing)
//! @brief Calculate prime runtime interval.
#define PRIME_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else

//! @brief Print prime result content.
#define PRIME_PRINT_RESULT_CONTENT(method)
//! @brief Store prime beginning runtime.
#define PRIME_RUNTIME_BEGIN
//! @brief Store prime ending runtime.
#define PRIME_RUNTIME_END
#endif

namespace numeric::prime
{
std::vector<uint32_t> PrimeSolution::eratosthenesMethod(const uint32_t max)
{
    PRIME_RUNTIME_BEGIN;
    std::vector<uint32_t> primeVector(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime.at(0) = false;
    isPrime.at(1) = false;
    for (uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime.at(i))
        {
            for (uint32_t j = (i * i); j <= max; j += i)
            {
                isPrime.at(j) = false;
            }
            primeVector.emplace_back(i);
        }
    }

    PRIME_RUNTIME_END;
    PRIME_PRINT_RESULT_CONTENT("Eratosthenes");
    return primeVector;
}

std::vector<uint32_t> PrimeSolution::eulerMethod(const uint32_t max)
{
    PRIME_RUNTIME_BEGIN;
    std::vector<uint32_t> primeVector(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime.at(0) = false;
    isPrime.at(1) = false;
    for (uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime.at(i))
        {
            primeVector.emplace_back(i);
        }

        for (uint32_t j = 1; (j <= primeVector.size()) && ((i * primeVector.at(j - 1)) <= max); ++j)
        {
            isPrime.at(static_cast<uint32_t>(i * primeVector.at(j - 1))) = false;
            if (0 == (i % primeVector.at(j - 1)))
            {
                break;
            }
        }
    }

    PRIME_RUNTIME_END;
    PRIME_PRINT_RESULT_CONTENT("Euler");
    return primeVector;
}

TargetBuilder::TargetBuilder(const uint32_t maxPositiveInteger) : maxPositiveInteger(maxPositiveInteger)
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nAll prime numbers less than " << maxPositiveInteger << ":" << std::endl;
#endif
}
} // namespace numeric::prime
