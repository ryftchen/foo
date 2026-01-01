//! @file reflection.hpp
//! @author ryftchen
//! @brief The declarations (reflection) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#include <string_view>
#include <tuple>

//! @brief Convert target string for reflection.
#define REFLECTION_STR(str)                                                                                       \
    (                                                                                                             \
        []() consteval                                                                                            \
        {                                                                                                         \
            constexpr std::basic_string_view refl = str;                                                          \
            return utility::reflection::Reflect::String<                                                          \
                utility::reflection::Reflect::ReflStr<typename decltype(refl)::value_type, refl.size()>{refl}>{}; \
        }())

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Static-reflection-related functions in the utility module.
namespace reflection
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_REFLECTION";
}
extern const char* version() noexcept;

//! @brief Reflection.
class Reflect
{
public:
    //! @brief Custom string type wrapper.
    //! @tparam Char - type of character in string
    //! @tparam Size - string size
    template <typename Char, std::size_t Size>
    class ReflStr
    {
    public:
        //! @brief Alias for the value type.
        using ValueType = Char;
        //! @brief Construct a new ReflStr object.
        //! @param str - target string
        consteval explicit ReflStr(const std::basic_string_view<ValueType> str)
        {
            for (std::size_t i = 0; i < size; ++i)
            {
                data[i] = str[i];
            }
        }

        //! @brief String data.
        ValueType data[Size + 1]{};
        //! @brief String size.
        static constexpr std::size_t size{Size};
    };

    //! @brief Custom string.
    //! @tparam Str - custom string with wrapper
    template <ReflStr Str>
    class String
    {
    public:
        //! @brief Alias for the character.
        using Char = typename decltype(Str)::ValueType;
        //! @brief Check whether it is the custom string type.
        //! @tparam Chars - type of target string
        //! @return be custom string type or not
        template <typename Chars>
        static consteval bool is(const Chars& /*str*/ = {})
        {
            return std::is_same_v<Chars, String>;
        }
        //! @brief Get the string data.
        //! @return string data
        static consteval auto data() { return Str.data; }
        //! @brief Get the string size.
        //! @return string size
        static consteval auto size() { return Str.size; }
        //! @brief Get the string for view only.
        //! @return string data for view only
        static consteval std::basic_string_view<Char> view() { return Str.data; }
    };

    //! @brief Finding by condition.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @param lit - string literal
    //! @param func - callable function
    //! @param seq - sequences related to arguments
    //! @return value index
    template <typename Lit, typename Func>
    static constexpr std::size_t findIf(const Lit& lit, Func&& func, const std::index_sequence<>& seq);
    //! @brief Finding by condition.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @tparam I0 - current index of elements
    //! @tparam Is - indices of sequence related to elements
    //! @param lit - string literal
    //! @param func - callable function
    //! @param seq - sequence related to elements
    //! @return value index
    template <typename Lit, typename Func, std::size_t I0, std::size_t... Is>
    static constexpr std::size_t findIf(const Lit& lit, Func&& func, const std::index_sequence<I0, Is...>& seq);
    //! @brief Calculate accumulation.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @tparam Ret - type of return value
    //! @param lit - string literal
    //! @param func - callable function
    //! @param ret - return value
    //! @param seq - sequences related to arguments
    //! @return result of accumulation
    template <typename Lit, typename Func, typename Ret>
    static constexpr auto acc(const Lit& lit, Func&& func, Ret ret, const std::index_sequence<>& seq);
    //! @brief Calculate accumulation.
    //! @tparam Lit - type of string literal
    //! @tparam Func - type of callable function
    //! @tparam Ret - type of return value
    //! @tparam I0 - current index of elements
    //! @tparam Is - indices of sequence related to elements
    //! @param lit - string literal
    //! @param func - callable function
    //! @param ret - return value
    //! @param seq - sequence related to elements
    //! @return result of accumulation
    template <typename Lit, typename Func, typename Ret, std::size_t I0, std::size_t... Is>
    static constexpr auto acc(const Lit& lit, Func&& func, Ret ret, const std::index_sequence<I0, Is...>& seq);
    //! @brief Calculate accumulation by the DFS algorithm.
    //! @tparam Depth - degree of depth
    //! @tparam Info - type of type information
    //! @tparam Ret - type of return value
    //! @tparam Func - type of callable function
    //! @param info - type information
    //! @param func - callable function
    //! @param ret - return value
    //! @return result of the accumulation
    template <std::size_t Depth, typename Info, typename Ret, typename Func>
    static constexpr auto dfsAcc(const Info& info, Func&& func, Ret ret);
    //! @brief Traverse the variable of node v in the DFS algorithm.
    //! @tparam Info - type of type information
    //! @tparam Obj - type of object to be traversed
    //! @tparam Func - type of callable function
    //! @param info - type information
    //! @param obj - object to be traversed
    //! @param func - callable function
    template <typename Info, typename Obj, typename Func>
    static constexpr void varInNodeV(const Info& info, Obj&& obj, Func&& func);
};

