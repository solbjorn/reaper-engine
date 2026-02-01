#pragma once

#include "_types.h"

#include <gsl/narrow>
#include <gsl/zstring>

#include <cstdarg>
#include <cstdio>
#include <string>

#define DEBUG_INVOKE __debugbreak()

class xrDebug
{
private:
    using lua_panic_handler = void (*)(s32);
    using lua_trace_handler = void (*)(bool);

    lua_panic_handler lua_panic{};
    lua_trace_handler lua_trace{};

public:
    [[nodiscard]] lua_panic_handler get_lua_panic() const { return lua_panic; }
    [[nodiscard]] lua_trace_handler get_lua_trace() const { return lua_trace; }

    [[nodiscard]] lua_panic_handler set_lua_panic(lua_panic_handler panic)
    {
        std::swap(lua_panic, panic);
        return panic;
    }

    [[nodiscard]] lua_trace_handler set_lua_trace(lua_trace_handler trace)
    {
        std::swap(lua_trace, trace);
        return trace;
    }

    void _initialize();

    const char* DXerror2string(const HRESULT code) const;
    const char* error2string(const DWORD code) const;

    [[noreturn]] void fail(const char* e1, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const std::string& e2, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const char* e2, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const char* e2, const char* e3, const char* file, gsl::index line, const char* function);
    [[noreturn]] void fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, gsl::index line, const char* function);
    [[noreturn]] void error(const HRESULT code, const char* e1, const char* file, gsl::index line, const char* function);
    [[noreturn]] void error(const HRESULT code, const char* e1, const char* e2, const char* file, gsl::index line, const char* function);
    [[noreturn]] void XR_PRINTF(5, 6) fatal(const char* file, gsl::index line, const char* function, const char* F, ...);
    void backend(const char* reason, const char* expression, const char* argument0, const char* argument1, const char* file, gsl::index line, const char* function);
    [[noreturn]] static void do_exit(const std::string& message);
};

// warning
// this function can be used for debug purposes only
IC XR_PRINTF(1, 2) std::string make_string(const char* format, ...)
{
    std::va_list args, args_copy;

    va_start(args, format);
    va_copy(args_copy, args);

    const auto sz = std::vsnprintf(nullptr, 0, format, args);
    if (sz <= 0)
        return std::string{};

    const auto n = gsl::narrow_cast<size_t>(sz);
    std::string result(n, '\0');
    std::vsnprintf(result.data(), n + 1, format, args_copy);

    va_end(args_copy);
    va_end(args);

    return result;
}

extern xrDebug Debug;
extern HWND gGameWindow;
extern bool ExitFromWinMain;

void LogStackTrace(const char* header, const bool dump_lua_locals = true);
void LogStackTrace(const char* header, _EXCEPTION_POINTERS* pExceptionInfo, bool dump_lua_locals = false);

// KRodin: отладочный хак для получения стека вызовов, при вызове проблемного кода внутри __try {...}
// Использовать примерно так:
// __except(ExceptStackTrace("stack trace:\n")) {...}
LONG DbgLogExceptionFilter(const char* header, _EXCEPTION_POINTERS* pExceptionInfo);
#define ExceptStackTrace(str) DbgLogExceptionFilter(str, GetExceptionInformation())

#include "xrDebug_macros.h"
