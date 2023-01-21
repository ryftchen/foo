#include "divisor.hpp"
#ifndef _NO_PRINT_AT_RUNTIME
#include "utility/include/common.hpp"
#include "utility/include/time.hpp"

#define DIVISOR_RESULT "\r\n*%-9s method:\r\n%s\r\n==>Run time: %8.5f ms\n"
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
#define DIVISOR_RUNTIME_BEGIN TIME_BEGIN(timing)
#define DIVISOR_RUNTIME_END TIME_END(timing)
#define DIVISOR_RUNTIME_INTERVAL TIME_INTERVAL(timing)
#else
#include <cmath>

#define DIVISOR_PRINT_RESULT_CONTENT(method)
#define DIVISOR_RUNTIME_BEGIN
#define DIVISOR_RUNTIME_END
#endif

namespace numeric::divisor
{
// Euclidean
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

// Stein
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
#ifndef _NO_PRINT_AT_RUNTIME
    std::cout << "\r\nAll common divisors of " << integer1 << " and " << integer2 << ":" << std::endl;
#endif
}
} // namespace numeric::divisor
