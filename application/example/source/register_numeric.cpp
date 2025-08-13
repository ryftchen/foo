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

//! @brief Check whether any numeric choices exist.
//! @return any numeric choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyNumeric>::forEachVarOf(
        manager(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });

    return isExist;
}

//! @brief Reset bit flags that manage numeric choices.
void clear()
{
    TypeInfo<ApplyNumeric>::forEachVarOf(manager(), [](const auto /*field*/, auto&& var) { var.reset(); });
}

//! @brief Find the position of bit flags to set a particular method.
//! @tparam T - type of target method
//! @param stringify - method name
//! @return position of bit flags
template <typename T>
static consteval std::size_t findPosition(const std::string_view stringify)
{
    return static_cast<std::size_t>(TypeInfo<T>::fields.template valueOfName<T>(stringify));
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
void updateChoice<ArithmeticMethod>(const std::string& target)
{
    constexpr auto category = Category::arithmetic;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(ArithmeticMethod::addition):
            bits.set(findPosition<ArithmeticMethod>(MACRO_STRINGIFY(addition)));
            break;
        case abbrValue(ArithmeticMethod::subtraction):
            bits.set(findPosition<ArithmeticMethod>(MACRO_STRINGIFY(subtraction)));
            break;
        case abbrValue(ArithmeticMethod::multiplication):
            bits.set(findPosition<ArithmeticMethod>(MACRO_STRINGIFY(multiplication)));
            break;
        case abbrValue(ArithmeticMethod::division):
            bits.set(findPosition<ArithmeticMethod>(MACRO_STRINGIFY(division)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
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
void updateChoice<DivisorMethod>(const std::string& target)
{
    constexpr auto category = Category::divisor;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(DivisorMethod::euclidean):
            bits.set(findPosition<DivisorMethod>(MACRO_STRINGIFY(euclidean)));
            break;
        case abbrValue(DivisorMethod::stein):
            bits.set(findPosition<DivisorMethod>(MACRO_STRINGIFY(stein)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
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
void updateChoice<IntegralMethod>(const std::string& target)
{
    constexpr auto category = Category::integral;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(IntegralMethod::trapezoidal):
            bits.set(findPosition<IntegralMethod>(MACRO_STRINGIFY(trapezoidal)));
            break;
        case abbrValue(IntegralMethod::simpson):
            bits.set(findPosition<IntegralMethod>(MACRO_STRINGIFY(simpson)));
            break;
        case abbrValue(IntegralMethod::romberg):
            bits.set(findPosition<IntegralMethod>(MACRO_STRINGIFY(romberg)));
            break;
        case abbrValue(IntegralMethod::gauss):
            bits.set(findPosition<IntegralMethod>(MACRO_STRINGIFY(gauss)));
            break;
        case abbrValue(IntegralMethod::monteCarlo):
            bits.set(findPosition<IntegralMethod>(MACRO_STRINGIFY(monteCarlo)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
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
void updateChoice<PrimeMethod>(const std::string& target)
{
    constexpr auto category = Category::prime;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(target.c_str()))
    {
        case abbrValue(PrimeMethod::eratosthenes):
            bits.set(findPosition<PrimeMethod>(MACRO_STRINGIFY(eratosthenes)));
            break;
        case abbrValue(PrimeMethod::euler):
            bits.set(findPosition<PrimeMethod>(MACRO_STRINGIFY(euler)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString<category>()} + " method: " + target + '.'};
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
