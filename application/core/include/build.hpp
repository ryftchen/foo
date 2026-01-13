//! @file build.hpp
//! @author ryftchen
//! @brief The declarations (build) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#pragma once

#ifndef _PRECOMPILED_HEADER
#include <string>
#else
#include "application/pch/precompiled_header.hpp"
#endif

//! @brief The application module.
namespace application // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Building-related functions in the application module.
namespace build
{
//! @brief Version number.
//! @return version
constexpr std::string_view version()
{
    return "0.1.0";
}

//! @brief Copyright information.
//! @return copyright
constexpr std::string_view copyright()
{
    return "Copyright (c) 2022-2026 ryftchen. All rights reserved.";
}

//! @brief ASCII banner.
//! @return banner
constexpr std::string_view banner()
{
    // clang-format off
    return R"(  ______   ______     ______    )" "\n"
           R"( /\  ___\ /\  __ \   /\  __ \   )" "\n"
           R"( \ \  __\ \ \ \/\ \  \ \ \/\ \  )" "\n"
           R"(  \ \_\    \ \_____\  \ \_____\ )" "\n"
           R"(   \/_/     \/_____/   \/_____/ )" "\n";
    // clang-format on
}

extern std::string revision();
extern std::string compiler();
extern std::string processor();
extern std::string date();
} // namespace build
} // namespace application