template <typename Lit, typename Func>
constexpr std::size_t Reflect::findIf(const Lit& /*lit*/, Func&& /*func*/, const std::index_sequence<>& /*seq*/)
{
    return -1;
}

template <typename Lit, typename Func, std::size_t I0, std::size_t... Is>
constexpr std::size_t Reflect::findIf(const Lit& lit, Func&& func, const std::index_sequence<I0, Is...>& /*seq*/)
{
    return func(lit.template get<I0>()) ? I0 : findIf(lit, std::forward<Func>(func), std::index_sequence<Is...>{});
}

template <typename Lit, typename Func, typename Ret>
constexpr auto Reflect::acc(const Lit& /*lit*/, Func&& /*func*/, Ret ret, const std::index_sequence<>& /*seq*/)
{
    return ret;
}

template <typename Lit, typename Func, typename Ret, std::size_t I0, std::size_t... Is>
constexpr auto Reflect::acc(const Lit& lit, Func&& func, Ret ret, const std::index_sequence<I0, Is...>& /*seq*/)
{
    return acc(
        lit, std::forward<Func>(func), func(std::move(ret), lit.template get<I0>()), std::index_sequence<Is...>{});
}

template <std::size_t Depth, typename Info, typename Ret, typename Func>
constexpr auto Reflect::dfsAcc(const Info& info, Func&& func, Ret ret)
{
    return info.bases.accumulate(
        std::move(ret),
        [&](const auto ret, const auto base)
        {
            if constexpr (base.isVirtual)
            {
                return dfsAcc<Depth + 1>(base.info, std::forward<Func>(func), ret);
            }
            else
            {
                return dfsAcc<Depth + 1>(
                    base.info, std::forward<Func>(func), std::forward<Func>(func)(ret, base.info, Depth + 1));
            }
        });
}

template <typename Info, typename Obj, typename Func>
constexpr void Reflect::varInNodeV(const Info& /*info*/, Obj&& obj, Func&& func)
{
    Info::fields.forEach(
        [&](const auto fld)
        {
            using Fld = std::decay_t<decltype(fld)>;
            if constexpr (!Fld::isStatic && !Fld::isFunction)
            {
                std::forward<Func>(func)(fld, std::forward<Obj>(obj).*(fld.value));
            }
        });
    Info::bases.forEach(
        [&](const auto base)
        {
            if constexpr (!base.isVirtual)
            {
                varInNodeV(base.info, base.info.forward(std::forward<Obj>(obj)), std::forward<Func>(func));
            }
        });
}

//! @brief Base class of named value.
//! @tparam Name - type of name
template <typename Name>
struct NamedValueBase
{
    //! @brief Alias for the name type.
    using NameType = Name;
    //! @brief Value name.
    static constexpr std::string_view name{NameType::view()};
};

