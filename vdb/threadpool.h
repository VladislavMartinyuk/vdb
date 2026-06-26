#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace vdb {

using Task = std::function<void()>;

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
                    std::invoke(f, args...);
                } else {
                    auto result = std::invoke(f, args...);
                    return result;
                }
            });

        auto future = packagedTask->get_future();

        auto task = Task{[ptask = packagedTask]() mutable { (*ptask)(); }};
        {
            std::unique_lock lock(m_mutex);
            m_queue.push(std::move(task));
        }
        m_cv.notify_one();

        return future;
    }

    template<typename Func, typename Callback, typename... Args>
    void enqueuWithCallback(Func &&func, Callback &&cb, Args &&...args)
    {
        using ReturnType = std::invoke_result_t<std::decay_t<Func>, std::decay_t<Args>...>;

        auto task = Task{[f = std::forward<Func>(func),
                          cb = std::forward<Callback>(cb),
                          ... args = std::forward<Args>(args)]() mutable {
            if constexpr (std::is_void_v<ReturnType>) {
                f(args...);
                cb();
            } else {
                auto result = f(args...);
                cb(std::move(result));
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
    std::queue<Task> m_queue;
    std::condition_variable m_cv;
    std::vector<std::thread> m_workers;
    std::atomic_bool m_isStoped{false};
};

} // namespace vdb

#endif // THREADPOOL_H
