//! @file register_algorithm.hpp
//! @author ryftchen
//! @brief The declarations (register_algorithm) in the application module.
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
//! @brief Algorithm-registering-related functions in the application module.
namespace reg_algo
{
extern const char* version() noexcept;

//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <typename T>
struct Bottom;

//! @brief Enumerate specific match methods.
enum MatchMethod : std::uint8_t
{
    //! @brief Rabin-Karp.
    rabinKarp,
    //! @brief Knuth-Morris-Pratt.
    knuthMorrisPratt,
    //! @brief Boyer-Moore.
    boyerMoore,
    //! @brief Horspool.
    horspool,
    //! @brief Sunday.
    sunday
};
//! @brief Store the maximum value of the MatchMethod enum.
template <>
struct Bottom<MatchMethod>
{
    //! @brief Maximum value of the MatchMethod enum.
    static constexpr std::uint8_t value{5};
};

//! @brief Enumerate specific notation methods.
enum NotationMethod : std::uint8_t
{
    //! @brief Prefix.
    prefix,
    //! @brief Postfix.
    postfix
};
//! @brief Store the maximum value of the NotationMethod enum.
template <>
struct Bottom<NotationMethod>
{
    //! @brief Maximum value of the NotationMethod enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Enumerate specific optimal methods.
enum OptimalMethod : std::uint8_t
{
    //! @brief Gradient.
    gradient,
    //! @brief Tabu.
    tabu,
    //! @brief Annealing.
    annealing,
    //! @brief Particle.
    particle,
    //! @brief Ant.
    ant,
    //! @brief Genetic.
    genetic
};
//! @brief Store the maximum value of the OptimalMethod enum.
template <>
struct Bottom<OptimalMethod>
{
    //! @brief Maximum value of the OptimalMethod enum.
    static constexpr std::uint8_t value{6};
};

//! @brief Enumerate specific search methods.
enum SearchMethod : std::uint8_t
{
    //! @brief Binary.
    binary,
    //! @brief Interpolation.
    interpolation,
    //! @brief Fibonacci.
    fibonacci
};
//! @brief Store the maximum value of the SearchMethod enum.
template <>
struct Bottom<SearchMethod>
{
    //! @brief Maximum value of the SearchMethod enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Enumerate specific sort methods.
enum SortMethod : std::uint8_t
{
    //! @brief Bubble.
    bubble,
    //! @brief Selection.
    selection,
    //! @brief Insertion.
    insertion,
    //! @brief Shell.
    shell,
    //! @brief Merge.
    merge,
    //! @brief Quick.
    quick,
    //! @brief Heap.
    heap,
    //! @brief Counting.
    counting,
    //! @brief Bucket.
    bucket,
    //! @brief Radix.
    radix
};
//! @brief Store the maximum value of the SortMethod enum.
template <>
struct Bottom<SortMethod>
{
    //! @brief Maximum value of the SortMethod enum.
    static constexpr std::uint8_t value{10};
};

//! @brief Manage algorithm choices.
class ApplyAlgorithm
{
public:
    //! @brief Enumerate specific algorithm choices.
    enum class Category : std::uint8_t
    {
        //! @brief Match.
        match,
        //! @brief Notation.
        notation,
        //! @brief Optimal.
        optimal,
        //! @brief Search.
        search,
        //! @brief Sort.
        sort
    };

    //! @brief Bit flags for managing match methods.
    std::bitset<Bottom<MatchMethod>::value> matchOpts;
    //! @brief Bit flags for managing notation methods.
    std::bitset<Bottom<NotationMethod>::value> notationOpts;
    //! @brief Bit flags for managing optimal methods.
    std::bitset<Bottom<OptimalMethod>::value> optimalOpts;
    //! @brief Bit flags for managing search methods.
    std::bitset<Bottom<SearchMethod>::value> searchOpts;
    //! @brief Bit flags for managing sort methods.
    std::bitset<Bottom<SortMethod>::value> sortOpts;