//! @brief Specialization for named value.
//! @tparam Name - type of name
//! @tparam Value - type of target value
template <typename Name, typename Value>
struct NamedValue : public NamedValueBase<Name>
{
    //! @brief Construct a new NamedValue object.
    //! @param val - target value
    constexpr explicit NamedValue(const Value val) : value{val} {}

    //! @brief The operator (==) overloading of NamedValue struct.
    //! @tparam RHS - type of right-hand side
    //! @param rhs - right-hand side
    //! @return be equal or not
    template <typename RHS>
    constexpr bool operator==(const RHS& rhs) const
    {
        if constexpr (std::is_same_v<Value, RHS>)
        {
            return value == rhs;
        }
        else
        {
            return false;
        }
    }

    //! @brief Named value.
    Value value{};
    //! @brief Flag to indicate whether it has a value.
    static constexpr bool hasValue{true};
};

//! @brief Specialization for named value.
//! @tparam Name - type of name
template <typename Name>
struct NamedValue<Name, void> : public NamedValueBase<Name>
{
    //! @brief The operator (==) overloading of NamedValue struct.
    //! @tparam RHS - type of right-hand side
    //! @return be equal or not
    template <typename RHS>
    constexpr bool operator==(const RHS& /*rhs*/) const
    {
        return false;
    }

    //! @brief Flag to indicate whether it has a value.
    static constexpr bool hasValue{false};
};

//! @brief The list of elements.
//! @tparam Es - type of list of elements
template <typename... Es>
class Elements
{
public:
    //! @brief Construct a new Elements object.
    //! @param es - list of elements
    constexpr explicit Elements(const Es... es) : elems{es...} {}

    //! @brief Size of list of the elements.
    static constexpr std::size_t size{sizeof...(Es)};
    //! @brief Accumulating.
    //! @tparam Init - type of initial accumulation
    //! @tparam Func - type of callable function
    //! @param init - initial accumulation
    //! @param func - callable function
    //! @return result of accumulation
    template <typename Init, typename Func>
    constexpr auto accumulate(Init init, Func&& func) const;
    //! @brief Iteration.
    //! @tparam Func - type of callable function
    //! @param func - callable function
    template <typename Func>
    constexpr void forEach(Func&& func) const;
    //! @brief Check whether it contains the custom string.
    //! @tparam Str - type of custom string
    //! @param name - value name
    //! @return contains or not
    template <typename Str>
    static constexpr bool contains(const Str& name = {});
    //! @brief Finding by condition.
    //! @tparam Func - type of callable function
    //! @param func - callable function
    //! @return value index
    template <typename Func>
    constexpr std::size_t findIf(Func&& func) const;
    //! @brief Finding.
    //! @tparam Str - type of custom string
    //! @param name - value name
    //! @return value index
    template <typename Str>
    constexpr const auto& find(const Str& name = {}) const;
    //! @brief Finding by value.
    //! @tparam Value - type of target value
    //! @param val - target value
    //! @return value index
    template <typename Value>
    constexpr std::size_t findValue(const Value& val) const;
    //! @brief Get the pointer of value by name.
    //! @tparam Value - type of value
    //! @tparam Str - type of custom string
    //! @param name - value name
    //! @return pointer of value
    template <typename Value, typename Str>
    constexpr const Value* valuePtrOfName(const Str& name) const;
    //! @brief Get the reference of value by name.
    //! @tparam Value - type of value
    //! @tparam Str - type of custom string
    //! @param name - value name
    //! @return reference of value
    template <typename Value, typename Str>
    constexpr const Value& valueOfName(const Str& name) const;
    //! @brief Get the name by value.
    //! @tparam Value - type of target value
    //! @tparam Char - type of character in custom string
    //! @param val - target value
    //! @return value name
    template <typename Value, typename Char = char>
    constexpr auto nameOfValue(const Value& val) const;
    //! @brief Push operation of the element.
    //! @tparam Elem - type of target element
    //! @param elem - target element
    //! @return position after pushing
    template <typename Elem>
    constexpr auto push(const Elem& elem) const;
    //! @brief Insert operation of the element.
    //! @tparam Elem - type of target element
    //! @param elem - target element
    //! @return position after inserting
    template <typename Elem>
    constexpr auto insert(const Elem& elem) const;
    //! @brief Get the value.
    //! @tparam Idx - value index
    //! @return target value
    template <std::size_t Idx>
    constexpr const auto& get() const;

private:
    //! @brief Element list.
    const std::tuple<Es...> elems{};
};

