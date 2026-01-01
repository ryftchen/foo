//! @file fsm.cpp
//! @author ryftchen
//! @brief The definitions (fsm) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "fsm.hpp"

namespace utility::fsm
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace utility::fsm
