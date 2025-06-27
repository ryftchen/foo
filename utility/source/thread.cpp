//! @file thread.cpp
//! @author ryftchen
//! @brief The definitions (thread) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

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

Thread::Thread(const std::size_t size)
{
    thdColl.reserve(size);
    std::generate_n(std::back_inserter(thdColl), size, [this]() { return std::thread{[this]() { workLoop(); }}; });
}

void Thread::workLoop()
{
    for (;;)
    {
        std::string thdName{};
        std::packaged_task<void()> thdTask{};
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
            ::pthread_setname_np(::pthread_self(), thdName.c_str());
        }
        thdTask();
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
    std::for_each(
        thdColl.begin(),
        thdColl.end(),
        [](auto& thd)
        {
            if (thd.joinable())
            {
                thd.join();
            }
        });
}
} // namespace utility::thread
