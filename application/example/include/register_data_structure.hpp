//! @file register_data_structure.hpp
//! @author ryftchen
//! @brief The declarations (register_data_structure) in the application module.
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
//! @brief Data-structure-registering-related functions in the application module.
namespace reg_ds
{
extern const char* version() noexcept;

//! @brief Represent the maximum value of an enum.
//! @tparam T - type of specific enum
template <typename T>
struct Bottom;

//! @brief Enumerate specific cache instances.
enum CacheInstance : std::uint8_t
{
    //! @brief First in first out.
    firstInFirstOut,
    //! @brief Least frequently used.
    leastFrequentlyUsed,
    //! @brief Least recently used.
    leastRecentlyUsed
};
//! @brief Store the maximum value of the CacheInstance enum.
template <>
struct Bottom<CacheInstance>
{
    //! @brief Maximum value of the CacheInstance enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Enumerate specific filter instances.
enum FilterInstance : std::uint8_t
{
    //! @brief Bloom.
    bloom,
    //! @brief Quotient.
    quotient
};
//! @brief Store the maximum value of the FilterInstance enum.
template <>
struct Bottom<FilterInstance>
{
    //! @brief Maximum value of the FilterInstance enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Enumerate specific graph instances.
enum GraphInstance : std::uint8_t
{
    //! @brief Undirected.
    undirected,
    //! @brief Directed.
    directed
};
//! @brief Store the maximum value of the GraphInstance enum.
template <>
struct Bottom<GraphInstance>
{
    //! @brief Maximum value of the GraphInstance enum.
    static constexpr std::uint8_t value{2};
};

//! @brief Enumerate specific heap instances.
enum HeapInstance : std::uint8_t
{
    //! @brief Binary.
    binary,
    //! @brief Leftist.
    leftist,
    //! @brief Skew.
    skew
};
//! @brief Store the maximum value of the HeapInstance enum.
template <>
struct Bottom<HeapInstance>
{
    //! @brief Maximum value of the HeapInstance enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Enumerate specific linear instances.
enum LinearInstance : std::uint8_t
{
    //! @brief Doubly linked list.
    doublyLinkedList,
    //! @brief Stack.
    stack,
    //! @brief Queue.
    queue
};
//! @brief Store the maximum value of the LinearInstance enum.
template <>
struct Bottom<LinearInstance>
{
    //! @brief Maximum value of the LinearInstance enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Enumerate specific tree instances.
enum TreeInstance : std::uint8_t
{
    //! @brief Binary search.
    binarySearch,
    //! @brief Adelson-Velsky-Landis.
    adelsonVelskyLandis,
    //! @brief Splay.
    splay
};
//! @brief Store the maximum value of the TreeInstance enum.
template <>
struct Bottom<TreeInstance>
{
    //! @brief Maximum value of the TreeInstance enum.
    static constexpr std::uint8_t value{3};
};

//! @brief Manage data structure choices.
class ApplyDataStructure
{
public:
    //! @brief Enumerate specific data structure choices.
    enum class Category : std::uint8_t
    {
        //! @brief Cache.
        cache,
        //! @brief Filter.
        filter,
        //! @brief Graph.
        graph,
        //! @brief Heap.
        heap,
        //! @brief Linear.
        linear,
        //! @brief Tree.
        tree
    };

    //! @brief Bit flags for managing cache instances.
    std::bitset<Bottom<CacheInstance>::value> cacheOpts;
    //! @brief Bit flags for managing filter instances.
    std::bitset<Bottom<FilterInstance>::value> filterOpts;
    //! @brief Bit flags for managing graph instances.
    std::bitset<Bottom<GraphInstance>::value> graphOpts;
    //! @brief Bit flags for managing heap instances.
    std::bitset<Bottom<HeapInstance>::value> heapOpts;
    //! @brief Bit flags for managing linear instances.
    std::bitset<Bottom<LinearInstance>::value> linearOpts;
    //! @brief Bit flags for managing tree instances.
    std::bitset<Bottom<TreeInstance>::value> treeOpts;

    //! @brief Check whether any data structure choices do not exist.
    //! @return any data structure choices do not exist or exist
    [[nodiscard]] bool empty() const
    {
        return cacheOpts.none() && filterOpts.none() && graphOpts.none() && heapOpts.none() && linearOpts.none()
            && treeOpts.none();
    }
    //! @brief Reset bit flags that manage data structure choices.
    void reset()
    {
        cacheOpts.reset();
        filterOpts.reset();
        graphOpts.reset();
        heapOpts.reset();
        linearOpts.reset();
        treeOpts.reset();
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
            case Category::cache:
                os << "CACHE";
                break;
            case Category::filter:
                os << "FILTER";
                break;
            case Category::graph:
                os << "GRAPH";
                break;
            case Category::heap:
                os << "HEAP";
                break;
            case Category::linear:
                os << "LINEAR";
                break;
            case Category::tree:
                os << "TREE";
                break;
            default:
                os << "UNKNOWN (" << static_cast<std::underlying_type_t<Category>>(cat) << ')';
                break;
        }

