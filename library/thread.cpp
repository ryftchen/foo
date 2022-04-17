#include "thread.hpp"

Thread::Thread(uint32_t count) : releaseReady(false)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        threadVector.emplace_back(
            [this]() -> decltype(auto)
            {
                for (;;)
                {
                    std::string name;
                    std::packaged_task<void()> task;
                    if (std::unique_lock<std::mutex> lock(queueMutex); true)
                    {
                        condition.wait(
                            lock,
                            [this]() -> decltype(auto)
                            {
                                return releaseReady || !taskQueue.empty();
                            });

                        if (releaseReady && taskQueue.empty())
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
                        pthread_setname_np(pthread_self(), name.c_str());
                    }
                    task();
                }
            });
    }
}

Thread::~Thread()
{
    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        producer.wait(
            lock,
            [this]() -> decltype(auto)
            {
                return taskQueue.empty();
            });
        releaseReady = true;
    }

    condition.notify_all();
    for (auto& thread : threadVector)
    {
        thread.join();
    }
}