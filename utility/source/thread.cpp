//! @file thread.cpp
//! @author ryftchen
//! @brief The definitions (thread) in the utility module.
//! @version 0.1
//! @copyright Copyright (c) 2022-2023

#include "thread.hpp"

namespace utility::thread
{
Thread::Thread(const std::uint32_t count)
{
    for (std::uint32_t i = 0; i < count; ++i)
    {
        threadVector.emplace_back(
            [this]() -> decltype(auto)
            {
                for (;;)
                {
                    std::string name;
                    std::packaged_task<void()> task;
                    if (std::unique_lock<std::mutex> lock(mtx); true)
                    {
                        cv.wait(
                            lock,
                            [this]() -> decltype(auto)
                            {
                                return (releaseReady.load() || !taskQueue.empty());
                            });

                        if (releaseReady.load() && taskQueue.empty())
                        {
                            return;
                        }

                        name = std::get<0>(taskQueue.front());
                        task = std::move(std::get<1>(taskQueue.front()));
                        taskQueue.pop();
                        if (taskQueue.empty())
                        {
                            producer.notify_one();
                        }
                    }
                    if (!name.empty())
                    {
                        ::pthread_setname_np(::pthread_self(), name.c_str());
                    }
                    task();
                }
            });
    }
}

Thread::~Thread()
{
    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        producer.wait(
            lock,
            [this]() -> decltype(auto)
            {
                return taskQueue.empty();
            });
        releaseReady.store(true);
    }

    cv.notify_all();
    for (auto& thread : threadVector)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
} // namespace utility::thread
