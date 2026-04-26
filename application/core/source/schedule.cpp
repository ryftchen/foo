//! @file schedule.cpp
//! @author ryftchen
//! @brief The definitions (schedule) in the application module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "schedule.hpp"

namespace application::schedule
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

//! @brief Enter the next phase of the coroutine.
//! @param awaitable - awaitable instance
void enterNextPhase(Awaitable& awaitable)
{
    if (!awaitable.done())
    {
        awaitable.resume();
    }
}

bool NativeManager::empty() const
{
    return nativeCategories.none();
}

void NativeManager::reset()
{
    nativeCategories.reset();
}

bool ExtraManager::empty() const
{
    return !extraHelping
        && std::ranges::none_of(extraChecklist, [](const auto& pair) { return pair.second.present(); });
}

void ExtraManager::reset()
{
    extraHelping = false;
    std::ranges::for_each(extraChecklist, [](const auto& pair) { pair.second.clear(); });
}

bool TaskScheduler::empty() const
{
    return NativeManager::empty() && ExtraManager::empty();
}

void TaskScheduler::reset()
{
    NativeManager::reset();
    ExtraManager::reset();
}
} // namespace application::schedule
