//! @file currying.cpp
//! @author ryftchen
//! @brief The definitions (currying) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "currying.hpp"

namespace utility::currying
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version()
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace utility::currying