template <typename... Es>
template <typename Init, typename Func>
constexpr auto Elements<Es...>::accumulate(Init init, Func&& func) const
{
    return Reflect::acc(*this, std::forward<Func>(func), std::move(init), std::make_index_sequence<size>{});
}

template <typename... Es>
template <typename Func>
constexpr void Elements<Es...>::forEach(Func&& func) const
{
    accumulate(
        0,
        [&](const auto /*func*/, const auto fld)
        {
            std::forward<Func>(func)(fld);
            return 0;
        });
}

template <typename... Es>
template <typename Str>
constexpr bool Elements<Es...>::contains(const Str& /*name*/)
{
    return (Es::NameType::template is<Str>() || ...);
}

template <typename... Es>
template <typename Func>
constexpr std::size_t Elements<Es...>::findIf(Func&& func) const
{
    return Reflect::findIf(*this, std::forward<Func>(func), std::make_index_sequence<size>{});
}

template <typename... Es>
template <typename Str>
constexpr const auto& Elements<Es...>::find(const Str& /*name*/) const
{
    constexpr std::size_t index = []() constexpr
    {
        constexpr decltype(Str::view()) names[]{Es::name...};
        for (std::size_t i = 0; i < size; ++i)
        {
            if (Str::view() == names[i])
            {
                return i;
            }
        }
        return static_cast<std::size_t>(-1);
    }();
    return get<index>();
}

template <typename... Es>
template <typename Value>
constexpr std::size_t Elements<Es...>::findValue(const Value& val) const
{
    return findIf([&val](const auto& elem) { return elem == val; });
}

template <typename... Es>
template <typename Value, typename Str>
constexpr const Value* Elements<Es...>::valuePtrOfName(const Str& name) const
{
    return accumulate(
        nullptr,
        [&name](const auto ret, const auto& elem)
        {
            if constexpr (std::is_same_v<Value, decltype(elem.value)>)
            {
                return (elem.name == name) ? &elem.value : ret;
            }
            else
            {
                return ret;
            }
        });
}

template <typename... Es>
template <typename Value, typename Str>
constexpr const Value& Elements<Es...>::valueOfName(const Str& name) const
{
    return *valuePtrOfName<Value>(name);
}

template <typename... Es>
template <typename Value, typename Char>
constexpr auto Elements<Es...>::nameOfValue(const Value& val) const
{
    return accumulate(
        std::basic_string_view<Char>{},
        [&val](const auto ret, const auto& elem) { return (elem == val) ? elem.name : ret; });
}

template <typename... Es>
template <typename Elem>
constexpr auto Elements<Es...>::push(const Elem& elem) const
{
    return std::apply([&elem](const auto&... es) { return Elements<Es..., Elem>{es..., elem}; }, elems);
}

template <typename... Es>
template <typename Elem>
constexpr auto Elements<Es...>::insert(const Elem& elem) const
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

template <typename... Es>
template <std::size_t Idx>
constexpr const auto& Elements<Es...>::get() const
{
    return std::get<Idx>(elems);
}

//! @brief Attribute in class.
//! @tparam Name - type of name
//! @tparam Value - type of target value
template <typename Name, typename Value>
struct Attr : public NamedValue<Name, Value>
{
    //! @brief Construct a new Attr object.
    //! @param val - target value
    constexpr Attr(const Name /*name*/, const Value val) : NamedValue<Name, Value>{val} {}
};

