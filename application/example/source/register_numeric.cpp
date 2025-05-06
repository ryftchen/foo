//! @file register_numeric.cpp
//! @author ryftchen
//! @brief The definitions (register_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "register_numeric.hpp"
#include "apply_numeric.hpp"

namespace application::reg_num
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

//! @brief Get the numeric choice manager.
//! @return reference of the ApplyNumeric object
ApplyNumeric& manager() noexcept
{
    static ApplyNumeric manager{};
    return manager;
}

namespace arithmetic
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_num::arithmetic::version;
}
} // namespace arithmetic
//! @brief Update arithmetic-related choice.
//! @param target - target method
template <>
void updateChoice<ArithmeticMethod>(const std::string_view target)
{
    constexpr auto category = Category::arithmetic;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrValue(ArithmeticMethod::addition):
            bits.set(ArithmeticMethod::addition);
            break;
        case abbrValue(ArithmeticMethod::subtraction):
            bits.set(ArithmeticMethod::subtraction);
            break;
        case abbrValue(ArithmeticMethod::multiplication):
            bits.set(ArithmeticMethod::multiplication);
            break;
        case abbrValue(ArithmeticMethod::division):
            bits.set(ArithmeticMethod::division);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}
//! @brief Run arithmetic-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<ArithmeticMethod>(const std::vector<std::string>& candidates)
{
    app_num::applyingArithmetic(candidates);
}

namespace divisor
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_num::divisor::version;
}
} // namespace divisor
//! @brief Update divisor-related choice.
//! @param target - target method
template <>
void updateChoice<DivisorMethod>(const std::string_view target)
{
    constexpr auto category = Category::divisor;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrValue(DivisorMethod::euclidean):
            bits.set(DivisorMethod::euclidean);
            break;
        case abbrValue(DivisorMethod::stein):
            bits.set(DivisorMethod::stein);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}
//! @brief Run divisor-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<DivisorMethod>(const std::vector<std::string>& candidates)
{
    app_num::applyingDivisor(candidates);
}

namespace integral
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_num::integral::version;
}
} // namespace integral
//! @brief Update integral-related choice.
//! @param target - target method
template <>
void updateChoice<IntegralMethod>(const std::string_view target)
{
    constexpr auto category = Category::integral;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrValue(IntegralMethod::trapezoidal):
            bits.set(IntegralMethod::trapezoidal);
            break;
        case abbrValue(IntegralMethod::simpson):
            bits.set(IntegralMethod::simpson);
            break;
        case abbrValue(IntegralMethod::romberg):
            bits.set(IntegralMethod::romberg);
            break;
        case abbrValue(IntegralMethod::gauss):
            bits.set(IntegralMethod::gauss);
            break;
        case abbrValue(IntegralMethod::monteCarlo):
            bits.set(IntegralMethod::monteCarlo);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}
//! @brief Run integral-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<IntegralMethod>(const std::vector<std::string>& candidates)
{
    app_num::applyingIntegral(candidates);
}

namespace prime
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    return app_num::prime::version;
}
} // namespace prime
//! @brief Update prime-related choice.
//! @param target - target method
template <>
void updateChoice<PrimeMethod>(const std::string_view target)
{
    constexpr auto category = Category::prime;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.data()))
    {
        case abbrValue(PrimeMethod::eratosthenes):
            bits.set(PrimeMethod::eratosthenes);
            break;
        case abbrValue(PrimeMethod::euler):
            bits.set(PrimeMethod::euler);
            break;
        default:
            bits.reset();
            throw std::logic_error{
                "Unexpected " + std::string{toString<category>()} + " method: " + target.data() + '.'};
    }
}
//! @brief Run prime-related choices.
//! @param candidates - container for the candidate target methods
template <>
void runChoices<PrimeMethod>(const std::vector<std::string>& candidates)
{
    app_num::applyingPrime(candidates);
}
} // namespace application::reg_num
