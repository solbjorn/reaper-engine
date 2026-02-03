#include "stdafx.h"

#include "sleep.h"

namespace xr
{
namespace detail
{
class opaque_timer;
}
} // namespace xr

template <>
struct std::default_delete<xr::detail::opaque_timer>
{
    constexpr void operator()(xr::detail::opaque_timer* ptr) const noexcept { CloseHandle(ptr); }
};

namespace xr
{
namespace detail
{
namespace
{
std::unique_ptr<xr::detail::opaque_timer> timer;
}

void timer_create()
{
    void* ptr = CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_QUERY_STATE | TIMER_MODIFY_STATE);
    xr::detail::timer.reset(static_cast<xr::detail::opaque_timer*>(ptr));
}

void timer_destroy() { xr::detail::timer.reset(); }

void nanosleep(std::chrono::nanoseconds count)
{
    constexpr auto max = std::chrono::microseconds{8};

    LARGE_INTEGER due;
    due.QuadPart = -((count > max ? max : count) / 100).count();

    SetWaitableTimerEx(xr::detail::timer.get(), &due, 0, nullptr, nullptr, nullptr, 0);
    WaitForSingleObjectEx(xr::detail::timer.get(), INFINITE, true);
}
} // namespace detail

// Based on https://blog.bearcats.nl/perfect-sleep-function/
tmc::task<void> sleep(std::chrono::nanoseconds count)
{
    auto curr = std::chrono::high_resolution_clock::now();
    decltype(curr) target = curr + count;

    while (target - curr >= std::chrono::microseconds{1900})
    {
        Sleep(1);
        curr = std::chrono::high_resolution_clock::now();
    }

    if (target - curr >= std::chrono::microseconds{950})
    {
        Sleep(0);
        curr = std::chrono::high_resolution_clock::now();
    }

    for (;;)
    {
        constexpr auto max = std::chrono::microseconds{475};

        const std::chrono::nanoseconds rem = target - curr;
        if (rem < std::chrono::microseconds{8})
            break;

        LARGE_INTEGER due;
        due.QuadPart = -((rem > max ? max : rem) / 100).count();

        SetWaitableTimerEx(xr::detail::timer.get(), &due, 0, nullptr, nullptr, nullptr, 0);
        WaitForSingleObjectEx(xr::detail::timer.get(), INFINITE, true);

        curr = std::chrono::high_resolution_clock::now();
    }

    while (target - curr >= std::chrono::nanoseconds{100})
    {
        YieldProcessor();
        curr = std::chrono::high_resolution_clock::now();
    }

    co_return;
}
} // namespace xr