//! @brief Attribute in class.
//! @tparam Name - type of name
template <typename Name>
struct Attr<Name, void> : public NamedValue<Name, void>
{
    //! @brief Construct a new Attr object.
    constexpr explicit Attr(const Name /*name*/) {}
};

//! @brief The list of attributes.
//! @tparam As - type of list of attributes
template <typename... As>
struct AttrList : public Elements<As...>
{
    //! @brief Construct a new AttrList object.
    //! @param as - list of attributes
    constexpr explicit AttrList(const As... as) : Elements<As...>{as...} {}
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
//! @tparam Value - type of target value
template <typename Value>
struct Trait : public TraitBase<true, false>
{
};

//! @brief Specialization for trait.
//! @tparam Obj - type of target object
//! @tparam Value - type of target value
template <typename Obj, typename Value>
struct Trait<Value Obj::*> : public TraitBase<false, std::is_function_v<Value>>
{
};

//! @brief Specialization for trait.
//! @tparam Value - type of target value
template <typename Value>
struct Trait<Value*> : public TraitBase<true, std::is_function_v<Value>>
{
};

//! @brief Field in class.
//! @tparam Name - type of name
//! @tparam Value - type of target value
//! @tparam Attrs - type of list of attributes
template <typename Name, typename Value, typename Attrs>
struct Field : public Trait<Value>, public NamedValue<Name, Value>
{
    //! @brief Construct a new Field object.
    //! @param val - target value
    //! @param as - list of attributes
    constexpr Field(const Name /*name*/, const Value val, const Attrs as = {}) : NamedValue<Name, Value>{val}, attrs{as}
    {
    }

    //! @brief Attribute list.
    const Attrs attrs{};
};

//! @brief The list of fields.
//! @tparam Fs - type of list of fields
template <typename... Fs>
struct FieldList : public Elements<Fs...>
{
    //! @brief Construct a new FieldList object.
    //! @param fs - list of fields
    constexpr explicit FieldList(const Fs... fs) : Elements<Fs...>{fs...} {}
};

//! @brief Type information.
//! @tparam UDT - type of user defined data
template <typename UDT>
struct TypeInfo;

//! @brief Public base class.
//! @tparam UDT - type of user defined data
//! @tparam IsVirtual - whether it is virtual base class
template <typename UDT, bool IsVirtual = false>
struct Base
{
    //! @brief Type information.
    static constexpr auto info{TypeInfo<UDT>{}};
    //! @brief Flag to indicate whether it is virtual base class.
    static constexpr bool isVirtual{IsVirtual};
};

//! @brief The list of public base classes.
//! @tparam Bs - type of list of public base classes
template <typename... Bs>
struct BaseList : public Elements<Bs...>
{
    //! @brief Construct a new BaseList object.
    //! @param bs - list of public base classes
    constexpr explicit BaseList(const Bs... bs) : Elements<Bs...>{bs...} {}
};

//! @brief The list of type informations.
//! @tparam Ts - type of list of type informations
template <typename... Ts>
struct TypeInfoList : public Elements<Ts...>
{
    //! @brief Construct a new TypeInfoList object.
    //! @param ts - list of type informations
    constexpr explicit TypeInfoList(const Ts... ts) : Elements<Ts...>{ts...} {}
};

//! @brief The base class that includes the implementation of type information.
//! @tparam Info - type of type information
//! @tparam Bs - type of list of public base classes
template <typename Info, typename... Bs>
class TypeInfoImpl
{
public:
    //! @brief Alias for the type.
    using Type = Info;
    //! @brief Public base class list.
    static constexpr BaseList bases{Bs{}...};

