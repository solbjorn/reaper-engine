#pragma once

void Log(std::string_view msg);
void Log(xr::detail::string_view fmt, xr::detail::format_args args);

template <typename... Args>
constexpr void Msg(xr::detail::format_string<Args...> fmt, Args&&... args)
{
    Log(fmt.get(), xr::detail::make_format_args(args...));
}

#define VPUSH(a) a.x, a.y, a.z

void Log(gsl::czstring msg, const Fvector& dop);
void Log(gsl::czstring msg, const Fmatrix& dop);

void CreateLog(BOOL no_log = FALSE);

extern xr_vector<xr_string> LogFile;
extern string_path logFName;

namespace xr
{
namespace detail
{
void log_init();
}

void log_flush();
} // namespace xr
