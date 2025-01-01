//! @file action.cpp
//! @author ryftchen
//! @brief The definitions (action) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "action.hpp"

namespace application::action
{
//! @brief Get memory pool when making multi-threading.
//! @return reference of the ResourcePool object
ResourcePool& resourcePool()
{
    static ResourcePool pooling{};
    return pooling;
}
} // namespace application::action
