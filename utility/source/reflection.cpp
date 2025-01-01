//! @file reflection.cpp
//! @author ryftchen
//! @brief The definitions (reflection) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "reflection.hpp"

namespace utility::reflection
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace utility::reflection
