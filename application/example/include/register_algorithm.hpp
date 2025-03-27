//! @file register_algorithm.hpp
//! @author ryftchen
//! @brief The declarations (register_algorithm) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include "apply_algorithm.hpp"

#include "application/core/include/configure.hpp"
#include "utility/include/common.hpp"
#include "utility/include/currying.hpp"
#include "utility/include/reflection.hpp"

//! @brief Reflect the algorithm category name and alias name to the field in the mapping.
#define REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                        \
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
//! @brief Reflect the entry under the algorithm category and choice name to the field in the mapping.
#define REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(entry, choice)             \
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

//! @brief Static reflection for ApplyAlgorithm. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::ApplyAlgorithm>
    : TypeInfoBase<application::app_algo::ApplyAlgorithm>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-algo"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(match   , m),
        REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(notation, n),
        REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(optimal , o),
        REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(search  , s),
        REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(sort    , S),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply algorithm"}};
};
//! @brief Static reflection for MatchMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::MatchMethod>
    : TypeInfoBase<application::app_algo::MatchMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"match"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(rabinKarp       , rab),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(knuthMorrisPratt, knu),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(boyerMoore      , boy),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(horspool        , hor),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(sunday          , sun),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "match-related choices\n"
        "- rab    Rabin-Karp\n"
        "- knu    Knuth-Morris-Pratt\n"
        "- boy    Boyer-Moore\n"
        "- hor    Horspool\n"
        "- sun    Sunday\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for NotationMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::NotationMethod>
    : TypeInfoBase<application::app_algo::NotationMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"notation"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(prefix , pre),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(postfix, pos),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "notation-related choices\n"
        "- pre    Prefix\n"
        "- pos    Postfix\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for OptimalMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::OptimalMethod>
    : TypeInfoBase<application::app_algo::OptimalMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"optimal"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(gradient , gra),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(tabu     , tab),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(annealing, ann),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(particle , par),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(ant      , ant),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(genetic  , gen),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "optimal-related choices\n"
        "- gra    Gradient Descent\n"
        "- tab    Tabu\n"
        "- ann    Simulated Annealing\n"
        "- par    Particle Swarm\n"
        "- ant    Ant Colony\n"
        "- gen    Genetic\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for SearchMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::SearchMethod>
    : TypeInfoBase<application::app_algo::SearchMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"search"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(binary       , bin),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(interpolation, int),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(fibonacci    , fib),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "search-related choices\n"
        "- bin    Binary\n"
        "- int    Interpolation\n"
        "- fib    Fibonacci\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for SortMethod. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::app_algo::SortMethod>
    : TypeInfoBase<application::app_algo::SortMethod>
{
    //! @brief Name.
    static constexpr std::string_view name{"sort"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(bubble   , bub),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(selection, sel),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(insertion, ins),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(shell    , she),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(merge    , mer),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(quick    , qui),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(heap     , hea),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(counting , cou),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(bucket   , buc),
        REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(radix    , rad),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "sort-related choices\n"
        "- bub    Bubble\n"
        "- sel    Selection\n"
        "- ins    Insertion\n"
        "- she    Shell\n"
        "- mer    Merge\n"
        "- qui    Quick\n"
        "- hea    Heap\n"
        "- cou    Counting\n"
        "- buc    Bucket\n"
        "- rad    Radix\n"
        "add the choices listed above"}};
};

#undef REG_ALGO_REFLECT_FIRST_LEVEL_FIELD
#undef REG_ALGO_REFLECT_SECOND_LEVEL_FIELD

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Algorithm-registering-related functions in the application module.
namespace reg_algo
{
extern const char* version() noexcept;

//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Get the name directly for sub-cli related registration.
//! @tparam T - type of sub-cli or sub-cli's category
//! @return name
template <typename T>
inline consteval std::string_view name()
{
    return TypeInfo<T>::name;
}
//! @brief Get the description directly for sub-cli related registration.
//! @tparam T - type of sub-cli or sub-cli's category
//! @return description
template <typename T>
inline consteval std::string_view descr()
{
    return TypeInfo<T>::attrs.find(REFLECTION_STR("descr")).value;
}
//! @brief Get the alias directly for sub-cli related registration.
//! @tparam T - type of sub-cli's category
//! @return alias
template <typename T>
inline consteval std::string_view alias()
{
    return TypeInfo<app_algo::ApplyAlgorithm>::fields.find(REFLECTION_STR(TypeInfo<T>::name))
        .attrs.find(REFLECTION_STR("alias"))
        .value;
}

//! @brief Alias for Category.
using Category = app_algo::ApplyAlgorithm::Category;
//! @brief Get the task name curried.
//! @return task name curried
inline auto taskNameCurried()
{
    return utility::currying::curry(configure::task::presetName, TypeInfo<app_algo::ApplyAlgorithm>::name);
}
//! @brief Convert category enumeration to string.
//! @tparam Cat - the specific value of Category enum
//! @return category name
template <Category Cat>
inline consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::match:
            return TypeInfo<app_algo::MatchMethod>::name;
        case Category::notation:
            return TypeInfo<app_algo::NotationMethod>::name;
        case Category::optimal:
            return TypeInfo<app_algo::OptimalMethod>::name;
        case Category::search:
            return TypeInfo<app_algo::SearchMethod>::name;
        case Category::sort:
            return TypeInfo<app_algo::SortMethod>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in algorithm choices.
//! @tparam Cat - the specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
inline constexpr auto& getCategoryOpts()
{
    return std::invoke(
        TypeInfo<app_algo::ApplyAlgorithm>::fields.find(REFLECTION_STR(toString<Cat>())).value, app_algo::manager());
}
//! @brief Get the alias of the category in algorithm choices.
//! @tparam Cat - the specific value of Category enum
//! @return alias of the category name
template <Category Cat>
inline consteval std::string_view getCategoryAlias()
{
    constexpr auto attr = TypeInfo<app_algo::ApplyAlgorithm>::fields.find(REFLECTION_STR(toString<Cat>()))
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
    static_assert(app_algo::Bottom<T>::value == TypeInfo<T>::fields.size);
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
} // namespace reg_algo
} // namespace application
