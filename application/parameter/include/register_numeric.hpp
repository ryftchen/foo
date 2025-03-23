//! @file apply_numeric.hpp
//! @author ryftchen
//! @brief The declarations (apply_numeric) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "application/core/include/configure.hpp"
#include "application/example/include/apply_numeric.hpp"
#include "utility/include/common.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/reflection.hpp"

//! @brief Reflect the numeric category name and alias name to the field in the command line argument mapping.
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
//! @brief Reflect the numeric category entry and choice name to the field in the command line argument mapping.
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
struct utility::reflection::TypeInfo<application::app_num::ApplyNumeric>
    : TypeInfoBase<application::app_num::ApplyNumeric>
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
struct utility::reflection::TypeInfo<application::app_num::ArithmeticMethod>
    : TypeInfoBase<application::app_num::ArithmeticMethod>
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
struct utility::reflection::TypeInfo<application::app_num::DivisorMethod>
    : TypeInfoBase<application::app_num::DivisorMethod>
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
struct utility::reflection::TypeInfo<application::app_num::IntegralMethod>
    : TypeInfoBase<application::app_num::IntegralMethod>
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
struct utility::reflection::TypeInfo<application::app_num::PrimeMethod>
    : TypeInfoBase<application::app_num::PrimeMethod>
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

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Numeric-registering-related functions in the application module.
namespace reg_num
{
extern const char* version() noexcept;

//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = app_num::ApplyNumeric::Category;

//! @brief Get the task name curried.
//! @return task name curried
inline auto taskNameCurried()
{
    return utility::currying::curry(configure::task::presetName, TypeInfo<app_num::ApplyNumeric>::name);
}
//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
inline consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::arithmetic:
            return TypeInfo<app_num::ArithmeticMethod>::name;
        case Category::divisor:
            return TypeInfo<app_num::DivisorMethod>::name;
        case Category::integral:
            return TypeInfo<app_num::IntegralMethod>::name;
        case Category::prime:
            return TypeInfo<app_num::PrimeMethod>::name;
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
    return std::invoke(
        TypeInfo<app_num::ApplyNumeric>::fields.find(REFLECTION_STR(toString<Cat>())).value, app_num::manager());
}
//! @brief Get the alias of the category in numeric choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
inline consteval std::string_view getCategoryAlias()
{
    constexpr auto attr = TypeInfo<app_num::ApplyNumeric>::fields.find(REFLECTION_STR(toString<Cat>()))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}
//! @brief Abbreviation value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return abbreviation value
template <typename T>
inline consteval std::size_t abbrVal(const T method)
{
    static_assert(app_num::Bottom<T>::value == TypeInfo<T>::fields.size);
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
} // namespace reg_num
} // namespace application
