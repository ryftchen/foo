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

namespace manage
{
//! @brief Get the numeric choice applier.
//! @return reference of the ApplyNumeric object
ApplyNumeric& choiceApplier()
{
    static ApplyNumeric applier{};
    return applier;
}

//! @brief Check whether any numeric choices exist.
//! @return any numeric choices exist or not
bool present()
{
    bool isExist = false;
    TypeInfo<ApplyNumeric>::forEachVarOf(
        choiceApplier(), [&isExist](const auto /*field*/, auto&& var) { isExist |= !var.none(); });
    return isExist;
}
//! @brief Reset bit flags that manage numeric choices.
void clear()
{
    TypeInfo<ApplyNumeric>::forEachVarOf(choiceApplier(), [](const auto /*field*/, auto&& var) { var.reset(); });
}
} // namespace manage

//! @brief Find the position of bit flags to set a particular method.
//! @tparam T - type of target method
//! @param stringify - method name
//! @return position of bit flags
template <typename T>
static consteval std::size_t mappedPos(const std::string_view stringify)
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
//! @brief Set arithmetic-related choice.
//! @param choice - target choice
template <>
void setChoice<ArithmeticMethod>(const std::string& choice)
{
    constexpr auto category = Category::arithmetic;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(ArithmeticMethod::addition):
            bits.set(mappedPos<ArithmeticMethod>(MACRO_STRINGIFY(addition)));
            break;
        case abbrLitHash(ArithmeticMethod::subtraction):
            bits.set(mappedPos<ArithmeticMethod>(MACRO_STRINGIFY(subtraction)));
            break;
        case abbrLitHash(ArithmeticMethod::multiplication):
            bits.set(mappedPos<ArithmeticMethod>(MACRO_STRINGIFY(multiplication)));
            break;
        case abbrLitHash(ArithmeticMethod::division):
            bits.set(mappedPos<ArithmeticMethod>(MACRO_STRINGIFY(division)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run arithmetic-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<ArithmeticMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set divisor-related choice.
//! @param choice - target choice
template <>
void setChoice<DivisorMethod>(const std::string& choice)
{
    constexpr auto category = Category::divisor;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(DivisorMethod::euclidean):
            bits.set(mappedPos<DivisorMethod>(MACRO_STRINGIFY(euclidean)));
            break;
        case abbrLitHash(DivisorMethod::stein):
            bits.set(mappedPos<DivisorMethod>(MACRO_STRINGIFY(stein)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run divisor-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<DivisorMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set integral-related choice.
//! @param choice - target choice
template <>
void setChoice<IntegralMethod>(const std::string& choice)
{
    constexpr auto category = Category::integral;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(IntegralMethod::trapezoidal):
            bits.set(mappedPos<IntegralMethod>(MACRO_STRINGIFY(trapezoidal)));
            break;
        case abbrLitHash(IntegralMethod::simpson):
            bits.set(mappedPos<IntegralMethod>(MACRO_STRINGIFY(simpson)));
            break;
        case abbrLitHash(IntegralMethod::romberg):
            bits.set(mappedPos<IntegralMethod>(MACRO_STRINGIFY(romberg)));
            break;
        case abbrLitHash(IntegralMethod::gauss):
            bits.set(mappedPos<IntegralMethod>(MACRO_STRINGIFY(gauss)));
            break;
        case abbrLitHash(IntegralMethod::monteCarlo):
            bits.set(mappedPos<IntegralMethod>(MACRO_STRINGIFY(monteCarlo)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run integral-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<IntegralMethod>(const std::vector<std::string>& candidates)
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
//! @brief Set prime-related choice.
//! @param choice - target choice
template <>
void setChoice<PrimeMethod>(const std::string& choice)
{
    constexpr auto category = Category::prime;
    auto& bits = categoryOpts<category>();

    switch (utility::common::bkdrHash(choice.c_str()))
    {
        case abbrLitHash(PrimeMethod::eratosthenes):
            bits.set(mappedPos<PrimeMethod>(MACRO_STRINGIFY(eratosthenes)));
            break;
        case abbrLitHash(PrimeMethod::euler):
            bits.set(mappedPos<PrimeMethod>(MACRO_STRINGIFY(euler)));
            break;
        default:
            bits.reset();
            throw std::logic_error{"Unexpected " + std::string{toString(category)} + " choice: " + choice + '.'};
    }
}
//! @brief Run prime-related candidates.
//! @param candidates - container for the candidate target choices
template <>
void runCandidates<PrimeMethod>(const std::vector<std::string>& candidates)
{
    app_num::applyingPrime(candidates);
}
} // namespace application::reg_num
