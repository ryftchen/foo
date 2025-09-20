//! @file register_numeric.hpp
//! @author ryftchen
//! @brief The declarations (register_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <bitset>
#include <functional>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "utility/include/common.hpp"
#include "utility/include/macro.hpp"
#include "utility/include/reflection.hpp"

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Numeric-registering-related functions in the application module.
namespace reg_num
{
extern const char* version() noexcept;

//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <typename T>
struct Bottom;

//! @brief Enumerate specific arithmetic methods.
enum class ArithmeticMethod : std::uint8_t
{
    //! @brief Addition.
    addition,
    //! @brief Subtraction.
    subtraction,
    //! @brief Multiplication.
    multiplication,
    //! @brief Division.
    division
};
//! @brief Store the maximum value of the ArithmeticMethod enum.
template <>
struct Bottom<ArithmeticMethod>
{
    //! @brief Maximum value of the ArithmeticMethod enum.
    static constexpr std::uint8_t value{4};
};

//! @brief Enumerate specific divisor methods.
enum class DivisorMethod : std::uint8_t
{
    //! @brief Euclidean.
    euclidean,
    //! @brief Stein.
    stein
};
//! @brief Store the maximum value of the DivisorMethod enum.
template <>
struct Bottom<DivisorMethod>
{
    //! @brief Maximum value of the DivisorMethod enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Enumerate specific integral methods.
enum class IntegralMethod : std::uint8_t
{
    //! @brief Trapezoidal.
    trapezoidal,
    //! @brief Simpson.
    simpson,
    //! @brief Romberg.
    romberg,
    //! @brief Gauss.
    gauss,
    //! @brief Monte-Carlo.
    monteCarlo
};
//! @brief Store the maximum value of the IntegralMethod enum.
template <>
struct Bottom<IntegralMethod>
{
    //! @brief Maximum value of the IntegralMethod enum.
    static constexpr std::uint8_t value{5};
};

//! @brief Enumerate specific prime methods.
enum class PrimeMethod : std::uint8_t
{
    //! @brief Eratosthenes.
    eratosthenes,
    //! @brief Euler.
    euler
};
//! @brief Store the maximum value of the PrimeMethod enum.
template <>
struct Bottom<PrimeMethod>
{
    //! @brief Maximum value of the PrimeMethod enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Manage numeric choices.
class ApplyNumeric
{
public:
    //! @brief Enumerate specific numeric choices.
    enum class Category : std::uint8_t
    {
        //! @brief Arithmetic.
        arithmetic,
        //! @brief Divisor.
        divisor,
        //! @brief Integral.
        integral,
        //! @brief Prime.
        prime
    };

    //! @brief Bit flags for managing arithmetic methods.
    std::bitset<Bottom<ArithmeticMethod>::value> arithmeticOpts;
    //! @brief Bit flags for managing divisor methods.
    std::bitset<Bottom<DivisorMethod>::value> divisorOpts;
    //! @brief Bit flags for managing integral methods.
    std::bitset<Bottom<IntegralMethod>::value> integralOpts;
    //! @brief Bit flags for managing prime methods.
    std::bitset<Bottom<PrimeMethod>::value> primeOpts;

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - current category
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::arithmetic:
                os << "ARITHMETIC";
                break;
            case Category::divisor:
                os << "DIVISOR";
                break;
            case Category::integral:
                os << "INTEGRAL";
                break;
            case Category::prime:
                os << "PRIME";
                break;
            default:
                os << "UNKNOWN (" << static_cast<std::underlying_type_t<Category>>(cat) << ')';
                break;
        }

        return os;
    }
};

//! @brief Manage the numeric choices.
namespace manage
{
extern ApplyNumeric& choiceApplier();

extern bool present();
extern void clear();
} // namespace manage

//! @brief Update choice.
//! @tparam T - type of target method
//! @param target - target method
template <typename T>
void updateChoice(const std::string& target);
//! @brief Run choices.
//! @tparam T - type of target method
//! @param candidates - container for the candidate target methods
template <typename T>
void runChoices(const std::vector<std::string>& candidates);

//! @brief Register arithmetic.
namespace arithmetic
{
extern const char* version() noexcept;
} // namespace arithmetic
template <>
void updateChoice<ArithmeticMethod>(const std::string& target);
template <>
void runChoices<ArithmeticMethod>(const std::vector<std::string>& candidates);

//! @brief Register divisor.
namespace divisor
{
extern const char* version() noexcept;
} // namespace divisor
template <>
void updateChoice<DivisorMethod>(const std::string& target);
template <>
void runChoices<DivisorMethod>(const std::vector<std::string>& candidates);

//! @brief Register integral.
namespace integral
{
extern const char* version() noexcept;
} // namespace integral
template <>
void updateChoice<IntegralMethod>(const std::string& target);
template <>
void runChoices<IntegralMethod>(const std::vector<std::string>& candidates);

//! @brief Register prime.
namespace prime
{
extern const char* version() noexcept;
} // namespace prime
template <>
void updateChoice<PrimeMethod>(const std::string& target);
template <>
void runChoices<PrimeMethod>(const std::vector<std::string>& candidates);
} // namespace reg_num
} // namespace application

