#include "stdafx.h"

#include "xrDebug.h"

#include "stacktrace_collector.h"

#include <cpptrace/from_current.hpp>
#include <cpptrace/utils.hpp>

#include <csignal>

#ifdef USE_OWN_MINI_DUMP
#include <DbgHelp.h>
#endif

#include <VersionHelpers.h>
#include <shellapi.h>
#include <use_ansi.h>

xrDebug Debug;

HWND gGameWindow{};
bool ExitFromWinMain{};

namespace xr
{
namespace
{
long (*orig)(_EXCEPTION_POINTERS*){nullptr};
xr_string maybe_trace;

void log_callback(cpptrace::log_level lvl, gsl::czstring msg)
{
    std::string_view pfx;

    switch (lvl)
    {
    case cpptrace::log_level::debug: pfx = "- "; break;
    case cpptrace::log_level::info: pfx = "* "; break;
    case cpptrace::log_level::warning: pfx = "~ "; break;
    case cpptrace::log_level::error: pfx = "! "; break;
    default: break;
    }

    Msg("{}cpptrace: {}", pfx, msg);
}

void show(std::string_view msg)
{
    static std::atomic<bool> hit{false};

    if (bool exp{false}; !hit.compare_exchange_strong(exp, true))
        return;

    if (Debug.to_log())
        Log(msg);

    xr::log_flush();

    tmc::post(xr::tmc_cpu_st_executor(), [] -> tmc::task<void> {
        ::ShowWindow(gGameWindow, SW_HIDE);
        co_return;
    }());

    while (::ShowCursor(true) < 0)
        continue;

    if (xr::is_debugger_present())
        return;

    if (!Debug.to_log())
    {
        tmc::post(xr::tmc_cpu_st_executor(), [](gsl::czstring msg) -> tmc::task<void> {
            ::MessageBoxA(gGameWindow, msg, "FATAL ERROR", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
            co_return;
        }(msg.data()));
    }
    else
    {
        ::ShellExecuteA(nullptr, "open", logFName, nullptr, nullptr, SW_SHOW);
    }
}

[[noreturn]] void failure_handler(const libassert::assertion_info& info)
{
    auto msg = xr::format("\n{}\nStack trace:\n{}", info.header(180 - 33), xr::maybe_trace.empty() ? info.print_stacktrace(180 - 33) : xr::maybe_trace);

    if (auto trace = Debug.get_lua_trace(); trace != nullptr)
        msg += trace();

    while (msg.back() == '\n')
        msg.pop_back();

    xr::show(std::move(msg));

    if (!xr::is_debugger_present())
        std::quick_exit(EXIT_SUCCESS);
    else
        xr::breakpoint();

    xr::unreachable();
}

void terminate()
{
    xr_string description;
    xr_string type;

    try
    {
        if (auto ep = std::current_exception(); ep)
            std::rethrow_exception(ep);
    }
    catch (const std::exception& ex)
    {
        description = ex.what();
        type = cpptrace::demangle(typeid(ex).name());
    }
    catch (const std::string& msg)
    {
        description = msg;
        type = cpptrace::demangle(typeid(msg).name());
    }
    catch (gsl::czstring msg)
    {
        description = msg;
        type = cpptrace::demangle(typeid(msg).name());
    }
    catch (...)
    {
        description = "unknown exception";
        type = "not available";
    }

    if (!type.empty())
        XR_PANIC(description, type);
    else
        XR_PANIC("unexpected application termination");
}

void signal_handler(s32 sig)
{
    std::string_view desc;

    switch (sig)
    {
    case SIGINT: desc = "execution interrupt"; break;
    case SIGILL: desc = "illegal instruction"; break;
    case SIGFPE: desc = "floating point exception"; break;
    case SIGSEGV: desc = "segment violation"; break;
    case SIGTERM: desc = "process termination"; break;
    case SIGBREAK: desc = "break sequence"; break;
    case SIGABRT:
    case SIGABRT_COMPAT: desc = "abnormal termination"; break;
    default: break;
    }

    XR_ASSERT(sig == 0, desc);
}

void new_handler()
{
    Memory.mem_compact();

    const auto process_heap = mem_usage_impl(nullptr, nullptr) / 1024;
    const auto str_economy = str_container::stat_economy() / 1024;
    const auto smem_economy = smem_container::stat_economy() / 1024;

    XR_PANIC("out of memory", process_heap, str_economy, smem_economy);
}

void invalid_parameter_handler(gsl::cwzstring expr, gsl::cwzstring fn, gsl::cwzstring f, u32 line, uintptr_t reserved)
{
    if (expr == nullptr || fn == nullptr || f == nullptr)
        XR_PANIC("invalid parameter", line, reserved);

    const sf::String expression{expr};
    const sf::String function{fn};
    const sf::String file{f};

    XR_PANIC("invalid parameter", expression, function, file, line, reserved);
}

// LuaJIT always throws a custom structured exception instead of calling to
// the user-defined panic handler. It is used for unwinding when inside a
// protected call, otherwise the exception sneaks out and finishes in the
// unhandled exception handler.
// If the code matches, redirect it to the Lua panic handler, which will
// print the error off the Lua stack, then eventually the stack trace and
// the locals if accessible.
class lua_panic_handler
{
private:
    static constexpr unsigned long lua_exception_base{0xe24c4a00};

    decltype(std::declval<xrDebug>().get_lua_panic()) handler{};
    s32 code{};

    [[noreturn]] void execute()
    {
        handler(code);
        xr::unreachable();
    }

public:
    constexpr explicit lua_panic_handler(const _EXCEPTION_RECORD& rec)
    {
        const auto maybe_code = rec.ExceptionCode ^ lua_exception_base;
        if (maybe_code > std::numeric_limits<u8>::max())
            return;

        handler = Debug.get_lua_panic();
        code = gsl::narrow_cast<s32>(maybe_code);
    }

    constexpr ~lua_panic_handler()
    {
        if (handler != nullptr)
            execute();
    }

    [[nodiscard]] constexpr explicit operator bool() const { return handler != nullptr; }
};

// An uncaught C++ exception finishes as an unhandled structured exception in
// the handler with the particular codes. If that's the case, the handler
// shouldn't exit immediately -- pass it to the default handler, which will
// call std::terminate() where it can be rethrown and logged.
[[nodiscard]] constexpr bool is_cpp_exception(const _EXCEPTION_RECORD& rec)
{
    if (rec.ExceptionCode != 0xe06d7363 || rec.NumberParameters != 4)
        return false;

    switch (rec.ExceptionInformation[0])
    {
    case 0x19930520:
    case 0x19930521:
    case 0x19930522:
    case 0x01994000: return true;
    default: return false;
    }
}

#ifdef USE_OWN_MINI_DUMP
void save_mini_dump(_EXCEPTION_POINTERS* info);
#endif

[[nodiscard]] long unhandled_filter(_EXCEPTION_POINTERS* info)
{
    static std::atomic<bool> hit{false};

out:
    if (bool exp{false}; !hit.compare_exchange_strong(exp, true))
        return xr::orig != nullptr ? xr::orig(info) : EXCEPTION_CONTINUE_SEARCH;

    const auto& rec = *info->ExceptionRecord;
    const auto cpp = xr::is_cpp_exception(rec);
    const xr::lua_panic_handler panic{rec};

    cpptrace::detail::maybe_collect_trace(info, EXCEPTION_EXECUTE_HANDLER);

    if (const auto& trace = cpptrace::from_current_exception(); !trace.empty())
        xr::maybe_trace = libassert::print_stacktrace(trace, 180 - 33);

#ifdef USE_OWN_MINI_DUMP
    save_mini_dump(info);
#endif

    if (!cpp && !panic)
    {
        const xr::ntstatus code{std::bit_cast<long>(rec.ExceptionCode)};
        XR_PANIC(code.what(), code, errno, xr::GetLastError());
    }

    goto out;
}
} // namespace
} // namespace xr

static const char* GetThreadName()
{
    if (IsWindows10OrGreater())
    {
        static const HMODULE KernelLib = GetModuleHandleA("kernel32.dll");
        using FuncGetThreadDescription = HRESULT (*)(HANDLE, PWSTR*);

        static const auto pGetThreadDescription = reinterpret_cast<FuncGetThreadDescription>(GetProcAddress(KernelLib, "GetThreadDescription"));
        if (pGetThreadDescription)
        {
            PWSTR wThreadName = nullptr;
            if (SUCCEEDED(pGetThreadDescription(GetCurrentThread(), &wThreadName)))
            {
                if (wThreadName)
                {
                    static string64 ResThreadName{};

                    WideCharToMultiByte(CP_OEMCP, 0, wThreadName, gsl::narrow_cast<s32>(wcslen(wThreadName)), ResThreadName, sizeof(ResThreadName), nullptr,
                                        nullptr);
                    LocalFree(wThreadName);

                    if (xr_strlen(ResThreadName) > 0)
                        return ResThreadName;
                }
            }
        }
    }

    return "UNKNOWN";
}

void LogStackTrace(const char* header, const bool)
{
    __try
    {
        Log("********************************************************************************");
        Msg("!![{}] Thread: [{}]", std::source_location::current().function_name(), GetThreadName());
        Log(BuildStackTrace(header));
        Log("********************************************************************************");
    }
    __finally
    {}
}

namespace
{
void gather_info(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, gsl::index line,
                 const char* function, char* assertion_info)
{
    xr_string unhandled;

    try
    {
        auto ep = std::current_exception();
        if (ep)
            std::rethrow_exception(ep);
    }
    catch (const std::exception& ex)
    {
        unhandled = ex.what();
    }
    catch (const std::string& msg)
    {
        unhandled = msg;
    }
    catch (gsl::czstring msg)
    {
        unhandled = msg;
    }
    catch (...)
    {
        unhandled = "Unknown exception type";
    }

    auto buffer = assertion_info;
    auto endline = "\n";
    auto prefix = "[error]";
    const bool extended_description = strchr(description, '\n') != nullptr;
    const bool extended_unhandled = unhandled.contains('\n');

    for (int i = 0; i < 2; ++i)
    {
        if (!i)
            buffer += sprintf(buffer, "%sFATAL ERROR%s%s", endline, endline, endline);

        buffer += sprintf(buffer, "%sExpression    : %s%s", prefix, expression, endline);
        buffer += sprintf(buffer, "%sFunction      : %s%s", prefix, function, endline);
        buffer += sprintf(buffer, "%sFile          : %s%s", prefix, file, endline);
        buffer += sprintf(buffer, "%sLine          : %zd%s", prefix, line, endline);

        if (extended_description)
            buffer += sprintf(buffer, "%sDescription   :%s%s%s%s", prefix, endline, endline, description, endline);
        else
            buffer += sprintf(buffer, "%sDescription   : %s%s", prefix, description, endline);

        if (argument0)
        {
            if (extended_description)
                buffer += sprintf(buffer, "%s", endline);

            if (argument1)
            {
                buffer += sprintf(buffer, "%sArgument 0    : %s%s", prefix, argument0, endline);
                buffer += sprintf(buffer, "%sArgument 1    : %s%s", prefix, argument1, endline);
            }
            else
            {
                buffer += sprintf(buffer, "%sArguments     : %s%s", prefix, argument0, endline);
            }
        }

        buffer += sprintf(buffer, "%s", endline);

        if (!unhandled.empty())
        {
            buffer += sprintf(buffer, "%sExpression    : %s%s", prefix, "unhandled exception", endline);
            buffer += sprintf(buffer, "%sFunction      : %s%s", prefix, function, endline);
            buffer += sprintf(buffer, "%sFile          : %s%s", prefix, file, endline);
            buffer += sprintf(buffer, "%sLine          : %zd%s", prefix, line, endline);

            if (extended_unhandled)
                buffer += sprintf(buffer, "%sDescription   :%s%s%s%s", prefix, endline, endline, unhandled.c_str(), endline);
            else
                buffer += sprintf(buffer, "%sDescription   : %s%s", prefix, unhandled.c_str(), endline);

            buffer += sprintf(buffer, "%s", endline);
        }

        if (!i)
        {
            Log(assertion_info);
            buffer = assertion_info;
            endline = "\r\n";
            prefix = "";
        }
    }

#ifdef USE_OWN_MINI_DUMP
    buffer += sprintf(buffer, "See log file and minidump for detailed information\r\n");
#else
    buffer += sprintf(buffer, "See log file for detailed information\r\n");
#endif
    LogStackTrace("!!stack trace:\n");
}
} // namespace

void xrDebug::backend(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, gsl::index line,
                      const char* function)
{
    static std::atomic<bool> hit{false};
    bool exp{false};

    if (!hit.compare_exchange_strong(exp, true))
        return;

    string4096 assertion_info;
    gather_info(expression, description, argument0, argument1, file, line, function, assertion_info);

    // KRodin: у меня этот способ не работает - происходит исключение внутри функции save_mini_dump(). Если сильно надо будет тут получать
    // минидампы - придумать другой способ.
    /*
#ifdef USE_OWN_MINI_DUMP
    if (!xr::is_debugger_present())
        save_mini_dump(nullptr);
#endif
    */

    auto endline = "\r\n";
    auto buffer = assertion_info + xr_strlen(assertion_info);
    buffer += sprintf(buffer, "%sPress OK to abort execution%s", endline, endline);

    xr::show({});

    if (!xr::is_debugger_present())
        quick_exit(EXIT_SUCCESS);
    else
        xr::breakpoint();
}

void xrDebug::fail(const char* e1, const char* file, gsl::index line, const char* function)
{
    backend("assertion failed", e1, nullptr, nullptr, file, line, function);
    xr::unreachable();
}

void xrDebug::fail(const char* e1, const std::string& e2, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2.c_str(), nullptr, nullptr, file, line, function);
    xr::unreachable();
}

