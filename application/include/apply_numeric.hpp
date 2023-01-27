//! @file apply_numeric.hpp
//! @author ryftchen
//! @brief The declarations (apply_numeric) in the application module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023
#pragma once

#include <bitset>
#include <sstream>
#include <vector>

//! @brief Numeric-applying-related functions in the application module.
namespace application::app_num
{
//! @brief Manage numeric tasks.
class NumericTask
{
public:
    //! @brief Represent the maximum value of an enum.
    //! @tparam T - type of specific enum
    template <class T>
    struct Bottom;

    //! @brief Enumerate specific numeric tasks.
    enum Type
    {
        arithmetic,
        divisor,
        integral,
        prime
    };

    //! @brief Enumerate specific arithmetic methods.
    enum ArithmeticMethod
    {
        addition,
        subtraction,
        multiplication,
        division
    };
    //! @brief Store the maximum value of the ArithmeticMethod enum.
    template <>
    struct Bottom<ArithmeticMethod>
    {
        static constexpr int value = 4;
    };

    //! @brief Enumerate specific divisor methods.
    enum DivisorMethod
    {
        euclidean,
        stein
    };
    //! @brief Store the maximum value of the DivisorMethod enum.
    template <>
    struct Bottom<DivisorMethod>
    {
        static constexpr int value = 2;
    };

    //! @brief Enumerate specific integral methods.
    enum IntegralMethod
    {
        trapezoidal,
        simpson,
        romberg,
        gauss,
        monteCarlo
    };
    //! @brief Store the maximum value of the IntegralMethod enum.
    template <>
    struct Bottom<IntegralMethod>
    {
        static constexpr int value = 5;
    };

    //! @brief Enumerate specific prime methods.
    enum PrimeMethod
    {
        eratosthenes,
        euler
    };
    //! @brief Store the maximum value of the PrimeMethod enum.
    template <>
    struct Bottom<PrimeMethod>
    {
        static constexpr int value = 2;
    };

    //! @brief Bit flags for managing arithmetic methods.
    std::bitset<Bottom<ArithmeticMethod>::value> arithmeticBit;
    //! @brief Bit flags for managing divisor methods.
    std::bitset<Bottom<DivisorMethod>::value> divisorBit;
    //! @brief Bit flags for managing integral methods.
    std::bitset<Bottom<IntegralMethod>::value> integralBit;
    //! @brief Bit flags for managing prime methods.
    std::bitset<Bottom<PrimeMethod>::value> primeBit;

    //! @brief Check whether any numeric tasks do not exist.
    //! @return any numeric tasks do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return (arithmeticBit.none() && divisorBit.none() && integralBit.none() && primeBit.none());
    }
    //! @brief Reset bit flags that manage numeric tasks.
    inline void reset()
    {
        arithmeticBit.reset();
        divisorBit.reset();
        integralBit.reset();
        primeBit.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Type enum.
    //! @param os - output stream object
    //! @param type - the specific value of Type enum
    //! @return reference of output stream object
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

//! @brief Get the bit flags of the method in numeric tasks.
//! @tparam T - type of the method
//! @return bit flags of the method
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

//! @brief Set the bit flags of the method in numeric tasks
//! @tparam T - type of the method
//! @param index - method index
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
} // namespace application::app_num
