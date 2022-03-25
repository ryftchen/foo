#pragma once
#include <atomic>
#include <functional>
#include <future>
#include <queue>
#include "log.hpp"

class Thread
{
public:
    Thread(uint32_t count);
    template <typename Function, typename... Args>
    decltype(auto) enqueue(const std::string& name, Function&& fun, Args&&... args);
    ~Thread();

private:
    std::vector<std::thread> threadVector;
    std::queue<std::pair<std::string, std::packaged_task<void()>>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable consumer;
    std::condition_variable producer;
    std::atomic<bool> releaseReady;
};

template <typename Function, typename... Args>
decltype(auto) Thread::enqueue(const std::string& name, Function&& fun, Args&&... args)
{
    std::packaged_task<std::invoke_result_t<Function, Args...>()> task(
        std::bind(std::forward<Function>(fun), std::forward<Args>(args)...));
    std::future<std::invoke_result_t<Function, Args...>> future = task.get_future();

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        try
        {
            if (releaseReady)
            {
                throw std::runtime_error("System error.");
            }
        }
        catch (std::runtime_error const& error)
        {
            LOGGER_ERR(error.what());
        }
        taskQueue.emplace(std::make_pair(name, std::move(task)));
    }
    consumer.notify_one();
    return future;
}
