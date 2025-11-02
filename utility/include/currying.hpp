//! @file currying.hpp
//! @author ryftchen
//! @brief The declarations (currying) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <tuple>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Currying-related functions in the utility module.
namespace currying
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_CURRYING";
}
extern const char* version() noexcept;

//! @brief Alias for the tuple concat result.
//! @tparam Tuples - type of tuples
template <typename... Tuples>
using TupleConcatResult = decltype(std::tuple_cat(std::declval<Tuples>()...));

//! @brief Head of function arguments.
//! @tparam IdxSeq - type of index sequence
//! @tparam Args - type of function arguments
template <typename IdxSeq, typename... Args>
struct ArgsHead;
//! @brief Head of function arguments.
//! @tparam Args - type of function arguments
template <typename... Args>
struct ArgsHead<std::integer_sequence<std::size_t>, Args...>
{
    //! @brief Alias for the empty tuple.
    using Type = std::tuple<>;
};
//! @brief Head of function arguments.
//! @tparam Args - type of function arguments
//! @tparam Rest - type of rest function arguments
//! @tparam I0 - current index of specific function arguments
//! @tparam Is - indices of sequence related to specific function arguments
template <typename Args, typename... Rest, std::size_t I0, std::size_t... Is>
struct ArgsHead<std::integer_sequence<std::size_t, I0, Is...>, Args, Rest...>
{
    //! @brief Alias for the tuple concat.
    using Type = TupleConcatResult<
        std::tuple<Args>,
        typename ArgsHead<std::integer_sequence<std::size_t, Is...>, Rest...>::Type>;
};
//! @brief Alias for the function arguments head type.
//! @tparam Num - number of specific function arguments
//! @tparam Args - type of function arguments
template <std::size_t Num, typename... Args>
using ArgsHeadType = typename ArgsHead<std::make_index_sequence<Num>, Args...>::Type;

//! @brief Exclusion of function arguments.
//! @tparam IdxSeq - type of index sequence
//! @tparam Args - type of function arguments
template <typename IdxSeq, typename... Args>
struct ArgsExcl;
//! @brief Exclusion of function arguments.
//! @tparam Args - type of function arguments
template <typename... Args>
struct ArgsExcl<std::integer_sequence<std::size_t>, Args...>
{
    //! @brief Alias for the rest tuple.
    using Type = std::tuple<Args...>;
};
//! @brief Exclusion of function arguments.
//! @tparam Args - type of function arguments
//! @tparam Rest - type of rest function arguments
//! @tparam I0 - current index of specific function arguments
//! @tparam Is - indices of sequence related to specific function arguments
template <typename Args, typename... Rest, std::size_t I0, std::size_t... Is>
struct ArgsExcl<std::integer_sequence<std::size_t, I0, Is...>, Args, Rest...>
{
    //! @brief Alias for the rest function arguments.
    using Type = typename ArgsExcl<std::integer_sequence<std::size_t, Is...>, Rest...>::Type;
};
//! @brief Alias for the function arguments exclusion type.
//! @tparam Num - number of specific function arguments
//! @tparam Args - type of function arguments
template <std::size_t Num, typename... Args>
using ArgsExclType = typename ArgsExcl<std::make_index_sequence<Num>, Args...>::Type;

//! @brief Completion of curry.
//! @tparam Call - type of original function
//! @tparam ArgsTuple - type of function arguments tuple
//! @tparam UncurriedArgsTuple - type of uncurried function arguments tuple
template <typename Call, typename ArgsTuple, typename UncurriedArgsTuple>
class Curried;
//! @brief Completion of curry.
//! @tparam Call - type of original function
//! @tparam CurriedArgs - type of curried function arguments
//! @tparam UncurriedArgs - type of uncurried function arguments
template <typename Call, typename... CurriedArgs, typename... UncurriedArgs>
class Curried<Call, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>
{
public:
    //! @brief Construct a new Curried object.
    //! @tparam CallType - type of original function
    //! @param call - wrapped function
    //! @param args - curried function arguments tuple
    template <typename CallType>
    Curried(CallType&& call, std::tuple<CurriedArgs...>&& args) :
        callable{std::forward<CallType>(call)}, curriedArgs{std::move(args)}
    {
    }

