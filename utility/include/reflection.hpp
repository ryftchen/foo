//! @file reflection.hpp
//! @author ryftchen
//! @brief The declarations (reflection) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include <string_view>
#include <tuple>

//! @brief Convert target string for reflection.
#define REFLECTION_STR(str)                                                                                       \
    (                                                                                                             \
        []                                                                                                        \
        {                                                                                                         \
            constexpr std::basic_string_view refl = str;                                                          \
            return utility::reflection::Reflect::String<                                                          \
                utility::reflection::Reflect::StrType<typename decltype(refl)::value_type, refl.size()>{refl}>{}; \
        }())

//! @brief The utility module.
namespace utility // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Static-reflection-related functions in the utility module.
namespace reflection
{
extern const char* version() noexcept;

//! @brief Reflection.
class Reflect
{
public:
    //! @brief Custom string type wrapper.
    //! @tparam Char - type of character in string
    //! @tparam N - string size
    template <typename Char, std::size_t N>
    struct StrType
    {
        //! @brief Alias for the value type.
        using ValueType = Char;

        //! @brief Construct a new StrType object.
        //! @param str - target string
        constexpr explicit StrType(std::basic_string_view<ValueType> str)
        {
            for (std::size_t i = 0; i < size; ++i)
            {
                data[i] = str[i];
            }
        }

        //! @brief String data.
        ValueType data[N + 1]{};
        //! @brief String size.
        static constexpr std::size_t size{N};
    };

    //! @brief Custom string.
    //! @tparam Str - custom string with wrapper
    template <StrType Str>
    struct String
    {
        //! @brief Alias for the character.
        using Char = typename decltype(Str)::ValueType;

        //! @brief Check whether it is the custom string type.
        //! @tparam T - type of target string
        //! @return be custom string type or not
        template <typename T>
        static inline constexpr bool is(T /*str*/ = {})
        {
            return std::is_same_v<T, String>;
        }
        //! @brief Get the string data.
        //! @return string data
        static inline constexpr auto data() { return Str.data; }
        //! @brief Get the string size.
        //! @return string size
        static inline constexpr auto size() { return Str.size; }
        //! @brief Get the string for view only.
        //! @return string data for view only
        static inline constexpr std::basic_string_view<Char> view() { return Str.data; }
    };