    //! @brief Check whether any algorithm choices do not exist.
    //! @return any algorithm choices do not exist or exist
    [[nodiscard]] bool empty() const
    {
        return matchOpts.none() && notationOpts.none() && optimalOpts.none() && searchOpts.none() && sortOpts.none();
    }
    //! @brief Reset bit flags that manage algorithm choices.
    void reset()
    {
        matchOpts.reset();
        notationOpts.reset();
        optimalOpts.reset();
        searchOpts.reset();
        sortOpts.reset();
    }

protected:
    //! @brief The operator (<<) overloading of the Category enum.
    //! @param os - output stream object
    //! @param cat - specific value of Category enum
    //! @return reference of the output stream object
    friend std::ostream& operator<<(std::ostream& os, const Category cat)
    {
        switch (cat)
        {
            case Category::match:
                os << "MATCH";
                break;
            case Category::notation:
                os << "NOTATION";
                break;
            case Category::optimal:
                os << "OPTIMAL";
                break;
            case Category::search:
                os << "SEARCH";
                break;
            case Category::sort:
                os << "SORT";
                break;
            default:
                os << "UNKNOWN (" << static_cast<std::underlying_type_t<Category>>(cat) << ')';
        }

        return os;
    }
};
extern ApplyAlgorithm& manager() noexcept;

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

//! @brief Register match.
namespace match
{
extern const char* version() noexcept;
} // namespace match
template <>
void updateChoice<MatchMethod>(const std::string& target);
template <>
void runChoices<MatchMethod>(const std::vector<std::string>& candidates);

//! @brief Register notation.
namespace notation
{
extern const char* version() noexcept;
} // namespace notation
template <>
void updateChoice<NotationMethod>(const std::string& target);
template <>
void runChoices<NotationMethod>(const std::vector<std::string>& candidates);

//! @brief Register optimal.
namespace optimal
{
extern const char* version() noexcept;
} // namespace optimal
template <>
void updateChoice<OptimalMethod>(const std::string& target);
template <>
void runChoices<OptimalMethod>(const std::vector<std::string>& candidates);

//! @brief Register search.
namespace search
{
extern const char* version() noexcept;
} // namespace search
template <>
void updateChoice<SearchMethod>(const std::string& target);
template <>
void runChoices<SearchMethod>(const std::vector<std::string>& candidates);

//! @brief Register sort.
namespace sort
{
extern const char* version() noexcept;
} // namespace sort
template <>
void updateChoice<SortMethod>(const std::string& target);
template <>
void runChoices<SortMethod>(const std::vector<std::string>& candidates);
} // namespace reg_algo
} // namespace application

//! @brief Reflect the algorithm category name and alias name to the field in the mapping.
#define REG_ALGO_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                      \
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
//! @brief Reflect the entry under the algorithm category and choice name to the field in the mapping.
#define REG_ALGO_REFLECT_SECOND_LEVEL_FIELD(entry, choice)            \
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
//! @brief Static reflection for ApplyAlgorithm. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_algo::ApplyAlgorithm>
    : TypeInfoBase<application::reg_algo::ApplyAlgorithm>
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
struct utility::reflection::TypeInfo<application::reg_algo::MatchMethod>
    : TypeInfoBase<application::reg_algo::MatchMethod>
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
struct utility::reflection::TypeInfo<application::reg_algo::NotationMethod>
    : TypeInfoBase<application::reg_algo::NotationMethod>
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
struct utility::reflection::TypeInfo<application::reg_algo::OptimalMethod>
    : TypeInfoBase<application::reg_algo::OptimalMethod>
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
struct utility::reflection::TypeInfo<application::reg_algo::SearchMethod>
    : TypeInfoBase<application::reg_algo::SearchMethod>
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
struct utility::reflection::TypeInfo<application::reg_algo::SortMethod>
    : TypeInfoBase<application::reg_algo::SortMethod>
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

namespace application::reg_algo
{
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = ApplyAlgorithm::Category;
//! @brief Convert category enumeration to string.
//! @tparam Cat - specific value of Category enum
//! @return category name
template <Category Cat>
consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::match:
            return TypeInfo<MatchMethod>::name;
        case Category::notation:
            return TypeInfo<NotationMethod>::name;
        case Category::optimal:
            return TypeInfo<OptimalMethod>::name;
        case Category::search:
            return TypeInfo<SearchMethod>::name;
        case Category::sort:
            return TypeInfo<SortMethod>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in algorithm choices.
//! @tparam Cat - specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& categoryOpts()
{
    return std::invoke(TypeInfo<ApplyAlgorithm>::fields.find(REFLECTION_STR(toString<Cat>())).value, manager());
}
//! @brief Abbreviation value for the target method.
//! @tparam T - type of target method
//! @param method - target method
//! @return abbreviation value
template <typename T>
consteval std::size_t abbrValue(const T method)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    std::size_t value = 0;
    TypeInfo<T>::fields.forEach(
        [method, &value](const auto field)
        {
            if (field.name == toString(method))
            {
                static_assert(field.attrs.size == 1);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
            }
        });

