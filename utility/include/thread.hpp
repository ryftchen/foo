//! @file thread.hpp
//! @author ryftchen
//! @brief The declarations (thread) in the utility module.
//! @version 1.0
//! @copyright Copyright (c) 2022-2023 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <future>
#include <queue>

//! @brief Thread-pool-related functions in the utility module.
namespace utility::thread
{
//! @brief Thread pool.
class Thread
{
public:
    //! @brief Construct a new Thread object.
    //! @param size - maximum number of threads
    explicit Thread(const std::size_t size);
    //! @brief Destroy the Thread object.
    virtual ~Thread();

    //! @brief Enqueue tasks that require multi-threading.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of function arguments
    //! @param name - thread name
    //! @param func - callable function
    //! @param args - function arguments
    //! @return result of thread execution
    template <typename Func, typename... Args>
    decltype(auto) enqueue(const std::string& name, Func&& func, Args&&... args);

private:
    //! @brief The container of target threads to join.
    std::vector<std::thread> threadContainer;
    //! @brief The queue of tasks.
    std::queue<std::pair<std::string, std::packaged_task<void()>>> taskQueue;
    //! @brief Mutex for controlling queue.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for queue. Use with mtx.
    std::condition_variable cv;
    //! @brief The synchronization condition for availability of resources.
    std::condition_variable producer;
    //! @brief Flag for ready release.
    std::atomic<bool> readyRelease{false};
};

template <typename Func, typename... Args>
decltype(auto) Thread::enqueue(const std::string& name, Func&& func, Args&&... args)
{
    std::packaged_task<std::invoke_result_t<Func, Args...>()> task(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    std::future<std::invoke_result_t<Func, Args...>> future = task.get_future();

    if (std::unique_lock<std::mutex> lock(mtx); true)
    {
        if (readyRelease.load())
        {
            throw std::logic_error("Coming to destructure.");
        }
        taskQueue.emplace(std::make_pair(name, std::move(task)));
    }
    cv.notify_one();
    return future;
}
} // namespace utility::thread