    //! @brief Forward cast from derived object to base object.
    //! @tparam Derived - type of derived object
    //! @param self - derived object
    //! @return base object
    template <typename Derived>
    static constexpr auto&& forward(Derived&& self);
    //! @brief Get all virtual base objects.
    //! @return accumulation of virtual base class
    static constexpr auto virtualBases();
    //! @brief Accumulating by the DFS algorithm.
    //! @tparam Ret - type of return value
    //! @tparam Func - type of callable function
    //! @param ret - type of return value
    //! @param func - callable function
    //! @return result of accumulation
    template <typename Ret, typename Func>
    static constexpr auto dfsAcc(Ret ret, Func&& func);
    //! @brief Iteration in the DFS algorithm.
    //! @tparam Func - type of callable function
    //! @param func - callable function
    template <typename Func>
    static constexpr void dfsForEach(Func&& func);
    //! @brief Iteration variable only.
    //! @tparam Obj - type of object to be traversed
    //! @tparam Func - type of callable function
    //! @param obj - object to be traversed
    //! @param func - callable function
    template <typename Obj, typename Func>
    static constexpr void forEachVarOf(Obj&& obj, Func&& func);
};

template <typename Info, typename... Bs>
template <typename Derived>
constexpr auto&& TypeInfoImpl<Info, Bs...>::forward(Derived&& self)
{
    if constexpr (std::is_same_v<std::decay_t<Derived>, Derived>)
    {
        return static_cast<Type&&>(std::forward<Derived>(self));
    }
    else if constexpr (std::is_same_v<std::decay_t<Derived>&, Derived>)
    {
        return static_cast<Type&>(std::forward<Derived>(self));
    }
    else
    {
        return static_cast<const std::decay_t<Derived>&>(std::forward<Derived>(self));
    }
}

template <typename Info, typename... Bs>
constexpr auto TypeInfoImpl<Info, Bs...>::virtualBases()
{
    return bases.accumulate(
        Elements<>{},
        [](const auto acc, const auto base)
        {
            auto concat = base.info.virtualBases().accumulate(
                acc, [](const auto acc, const auto base) { return acc.insert(base); });
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

template <typename Info, typename... Bs>
template <typename Ret, typename Func>
constexpr auto TypeInfoImpl<Info, Bs...>::dfsAcc(Ret ret, Func&& func)
{
    return Reflect::dfsAcc<0>(
        TypeInfo<Type>{},
        std::forward<Func>(func),
        virtualBases().accumulate(
            std::forward<Func>(func)(std::move(ret), TypeInfo<Type>{}, 0),
            [&](const auto acc, const auto vb) { return std::forward<Func>(func)(std::move(acc), vb, 1); }));
}

template <typename Info, typename... Bs>
template <typename Func>
constexpr void TypeInfoImpl<Info, Bs...>::dfsForEach(Func&& func)
{
    dfsAcc(
        0,
        [&](const auto /*func*/, const auto info, const auto der)
        {
            std::forward<Func>(func)(info, der);
            return 0;
        });
}

template <typename Info, typename... Bs>
template <typename Obj, typename Func>
constexpr void TypeInfoImpl<Info, Bs...>::forEachVarOf(Obj&& obj, Func&& func)
{
    virtualBases().forEach(
        [&](const auto vb)
        {
            vb.fields.forEach(
                [&](const auto fld)
                {
                    using Fld = std::decay_t<decltype(fld)>;
                    if constexpr (!Fld::isStatic && !Fld::isFunction)
                    {
                        std::forward<Func>(func)(fld, std::forward<Obj>(obj).*(fld.value));
                    }
                });
        });
    Reflect::varInNodeV(TypeInfo<Type>{}, std::forward<Obj>(obj), std::forward<Func>(func));
}

//! @brief Attribute in class.
//! @tparam Name - type of name
template <typename Name>
Attr(Name) -> Attr<Name, void>;

//! @brief Field in class.
//! @tparam Name - type of name
//! @tparam Value - type of target value
template <typename Name, typename Value>
Field(Name, Value) -> Field<Name, Value, AttrList<>>;
} // namespace reflection
} // namespace utility