void xrDebug::fail(const char* e1, const char* e2, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2, nullptr, nullptr, file, line, function);
    xr::unreachable();
}

void xrDebug::fail(const char* e1, const char* e2, const char* e3, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2, e3, nullptr, file, line, function);
    xr::unreachable();
}

void xrDebug::fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2, e3, e4, file, line, function);
    xr::unreachable();
}

#ifdef USE_OWN_MINI_DUMP
namespace xr
{
namespace
{
void save_mini_dump(_EXCEPTION_POINTERS* info)
{
    __try
    {
        string_path szDumpPath;
        string64 t_stemp;

        timestamp(t_stemp);
        strcpy_s(szDumpPath, Core.ApplicationName);
        strcat_s(szDumpPath, "_");
        strcat_s(szDumpPath, Core.UserName);
        strcat_s(szDumpPath, "_");
        strcat_s(szDumpPath, t_stemp);
        strcat_s(szDumpPath, ".mdmp");

        __try
        {
            if (FS.path_exist("$logs$"))
                std::ignore = FS.update_path(szDumpPath, "$logs$", szDumpPath);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            string_path temp;
            strcpy_s(temp, szDumpPath);
            strcpy_s(szDumpPath, "logs/");
            strcat_s(szDumpPath, temp);
        }

        // create the file
        auto hFile = ::CreateFileA(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            // try to place into current directory
            std::memmove(szDumpPath, szDumpPath + 5, gsl::narrow_cast<size_t>(xr_strlen(szDumpPath)));
            hFile = ::CreateFileA(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        }
        if (hFile != INVALID_HANDLE_VALUE)
        {
            _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

            ExInfo.ThreadId = ::GetCurrentThreadId();
            ExInfo.ExceptionPointers = info;
            ExInfo.ClientPointers = false;

            // write the dump
            auto dump_flags = MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory);

            BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, nullptr, nullptr);
            if (bOK)
                Msg("--Saved dump file to [{}]", szDumpPath);
            else
                Msg("!!Failed to save dump file to [{}] (error [{}])", szDumpPath, xr::GetLastError());

            ::CloseHandle(hFile);
        }
        else
        {
            Msg("!!Failed to create dump file [{}] (error [{}])", szDumpPath, xr::GetLastError());
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Msg("Exception catched in function [{}]", std::source_location::current().function_name());
    }
}
} // namespace
} // namespace xr
#endif

