#pragma once

#include <utility>
namespace numeric::arithmetic
{
class ArithmeticSolution
{
public:
    ArithmeticSolution(const int integer1, const int integer2);
    virtual ~ArithmeticSolution() = default;

    [[nodiscard]] int additionMethod(const int augend, const int addend) const;
    [[nodiscard]] int subtractionMethod(const int minuend, const int subtrahend) const;
    [[nodiscard]] int multiplicationMethod(const int multiplier, const int multiplicand) const;
    [[nodiscard]] int divisionMethod(const int dividend, const int divisor) const;

    [[nodiscard]] inline std::pair<int, int> getIntegers() const;

private:
    const int integer1;
    const int integer2;

    static inline int bitAdd(const int a, const int b);
    static inline int bitSub(const int a, const int b);
    static inline int bitAbs(const int a);
};

inline std::pair<int, int> ArithmeticSolution::getIntegers() const
{
    return std::make_pair(integer1, integer2);
}

inline int ArithmeticSolution::bitAdd(const int a, const int b)
{
    const int sum = a ^ b, carry = (a & b) << 1;
    return ((sum & carry) ? bitAdd(sum, carry) : (sum ^ carry));
}

inline int ArithmeticSolution::bitSub(const int a, const int b)
{
    return bitAdd(a, bitAdd(~b, 1));
}

inline int ArithmeticSolution::bitAbs(const int a)
{
    const int mask = a >> (sizeof(int) * 8 - 1);
    return ((a ^ mask) - mask);
}
} // namespace numeric::arithmetic
