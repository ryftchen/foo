//! @file note.cpp
//! @author ryftchen
//! @brief The definitions (note) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "note.hpp"

#ifndef _PRECOMPILED_HEADER
#include <format>
#else
#include "application/pch/precompiled_header.hpp"
#endif // _PRECOMPILED_HEADER

#include "utility/include/macro.hpp"

namespace application::note
{
//! @brief Compiler name.
//! @return compiler
static constexpr std::string_view compiler()
{
#ifdef __clang__
    return "clang " MACRO_STRINGIFY(__clang_major__) "." MACRO_STRINGIFY(__clang_minor__) "." MACRO_STRINGIFY(
        __clang_patchlevel__) "";
#elif __GNUC__
    return "gcc " MACRO_STRINGIFY(__GNUC__) "." MACRO_STRINGIFY(__GNUC_MINOR__) "." MACRO_STRINGIFY(
        __GNUC_PATCHLEVEL__) "";
#else
#pragma message("Unknown compiler at compile time.")
    return "unknown compiler";
#endif // __clang__, __GNUC__
}

//! @brief Target processor.
//! @return processor
static constexpr std::string_view processor()
{
#ifdef _TARGET_PROCESSOR
    return _TARGET_PROCESSOR;
#else
#pragma message("Unknown processor at compile time.")
    return "unknown processor";
#endif // _TARGET_PROCESSOR
}

//! @brief Build date.
//! @return date
static constexpr std::string_view date()
{
#if defined(__DATE__) && defined(__TIME__)
    return "" __DATE__ " " __TIME__ "";
#else
#pragma message("Unknown date at compile time.")
    return "unknown date";
#endif // defined(__DATE__) && defined(__TIME__)
}

//! @brief Program overview.
//! @return overview
std::string overview()
{
    return std::format(
        "\033[7m\033[49m{}"
#ifndef NDEBUG
        "            DEBUG VERSION {} "
#else
        "          RELEASE VERSION {} "
#endif // NDEBUG
        "\033[0m\n{}\nBuilt with {} for {} on {}.\n",
        banner(),
        version(),
        copyright(),
        compiler(),
        processor(),
        date());
}
} // namespace application::note
