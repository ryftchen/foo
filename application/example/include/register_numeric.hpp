//! @file register_numeric.hpp
//! @author ryftchen
//! @brief The declarations (register_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <bitset>
#include <functional>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

#include "utility/include/common.hpp"
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
enum ArithmeticMethod : std::uint8_t
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
enum DivisorMethod : std::uint8_t
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
enum IntegralMethod : std::uint8_t
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
enum PrimeMethod : std::uint8_t
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
    enum Category : std::uint8_t
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
    std::bitset<Bottom<ArithmeticMethod>::value> arithmeticOpts{};
    //! @brief Bit flags for managing divisor methods.
    std::bitset<Bottom<DivisorMethod>::value> divisorOpts{};
    //! @brief Bit flags for managing integral methods.
    std::bitset<Bottom<IntegralMethod>::value> integralOpts{};
    //! @brief Bit flags for managing prime methods.
    std::bitset<Bottom<PrimeMethod>::value> primeOpts{};

    //! @brief Check whether any numeric choices do not exist.
    //! @return any numeric choices do not exist or exist
    [[nodiscard]] inline bool empty() const
    {
        return arithmeticOpts.none() && divisorOpts.none() && integralOpts.none() && primeOpts.none();
    }
    //! @brief Reset bit flags that manage numeric choices.
    inline void reset()
    {
        arithmeticOpts.reset();
        divisorOpts.reset();
        integralOpts.reset();
        primeOpts.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - the specific value of Category enum
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
extern ApplyNumeric& manager();

//! @brief Update choice.
//! @tparam T - type of target method
//! @param target - target method
template <typename T>
void updateChoice(const std::string_view target);
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
void updateChoice<ArithmeticMethod>(const std::string_view target);
template <>
void runChoices<ArithmeticMethod>(const std::vector<std::string>& candidates);

//! @brief Register divisor.
namespace divisor
{
extern const char* version() noexcept;
} // namespace divisor
template <>
void updateChoice<DivisorMethod>(const std::string_view target);
template <>
void runChoices<DivisorMethod>(const std::vector<std::string>& candidates);

//! @brief Register integral.
namespace integral
{
extern const char* version() noexcept;
} // namespace integral
template <>
void updateChoice<IntegralMethod>(const std::string_view target);
template <>
void runChoices<IntegralMethod>(const std::vector<std::string>& candidates);

//! @brief Register prime.
namespace prime
{
extern const char* version() noexcept;
} // namespace prime
template <>
void updateChoice<PrimeMethod>(const std::string_view target);
template <>
void runChoices<PrimeMethod>(const std::vector<std::string>& candidates);
} // namespace reg_num
} // namespace application

//! @brief Reflect the numeric category name and alias name to the field in the mapping.
#define REG_NUM_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                         \
    Field                                                                                          \
    {                                                                                              \
        REFLECTION_STR(COMMON_STRINGIFY(category)), &Type::COMMON_CONCAT(category, Opts), AttrList \
        {                                                                                          \
            Attr                                                                                   \
            {                                                                                      \
                REFLECTION_STR("alias"), COMMON_STRINGIFY(alias)                                   \
            }                                                                                      \
        }                                                                                          \
    }
//! @brief Reflect the entry under the numeric category and choice name to the field in the mapping.
#define REG_NUM_REFLECT_SECOND_LEVEL_FIELD(entry, choice)              \
    Field                                                              \
    {                                                                  \
        REFLECTION_STR(COMMON_STRINGIFY(entry)), Type::entry, AttrList \
        {                                                              \
            Attr                                                       \
            {                                                          \
                REFLECTION_STR("choice"), COMMON_STRINGIFY(choice)     \
            }                                                          \
        }                                                              \
    }
//! @brief Static reflection for ApplyNumeric. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_num::ApplyNumeric>
    : TypeInfoBase<application::reg_num::ApplyNumeric>
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
    : TypeInfoBase<application::reg_num::ArithmeticMethod>
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
    : TypeInfoBase<application::reg_num::DivisorMethod>
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
    : TypeInfoBase<application::reg_num::IntegralMethod>
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
    : TypeInfoBase<application::reg_num::PrimeMethod>
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
//! @brief Alias for Category.
using Category = ApplyNumeric::Category;
//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
inline consteval std::string_view toString()
{
    switch (Cat)
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
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
inline constexpr auto& getCategoryOpts()
{
    return std::invoke(TypeInfo<ApplyNumeric>::fields.find(REFLECTION_STR(toString<Cat>())).value, manager());
}
//! @brief Abbreviation value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return abbreviation value
template <typename T>
inline consteval std::size_t abbrVal(const T method)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    std::size_t value = 0;
    TypeInfo<T>::fields.forEach(
        [method, &value](const auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(1 == field.attrs.size);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
            }
        });

    return value;
}

// clang-format off
//! @brief Mapping table for enum and string about arithmetic methods. X macro.
#define REG_NUM_ARITHMETIC_METHOD_TABLE    \
    ELEM(addition      , "addition"      ) \
    ELEM(subtraction   , "subtraction"   ) \
    ELEM(multiplication, "multiplication") \
    ELEM(division      , "division"      )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of ArithmeticMethod enum
//! @return method name
inline constexpr std::string_view toString(const ArithmeticMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {REG_NUM_ARITHMETIC_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<ArithmeticMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef REG_NUM_ARITHMETIC_METHOD_TABLE

// clang-format off
//! @brief Mapping table for enum and string about divisor methods. X macro.
#define REG_NUM_DIVISOR_METHOD_TABLE \
    ELEM(euclidean, "euclidean")     \
    ELEM(stein    , "stein"    )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of DivisorMethod enum
//! @return method name
inline constexpr std::string_view toString(const DivisorMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {REG_NUM_DIVISOR_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<DivisorMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef REG_NUM_DIVISOR_METHOD_TABLE

// clang-format off
//! @brief Mapping table for enum and string about integral methods. X macro.
#define REG_NUM_INTEGRAL_METHOD_TABLE \
    ELEM(trapezoidal, "trapezoidal")  \
    ELEM(simpson    , "simpson"    )  \
    ELEM(romberg    , "romberg"    )  \
    ELEM(gauss      , "gauss"      )  \
    ELEM(monteCarlo , "monteCarlo" )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of IntegralMethod enum
//! @return method name
inline constexpr std::string_view toString(const IntegralMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {REG_NUM_INTEGRAL_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<IntegralMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef REG_NUM_INTEGRAL_METHOD_TABLE

// clang-format off
//! @brief Mapping table for enum and string about prime methods. X macro.
#define REG_NUM_PRIME_METHOD_TABLE     \
    ELEM(eratosthenes, "eratosthenes") \
    ELEM(euler       , "euler"       )
// clang-format on
//! @brief Convert method enumeration to string.
//! @param method - the specific value of PrimeMethod enum
//! @return method name
inline constexpr std::string_view toString(const PrimeMethod method)
{
//! @cond
#define ELEM(val, str) str,
    constexpr std::string_view table[] = {REG_NUM_PRIME_METHOD_TABLE};
    static_assert((sizeof(table) / sizeof(table[0])) == Bottom<PrimeMethod>::value);
    return table[method];
//! @endcond
#undef ELEM
}
#undef REG_NUM_PRIME_METHOD_TABLE
} // namespace application::reg_num