    return value;
}

//! @brief Convert method enumeration to string.
//! @param method - specific value of MatchMethod enum
//! @return method name
constexpr std::string_view toString(const MatchMethod method)
{
    constexpr std::array<std::string_view, Bottom<MatchMethod>::value> stringify = {
        MACRO_STRINGIFY(rabinKarp),
        MACRO_STRINGIFY(knuthMorrisPratt),
        MACRO_STRINGIFY(boyerMoore),
        MACRO_STRINGIFY(horspool),
        MACRO_STRINGIFY(sunday)};
    return stringify.at(method);
}

//! @brief Convert method enumeration to string.
//! @param method - specific value of NotationMethod enum
//! @return method name
constexpr std::string_view toString(const NotationMethod method)
{
    constexpr std::array<std::string_view, Bottom<NotationMethod>::value> stringify = {
        MACRO_STRINGIFY(prefix), MACRO_STRINGIFY(postfix)};
    return stringify.at(method);
}

//! @brief Convert method enumeration to string.
//! @param method - specific value of OptimalMethod enum
//! @return method name
constexpr std::string_view toString(const OptimalMethod method)
{
    constexpr std::array<std::string_view, Bottom<OptimalMethod>::value> stringify = {
        MACRO_STRINGIFY(gradient),
        MACRO_STRINGIFY(tabu),
        MACRO_STRINGIFY(annealing),
        MACRO_STRINGIFY(particle),
        MACRO_STRINGIFY(ant),
        MACRO_STRINGIFY(genetic)};
    return stringify.at(method);
}

//! @brief Convert method enumeration to string.
//! @param method - specific value of SearchMethod enum
//! @return method name
constexpr std::string_view toString(const SearchMethod method)
{
    constexpr std::array<std::string_view, Bottom<SearchMethod>::value> stringify = {
        MACRO_STRINGIFY(binary), MACRO_STRINGIFY(interpolation), MACRO_STRINGIFY(fibonacci)};
    return stringify.at(method);
}

//! @brief Convert method enumeration to string.
//! @param method - specific value of SortMethod enum
//! @return method name
constexpr std::string_view toString(const SortMethod method)
{
    constexpr std::array<std::string_view, Bottom<SortMethod>::value> stringify = {
        MACRO_STRINGIFY(bubble),
        MACRO_STRINGIFY(selection),
        MACRO_STRINGIFY(insertion),
        MACRO_STRINGIFY(shell),
        MACRO_STRINGIFY(merge),
        MACRO_STRINGIFY(quick),
        MACRO_STRINGIFY(heap),
        MACRO_STRINGIFY(counting),
        MACRO_STRINGIFY(bucket),
        MACRO_STRINGIFY(radix)};
    return stringify.at(method);
}
} // namespace application::reg_algo
