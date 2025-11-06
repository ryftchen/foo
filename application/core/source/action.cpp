//! @file action.cpp
//! @author ryftchen
//! @brief The definitions (action) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#include "action.hpp"

namespace application::action
{
std::atomic_bool Awaitable::active = false;

Awaitable::Awaitable(const std::coroutine_handle<promise_type>& handle) : handle{handle}
{
    if (active.load())
    {
        throw std::runtime_error{"There can only be one awaitable instance active at any given time."};
    }
    active.store(true);
}

Awaitable::~Awaitable()
{
    active.store(false);
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
