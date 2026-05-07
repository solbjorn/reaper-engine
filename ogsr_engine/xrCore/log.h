#pragma once

#include <SFML/System/String.hpp>

#ifdef XR_USE_FMT

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wnan-infinity-disabled");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");

#include <enchantum/fmt_format.hpp>

XR_DIAG_POP();

#include <fmt/std.h>

namespace xr
{
using fmt::format;

namespace detail
{
using fmt::format_args;
using fmt::format_string;
using fmt::make_format_args;
using fmt::string_view;
using fmt::vformat;
} // namespace detail
} // namespace xr

[[nodiscard]] constexpr auto format_as(const shared_str& str) { return std::string_view{str}; }

namespace sf
{
[[nodiscard]] constexpr auto format_as(const sf::String& str)
{
    auto utf = str.toUtf8();
    return *reinterpret_cast<xr_string*>(&utf);
}
} // namespace sf

#else // !XR_USE_FMT

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");

#include <enchantum/std_format.hpp>

XR_DIAG_POP();

namespace xr
{
using std::format;

namespace detail
{
using std::format_args;
using std::format_string;
using std::make_format_args;
using std::string_view;
using std::vformat;
} // namespace detail
} // namespace xr

template <>
struct std::formatter<shared_str> : std::formatter<std::string_view>
{
    [[nodiscard]] constexpr auto format(const shared_str& str, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(std::string_view{str}, ctx);
    }
};

template <>
struct std::formatter<sf::String> : std::formatter<xr_string>
{
    [[nodiscard]] constexpr auto format(const sf::String& str, std::format_context& ctx) const
    {
        auto utf = str.toUtf8();
        return std::formatter<xr_string>::format(*reinterpret_cast<xr_string*>(&utf), ctx);
    }
};

#endif // !XR_USE_FMT

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
