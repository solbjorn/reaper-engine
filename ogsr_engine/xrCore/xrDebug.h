#pragma once

#include "_types.h"

#include "../xrExternal/assert.h"

#include <gsl/util>

#include <atomic>
#include <source_location>

class xrDebug
{
private:
    using lua_panic_handler = void (*)(s32);
    using lua_trace_handler = std::string (*)();

    std::atomic<bool> log{false};

    lua_panic_handler lua_panic{nullptr};
    lua_trace_handler lua_trace{nullptr};

public:
    [[nodiscard]] constexpr lua_panic_handler get_lua_panic() const { return lua_panic; }
    [[nodiscard]] constexpr lua_trace_handler get_lua_trace() const { return lua_trace; }

    [[nodiscard]] constexpr lua_panic_handler set_lua_panic(lua_panic_handler panic)
    {
        std::swap(lua_panic, panic);
        return panic;
    }

    [[nodiscard]] constexpr lua_trace_handler set_lua_trace(lua_trace_handler trace)
    {
        std::swap(lua_trace, trace);
        return trace;
    }

    static void _initialize();

    [[nodiscard]] constexpr bool to_log() const { return log; }
    constexpr void to_log(bool val) { log = val; }

    [[nodiscard]] static std::string format_system(unsigned long code, bool module = false);

    [[noreturn]] void fail(const char* e1, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const std::string& e2, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const char* e2, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const char* e2, const char* e3, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, gsl::index line, const char* function);
    void backend(const char* reason, const char* expression, const char* argument0, const char* argument1, const char* file, gsl::index line,
                 const char* function);
};

namespace xr
{
// Strictly-typed Windows error codes with custom formatters (in format.h).

class hresult
{
private:
    unsigned long val;

public:
    constexpr explicit hresult(long val) : val{std::bit_cast<unsigned long>(val)} {}

    [[nodiscard]] constexpr auto code() const { return val; }
    [[nodiscard]] constexpr explicit operator bool() const { return val == 0; }
    [[nodiscard]] std::string what() const { return xrDebug::format_system(val); }
};

class last_error
{
private:
    unsigned long val;

public:
    constexpr explicit last_error(unsigned long val) : val{val} {}

    [[nodiscard]] constexpr auto code() const { return val; }
    [[nodiscard]] std::string what() const { return xrDebug::format_system(val); }
};

class ntstatus
{
private:
    unsigned long val;

public:
    constexpr explicit ntstatus(long val) : val{std::bit_cast<unsigned long>(val)} {}

    [[nodiscard]] constexpr auto code() const { return val; }
    [[nodiscard]] std::string what() const { return xrDebug::format_system(val, true); }
};

[[nodiscard]] constexpr auto GetLastError() { return xr::last_error{::GetLastError()}; }
[[nodiscard]] constexpr auto hr(auto val) { return xr::hresult{val}; }

// Generic shortcuts

using libassert::is_debugger_present;

constexpr void breakpoint() noexcept { LIBASSERT_BREAKPOINT(); }
[[noreturn]] constexpr void unreachable() { LIBASSERT_UNREACHABLE(); }

#define XR_ASSERT(...) LIBASSERT_ASSERT(__VA_ARGS__)
#define XR_ASSERT_VAL(...) LIBASSERT_ASSERT_VAL(__VA_ARGS__)

#ifdef NDEBUG
// Don't evaluate, but check that it compiles
#define XR_DEBUG_ASSERT(...) \
    do \
    { \
        if constexpr (false) \
            LIBASSERT_ASSERT(__VA_ARGS__); \
    } while (0)
#else
#define XR_DEBUG_ASSERT(...) LIBASSERT_DEBUG_ASSERT(__VA_ARGS__)
#endif

#define XR_DEBUG_ASSERT_VAL(...) LIBASSERT_DEBUG_ASSERT_VAL(__VA_ARGS__)
#define XR_PANIC(...) LIBASSERT_PANIC(__VA_ARGS__)
} // namespace xr

extern xrDebug Debug;
extern HWND gGameWindow;
extern bool ExitFromWinMain;

void LogStackTrace(const char* header, const bool dump_lua_locals = true);

#include "xrDebug_macros.h"
