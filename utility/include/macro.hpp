//! @file macro.hpp
//! @author ryftchen
//! @brief The declarations (macro) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdio>
#include <cstdlib>
#include <utility>

//! @brief Do stringification.
#define MACRO_STRINGIFY(x) MACRO_STRINGIFY_OP(x)
//! @brief The stringification operator.
#define MACRO_STRINGIFY_OP(x) #x

//! @brief Do concatenation.
#define MACRO_CONCAT(x, y) MACRO_CONCAT_OP(x, y)
//! @brief The concatenation operator.
#define MACRO_CONCAT_OP(x, y) x##y

//! @brief Mark to ignore.
#define MACRO_IGNORE(...)                       \
    do                                          \
    {                                           \
        utility::macro::ignoreAll(__VA_ARGS__); \
    }                                           \
    while (false)

//! @brief Always assert.
#define MACRO_ASSERT(expr) \
    (static_cast<bool>(expr) ? static_cast<void>(0) : utility::macro::assertFail(#expr, __FILE__, __LINE__, __func__))

//! @brief Defer process.
#define MACRO_DEFER(call) \
    [[maybe_unused]] const auto MACRO_CONCAT(_deferGuard, __LINE__) = utility::macro::makeDeferral(call)

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Public-macro-related functions in the utility module.
namespace macro
{
extern const char* version() noexcept;

//! @brief Safely ignore all provided arguments.
//! @tparam Args - type of all provided arguments
template <typename... Args>
inline constexpr void ignoreAll(Args&&... /*args*/) noexcept
{
}

//! @brief Assertion failed.
//! @param assertion - assertion expression
//! @param file - file path
//! @param line - line number
//! @param function - function name
[[noreturn]] inline void assertFail(
    const char* const assertion, const char* const file, const unsigned int line, const char* const function) noexcept
{
    std::fprintf(::stderr, "%s:%d: %s: Assertion `%s` failed.\n", file, line, function, assertion);
    std::abort();
}

//! @brief Defer the execution of the callable until it is out of scope.
//! @tparam T - type of deferred callable
template <typename T>
class Defer
{
public:
    //! @brief Construct a new Defer object.
    //! @tparam U - type of deferred callable
    //! @param deferred - deferred callable
    template <typename U>
    explicit Defer(U&& deferred) : deferred{std::forward<U>(deferred)}
    {
    }
    //! @brief Destroy the Defer object.
    virtual ~Defer() { deferred(); }
    //! @brief Construct a new Defer object.
    Defer(const Defer&) = delete;
    //! @brief Construct a new Defer object.
    Defer(Defer&&) = delete;
    //! @brief The operator (=) overloading of Defer class.
    //! @return reference of the Defer object
    Defer& operator=(const Defer&) = delete;
    //! @brief The operator (=) overloading of Defer class.
    //! @return reference of the Defer object
    Defer& operator=(Defer&&) = delete;

private:
    //! @brief The deferred callable.
    T deferred{};
};
//! @brief Create a deferral from a callable.
//! @tparam T - type of deferred callable
//! @param deferred - deferred callable
//! @return deferral
template <typename T>
inline constexpr auto makeDeferral(T&& deferred) noexcept
{
    return Defer<std::decay_t<T>>(std::forward<T>(deferred));
}
} // namespace macro
} // namespace utility
