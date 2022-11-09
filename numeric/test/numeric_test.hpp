#pragma once

#include <bitset>
#include <sstream>
#include <vector>

namespace num_tst
{
class NumericTask
{
public:
    template <class T>
    struct Bottom;

    enum Type
    {
        arithmetic,
        divisor,
        integral,
        prime
    };

    enum ArithmeticMethod
    {
        addition,
        subtraction,
        multiplication,
        division
    };
    template <>
    struct Bottom<ArithmeticMethod>
    {
        static constexpr int value = 4;
    };

    enum DivisorMethod
    {
        euclidean,
        stein
    };
    template <>
    struct Bottom<DivisorMethod>
    {
        static constexpr int value = 2;
    };

    enum IntegralMethod
    {
        trapezoidal,
        simpson,
        romberg,
        gauss,
        monteCarlo
    };
    template <>
    struct Bottom<IntegralMethod>
    {
        static constexpr int value = 5;
    };

    enum PrimeMethod
    {
        eratosthenes,
        euler
    };
    template <>
    struct Bottom<PrimeMethod>
    {
        static constexpr int value = 2;
    };

    std::bitset<Bottom<ArithmeticMethod>::value> arithmeticBit;
    std::bitset<Bottom<DivisorMethod>::value> divisorBit;
    std::bitset<Bottom<IntegralMethod>::value> integralBit;
    std::bitset<Bottom<PrimeMethod>::value> primeBit;

    [[nodiscard]] bool empty() const
    {
        return (arithmeticBit.none() && divisorBit.none() && integralBit.none() && primeBit.none());
    }
    void reset()
    {
        arithmeticBit.reset();
        divisorBit.reset();
        integralBit.reset();
        primeBit.reset();
    }

protected:
    friend std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        switch (type)
        {
            case Type::arithmetic:
                os << "ARITHMETIC";
                break;
            case Type::divisor:
                os << "DIVISOR";
                break;
            case Type::integral:
                os << "INTEGRAL";
                break;
            case Type::prime:
                os << "PRIME";
                break;
            default:
                os << "UNKNOWN: " << static_cast<std::underlying_type_t<Type>>(type);
        }
        return os;
    }
};

extern NumericTask& getTask();

template <typename T>
auto getBit()
{
    if constexpr (std::is_same_v<T, NumericTask::ArithmeticMethod>)
    {
        return getTask().arithmeticBit;
    }
    else if constexpr (std::is_same_v<T, NumericTask::DivisorMethod>)
    {
        return getTask().divisorBit;
    }
    else if constexpr (std::is_same_v<T, NumericTask::IntegralMethod>)
    {
        return getTask().integralBit;
    }
    else if constexpr (std::is_same_v<T, NumericTask::PrimeMethod>)
    {
        return getTask().primeBit;
    }
}

template <typename T>
void setBit(const int index)
{
    if constexpr (std::is_same_v<T, NumericTask::ArithmeticMethod>)
    {
        getTask().arithmeticBit.set(NumericTask::ArithmeticMethod(index));
    }
    else if constexpr (std::is_same_v<T, NumericTask::DivisorMethod>)
    {
        getTask().divisorBit.set(NumericTask::DivisorMethod(index));
    }
    else if constexpr (std::is_same_v<T, NumericTask::IntegralMethod>)
    {
        getTask().integralBit.set(NumericTask::IntegralMethod(index));
    }
    else if constexpr (std::is_same_v<T, NumericTask::PrimeMethod>)
    {
        getTask().primeBit.set(NumericTask::PrimeMethod(index));
    }
}

extern void runArithmetic(const std::vector<std::string>& targets);
extern void updateArithmeticTask(const std::string& target);
extern void runDivisor(const std::vector<std::string>& targets);
extern void updateDivisorTask(const std::string& target);
extern void runIntegral(const std::vector<std::string>& targets);
extern void updateIntegralTask(const std::string& target);
extern void runPrime(const std::vector<std::string>& targets);
extern void updatePrimeTask(const std::string& target);
} // namespace num_tst
