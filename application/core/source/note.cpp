//! @file note.cpp
//! @author ryftchen
//! @brief The definitions (note) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "note.hpp"

#include "utility/include/macro.hpp"

namespace application::note
{
//! @brief Commit revision.
//! @return revision
std::string revision()
{
#ifdef _COMMIT_REVISION
    return _COMMIT_REVISION;
#else
#pragma message("Unknown revision at compile time.")
    return "unknown revision";
#endif // _COMMIT_REVISION
}

//! @brief Compiler name.
//! @return compiler
std::string compiler()
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
std::string processor()
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
std::string date()
{
#if defined(__DATE__) && defined(__TIME__)
    return "" __DATE__ " " __TIME__ "";
#else
#pragma message("Unknown date at compile time.")
    return "unknown date";
#endif // defined(__DATE__) && defined(__TIME__)
}
} // namespace application::note