//! @brief Reflect the numeric category name and alias name to the field in the mapping.
#define REG_NUM_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                       \
    Field                                                                                        \
    {                                                                                            \
        REFLECTION_STR(MACRO_STRINGIFY(category)), &Type::MACRO_CONCAT(category, Opts), AttrList \
        {                                                                                        \
            Attr                                                                                 \
            {                                                                                    \
                REFLECTION_STR("alias"), MACRO_STRINGIFY(alias)                                  \
            }                                                                                    \
        }                                                                                        \
    }
//! @brief Reflect the entry under the numeric category and choice name to the field in the mapping.
#define REG_NUM_REFLECT_SECOND_LEVEL_FIELD(entry, choice)             \
    Field                                                             \
    {                                                                 \
        REFLECTION_STR(MACRO_STRINGIFY(entry)), Type::entry, AttrList \
        {                                                             \
            Attr                                                      \
            {                                                         \
                REFLECTION_STR("choice"), MACRO_STRINGIFY(choice)     \
            }                                                         \
        }                                                             \
    }
//! @brief Static reflection for ApplyNumeric. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_num::ApplyNumeric>
    : public TypeInfoImpl<application::reg_num::ApplyNumeric>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-num"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_NUM_REFLECT_FIRST_LEVEL_FIELD(arithmetic, a),
        REG_NUM_REFLECT_FIRST_LEVEL_FIELD(divisor   , d),
        REG_NUM_REFLECT_FIRST_LEVEL_FIELD(integral  , i),
        REG_NUM_REFLECT_FIRST_LEVEL_FIELD(prime     , p),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply numeric"}};
};
//! @brief Static reflection for ArithmeticMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_num::ArithmeticMethod>
    : public TypeInfoImpl<application::reg_num::ArithmeticMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"arithmetic"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(addition      , add),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(subtraction   , sub),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(multiplication, mul),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(division      , div),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "arithmetic-related choices\n"
        "- add    Addition\n"
        "- sub    Subtraction\n"
        "- mul    Multiplication\n"
        "- div    Division\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for DivisorMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_num::DivisorMethod>
    : public TypeInfoImpl<application::reg_num::DivisorMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"divisor"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(euclidean, euc),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(stein    , ste),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "divisor-related choices\n"
        "- euc    Euclidean\n"
        "- ste    Stein\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for IntegralMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_num::IntegralMethod>
    : public TypeInfoImpl<application::reg_num::IntegralMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"integral"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(trapezoidal, tra),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(simpson    , sim),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(romberg    , rom),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(gauss      , gau),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(monteCarlo , mon),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "integral-related choices\n"
        "- tra    Trapezoidal\n"
        "- sim    Adaptive Simpson's 1/3\n"
        "- rom    Romberg\n"
        "- gau    Gauss-Legendre's 5-Points\n"
        "- mon    Monte-Carlo\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for PrimeMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_num::PrimeMethod>
    : public TypeInfoImpl<application::reg_num::PrimeMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"prime"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(eratosthenes, era),
        REG_NUM_REFLECT_SECOND_LEVEL_FIELD(euler       , eul),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "prime-related choices\n"
        "- era    Eratosthenes\n"
        "- eul    Euler\n"
        "add the choices listed above"}};
};
#undef REG_NUM_REFLECT_FIRST_LEVEL_FIELD
#undef REG_NUM_REFLECT_SECOND_LEVEL_FIELD

namespace application::reg_num
{
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for the category.
using Category = ApplyNumeric::Category;
//! @brief Convert category enumeration to string.
//! @param cat - target category
//! @return category name
consteval std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::arithmetic:
            return TypeInfo<ArithmeticMethod>::name;
        case Category::divisor:
            return TypeInfo<DivisorMethod>::name;
        case Category::integral:
            return TypeInfo<IntegralMethod>::name;
        case Category::prime:
            return TypeInfo<PrimeMethod>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in numeric choices.
//! @tparam Cat - target category
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& categoryOpts()
{
    return std::invoke(
        TypeInfo<ApplyNumeric>::fields.find(REFLECTION_STR(toString(Cat))).value, manage::choiceApplier());
}
//! @brief The literal hash value of the abbreviation for the candidate method.
//! @tparam T - type of candidate method
//! @param method - candidate method
//! @return literal hash value
template <typename T>
consteval std::size_t abbrLitHash(const T method)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    constexpr auto refl = REFLECTION_STR("choice");
    std::size_t value = 0;
    TypeInfo<T>::fields.findIf(
        [refl, method, &value](const auto field)
        {
            if (field.name == TypeInfo<T>::fields.nameOfValue(method))
            {
                static_assert(field.attrs.contains(refl) && (field.attrs.size == 1));
                const auto attr = field.attrs.find(refl);
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
                return true;
            }
            return false;
        });

    return value;
}
} // namespace application::reg_num
