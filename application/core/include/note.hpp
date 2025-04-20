//! @file note.hpp
//! @author ryftchen
//! @brief The declarations (note) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <string>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Notes-related functions in the application module.
namespace note
{
//! @brief Version number.
//! @return version
inline constexpr std::string_view version()
{
    return "0.1.0";
}

//! @brief Copyright information.
//! @return copyright
inline constexpr std::string_view copyright()
{
    return "Copyright (c) 2022-2025 ryftchen. All rights reserved.";
}

//! @brief ASCII icon banner.
//! @return ASCII icon
inline constexpr std::string_view icon()
{
    // clang-format off
    return R"(  ______   ______     ______    )" "\n"
           R"( /\  ___\ /\  __ \   /\  __ \   )" "\n"
           R"( \ \  __\ \ \ \/\ \  \ \ \/\ \  )" "\n"
           R"(  \ \_\    \ \_____\  \ \_____\ )" "\n"
           R"(   \/_/     \/_____/   \/_____/ )" "\n";
    // clang-format on
}

extern std::string compiler();
extern std::string processor();
extern std::string buildDate();
} // namespace note
} // namespace application
