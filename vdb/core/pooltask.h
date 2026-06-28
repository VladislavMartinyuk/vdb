#ifndef VDB_POOLTASK_H
#define VDB_POOLTASK_H

#include <memory>

namespace vdb {

class PoolTask
{
public:
    PoolTask() = default;
    ~PoolTask() = default;

    template<typename Func, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Func>, PoolTask>>>
    PoolTask(Func&& func)
        : m_impl(std::make_unique<Impl<std::decay_t<Func>>>(std::forward<Func>(func)))
    {}

    PoolTask(const PoolTask&) = delete;
    PoolTask& operator=(const PoolTask&) = delete;

    PoolTask(PoolTask&&) noexcept = default;
    PoolTask& operator=(PoolTask&&) noexcept = default;

    void operator()()
    {
        if (m_impl) {
            m_impl->call();
        }
    }

private:
    struct Base
    {
        Base() = default;
        virtual ~Base() = default;
        virtual void call() = 0;
    };

    template<typename Func>
    struct Impl : Base
    {
        Impl(Func&& func)
            : m_func(std::forward<Func>(func))
        {}
        void call() override { m_func(); }
        Func m_func;
    };

    std::unique_ptr<Base> m_impl;
};

}


#endif //VDB_POOLTASK_H
