#ifndef __XRCORE_SLEEP_H
#define __XRCORE_SLEEP_H

namespace xr
{
namespace detail
{
void timer_create();
void timer_destroy();

void nanosleep(std::chrono::nanoseconds count);
} // namespace detail

// To use with std::__libcpp_thread_poll_with_backoff(). Backoff order:
// 1) 64 pure spins;
// 2) spins followed by yields for up to 4 us;
// 3) short sleeps from 2 to 8 us until the condition is met.
[[nodiscard]] constexpr std::__backoff_results backoff(std::chrono::nanoseconds elapsed)
{
    if (elapsed > std::chrono::microseconds{4})
        xr::detail::nanosleep(elapsed / 2);
    else
        YieldProcessor();

    return std::__backoff_results::__continue_poll;
}

void sleep(std::chrono::nanoseconds count);
} // namespace xr

#endif // !__XRCORE_SLEEP_H
