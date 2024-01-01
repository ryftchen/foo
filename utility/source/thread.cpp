//! @file thread.cpp
//! @author ryftchen
//! @brief The definitions (thread) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2024 ryftchen. All rights reserved.

#include "thread.hpp"

namespace utility::thread
{
Thread::Thread(const std::size_t size)
{
    for (std::size_t i = 0; i < size; ++i)
    {
        threadContainer.emplace_back(
            [this]()
            {
                for (;;)
                {
                    std::string name;
                    std::packaged_task<void()> task;
                    if (std::unique_lock<std::mutex> lock(mtx); true)
                    {
                        cv.wait(
                            lock,
                            [this]()
                            {
                                return (readyRelease.load() || !taskQueue.empty());
                            });

                        if (readyRelease.load() && taskQueue.empty())
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
            [this]()
            {
                return taskQueue.empty();
            });
        readyRelease.store(true);
    }

    cv.notify_all();
    for (auto& thread : threadContainer)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
} // namespace utility::thread
