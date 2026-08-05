#ifndef __XRCORE_SENTRY_H
#define __XRCORE_SENTRY_H

#ifdef XR_SENTRY
namespace xr
{
class sentry_helper
{
private:
    // Temporary Sentry root until VFS is initialized
    std::filesystem::path tmp;

public:
    // Crash message and stacktrace to attach to Sentry event
    u64 maybe_msg;
    u64 maybe_trace;

    sentry_helper();
    ~sentry_helper();

    sentry_helper& operator=(sentry_helper&&) = delete;

    [[nodiscard]] s32 init(gsl::czstring log);
    void close();

    void event_msg(std::string_view msg);
    void event_trace(const cpptrace::stacktrace& trace);
};

namespace detail
{
inline xr::sentry_helper sentry;
}

[[nodiscard]] constexpr auto& sentry() { return xr::detail::sentry; }
} // namespace xr
#endif // XR_SENTRY

#endif // !__XRCORE_SENTRY_H
