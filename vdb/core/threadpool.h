#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "pooltask.h"

namespace vdb {

class ThreadPool
{
public:
    ThreadPool(std::size_t kThreads);
    ~ThreadPool();

    bool hasTasks() const;

    template<typename Func, typename... Args>
    auto enqueue(Func &&func, Args &&...args)
        -> std::future<std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>>
    {
        using ReturnType = std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>;

        auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(
            [f = std::forward<Func>(func), ... args = std::forward<Args>(args)]() mutable {
                if constexpr (std::is_void_v<ReturnType>) {
                    std::invoke(std::move(f), std::move(args)...);
                } else {
                    auto result = std::invoke(std::move(f), std::move(args)...);
                    return result;
                }
            });

        auto future = packagedTask->get_future();

        PoolTask task([ptask = std::move(packagedTask)]() { (*ptask)(); });
        {
            std::unique_lock lock(m_mutex);
            m_queue.push(std::move(task));
        }
        m_cv.notify_one();

        return future;
    }

    template<typename Func, typename Callback, typename... Args>
    void enqueueWithCallback(Func &&func, Callback &&cb, Args &&...args)
    {
        using ReturnType = std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>;

        auto task = PoolTask{[f = std::forward<Func>(func),
                          cb = std::forward<Callback>(cb),
                          ... args = std::forward<Args>(args)]() mutable {
            if constexpr (std::is_void_v<ReturnType>) {
                std::invoke(std::move(f), std::move(args)...);
                std::invoke(std::move(cb));
            } else {
                auto result = std::invoke(std::move(f), std::move(args)...);
                std::invoke(std::move(cb), std::move(result));
            }
        }};

        {
            std::unique_lock lock(m_mutex);
            m_queue.push(std::move(task));
        }
        m_cv.notify_one();
    }

private:
    mutable std::mutex m_mutex;
    std::queue<PoolTask> m_queue;
    std::condition_variable m_cv;
    std::vector<std::thread> m_workers;
    std::atomic_bool m_isStopped{false};
};

} // namespace vdb

#endif // THREADPOOL_H
