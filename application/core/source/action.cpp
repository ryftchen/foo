//! @file action.cpp
//! @author ryftchen
//! @brief The definitions (action) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "action.hpp"

namespace application::action
{
Awaitable::~Awaitable()
{
    if (handle)
    {
        handle.destroy();
    }
}

void Awaitable::resume() const
{
    if (handle)
    {
        handle.resume();
    }
}

bool Awaitable::done() const
{
    return !handle || handle.done();
}
} // namespace application::action
