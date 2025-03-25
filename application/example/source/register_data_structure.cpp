//! @file register_data_structure.cpp
//! @author ryftchen
//! @brief The definitions (register_data_structure) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "register_data_structure.hpp"

namespace application::reg_ds
{
//! @brief Register version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}
} // namespace application::reg_ds