    //! @brief The operator (()) overloading of Curried class.
    //! @param args - uncurried function arguments
    //! @return function execution
    decltype(auto) operator()(UncurriedArgs... args) const;
    //! @brief To curry.
    //! @tparam Args - type of function arguments
    //! @param args - function arguments tuple
    //! @return curried result
    template <typename... Args>
    inline auto curry(std::tuple<Args...>&& args) const&;
    //! @brief To curry.
    //! @tparam Args - type of function arguments
    //! @param args - function arguments tuple
    //! @return curried result
    template <typename... Args>
    inline auto curry(std::tuple<Args...>&& args) &&;

private:
    //! @brief Wrapped function.
    const Call callable{};
    //! @brief Curried function arguments tuple.
    mutable std::tuple<CurriedArgs...> curriedArgs{};
};

template <typename Call, typename... CurriedArgs, typename... UncurriedArgs>
decltype(auto) Curried<Call, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>::operator()(
    UncurriedArgs... args) const
{
    auto uncurriedArgs = std::tuple<UncurriedArgs...>(std::forward<UncurriedArgs>(args)...);
    return std::apply(callable, std::tuple_cat(curriedArgs, std::move(uncurriedArgs)));
}

template <typename Call, typename... CurriedArgs, typename... UncurriedArgs>
template <typename... Args>
inline auto Curried<Call, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>::curry(
    std::tuple<Args...>&& args) const&
{
    using OverlayCurried = Curried<
        Call,
        TupleConcatResult<std::tuple<CurriedArgs...>, std::tuple<Args...>>,
        ArgsExclType<sizeof...(Args), UncurriedArgs...>>;
    return OverlayCurried(callable, std::tuple_cat(curriedArgs, std::move(args)));
}

template <typename Call, typename... CurriedArgs, typename... UncurriedArgs>
template <typename... Args>
inline auto Curried<Call, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>::curry(
    std::tuple<Args...>&& args) &&
{
    using OverlayCurried = Curried<
        Call,
        TupleConcatResult<std::tuple<CurriedArgs...>, std::tuple<Args...>>,
        ArgsExclType<sizeof...(Args), UncurriedArgs...>>;
    return OverlayCurried(std::move(callable), std::tuple_cat(std::move(curriedArgs), std::move(args)));
}

//! @brief Package curry.
//! @tparam CurriedArgsTuple - type of curried function arguments tuple
//! @tparam UncurriedArgsTuple - type of uncurried function arguments tuple
template <typename CurriedArgsTuple, typename UncurriedArgsTuple>
struct Curry;
//! @brief Package curry.
//! @tparam CurriedArgs - type of curried function arguments
//! @tparam UncurriedArgs - type of uncurried function arguments
template <typename... CurriedArgs, typename... UncurriedArgs>
struct Curry<std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>
{
    //! @brief To curry for internal.
    //! @tparam Ret - type of return value
    //! @tparam Args - type of function arguments
    //! @param call - wrapped function
    //! @param args - function arguments
    //! @return curried result
    template <typename Ret, typename... Args>
    static auto process(const std::function<Ret(CurriedArgs..., UncurriedArgs...)>& call, Args&&... args)
    {
        using Call = std::function<Ret(CurriedArgs..., UncurriedArgs...)>;
        using OverlayCurried = Curried<Call, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>;
        return OverlayCurried(call, std::tuple<CurriedArgs...>(std::forward<Args>(args)...));
    }
    //! @brief To curry for internal.
    //! @tparam Ret - type of return value
    //! @tparam Args - type of function arguments
    //! @param call - wrapped function
    //! @param args - function arguments
    //! @return curried result
    template <typename Ret, typename... Args>
    static auto process(std::function<Ret(CurriedArgs..., UncurriedArgs...)>&& call, Args&&... args)
    {
        using Call = std::function<Ret(CurriedArgs..., UncurriedArgs...)>;
        using OverlayCurried = Curried<Call, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>;
        return OverlayCurried(std::move(call), std::tuple<CurriedArgs...>(std::forward<Args>(args)...));
    }
};

