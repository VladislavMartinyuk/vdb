#include "threadpool.h"

using namespace vdb;

ThreadPool::ThreadPool(size_t kThreads)
{
    m_workers.reserve(kThreads);
    for (int i = 0; i < kThreads; i++) {
        m_workers.emplace_back([this]() {
            while (true) {
                Task task;
                {
                    std::unique_lock lock(m_mutex);
                    m_cv.wait(lock, [this]() { return !m_queue.empty() || m_isStoped; });
                    if (m_isStoped && m_queue.empty()) {
                        return;
                    }
                    task = std::move(m_queue.front());
                    m_queue.pop();
                }
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    m_isStoped.store(true);

    m_cv.notify_all();

    for (auto &thread : m_workers) {
        thread.join();
    }
}

bool ThreadPool::hasTasks() const
{
    std::lock_guard lock(m_mutex);
    return !m_queue.empty();
}