    //! @brief Finding by condition.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @param lit - string literal
    //! @param func - callable function
    //! @param sequence - sequences related to arguments
    //! @return value index
    template <class Lit, class Func>
    static inline constexpr std::size_t findIf(const Lit& lit, Func&& func, std::index_sequence<> sequence);
    //! @brief Finding by condition.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @tparam N0 - first index of sequences related to arguments
    //! @tparam Ns - last index of sequences related to arguments
    //! @param lit - string literal
    //! @param func - callable function
    //! @param sequence - sequences related to arguments
    //! @return value index
    template <class Lit, class Func, std::size_t N0, std::size_t... Ns>
    static inline constexpr std::size_t findIf(const Lit& lit, Func&& func, std::index_sequence<N0, Ns...> sequence);
    //! @brief Calculate accumulation.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @tparam Ret - type of return value
    //! @param lit - string literal
    //! @param func - callable function
    //! @param ret - return value
    //! @param sequence - sequences related to arguments
    //! @return result of accumulation
    template <class Lit, class Func, class Ret>
    static inline constexpr auto acc(const Lit& lit, Func&& func, Ret ret, std::index_sequence<> sequence);
    //! @brief Calculate accumulation.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @tparam Ret - type of return value
    //! @tparam N0 - first index of sequences related to arguments
    //! @tparam Ns - last index of sequences related to arguments
    //! @param lit - string literal
    //! @param func - callable function
    //! @param ret - return value
    //! @param sequence - sequences related to arguments
    //! @return result of accumulation
    template <class Lit, class Func, class Ret, std::size_t N0, std::size_t... Ns>
    static inline constexpr auto acc(const Lit& lit, Func&& func, Ret ret, std::index_sequence<N0, Ns...> sequence);
    //! @brief Calculate accumulation by the DFS algorithm.
    //! @tparam D - degree of depth
    //! @tparam T - type of type information
    //! @tparam Ret - type of return value
    //! @tparam Func - type of callable function
    //! @param info - type information
    //! @param func - callable function
    //! @param ret - return value
    //! @return result of the accumulation
    template <std::size_t D, class T, class Ret, class Func>
    static inline constexpr auto dfsAcc(T info, Func&& func, Ret ret);
    //! @brief Traverse the variable of node v in the DFS algorithm.
    //! @tparam T - type of type information
    //! @tparam U - type of object to be traversed
    //! @tparam Func - type of callable function
    //! @param info - type information
    //! @param obj - object to be traversed
    //! @param func - callable function
    template <class T, class U, class Func>
    static inline constexpr void varInNodeV(T info, U&& obj, Func&& func);
};

template <class Lit, class Func>
inline constexpr std::size_t Reflect::findIf(const Lit& /*lit*/, Func&& /*func*/, std::index_sequence<> /*sequence*/)
{
    return -1;
}

template <class Lit, class Func, std::size_t N0, std::size_t... Ns>
inline constexpr std::size_t Reflect::findIf(const Lit& lit, Func&& func, std::index_sequence<N0, Ns...> /*sequence*/)
{
    return func(lit.template get<N0>()) ? N0 : findIf(lit, std::forward<Func>(func), std::index_sequence<Ns...>{});
}

template <class Lit, class Func, class Ret>
inline constexpr auto Reflect::acc(const Lit& /*lit*/, Func&& /*func*/, Ret ret, std::index_sequence<> /*sequence*/)
{
    return ret;
}

template <class Lit, class Func, class Ret, std::size_t N0, std::size_t... Ns>
inline constexpr auto Reflect::acc(const Lit& lit, Func&& func, Ret ret, std::index_sequence<N0, Ns...> /*sequence*/)
{
    return acc(
        lit, std::forward<Func>(func), func(std::move(ret), lit.template get<N0>()), std::index_sequence<Ns...>{});
}

template <std::size_t D, class T, class Ret, class Func>
inline constexpr auto Reflect::dfsAcc(T info, Func&& func, Ret ret)
{
    return info.bases.accumulate(
        std::move(ret),
        [&](auto ret, auto base)
        {
            if constexpr (base.isVirtual)
            {
                return dfsAcc<D + 1>(base.info, std::forward<Func>(func), std::move(ret));
            }
            else
            {
                return dfsAcc<D + 1>(
                    base.info, std::forward<Func>(func), std::forward<Func>(func)(std::move(ret), base.info, D + 1));
            }
        });
}

template <class T, class U, class Func>
inline constexpr void Reflect::varInNodeV(T /*info*/, U&& obj, Func&& func)
{
    T::fields.forEach(
        [&](auto&& fld)
        {
            using Fld = std::decay_t<decltype(fld)>;
            if constexpr (!Fld::isStatic && !Fld::isFunction)
            {
                std::forward<Func>(func)(fld, std::forward<U>(obj).*(fld.value));
            }
        });
    T::bases.forEach(
        [&](auto base)
        {
            if constexpr (!base.isVirtual)
            {
                varInNodeV(base.info, base.info.forward(std::forward<U>(obj)), std::forward<Func>(func));
            }
        });
}

//! @brief Base class of named value.
//! @tparam Name - type of name
template <class Name>
struct NamedValueBase
{
    //! @brief Alias for the name type.
    using NameType = Name;
    //! @brief Value name.
    static constexpr std::string_view name{NameType::view()};
};

//! @brief Specialization for named value.
//! @tparam Name - type of name
//! @tparam T - type of target value
template <class Name, class T>
struct NamedValue : NamedValueBase<Name>
{
    //! @brief Construct a new NamedValue object.
    //! @param val - target value
    constexpr explicit NamedValue(T val) : value(val) {}

    //! @brief The operator (==) overloading of NamedValue struct.
    //! @tparam U - type of value
    //! @param val - named value
    //! @return be equal or not equal
    template <class U>
    constexpr bool operator==(U val) const
    {
        if constexpr (std::is_same_v<T, U>)
        {
            return value == val;
        }
        else
        {
            return false;
        }
    }