        return os;
    }
};
extern ApplyDataStructure& manager() noexcept;

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

//! @brief Register cache.
namespace cache
{
extern const char* version() noexcept;
} // namespace cache
template <>
void updateChoice<CacheInstance>(const std::string& target);
template <>
void runChoices<CacheInstance>(const std::vector<std::string>& candidates);

//! @brief Register filter.
namespace filter
{
extern const char* version() noexcept;
} // namespace filter
template <>
void updateChoice<FilterInstance>(const std::string& target);
template <>
void runChoices<FilterInstance>(const std::vector<std::string>& candidates);

//! @brief Register graph.
namespace graph
{
extern const char* version() noexcept;
} // namespace graph
template <>
void updateChoice<GraphInstance>(const std::string& target);
template <>
void runChoices<GraphInstance>(const std::vector<std::string>& candidates);

//! @brief Register heap.
namespace heap
{
extern const char* version() noexcept;
} // namespace heap
template <>
void updateChoice<HeapInstance>(const std::string& target);
template <>
void runChoices<HeapInstance>(const std::vector<std::string>& candidates);

//! @brief Register linear.
namespace linear
{
extern const char* version() noexcept;
} // namespace linear
template <>
void updateChoice<LinearInstance>(const std::string& target);
template <>
void runChoices<LinearInstance>(const std::vector<std::string>& candidates);

//! @brief Register tree.
namespace tree
{
extern const char* version() noexcept;
} // namespace tree
template <>
void updateChoice<TreeInstance>(const std::string& target);
template <>
void runChoices<TreeInstance>(const std::vector<std::string>& candidates);
} // namespace reg_ds
} // namespace application

//! @brief Reflect the data structure category name and alias name to the field in the mapping.
#define REG_DS_REFLECT_FIRST_LEVEL_FIELD(category, alias)                                        \
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
//! @brief Reflect the entry under the data structure category and choice name to the field in the mapping.
#define REG_DS_REFLECT_SECOND_LEVEL_FIELD(entry, choice)              \
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
//! @brief Static reflection for ApplyDataStructure. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::ApplyDataStructure>
    : TypeInfoBase<application::reg_ds::ApplyDataStructure>
{
    //! @brief Name.
    static constexpr std::string_view name{"app-ds"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(cache , c),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(filter, f),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(graph , g),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(heap  , H),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(linear, l),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(tree  , t),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply data structure"}};
};
//! @brief Static reflection for CacheInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::CacheInstance>
    : TypeInfoBase<application::reg_ds::CacheInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"cache"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(firstInFirstOut    , fir),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(leastFrequentlyUsed, fre),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(leastRecentlyUsed  , rec),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "cache-related choices\n"
        "- fir    First In First Out\n"
        "- fre    Least Frequently Used\n"
        "- rec    Least Recently Used\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for FilterInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::FilterInstance>
    : TypeInfoBase<application::reg_ds::FilterInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"filter"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(bloom   , blo),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(quotient, quo),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "filter-related choices\n"
        "- blo    Bloom\n"
        "- quo    Quotient\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for GraphInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::GraphInstance>
    : TypeInfoBase<application::reg_ds::GraphInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"graph"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(undirected, und),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(directed  , dir),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "graph-related choices\n"
        "- und    Undirected\n"
        "- dir    Directed\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for HeapInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::HeapInstance>
    : TypeInfoBase<application::reg_ds::HeapInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"heap"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(binary , bin),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(leftist, lef),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(skew   , ske),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "heap-related choices\n"
        "- bin    Binary\n"
        "- lef    Leftist\n"
        "- ske    Skew\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for LinearInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::LinearInstance>
    : TypeInfoBase<application::reg_ds::LinearInstance>
{
    //! @brief Name.
    static constexpr std::string_view name{"linear"};
    // clang-format off
    //! @brief Field list.
    static constexpr FieldList fields
    {
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(doublyLinkedList, lin),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(stack           , sta),
        REG_DS_REFLECT_SECOND_LEVEL_FIELD(queue           , que),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{
        REFLECTION_STR("descr"),
        "linear-related choices\n"
        "- lin    Doubly Linked List\n"
        "- sta    Stack\n"
        "- que    Queue\n"
        "add the choices listed above"}};
};
//! @brief Static reflection for TreeInstance. Used to map command line arguments.
template <>
struct utility::reflection::TypeInfo<application::reg_ds::TreeInstance>
    : TypeInfoBase<application::reg_ds::TreeInstance>
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

namespace application::reg_ds
{
//! @brief Alias for the type information.
//! @tparam T - type of target object
template <typename T>
using TypeInfo = utility::reflection::TypeInfo<T>;
//! @brief Alias for Category.
using Category = ApplyDataStructure::Category;
//! @brief Convert category enumeration to string.
//! @tparam Cat - specific value of Category enum
//! @return category name
template <Category Cat>
consteval std::string_view toString()
{
    switch (Cat)
    {
        case Category::cache:
            return TypeInfo<CacheInstance>::name;
        case Category::filter:
            return TypeInfo<FilterInstance>::name;
        case Category::graph:
            return TypeInfo<GraphInstance>::name;
        case Category::heap:
            return TypeInfo<HeapInstance>::name;
        case Category::linear:
            return TypeInfo<LinearInstance>::name;
        case Category::tree:
            return TypeInfo<TreeInstance>::name;
        default:
            break;
    }

    return {};
}
//! @brief Get the bit flags of the category in data structure choices.
//! @tparam Cat - specific value of Category enum
//! @return reference of the category bit flags
template <Category Cat>
constexpr auto& categoryOpts()
{
    return std::invoke(TypeInfo<ApplyDataStructure>::fields.find(REFLECTION_STR(toString<Cat>())).value, manager());
}
//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <typename T>
consteval std::size_t abbrValue(const T instance)
{
    static_assert(Bottom<T>::value == TypeInfo<T>::fields.size);
    std::size_t value = 0;
    TypeInfo<T>::fields.forEach(
        [instance, &value](const auto field)
        {
            if (field.name == toString(instance))
            {
                static_assert(field.attrs.size == 1);
                const auto attr = field.attrs.find(REFLECTION_STR("choice"));
                static_assert(attr.hasValue);
                value = utility::common::operator""_bkdrHash(attr.value);
            }
        });

    return value;
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of CacheInstance enum
//! @return instance name
constexpr std::string_view toString(const CacheInstance instance)
{
    constexpr std::array<std::string_view, Bottom<CacheInstance>::value> stringify = {
        MACRO_STRINGIFY(firstInFirstOut), MACRO_STRINGIFY(leastFrequentlyUsed), MACRO_STRINGIFY(leastRecentlyUsed)};
    return stringify.at(instance);
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of FilterInstance enum
//! @return instance name
constexpr std::string_view toString(const FilterInstance instance)
{
    constexpr std::array<std::string_view, Bottom<FilterInstance>::value> stringify = {
        MACRO_STRINGIFY(bloom), MACRO_STRINGIFY(quotient)};
    return stringify.at(instance);
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of GraphInstance enum
//! @return instance name
constexpr std::string_view toString(const GraphInstance instance)
{
    constexpr std::array<std::string_view, Bottom<GraphInstance>::value> stringify = {
        MACRO_STRINGIFY(undirected), MACRO_STRINGIFY(directed)};
    return stringify.at(instance);
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of HeapInstance enum
//! @return instance name
constexpr std::string_view toString(const HeapInstance instance)
{
    constexpr std::array<std::string_view, Bottom<HeapInstance>::value> stringify = {
        MACRO_STRINGIFY(binary), MACRO_STRINGIFY(leftist), MACRO_STRINGIFY(skew)};
    return stringify.at(instance);
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of LinearInstance enum
//! @return instance name
constexpr std::string_view toString(const LinearInstance instance)
{
    constexpr std::array<std::string_view, Bottom<LinearInstance>::value> stringify = {
        MACRO_STRINGIFY(doublyLinkedList), MACRO_STRINGIFY(stack), MACRO_STRINGIFY(queue)};
    return stringify.at(instance);
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of TreeInstance enum
//! @return instance name
constexpr std::string_view toString(const TreeInstance instance)
{
    constexpr std::array<std::string_view, Bottom<TreeInstance>::value> stringify = {
        MACRO_STRINGIFY(binarySearch), MACRO_STRINGIFY(adelsonVelskyLandis), MACRO_STRINGIFY(splay)};
    return stringify.at(instance);
}
} // namespace application::reg_ds
