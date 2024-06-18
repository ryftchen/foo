//! @file note.cpp
//! @author ryftchen
//! @brief The definitions (note) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "note.hpp"

namespace application::note
{
//! @brief Compiler name.
//! @return compiler
std::string compiler()
{
//! @cond
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#ifdef __clang__
    return "clang " TO_STRING(__clang_major__) "." TO_STRING(__clang_minor__) "." TO_STRING(__clang_patchlevel__) "";
#elif __GNUC__
    return "gcc " TO_STRING(__GNUC__) "." TO_STRING(__GNUC_MINOR__) "." TO_STRING(__GNUC_PATCHLEVEL__) "";
#else
    return "unknown compiler";
#endif // __clang__
//! @endcond
#undef STRINGIFY
#undef TO_STRING
}

//! @brief Target processor.
//! @return processor
std::string processor()
{
#ifdef __TARGET_PROCESSOR
    return __TARGET_PROCESSOR;
#else
    return "unknown processor";
#endif // __TARGET_PROCESSOR
}

//! @brief Build date of compiling.
//! @return build date
std::string buildDate()
{
#if defined(__DATE__) && defined(__TIME__)
    return "" __DATE__ " " __TIME__ "";
#else
    return "unknown build date";
#endif // defined(__DATE__) && defined(__TIME__)
}
} // namespace application::note
