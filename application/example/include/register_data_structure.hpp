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

//! @brief Enumerate specific linear instances.
enum LinearInstance : std::uint8_t
{
    //! @brief Linked list.
    linkedList,
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
    enum Category : std::uint8_t
    {
        //! @brief Linear.
        linear,
        //! @brief Tree.
        tree
    };

    //! @brief Bit flags for managing linear instances.
    std::bitset<Bottom<LinearInstance>::value> linearOpts{};
    //! @brief Bit flags for managing tree instances.
    std::bitset<Bottom<TreeInstance>::value> treeOpts{};

    //! @brief Check whether any data structure choices do not exist.
    //! @return any data structure choices do not exist or exist
    [[nodiscard]] inline bool empty() const { return linearOpts.none() && treeOpts.none(); }
    //! @brief Reset bit flags that manage data structure choices.
    inline void reset()
    {
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
void updateChoice(const std::string_view target);
//! @brief Run choices.
//! @tparam T - type of target instance
//! @param candidates - container for the candidate target instances
template <typename T>
void runChoices(const std::vector<std::string>& candidates);

//! @brief Register linear.
namespace linear
{
extern const char* version() noexcept;
} // namespace linear
template <>
void updateChoice<LinearInstance>(const std::string_view target);
template <>
void runChoices<LinearInstance>(const std::vector<std::string>& candidates);

//! @brief Register tree.
namespace tree
{
extern const char* version() noexcept;
} // namespace tree
template <>
void updateChoice<TreeInstance>(const std::string_view target);
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
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(linear, l),
        REG_DS_REFLECT_FIRST_LEVEL_FIELD(tree  , t),
    };
    // clang-format on
    //! @brief Attribute list.
    static constexpr AttrList attrs{Attr{REFLECTION_STR("descr"), "apply data structure"}};
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
inline consteval std::string_view toString()
{
    switch (Cat)
    {
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
inline constexpr auto& categoryOpts()
{
    return std::invoke(TypeInfo<ApplyDataStructure>::fields.find(REFLECTION_STR(toString<Cat>())).value, manager());
}
//! @brief Abbreviation value for the target instance.
//! @tparam T - type of target instance
//! @param instance - target instance
//! @return abbreviation value
template <typename T>
inline consteval std::size_t abbrValue(const T instance)
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
//! @param instance - specific value of LinearInstance enum
//! @return instance name
inline constexpr std::string_view toString(const LinearInstance instance)
{
    constexpr std::array<std::string_view, Bottom<LinearInstance>::value> stringify = {
        MACRO_STRINGIFY(linkedList), MACRO_STRINGIFY(stack), MACRO_STRINGIFY(queue)};
    return stringify.at(instance);
}

//! @brief Convert instance enumeration to string.
//! @param instance - specific value of TreeInstance enum
//! @return instance name
inline constexpr std::string_view toString(const TreeInstance instance)
{
    constexpr std::array<std::string_view, Bottom<TreeInstance>::value> stringify = {
        MACRO_STRINGIFY(binarySearch), MACRO_STRINGIFY(adelsonVelskyLandis), MACRO_STRINGIFY(splay)};
    return stringify.at(instance);
}
} // namespace application::reg_ds
