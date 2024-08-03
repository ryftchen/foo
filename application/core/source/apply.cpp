//! @file apply.cpp
//! @author ryftchen
//! @brief The definitions (apply) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "apply.hpp"

namespace application::apply
{
//! @brief Get memory pool when making multi-threading.
//! @return reference of the ResourcePool object
ResourcePool& resourcePool()
{
    static ResourcePool pooling{};
    return pooling;
}
} // namespace application::apply