//! @brief To curry.
//! @tparam Ret - type of return value
//! @tparam FullArgs - type of full function arguments
//! @tparam Args - type of function arguments
//! @param call - wrapped function
//! @param args - function arguments
//! @return curried result
template <typename Ret, typename... FullArgs, typename... Args>
inline auto curry(std::function<Ret(FullArgs...)>&& call, Args&&... args)
{
    using CurriedArgsTuple = ArgsHeadType<sizeof...(Args), FullArgs...>;
    using UncurriedArgsTuple = ArgsExclType<sizeof...(Args), FullArgs...>;
    using CurryWrapper = Curry<CurriedArgsTuple, UncurriedArgsTuple>;
    return CurryWrapper::process(std::move(call), std::forward<Args>(args)...);
}

//! @brief To curry.
//! @tparam Ret - type of return value
//! @tparam FullArgs - type of full function arguments
//! @tparam Args - type of function arguments
//! @param call - wrapped function
//! @param args - function arguments
//! @return curried result
template <typename Ret, typename... FullArgs, typename... Args>
inline auto curry(const std::function<Ret(FullArgs...)>& call, Args&&... args)
{
    using CurriedArgsTuple = ArgsHeadType<sizeof...(Args), FullArgs...>;
    using UncurriedArgsTuple = ArgsExclType<sizeof...(Args), FullArgs...>;
    using CurryWrapper = Curry<CurriedArgsTuple, UncurriedArgsTuple>;
    return CurryWrapper::process(call, std::forward<Args>(args)...);
}

//! @brief To curry.
//! @tparam Ret - type of return value
//! @tparam FullArgs - type of full function arguments
//! @tparam Args - type of function arguments
//! @param func - original function
//! @param args - function arguments
//! @return curried result
template <typename Ret, typename... FullArgs, typename... Args>
inline auto curry(Ret (*func)(FullArgs...), Args&&... args)
{
    std::function<Ret(FullArgs...)> call = func;
    return curry(std::move(call), std::forward<Args>(args)...);
}

//! @brief To curry.
//! @tparam Ret - type of return value
//! @tparam Obj - type of object to which the member belongs
//! @tparam FullArgs - type of full function arguments
//! @param func - original function
//! @return curried result
template <typename Ret, typename Obj, typename... FullArgs>
inline auto curry(Ret (Obj::*func)(FullArgs...))
{
    std::function<Ret(Obj*, FullArgs...)> call = func;
    return curry(std::move(call));
}

//! @brief To curry.
//! @tparam Ret - type of return value
//! @tparam Obj - type of object to which the member belongs
//! @tparam FullArgs - type of full function arguments
//! @tparam Args - type of function arguments
//! @param func - original function
//! @param caller - object to which the member belongs
//! @param args - function arguments
//! @return curried result
template <typename Ret, typename Obj, typename... FullArgs, typename... Args>
inline auto curry(Ret (Obj::*func)(FullArgs...), Obj* caller, Args&&... args)
{
    std::function<Ret(Obj*, FullArgs...)> call = func;
    return curry(std::move(call), caller, std::forward<Args>(args)...);
}

//! @brief To curry.
//! @tparam CurriedArgsList - type of curried function arguments list
//! @tparam Args - type of function arguments
//! @param curried - curried object
//! @param args - function arguments
//! @return curried result
template <typename... CurriedArgsList, typename... Args>
inline auto curry(const Curried<CurriedArgsList...>& curried, Args&&... args)
{
    return curried.curry(std::tuple<Args...>(std::forward<Args>(args)...));
}

//! @brief To curry.
//! @tparam CurriedArgsList - type of curried function arguments list
//! @tparam Args - type of function arguments
//! @param curried - curried object
//! @param args - function arguments
//! @return curried result
template <typename... CurriedArgsList, typename... Args>
inline auto curry(Curried<CurriedArgsList...>&& curried, Args&&... args)
{
    return std::move(curried).curry(std::tuple<Args...>(std::forward<Args>(args)...));
}
} // namespace currying
} // namespace utility
