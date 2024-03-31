//! @file reflection.tpp
//! @author ryftchen
//! @brief The definitions (reflection) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#include "reflection.hpp"

namespace utility::reflection
{
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
} // namespace utility::reflection
