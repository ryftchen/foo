//! @file thread.cpp
//! @author ryftchen
//! @brief The definitions (thread) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2026 ryftchen. All rights reserved.

#include "thread.hpp"

#include <algorithm>

namespace utility::thread
{
//! @brief Function version number.
//! @return version number (major.minor.patch)
const char* version() noexcept
{
    static const char* const ver = "0.1.0";
    return ver;
}

Thread::Thread(const std::size_t capacity)
{
    thdColl.reserve(capacity);
    std::generate_n(std::back_inserter(thdColl), capacity, [this]() { return std::thread{[this]() { workLoop(); }}; });
}

void Thread::workLoop()
{
    std::string thdName{};
    std::packaged_task<void()> thdTask{};
    for (constexpr std::uint8_t nameLen = 15;;)
    {
        if (std::unique_lock<std::mutex> lock(mtx); true)
        {
            cond.wait(lock, [this]() { return releaseReady.load() || !taskQueue.empty(); });
            if (releaseReady.load() && taskQueue.empty())
            {
                return;
            }

            std::tie(thdName, thdTask) = std::move(taskQueue.front());
            taskQueue.pop();
            if (taskQueue.empty())
            {
                lock.unlock();
                producer.notify_one();
            }
        }

        if (!thdName.empty())
        {
            if (thdName.size() > nameLen)
            {
                thdName.resize(nameLen);
            }
            ::pthread_setname_np(::pthread_self(), thdName.c_str());
        }
        if (thdTask.valid())
        {
            thdTask();
        }
    }
}

Thread::~Thread()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        producer.wait(lock, [this]() { return taskQueue.empty(); });
        releaseReady.store(true);
    }

    cond.notify_all();
    std::ranges::for_each(
        thdColl,
        [](auto& thd)
        {
            if (thd.joinable())
            {
                thd.join();
            }
        });
}
} // namespace utility::thread
