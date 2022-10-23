#include "sieve.hpp"
#include <bitset>
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

#define SIEVE_RESULT "\r\n*%-9s method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define SIEVE_PRINT_RESULT_CONTENT(method)                                      \
    do                                                                          \
    {                                                                           \
        const uint32_t arrayBufferSize = sieveVector.size() * maxAlignOfPrint;  \
        char arrayBuffer[arrayBufferSize + 1];                                  \
        arrayBuffer[0] = '\0';                                                  \
        COMMON_PRINT(                                                           \
            SIEVE_RESULT,                                                       \
            method,                                                             \
            formatIntegerVector(sieveVector, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                             \
    }                                                                           \
    while (0)

namespace num_sieve
{
Sieve::Sieve()
{
    std::cout << "\r\nAll prime numbers less than " << maxPositiveInteger << ":" << std::endl;
}

// Eratosthenes
std::vector<uint32_t> Sieve::eratosthenesMethod(const uint32_t max) const
{
    TIME_BEGIN(timing);
    std::vector<uint32_t> sieveVector(0);
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
            sieveVector.emplace_back(i);
        }
    }

    TIME_END(timing);
    SIEVE_PRINT_RESULT_CONTENT("Eratosthenes");
    return sieveVector;
}

// Euler
std::vector<uint32_t> Sieve::eulerMethod(const uint32_t max) const
{
    TIME_BEGIN(timing);
    std::vector<uint32_t> sieveVector(0);
    std::vector<bool> isPrime(max + 1, true);

    isPrime.at(0) = false;
    isPrime.at(1) = false;
    for (uint32_t i = 2; i <= max; ++i)
    {
        if (isPrime.at(i))
        {
            sieveVector.emplace_back(i);
        }

        for (uint32_t j = 1; (j <= sieveVector.size()) && ((i * sieveVector.at(j - 1)) <= max); ++j)
        {
            isPrime.at(static_cast<uint32_t>(i * sieveVector.at(j - 1))) = false;
            if (0 == (i % sieveVector.at(j - 1)))
            {
                break;
            }
        }
    }

    TIME_END(timing);
    SIEVE_PRINT_RESULT_CONTENT("Euler");
    return sieveVector;
}
} // namespace num_sieve
