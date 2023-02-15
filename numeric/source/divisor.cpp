//! @file divisor.cpp
//! @author ryftchen
//! @brief The definitions (divisor) in the numeric module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "divisor.hpp"
#ifndef __PRECOMPILED_HEADER
#endif
#ifdef __RUNTIME_PRINTING
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

//! @brief Display divisor result.
#define DIVISOR_RESULT "\r\n*%-9s method:\r\n%s\r\n==>Run time: %8.5f ms\n"
//! @brief Print divisor result content.
#define DIVISOR_PRINT_RESULT_CONTENT(method)                                                                   \
    do                                                                                                         \
    {                                                                                                          \
        const uint32_t arrayBufferSize = divisorVector.size() * maxAlignOfPrint;                               \
        char arrayBuffer[arrayBufferSize + 1];                                                                 \
        arrayBuffer[0] = '\0';                                                                                 \
        COMMON_PRINT(                                                                                          \
            DIVISOR_RESULT,                                                                                    \
            method,                                                                                            \
            TargetBuilder::template formatIntegerVector<int>(divisorVector, arrayBuffer, arrayBufferSize + 1), \
            DIVISOR_RUNTIME_INTERVAL);                                                                         \
    }                                                                                                          \
    while (0)
//! @brief Store divisor beginning runtime.
#define DIVISOR_RUNTIME_BEGIN TIME_BEGIN(timing)
//! @brief Store divisor ending runtime.
#define DIVISOR_RUNTIME_END TIME_END(timing)
//! @brief Calculate divisor runtime interval.
#define DIVISOR_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else
#include <cmath>

//! @brief Print divisor result content.
#define DIVISOR_PRINT_RESULT_CONTENT(method)
//! @brief Store divisor beginning runtime.
#define DIVISOR_RUNTIME_BEGIN
//! @brief Store divisor ending runtime.
#define DIVISOR_RUNTIME_END
#endif

namespace numeric::divisor
{
std::vector<int> DivisorSolution::euclideanMethod(int a, int b)
{
    DIVISOR_RUNTIME_BEGIN;
    a = std::abs(a);
    b = std::abs(b);

    while (b)
    {
        int temp = a % b;
        a = b;
        b = temp;
    }
    const int greatestCommonDivisor = a;
    std::vector<int> divisorVector = getAllDivisors(greatestCommonDivisor);

    DIVISOR_RUNTIME_END;
    DIVISOR_PRINT_RESULT_CONTENT("Euclidean");
    return divisorVector;
}

std::vector<int> DivisorSolution::steinMethod(int a, int b)
{
    DIVISOR_RUNTIME_BEGIN;
    int greatestCommonDivisor = 0, c = 0;
    a = std::abs(a);
    b = std::abs(b);

    while ((0 == (a & 0x1)) && (0 == (b & 0x1)))
    {
        a = a >> 1;
        b = b >> 1;
        ++c;
    }
    if (0 == (a & 0x1))
    {
        a = a >> 1;
        greatestCommonDivisor = steinRecursive(a, b) << c;
    }
    else
    {
        greatestCommonDivisor = steinRecursive(b, a) << c;
    }
    std::vector<int> divisorVector = getAllDivisors(greatestCommonDivisor);

    DIVISOR_RUNTIME_END;
    DIVISOR_PRINT_RESULT_CONTENT("Stein");
    return divisorVector;
}

int DivisorSolution::steinRecursive(int a, int b)
{
    if (0 == a)
    {
        return b;
    }
    if (0 == b)
    {
        return a;
    }

    while (0 == (a & 0x1))
    {
        a = a >> 1;
    }
    if (a < b)
    {
        b = (b - a) >> 1;
        return steinRecursive(b, a);
    }
    else
    {
        a = (a - b) >> 1;
        return steinRecursive(a, b);
    }
}

std::vector<int> DivisorSolution::getAllDivisors(const int greatestCommonDivisor)
{
    std::vector<int> divisors(0);
    for (int i = 1; i <= std::sqrt(greatestCommonDivisor); ++i)
    {
        if (0 == (greatestCommonDivisor % i))
        {
            divisors.emplace_back(i);
            if ((greatestCommonDivisor / i) != i)
            {
                divisors.emplace_back(greatestCommonDivisor / i);
            }
        }
    }
    std::sort(divisors.begin(), divisors.end());
    return divisors;
}

TargetBuilder::TargetBuilder(const int integer1, const int integer2) : integer1(integer1), integer2(integer2)
{
#ifdef __RUNTIME_PRINTING
    std::cout << "\r\nAll common divisors of " << integer1 << " and " << integer2 << ":" << std::endl;
#endif
}
} // namespace numeric::divisor
