#include "prime.hpp"
#include <bitset>
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

#define PRIME_RESULT "\r\n*%-9s method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define PRIME_PRINT_RESULT_CONTENT(method)                                      \
    do                                                                          \
    {                                                                           \
        const uint32_t arrayBufferSize = primeVector.size() * maxAlignOfPrint;  \
        char arrayBuffer[arrayBufferSize + 1];                                  \
        arrayBuffer[0] = '\0';                                                  \
        COMMON_PRINT(                                                           \
            PRIME_RESULT,                                                       \
            method,                                                             \
            formatIntegerVector(primeVector, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                             \
    }                                                                           \
    while (0)
#define PRIME_RUNTIME_BEGIN TIME_BEGIN(timing)
#define PRIME_RUNTIME_END TIME_END(timing)
#define PRIME_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else

#define PRIME_PRINT_RESULT_CONTENT(method)
#define PRIME_RUNTIME_BEGIN
#define PRIME_RUNTIME_END
#endif

namespace numeric::prime
{
PrimeSolution::PrimeSolution(const uint32_t maxPositiveInteger) : maxPositiveInteger(maxPositiveInteger)
{
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nAll prime numbers less than " << maxPositiveInteger << ":" << std::endl;
#endif
}

// Eratosthenes
std::vector<uint32_t> PrimeSolution::eratosthenesMethod(const uint32_t max) const
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

// Euler
std::vector<uint32_t> PrimeSolution::eulerMethod(const uint32_t max) const
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
} // namespace numeric::prime