std::string xrDebug::format_system(unsigned long code, bool module)
{
    constexpr auto drop = [] [[nodiscard]] (char ch) { return ch == ' ' || ch == '.' || ch == '\n' || ch == '\r'; };
    constexpr unsigned long lang{0x409}; // en-US

    unsigned long flags{FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS};
    ::HINSTANCE__* handle{nullptr};
    gsl::zstring str{nullptr};

    if (module)
    {
        handle = ::LoadLibraryA("ntdll.dll");
        flags |= FORMAT_MESSAGE_FROM_HMODULE;
    }

    const auto _ = gsl::finally([str, handle] {
        ::LocalFree(str);

        if (handle != nullptr)
            ::FreeLibrary(handle);
    });

    auto n = ::FormatMessageA(flags, handle, code, lang, reinterpret_cast<gsl::zstring>(&str), 0, nullptr);
    while (n > 0 && drop(str[n - 1]))
        --n;

    if (n == 0)
        return "unknown error";

    std::string ret{str, n};
    std::ranges::replace(ret, '\n', ' ');

    return ret;
}

void xrDebug::_initialize()
{
    cpptrace::set_log_callback(&xr::log_callback);

    libassert::set_color_scheme(libassert::color_scheme::blank);
    libassert::set_failure_handler(&xr::failure_handler);

    std::atexit([] { XR_ASSERT(ExitFromWinMain, "unexpected application exit"); });
    std::set_terminate(&xr::terminate);

    ::_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    std::signal(SIGINT, nullptr);

    for (auto sig : std::array{SIGILL, SIGFPE, SIGTERM, SIGABRT, SIGABRT_COMPAT})
        std::signal(sig, &xr::signal_handler);

    ::_set_new_mode(1);
    std::set_new_handler(&xr::new_handler);

    ::_set_invalid_parameter_handler(&xr::invalid_parameter_handler);
    ::_set_purecall_handler([] { XR_PANIC("pure virtual function call"); });

    xr::orig = ::SetUnhandledExceptionFilter(&xr::unhandled_filter);

    xr::detail::log_init();
}
