#pragma once

namespace num_arithmetic
{
inline constexpr int integer1 = 1073741823;
inline constexpr int integer2 = -2;

class Arithmetic
{
public:
    Arithmetic();
    virtual ~Arithmetic() = default;

    [[nodiscard]] static int additionMethod(const int augend, const int addend);
    [[nodiscard]] static int subtractionMethod(const int minuend, const int subtrahend);
    [[nodiscard]] static int multiplicationMethod(const int multiplier, const int multiplicand);
    [[nodiscard]] static int divisionMethod(const int dividend, const int divisor);

private:
    static int inline bitAdd(const int a, const int b);
    static int inline bitSub(const int a, const int b);
    static int inline bitAbs(const int a);
};

int inline Arithmetic::bitAdd(const int a, const int b)
{
    const int sum = a ^ b, carry = (a & b) << 1;
    return ((sum & carry) ? bitAdd(sum, carry) : (sum ^ carry));
}

int inline Arithmetic::bitSub(const int a, const int b)
{
    return bitAdd(a, bitAdd(~b, 1));
}

int inline Arithmetic::bitAbs(const int a)
{
    const int mask = a >> (sizeof(int) * 8 - 1);
    return ((a ^ mask) - mask);
}
} // namespace num_arithmetic