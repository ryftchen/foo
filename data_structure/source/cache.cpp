//! @file cache.cpp
//! @author ryftchen
//! @brief The definitions (cache) in the data structure module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "cache.hpp"

namespace date_structure::cache
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace date_structure::cache
