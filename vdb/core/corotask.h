#ifndef VDB_TASK_H
#define VDB_TASK_H

#include <coroutine>
#include <exception>

namespace vdb {

template<typename T>
struct CoroTask
{
    struct promise_type
    {
        using CoroHandler = std::coroutine_handle<promise_type>;
        auto get_return_object() { return CoroTask{CoroHandler::from_promise(*this)}; }

        auto initial_suspend() noexcept { return std::suspend_never{}; }
        auto final_suspend() noexcept
        {
            struct FinalSuspend
            {
                bool await_ready() noexcept { return false; }
                std::coroutine_handle<> await_suspend(CoroHandler handler) noexcept
                {
                    if (handler.promise().Caller) {
                        return handler.promise().Caller;
                    }
                    return std::noop_coroutine();
                }
                void await_resume() noexcept {}
            } fSuspend;
            return fSuspend;
        }

        void return_value(T value) noexcept { currentValue = std::move(value); }
        void unhandled_exception() { std::terminate(); }

        T currentValue;
        std::coroutine_handle<> Caller;
    };

    T current_value() const { return m_handler.promise().currentValue; }
    bool resume() { return m_handler ? (m_handler.resume(), !m_handler.done()) : false; }
    bool await_ready() noexcept { return false; }
    void await_suspend(std::coroutine_handle<> coroutine) noexcept
    {
        m_handler.promise().Caller = coroutine;
    }
    T await_resume() noexcept
    {
        return m_handler.promise().currentValue;
    }

    using CoroHandler = std::coroutine_handle<promise_type>;

    CoroTask(CoroHandler handler)
        : m_handler(handler)
    {}
    CoroTask(const CoroTask&) = delete;
    CoroTask& operator=(const CoroTask&) = delete;
    CoroTask(CoroTask&& rhs) noexcept
        : m_handler(rhs.m_handler)
    {
        rhs.m_handler = nullptr;
    };
    CoroTask& operator=(CoroTask&& rhs) noexcept
    {
        m_handler = std::move(rhs.m_handler);
        return *this;
    }
    ~CoroTask()
    {
        if (m_handler) {
            m_handler.destroy();
        }
    }

    CoroHandler m_handler;
};

} // namespace vdb

#endif //VDB_TASK_H
