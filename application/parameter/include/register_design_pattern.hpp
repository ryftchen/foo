//! @file register_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (register_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "application/core/include/configure.hpp"
#include "application/example/include/apply_design_pattern.hpp"
#include "utility/include/common.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/reflection.hpp"

//! @brief Reflect the design pattern category name and alias name to the field in the mapping.
#define REG_DP_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                          \
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
//! @brief Reflect the entry under the design pattern category and choice name to the field in the mapping.
#define REG_DP_REFLECT_SECOND_LEVEL_FIELD(entry, choice)               \
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

//! @brief Static reflection for ApplyDesignPattern. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_dp::ApplyDesignPattern>
    : TypeInfoBase<application::app_dp::ApplyDesignPattern>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-dp"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DP_REFLECT_FIRST_LEVEL_FIELD(behavioral, b),
        REG_DP_REFLECT_FIRST_LEVEL_FIELD(creational, c),
        REG_DP_REFLECT_FIRST_LEVEL_FIELD(structural, s),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply design pattern"}};
};
//! @brief Static reflection for BehavioralInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_dp::BehavioralInstance>
    : TypeInfoBase<application::app_dp::BehavioralInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"behavioral"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(chainOfResponsibility, cha),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(command              , com),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(interpreter          , int),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(iterator             , ite),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(mediator             , med),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(memento              , mem),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(observer             , obs),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(state                , sta),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(strategy             , str),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(templateMethod       , tem),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(visitor              , vis),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "behavioral-related choices\n"
        "- cha    Chain Of Responsibility\n"
        "- com    Command\n"
        "- int    Interpreter\n"
        "- ite    Iterator\n"
        "- med    Mediator\n"
        "- mem    Memento\n"
        "- obs    Observer\n"
        "- sta    State\n"
        "- str    Strategy\n"
        "- tem    Template Method\n"
        "- vis    Visitor\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for CreationalInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_dp::CreationalInstance>
    : TypeInfoBase<application::app_dp::CreationalInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"creational"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(abstractFactory, abs),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(builder        , bui),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(factoryMethod  , fac),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(prototype      , pro),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(singleton      , sin),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "creational-related choices\n"
        "- abs    Abstract Factory\n"
        "- bui    Builder\n"
        "- fac    Factory Method\n"
        "- pro    Prototype\n"
        "- sin    Singleton\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for StructuralInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_dp::StructuralInstance>
    : TypeInfoBase<application::app_dp::StructuralInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"structural"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(adapter  , ada),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(bridge   , bri),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(composite, com),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(decorator, dec),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(facade   , fac),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(flyweight, fly),
        REG_DP_REFLECT_SECOND_LEVEL_FIELD(proxy    , pro),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "structural-related choices\n"
        "- ada    Adapter\n"
        "- bri    Bridge\n"
        "- com    Composite\n"
        "- dec    Decorator\n"
        "- fac    Facade\n"
        "- fly    Flyweight\n"
        "- pro    Proxy\n"
        "add the choices listed above"}};
};

#undef REG_DP_REFLECT_FIRST_LEVEL_FIELD
#undef REG_DP_REFLECT_SECOND_LEVEL_FIELD

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Design-pattern-registering-related functions in the application module.
namespace reg_dp
{
extern const char* version() noexcept;

//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = app_dp::ApplyDesignPattern::Category;

//! @brief Get the task name curried.
//! @return task name curried
inline auto taskNameCurried()
{
    return utility::currying::curry(configure::task::presetName, TypeInfo<app_dp::ApplyDesignPattern>::name);
}
//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
inline consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::behavioral:
            return TypeInfo<app_dp::BehavioralInstance>::name;
        case Category::creational:
            return TypeInfo<app_dp::CreationalInstance>::name;
        case Category::structural:
            return TypeInfo<app_dp::StructuralInstance>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in design pattern choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
inline constexpr auto& getCategoryOpts()
{
    return std::invoke(
        TypeInfo<app_dp::ApplyDesignPattern>::fields.find(REFLECTION_STR(toString<Cat>())).value, app_dp::manager());
}
//! @brief Get the alias of the category in design pattern choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
inline consteval std::string_view getCategoryAlias()
{
    constexpr auto attr = TypeInfo<app_dp::ApplyDesignPattern>::fields.find(REFLECTION_STR(toString<Cat>()))
                              .attrs.find(REFLECTION_STR("alias"));
    static_assert(attr.hasValue);
    return attr.value;
}
//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <typename T>
inline consteval std::size_t abbrVal(const T instance)
{
    static_assert(app_dp::Bottom<T>::value == TypeInfo<T>::fields.size);
    std::size_t value = 0;
    TypeInfo<T>::fields.forEach(
        [instance, &value](const auto field)
        {
            if (field.name == toString(instance))
            {
                static_assert(1 == field.attrs.size);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
            }
        });

    return value;
}
} // namespace reg_dp
} // namespace application
