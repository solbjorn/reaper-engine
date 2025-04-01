#ifndef __XRCORE_TASK_H
#define __XRCORE_TASK_H

#include <oneapi/tbb/task_group.h>

class alignas(16) xr_task_group : public oneapi::tbb::task_group
{
private:
    oneapi::tbb::detail::d1::small_object_allocator alloc{};

public:
    xr_task_group(oneapi::tbb::detail::d1::small_object_allocator& alloc) : alloc(alloc) {}

    void put() { alloc.delete_object<xr_task_group>(this); }

    void wait_put()
    {
        wait();
        put();
    }

    void cancel_put()
    {
        cancel();
        put();
    }
};

inline xr_task_group& xr_task_group_get()
{
    oneapi::tbb::detail::d1::small_object_allocator alloc;
    return *alloc.new_object<xr_task_group>(alloc);
}

template <typename... Fs>
inline xr_task_group& xr_task_group_run(Fs&&... fs)
{
    auto& tg = xr_task_group_get();

    for (const auto f : {fs...})
        tg.run(f);

    return tg;
}

#endif /* __XRCORE_TASK_H */
