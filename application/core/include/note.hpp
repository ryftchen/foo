//! @file note.hpp
//! @author ryftchen
//! @brief The declarations (note) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#pragma once

#ifndef __PRECOMPILED_HEADER
#include <string>
#else
#include "application/pch/precompiled_header.hpp"
#endif // __PRECOMPILED_HEADER

//! @brief The application module.
namespace application // NOLINT (modernize-concat-nested-namespaces)
{
//! @brief Notes-related functions in the application module.
namespace note
{
extern std::string copyright();
extern std::string compiler();
extern std::string processor();
extern std::string buildDate();
} // namespace note
} // namespace application
