//! @file macro.hpp
//! @author ryftchen
//! @brief The declarations (macro) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdio>
#include <cstdlib>

//! @brief Mark to ignore.
#define MACRO_IGNORE(...)                       \
    do                                          \
    {                                           \
        utility::macro::ignoreAll(__VA_ARGS__); \
    }                                           \
    while (0)

//! @brief Do stringification.
#define MACRO_STRINGIFY(x) MACRO_STRINGIFY_OP(x)
//! @brief The stringification operator.
#define MACRO_STRINGIFY_OP(x) #x

//! @brief Do concatenation.
#define MACRO_CONCAT(x, y) MACRO_CONCAT_OP(x, y)
//! @brief The concatenation operator.
#define MACRO_CONCAT_OP(x, y) x##y

//! @brief Always assert.
#define MACRO_ASSERT(cond)                          \
    (static_cast<bool>(cond) ? static_cast<void>(0) \
                             : utility::macro::assertFailure(#cond, __FILE__, __LINE__, __func__))

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Public-macro-related functions in the utility module.
namespace macro
{
extern const char* version() noexcept;

//! @brief Safely ignore all provided arguments.
//! @tparam Args - type of all provided arguments
//! @param args - all provided arguments
template <typename... Args>
[[gnu::always_inline]] inline constexpr void ignoreAll(Args&&... args)
{
    (static_cast<void>(args), ...);
}

//! @brief Assertion failed.
//! @param expr - condition expression
//! @param file - file path
//! @param line - line number
//! @param func - function name
[[gnu::always_inline]] inline void assertFailure(
    const char* const expr, const char* const file, const int line, const char* const func)
{
    std::fprintf(::stderr, "%s:%d: %s: Assertion '%s' failed.\n", file, line, func, expr);
    std::abort();
}
} // namespace macro
} // namespace utility