    //! @brief Named value.
    T value{};
    //! @brief Flag to indicate whether it has a value.
    static constexpr bool hasValue{true};
};

//! @brief Specialization for named value.
//! @tparam Name - type of name
template <class Name>
struct NamedValue<Name, void> : NamedValueBase<Name>
{
    //! @brief The operator (==) overloading of NamedValue struct.
    //! @tparam U - type of value
    //! @return be equal or not equal
    template <class U>
    constexpr bool operator==(U /*val*/) const
    {
        return false;
    }

    //! @brief Flag to indicate whether it has a value.
    static constexpr bool hasValue{false};
};

//! @brief The list of elements.
//! @tparam Es - type of list of elements
template <typename... Es>
struct ElemList
{
    //! @brief Construct a new ElemList object.
    //! @param es - list of elements
    constexpr explicit ElemList(Es... es) : elems(es...) {}

    //! @brief Element list.
    std::tuple<Es...> elems{};
    //! @brief Size of list of the elements.
    static constexpr std::size_t size{sizeof...(Es)};

    //! @brief Accumulating.
    //! @tparam Init - type of initial accumulation
    //! @tparam Func - type of callable function
    //! @param init - initial accumulation
    //! @param func - callable function
    //! @return result of accumulation
    template <class Init, class Func>
    constexpr auto accumulate(Init init, Func&& func) const
    {
        return Reflect::acc(*this, std::forward<Func>(func), std::move(init), std::make_index_sequence<size>{});
    }
    //! @brief Iteration.
    //! @tparam Func - type of callable function
    //! @param func - callable function
    template <class Func>
    constexpr void forEach(Func&& func) const
    {
        accumulate(
            0,
            [&](auto, const auto& fld)
            {
                std::forward<Func>(func)(fld);
                return 0;
            });
    }
    //! @brief Check whether it contains the custom string.
    //! @tparam Str - type of custom string
    //! @return contain or not contain
    template <class Str>
    static constexpr bool contain(Str /*str*/ = {})
    {
        return (Es::NameType::template is<Str>() || ...);
    }
    //! @brief Finding by condition.
    //! @tparam Func - type of callable function
    //! @param func - callable function
    //! @return value index
    template <class Func>
    constexpr std::size_t findIf(Func&& func) const
    {
        return Reflect::findIf(*this, std::forward<Func>(func), std::make_index_sequence<size>{});
    }
    //! @brief Finding.
    //! @tparam Str - type of custom string
    //! @return value index
    template <class Str>
    constexpr const auto& find(Str /*str*/ = {}) const
    {
        constexpr std::size_t idx = []() constexpr
        {
            constexpr decltype(Str::view()) names[]{Es::name...};
            for (std::size_t i = 0; i < size; i++)
            {
                if (Str::view() == names[i])
                {
                    return i;
                }
            }
            return static_cast<std::size_t>(-1);
        }();
        return get<idx>();
    }
    //! @brief Finding by value.
    //! @tparam T - type of target value
    //! @param val - target value
    //! @return value index
    template <class T>
    constexpr std::size_t findValue(const T& val) const
    {
        return findIf(
            [&val](auto elem)
            {
                return elem == val;
            });
    }
    //! @brief Get the pointer of value by name.
    //! @tparam T - type of value
    //! @tparam Str - type of custom string
    //! @param name - value name
    //! @return pointer of value
    template <typename T, typename Str>
    constexpr const T* valuePtrOfName(Str name) const
    {
        return accumulate(
            nullptr,
            [name](auto ret, const auto& elem)
            {
                if constexpr (std::is_same_v<decltype(elem.value), T>)
                {
                    return (elem.name == name) ? &elem.value : ret;
                }
                else
                {
                    return ret;
                }
            });
    }
    //! @brief Get the reference of value by name.
    //! @tparam T - type of value
    //! @tparam Str - type of custom string
    //! @param name - value name
    //! @return reference of value
    template <typename T, typename Str>
    constexpr const T& valueOfName(Str name) const
    {
        return *valuePtrOfName<T>(name);
    }
    //! @brief Get the name by value.
    //! @tparam T - type of target value
    //! @tparam Char - type of character in custom string
    //! @param val - target value
    //! @return value name
    template <class T, class Char = char>
    constexpr auto nameOfValue(const T& val) const
    {
        return accumulate(
            std::basic_string_view<Char>{},
            [&val](auto ret, auto&& elem)
            {
                return (elem == val) ? elem.name : ret;
            });
    }
    //! @brief Push operation of the element.
    //! @tparam Elem - type of target element
    //! @param elem - target element
    //! @return position after pushing
    template <class Elem>
    constexpr auto push(Elem elem) const
    {
        return std::apply(
            [elem](auto... es)
            {
                return ElemList<Es..., Elem>{es..., elem};
            },
            elems);
    }
    //! @brief Insert operation of the element.
    //! @tparam Elem - type of target element
    //! @param elem - target element
    //! @return position after inserting
    template <class Elem>
    constexpr auto insert(Elem elem) const
    {
        if constexpr ((std::is_same_v<Es, Elem> || ...))
        {
            return *this;
        }
        else
        {
            return push(elem);
        }
    }
    //! @brief Get the value.
    //! @tparam N - value index
    //! @return target value
    template <std::size_t N>
    [[nodiscard]] constexpr const auto& get() const
    {
        return std::get<N>(elems);
    }
};

//! @brief Attribute in class.
//! @tparam Name - type of name
//! @tparam T - type of target value
template <class Name, class T>
struct Attr : NamedValue<Name, T>
{
    //! @brief Construct a new Attr object.
    //! @param val - target value
    constexpr Attr(Name /*name*/, T val) : NamedValue<Name, T>(val) {}
};

//! @brief Attribute in class.
//! @tparam Name - type of name
template <class Name>
struct Attr<Name, void> : NamedValue<Name, void>
{
    //! @brief Construct a new Attr object.
    constexpr explicit Attr(Name /*name*/) {}
};

//! @brief The list of attributes.
//! @tparam As - type of list of attributes
template <typename... As>
struct AttrList : ElemList<As...>
{
    //! @brief Construct a new AttrList object.
    //! @param as - list of attributes
    constexpr explicit AttrList(As... as) : ElemList<As...>(as...) {}
};

//! @brief Base class of trait.
//! @tparam IsStat - whether it is static
//! @tparam IsFunc - whether it is a function
template <bool IsStat, bool IsFunc>
struct TraitBase
{
    //! @brief Flag to indicate whether it is static.
    static constexpr bool isStatic{IsStat};
    //! @brief Flag to indicate whether it is a function.
    static constexpr bool isFunction{IsFunc};
};

//! @brief Specialization for trait.
//! @tparam T - type of target value
template <class T>
struct Trait : TraitBase<true, false>
{
};

//! @brief Specialization for trait.
//! @tparam U - type of target object
//! @tparam T - type of target value
template <class U, class T>
struct Trait<T U::*> : TraitBase<false, std::is_function_v<T>>
{
};

//! @brief Specialization for trait.
//! @tparam T - type of target value
template <class T>
struct Trait<T*> : TraitBase<true, std::is_function_v<T>>
{
};

//! @brief Field in class.
//! @tparam Name - type of name
//! @tparam T - type of target value
//! @tparam Attrs - type of list of attributes
template <class Name, class T, class Attrs>
struct Field : Trait<T>, NamedValue<Name, T>
{
    //! @brief Construct a new Field object.
    //! @param val - target value
    //! @param as - list of attributes
    constexpr Field(Name /*name*/, T val, Attrs as = {}) : NamedValue<Name, T>(val), attrs(as) {}

