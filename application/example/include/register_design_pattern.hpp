//! @file register_design_pattern.hpp
//! @author ryftchen
//! @brief The declarations (register_design_pattern) in the application module.
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
//! @brief Design-pattern-registering-related functions in the application module.
namespace reg_dp
{
extern const char* version() noexcept;

//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <typename T>
struct Bottom;

//! @brief Enumerate specific behavioral instances.
enum class BehavioralInstance : std::uint8_t
{
    //! @brief Chain of responsibility.
    chainOfResponsibility,
    //! @brief Command.
    command,
    //! @brief Interpreter.
    interpreter,
    //! @brief Iterator.
    iterator,
    //! @brief Mediator.
    mediator,
    //! @brief Memento.
    memento,
    //! @brief Observer.
    observer,
    //! @brief State.
    state,
    //! @brief Strategy.
    strategy,
    //! @brief Template method.
    templateMethod,
    //! @brief Visitor.
    visitor
};
//! @brief Store the maximum value of the BehavioralInstance enum.
template <>
struct Bottom<BehavioralInstance>
{
    //! @brief Maximum value of the BehavioralInstance enum.
    static constexpr std::uint8_t value{11};
};

//! @brief Enumerate specific creational instances.
enum class CreationalInstance : std::uint8_t
{
    //! @brief Abstract factory.
    abstractFactory,
    //! @brief Builder.
    builder,
    //! @brief Factory method.
    factoryMethod,
    //! @brief Prototype.
    prototype,
    //! @brief Singleton.
    singleton
};
//! @brief Store the maximum value of the CreationalInstance enum.
template <>
struct Bottom<CreationalInstance>
{
    //! @brief Maximum value of the CreationalInstance enum.
    static constexpr std::uint8_t value{5};
};

//! @brief Enumerate specific structural instances.
enum class StructuralInstance : std::uint8_t
{
    //! @brief Adapter.
    adapter,
    //! @brief Bridge.
    bridge,
    //! @brief Composite.
    composite,
    //! @brief Decorator.
    decorator,
    //! @brief Facade.
    facade,
    //! @brief Flyweight.
    flyweight,
    //! @brief Proxy.
    proxy
};
//! @brief Store the maximum value of the StructuralInstance enum.
template <>
struct Bottom<StructuralInstance>
{
    //! @brief Maximum value of the StructuralInstance enum.
    static constexpr std::uint8_t value{7};
};

//! @brief Manage design pattern choices.
class ApplyDesignPattern
{
public:
    //! @brief Enumerate specific design pattern choices.
    enum class Category : std::uint8_t
    {
        //! @brief Behavioral.
        behavioral,
        //! @brief Creational.
        creational,
        //! @brief Structural.
        structural
    };

    //! @brief Bit flags for managing behavioral instances.
    std::bitset<Bottom<BehavioralInstance>::value> behavioralOpts;
    //! @brief Bit flags for managing creational instances.
    std::bitset<Bottom<CreationalInstance>::value> creationalOpts;
    //! @brief Bit flags for managing structural instances.
    std::bitset<Bottom<StructuralInstance>::value> structuralOpts;

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - specific value of Category enum
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::behavioral:
                os << "BEHAVIORAL";
                break;
            case Category::creational:
                os << "CREATIONAL";
                break;
            case Category::structural:
                os << "STRUCTURAL";
                break;
            default:
                os << "UNKNOWN (" << static_cast<std::underlying_type_t<Category>>(cat) << ')';
                break;
        }

        return os;
    }
};

//! @brief Manage the design pattern choices.
namespace manage
{
extern ApplyDesignPattern& applier();

extern bool present();
extern void clear();
} // namespace manage

//! @brief Update choice.
//! @tparam T - type of target instance
//! @param target - target instance
template <typename T>
void updateChoice(const std::string& target);
//! @brief Run choices.
//! @tparam T - type of target instance
//! @param candidates - container for the candidate target instances
template <typename T>
void runChoices(const std::vector<std::string>& candidates);

//! @brief Register behavioral.
namespace behavioral
{
extern const char* version() noexcept;
} // namespace behavioral
template <>
void updateChoice<BehavioralInstance>(const std::string& target);
template <>
void runChoices<BehavioralInstance>(const std::vector<std::string>& candidates);

//! @brief Register creational.
namespace creational
{
extern const char* version() noexcept;
} // namespace creational
template <>
void updateChoice<CreationalInstance>(const std::string& target);
template <>
void runChoices<CreationalInstance>(const std::vector<std::string>& candidates);

//! @brief Register structural.
namespace structural
{
extern const char* version() noexcept;
} // namespace structural
template <>
void updateChoice<StructuralInstance>(const std::string& target);
template <>
void runChoices<StructuralInstance>(const std::vector<std::string>& candidates);
} // namespace reg_dp
} // namespace application

//! @brief Reflect the design pattern category name and alias name to the field in the mapping.
#define REG_DP_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                        \
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
//! @brief Reflect the entry under the design pattern category and choice name to the field in the mapping.
#define REG_DP_REFLECT_SECOND_LEVEL_FIELD(entry, choice)              \
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
//! @brief Static reflection for ApplyDesignPattern. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_dp::ApplyDesignPattern>
    : public TypeInfoImpl<application::reg_dp::ApplyDesignPattern>
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
struct utility::reflection::TypeInfo<application::reg_dp::BehavioralInstance>
    : public TypeInfoImpl<application::reg_dp::BehavioralInstance>
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
struct utility::reflection::TypeInfo<application::reg_dp::CreationalInstance>
    : public TypeInfoImpl<application::reg_dp::CreationalInstance>
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
struct utility::reflection::TypeInfo<application::reg_dp::StructuralInstance>
    : public TypeInfoImpl<application::reg_dp::StructuralInstance>
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

namespace application::reg_dp
{
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = ApplyDesignPattern::Category;
//! @brief Convert category enumeration to string.
//! @param cat - specific value of Category enum
//! @return category name
consteval std::string_view toString(const Category cat)
{
    switch (cat)
    {
        case Category::behavioral:
            return TypeInfo<BehavioralInstance>::name;
        case Category::creational:
            return TypeInfo<CreationalInstance>::name;
        case Category::structural:
            return TypeInfo<StructuralInstance>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in design pattern choices.
//! @tparam Cat - specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& categoryOpts()
{
    return std::invoke(
        TypeInfo<ApplyDesignPattern>::fields.find(REFLECTION_STR(toString(Cat))).value, manage::applier());
}
//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <typename T>
consteval std::size_t abbrValue(const T instance)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    constexpr auto refl = REFLECTION_STR("choice");
    std::size_t value = 0;
    TypeInfo<T>::fields.findIf(
        [refl, instance, &value](const auto field)
        {
            if (field.name == TypeInfo<T>::fields.nameOfValue(instance))
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
} // namespace application::reg_dp
