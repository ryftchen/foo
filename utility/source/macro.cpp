//! @file macro.cpp
//! @author ryftchen
//! @brief The definitions (macro) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "macro.hpp"

namespace utility::macro
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace utility::macro