    //! @brief Attribute list.
    Attrs attrs{};
};

//! @brief The list of fields.
//! @tparam Fs - type of list of fields
template <typename... Fs>
struct FieldList : ElemList<Fs...>
{
    //! @brief Construct a new FieldList object.
    //! @param fs - list of fields
    constexpr explicit FieldList(Fs... fs) : ElemList<Fs...>(fs...) {}
};

//! @brief Type information.
//! @tparam T - type of target object
template <class T>
struct TypeInfo;

//! @brief Public base class.
//! @tparam T - type of target object
//! @tparam IsVirtual - whether it is virtual base class
template <class T, bool IsVirtual = false>
struct Base
{
    //! @brief Type information.
    static constexpr auto info{TypeInfo<T>{}};
    //! @brief Flag to indicate whether it is virtual base class.
    static constexpr bool isVirtual{IsVirtual};
};

//! @brief The list of public base classes.
//! @tparam Bs - type of list of public base classes
template <typename... Bs>
struct BaseList : ElemList<Bs...>
{
    //! @brief Construct a new BaseList object.
    //! @param bs - list of public base classes
    constexpr explicit BaseList(Bs... bs) : ElemList<Bs...>(bs...) {}
};

//! @brief The list of type informations.
//! @tparam Ts - type of list of type informations
template <typename... Ts>
struct TypeInfoList : ElemList<Ts...>
{
    //! @brief Construct a new TypeInfoList object.
    //! @param ts - list of type informations
    constexpr explicit TypeInfoList(Ts... ts) : ElemList<Ts...>(ts...) {}
};

//! @brief Base class of type information.
//! @tparam T - type of type information
//! @tparam Bs - type of list of public base classes
template <class T, typename... Bs>
struct TypeInfoBase
{
    //! @brief Alias for the type.
    using Type = T;
    //! @brief Public base class list.
    static constexpr BaseList bases{Bs{}...};

