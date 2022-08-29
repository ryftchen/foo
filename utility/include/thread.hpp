#pragma once

#include <atomic>
#include <functional>
#include <future>
#include <queue>

namespace util_thread
{
class Thread
{
public:
    explicit Thread(uint32_t count);
    template <typename Func, typename... Args>
    decltype(auto) enqueue(const std::string& name, Func&& func, Args&&... args);
    virtual ~Thread();

private:
    std::vector<std::thread> threadVector;
    std::queue<std::pair<std::string, std::packaged_task<void()>>> taskQueue;
    mutable std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable producer;
    std::atomic<bool> releaseReady{false};
};

template <typename Func, typename... Args>
decltype(auto) Thread::enqueue(const std::string& name, Func&& func, Args&&... args)
{
    std::packaged_task<std::invoke_result_t<Func, Args...>()> task(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    std::future<std::invoke_result_t<Func, Args...>> future = task.get_future();

    if (std::unique_lock<std::mutex> lock(queueMutex); true)
    {
        if (releaseReady)
        {
            throw std::logic_error("Coming to destructure.");
        }
        taskQueue.emplace(std::make_pair(name, std::move(task)));
    }
    condition.notify_one();
    return future;
}
} // namespace util_thread
