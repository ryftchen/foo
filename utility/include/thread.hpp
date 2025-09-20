//! @file thread.hpp
//! @author ryftchen
//! @brief The declarations (thread) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <functional>
#include <future>
#include <queue>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Thread-pool-related functions in the utility module.
namespace thread
{
extern const char* version() noexcept;

//! @brief Thread pool.
class Thread
{
public:
    //! @brief Construct a new Thread object.
    //! @param capacity - maximum number of threads
    explicit Thread(const std::size_t capacity);
    //! @brief Destroy the Thread object.
    virtual ~Thread();
    //! @brief Construct a new Thread object.
    Thread(const Thread&) = delete;
    //! @brief Construct a new Thread object.
    Thread(Thread&&) noexcept = delete;
    //! @brief The operator (=) overloading of Thread class.
    //! @return reference of the Thread object
    Thread& operator=(const Thread&) = delete;
    //! @brief The operator (=) overloading of Thread class.
    //! @return reference of the Thread object
    Thread& operator=(Thread&&) noexcept = delete;

    //! @brief Enqueue tasks that require multi-threading.
    //! @tparam Func - type of callable function
    //! @tparam Args - type of function arguments
    //! @param name - thread name
    //! @param func - callable function
    //! @param args - function arguments
    //! @return result of thread execution
    template <typename Func, typename... Args>
    decltype(auto) enqueue(const std::string_view name, Func&& func, Args&&... args);

private:
    //! @brief The container of target threads to join.
    std::vector<std::thread> thdColl;
    //! @brief The queue of tasks.
    std::queue<std::pair<std::string, std::packaged_task<void()>>> taskQueue;
    //! @brief Mutex for controlling queue.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for queue. Use with mtx.
    std::condition_variable cond;
    //! @brief The synchronization condition for availability of resources.
    std::condition_variable producer;
    //! @brief Flag for ready release.
    std::atomic_bool releaseReady{false};

    //! @brief The work loop.
    void workLoop();
};

template <typename Func, typename... Args>
decltype(auto) Thread::enqueue(const std::string_view name, Func&& func, Args&&... args)
{
    std::packaged_task<std::invoke_result_t<Func, Args...>()> task(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    auto future = task.get_future();

    std::unique_lock<std::mutex> lock(mtx);
    if (releaseReady.load())
    {
        throw std::logic_error{"The thread resource is coming to be released."};
    }
    taskQueue.emplace(std::make_pair(name, std::move(task)));
    lock.unlock();
    cond.notify_one();

    return future;
}
} // namespace thread
} // namespace utility
