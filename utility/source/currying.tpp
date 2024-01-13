//! @file currying.tpp
//! @author ryftchen
//! @brief The definitions (currying) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "currying.hpp"

namespace utility::currying
{
template <typename Callable, typename... CurriedArgs, typename... UncurriedArgs>
decltype(auto) Curried<Callable, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>::operator()(
    UncurriedArgs... args) const
{
    auto uncurriedArgs = std::tuple<UncurriedArgs...>(std::forward<UncurriedArgs>(args)...);
    return std::apply(callable, std::tuple_cat(curriedArgs, std::move(uncurriedArgs)));
}

template <typename Callable, typename... CurriedArgs, typename... UncurriedArgs>
template <typename... Args>
inline auto Curried<Callable, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>::curry(
    std::tuple<Args...>&& args) const&
{
    using OverlayCurried = Curried<
        Callable,
        TupleConcatResult<std::tuple<CurriedArgs...>, std::tuple<Args...>>,
        ArgsExclType<sizeof...(Args), UncurriedArgs...>>;

    return OverlayCurried(callable, std::tuple_cat(curriedArgs, args));
}

template <typename Callable, typename... CurriedArgs, typename... UncurriedArgs>
template <typename... Args>
inline auto Curried<Callable, std::tuple<CurriedArgs...>, std::tuple<UncurriedArgs...>>::curry(
    std::tuple<Args...>&& args) &&
{
    using OverlayCurried = Curried<
        Callable,
        TupleConcatResult<std::tuple<CurriedArgs...>, std::tuple<Args...>>,
        ArgsExclType<sizeof...(Args), UncurriedArgs...>>;

    return OverlayCurried(std::move(callable), std::tuple_cat(std::move(curriedArgs), args));
}
} // namespace utility::currying
