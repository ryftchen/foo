#include "divisor.hpp"
#include "file.hpp"
#include "time.hpp"

#define DIVISOR_RESULT "\r\n*%-9s method:\r\n%s\r\n==>Run time: %8.5f ms\n"
#define DIVISOR_PRINT_RESULT_CONTENT(method)                                      \
    do                                                                            \
    {                                                                             \
        const uint32_t arrayBufferSize = divisorVector.size() * maxAlignOfPrint;  \
        char arrayBuffer[arrayBufferSize + 1];                                    \
        arrayBuffer[0] = '\0';                                                    \
        FORMAT_PRINT(                                                             \
            DIVISOR_RESULT,                                                       \
            method,                                                               \
            formatIntegerVector(divisorVector, arrayBuffer, arrayBufferSize + 1), \
            TIME_INTERVAL(timing));                                               \
    }                                                                             \
    while (0)

namespace num_divisor
{
Divisor::Divisor()
{
    std::cout << "\r\nAll common divisors of " << integer1 << " and " << integer2 << ":" << std::endl;
}

std::vector<int> Divisor::getAllDivisors(const int greatestCommonDivisor)
{
    std::vector<int> divisor;
    for (int i = 1; i <= std::sqrt(greatestCommonDivisor); ++i)
    {
        if (0 == greatestCommonDivisor % i)
        {
            divisor.emplace_back(i);
            if ((greatestCommonDivisor / i) != i)
            {
                divisor.emplace_back(greatestCommonDivisor / i);
            }
        }
    }
    std::sort(divisor.begin(), divisor.end());
    return divisor;
}

// Euclid method
std::vector<int> Divisor::euclidMethod(int a, int b) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
    DIVISOR_PRINT_RESULT_CONTENT("Euclid");
    return divisorVector;
}

// Stein method
std::vector<int> Divisor::steinMethod(int a, int b) const
{
    TIME_BEGIN(timing);
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

    TIME_END(timing);
    DIVISOR_PRINT_RESULT_CONTENT("Stein");
    return divisorVector;
}

int Divisor::steinRecursive(int a, int b)
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
} // namespace num_divisor