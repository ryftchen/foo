//! @file note.cpp
//! @author ryftchen
//! @brief The definitions (note) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "note.hpp"

#ifndef __PRECOMPILED_HEADER
#include <sstream>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

namespace application::note
{
//! @brief Version number.
//! @return version
std::string version()
{
    return "0.1.0";
}

//! @brief Copyright information.
//! @return copyright
std::string copyright()
{
    return "Copyright (c) 2022-2024 ryftchen. All rights reserved.";
}

//! @brief Compiler name.
//! @return compiler
std::string compiler()
{
    std::ostringstream os;
    os <<
#ifdef __clang__
        "clang " << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__;
#elif __GNUC__
        "gcc " << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
#else
        "other compiler";
#endif // __clang__
    return std::move(os).str();
}

//! @brief Target processor.
//! @return processor
std::string processor()
{
#ifdef __TARGET_PROCESSOR
    return __TARGET_PROCESSOR;
#else
    return "other processor";
#endif // __TARGET_PROCESSOR
}

//! @brief Build date of compiling.
//! @return build date
std::string buildDate()
{
    return "" __DATE__ " " __TIME__ "";
}
} // namespace application::note