    //! @brief Forward cast from derived class object to base class object.
    //! @tparam U - type of derived class object
    //! @param derived - derived class object
    //! @return base class object
    template <class U>
    static constexpr auto&& forward(U&& derived)
    {
        if constexpr (std::is_same_v<std::decay_t<U>, U>)
        {
            return static_cast<Type&&>(derived);
        }
        else if constexpr (std::is_same_v<std::decay_t<U>&, U>)
        {
            return static_cast<Type&>(derived);
        }
        else
        {
            return static_cast<const std::decay_t<U>&>(derived);
        }
    }
    //! @brief Get all virtual base class objects.
    //! @return accumulation of virtual base class
    static constexpr auto virtualBases()
    {
        return bases.accumulate(
            ElemList<>{},
            [](auto acc, auto base)
            {
                auto concat = base.info.virtualBases().accumulate(
                    acc,
                    [](auto acc, auto base)
                    {
                        return acc.insert(base);
                    });
                if constexpr (!base.isVirtual)
                {
                    return concat;
                }
                else
                {
                    return concat.insert(base.info);
                }
            });
    }
    //! @brief Accumulating by the DFS algorithm.
    //! @tparam Ret - type of return value
    //! @tparam Func - type of callable function
    //! @param ret - type of return value
    //! @param func - callable function
    //! @return result of accumulation
    template <class Ret, class Func>
    static constexpr auto dfsAcc(Ret ret, Func&& func)
    {
        return Reflect::dfsAcc<0>(
            TypeInfo<Type>{},
            std::forward<Func>(func),
            virtualBases().accumulate(
                std::forward<Func>(func)(std::move(ret), TypeInfo<Type>{}, 0),
                [&](auto acc, auto vb)
                {
                    return std::forward<Func>(func)(std::move(acc), vb, 1);
                }));
    }
    //! @brief Iteration in the DFS algorithm.
    //! @tparam Func - type of callable function
    //! @param func - callable function
    template <class Func>
    static constexpr void dfsForEach(Func&& func)
    {
        dfsAcc(
            0,
            [&](auto, auto info, auto der)
            {
                std::forward<Func>(func)(info, der);
                return 0;
            });
    }
    //! @brief Iteration variable only.
    //! @tparam U - type of object to be traversed
    //! @tparam Func - type of callable function
    //! @param obj - object to be traversed
    //! @param func - callable function
    template <class U, class Func>
    static constexpr void forEachVarOf(U&& obj, Func&& func)
    {
        virtualBases().forEach(
            [&](auto vb)
            {
                vb.fields.forEach(
                    [&](const auto& fld)
                    {
                        using Fld = std::decay_t<decltype(fld)>;
                        if constexpr (!Fld::isStatic && !Fld::isFunction)
                        {
                            std::forward<Func>(func)(fld, std::forward<U>(obj).*(fld.value));
                        }
                    });
            });
        Reflect::varInNodeV(TypeInfo<Type>{}, std::forward<U>(obj), std::forward<Func>(func));
    }
};

//! @brief Attribute in class.
//! @tparam Name - type of name
template <class Name>
Attr(Name) -> Attr<Name, void>;

//! @brief Field in class.
//! @tparam Name - type of name
//! @tparam T - type of target value
template <class Name, class T>
Field(Name, T) -> Field<Name, T, AttrList<>>;
} // namespace reflection
} // namespace utility
