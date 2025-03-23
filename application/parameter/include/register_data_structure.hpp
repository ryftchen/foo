//! @file register_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (register_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "application/core/include/configure.hpp"
#include "application/example/include/apply_data_structure.hpp"
#include "utility/include/common.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/reflection.hpp"

//! @brief Reflect the data structure category name and alias name to the field in the mapping.
#define REG_DS_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                          \
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
//! @brief Reflect the entry under the data structure category and choice name to the field in the mapping.
#define REG_DS_REFLECT_SECOND_LEVEL_FIELD(entry, choice)               \
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

//! @brief Static reflection for ApplyDataStructure. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_ds::ApplyDataStructure>
    : TypeInfoBase<application::app_ds::ApplyDataStructure>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-ds"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(linear, l),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(tree  , t),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply data structure"}};
};
//! @brief Static reflection for LinearInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_ds::LinearInstance>
    : TypeInfoBase<application::app_ds::LinearInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"linear"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(linkedList, lin),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(stack     , sta),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(queue     , que),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "linear-related choices\n"
        "- lin    Linked List\n"
        "- sta    Stack\n"
        "- que    Queue\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for TreeInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_ds::TreeInstance>
    : TypeInfoBase<application::app_ds::TreeInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"tree"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(binarySearch       , bin),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(adelsonVelskyLandis, ade),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(splay              , spl),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "tree-related choices\n"
        "- bin    Binary Search\n"
        "- ade    Adelson-Velsky-Landis\n"
        "- spl    Splay\n"
        "add the choices listed above"}};
};

#undef REG_DS_REFLECT_FIRST_LEVEL_FIELD
#undef REG_DS_REFLECT_SECOND_LEVEL_FIELD

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Data-structure-registering-related functions in the application module.
namespace reg_ds
{
extern const char* version() noexcept;

//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = app_ds::ApplyDataStructure::Category;

//! @brief Get the task name curried.
//! @return task name curried
inline auto taskNameCurried()
{
    return utility::currying::curry(configure::task::presetName, TypeInfo<app_ds::ApplyDataStructure>::name);
}
//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
inline consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::linear:
            return TypeInfo<app_ds::LinearInstance>::name;
        case Category::tree:
            return TypeInfo<app_ds::TreeInstance>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in data structure choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
inline constexpr auto& getCategoryOpts()
{
    return std::invoke(
        TypeInfo<app_ds::ApplyDataStructure>::fields.find(REFLECTION_STR(toString<Cat>())).value, app_ds::manager());
}
//! @brief Get the alias of the category in data structure choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
inline consteval std::string_view getCategoryAlias()
{
    constexpr auto attr = TypeInfo<app_ds::ApplyDataStructure>::fields.find(REFLECTION_STR(toString<Cat>()))
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
    static_assert(app_ds::Bottom<T>::value == TypeInfo<T>::fields.size);
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
} // namespace reg_ds
} // namespace application
