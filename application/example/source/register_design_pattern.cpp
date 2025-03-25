//! @file register_design_pattern.cpp
//! @author ryftchen
//! @brief The definitions (register_design_pattern) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "register_design_pattern.hpp"

namespace application::reg_dp
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace application::reg_dp
