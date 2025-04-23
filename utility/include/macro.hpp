//! @file macro.hpp
//! @author ryftchen
//! @brief The declarations (macro) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <cstdio>
#include <cstdlib>

//! @brief Mark as unused.
#define MACRO_UNUSED(param)       \
    do                            \
    {                             \
        static_cast<void>(param); \
    }                             \
    while (0)

//! @brief Always assert.
#define MACRO_ASSERT(cond)                          \
    (static_cast<bool>(cond) ? static_cast<void>(0) \
                             : utility::macro::assertFailure(#cond, __FILE__, __LINE__, __func__))

//! @brief Do stringify.
#define MACRO_STRINGIFY(x) MACRO_INTERNAL_STRINGIFY(x)
//! @brief Do stringify for internal use.
#define MACRO_INTERNAL_STRINGIFY(x) #x

//! @brief Do concatenation.
#define MACRO_CONCAT(x, y) MACRO_INTERNAL_CONCAT(x, y)
//! @brief Do concatenation for internal use.
#define MACRO_INTERNAL_CONCAT(x, y) x##y

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Public-macro-related functions in the utility module.
namespace macro
{
extern const char* version() noexcept;

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
