#include "stdafx.h"

#include "xrdebug.h"

#include "stacktrace_collector.h"

#include <sstream>

#include <signal.h> // for signals
#include <use_ansi.h> // for _set_new_mode

#include <VersionHelpers.h>
#include <shellapi.h>

xrDebug Debug;

HWND gGameWindow{};
bool ExitFromWinMain{};

namespace
{
long (*orig)(_EXCEPTION_POINTERS*){};
bool error_after_dialog{};
} // namespace

static void ShowErrorMessage(const char* msg, const bool show_msg = false)
{
    const bool on_main_thread = Core.OnMainThread();

    if (on_main_thread)
    {
        ShowWindow(gGameWindow, SW_HIDE);

        while (ShowCursor(TRUE) < 0)
            ;
    }

    if (!IsDebuggerPresent())
    {
        if (show_msg && on_main_thread)
            MessageBox(gGameWindow, msg, "FATAL ERROR", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        else
            ShellExecute(nullptr, "open", logFName, nullptr, nullptr, SW_SHOW);
    }
}

static const char* GetThreadName()
{
    if (IsWindows10OrGreater())
    {
        static const HMODULE KernelLib = GetModuleHandle("kernel32.dll");
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

                    WideCharToMultiByte(CP_OEMCP, 0, wThreadName, gsl::narrow_cast<s32>(wcslen(wThreadName)), ResThreadName, sizeof(ResThreadName), nullptr, nullptr);
                    LocalFree(wThreadName);

                    if (xr_strlen(ResThreadName) > 0)
                        return ResThreadName;
                }
            }
        }
    }

    return "UNKNOWN";
}

void LogStackTrace(const char* header, const bool dump_lua_locals)
{
    __try
    {
        if (auto pCrashHandler = Debug.get_lua_trace())
            pCrashHandler(dump_lua_locals);
        Log("********************************************************************************");
        Msg("!![%s] Thread: [%s]", __FUNCTION__, GetThreadName());
        Log(BuildStackTrace(header));
        Log("********************************************************************************");
    }
    __finally
    {}
}

void LogStackTrace(const char* header, _EXCEPTION_POINTERS* pExceptionInfo, bool dump_lua_locals)
{
    __try
    {
        if (auto pCrashHandler = Debug.get_lua_trace())
            pCrashHandler(dump_lua_locals);
        Log("********************************************************************************");
        Msg("!![%s] Thread: [%s], ExceptionCode: [0x%lx]", __FUNCTION__, GetThreadName(), pExceptionInfo->ExceptionRecord->ExceptionCode);
        auto save = *pExceptionInfo->ContextRecord;
        Log(BuildStackTrace(header, pExceptionInfo->ContextRecord));
        *pExceptionInfo->ContextRecord = save;
        Log("********************************************************************************");
    }
    __finally
    {}
}

LONG DbgLogExceptionFilter(const char* header, _EXCEPTION_POINTERS* pExceptionInfo)
{
    LogStackTrace(header, pExceptionInfo);

    return EXCEPTION_EXECUTE_HANDLER;
}

namespace
{
void gather_info(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, gsl::index line, const char* function,
                 char* assertion_info)
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

void xrDebug::do_exit(const std::string& message)
{
    ShowErrorMessage(message.c_str(), true);

    if (!IsDebuggerPresent())
        quick_exit(EXIT_SUCCESS);
    else
        DEBUG_INVOKE;

    std::unreachable();
}

void xrDebug::backend(const char* expression, const char* description, const char* argument0, const char* argument1, const char* file, gsl::index line, const char* function)
{
    static std::recursive_mutex CS;
    std::scoped_lock<decltype(CS)> lock(CS);

    string4096 assertion_info;
    gather_info(expression, description, argument0, argument1, file, line, function, assertion_info);

    /* KRodin: у меня этот способ не работает - происходит исключение внутри функции save_mini_dump(). Если сильно надо будет тут получать минидампы - придумать другой способ.
    #ifdef USE_OWN_MINI_DUMP
        if ( !IsDebuggerPresent() )
            save_mini_dump(nullptr);
    #endif
    */
    auto endline = "\r\n";
    auto buffer = assertion_info + xr_strlen(assertion_info);
    buffer += sprintf(buffer, "%sPress OK to abort execution%s", endline, endline);

    error_after_dialog = true;

    ShowErrorMessage(assertion_info);

    if (!IsDebuggerPresent())
        quick_exit(EXIT_SUCCESS);
    else
        DEBUG_INVOKE;
}

const char* xrDebug::DXerror2string(const HRESULT code) const { return error2string(gsl::narrow_cast<unsigned long>(code)); }

const char* xrDebug::error2string(const DWORD code) const
{
    static string1024 desc_storage;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, code, 0, desc_storage, sizeof(desc_storage) - 1, nullptr);
    return desc_storage;
}

void xrDebug::error(const HRESULT hr, const char* expr, const char* file, gsl::index line, const char* function)
{
    backend(DXerror2string(hr), expr, nullptr, nullptr, file, line, function);
    std::unreachable();
}

void xrDebug::error(const HRESULT hr, const char* expr, const char* e2, const char* file, gsl::index line, const char* function)
{
    backend(DXerror2string(hr), expr, e2, nullptr, file, line, function);
    std::unreachable();
}

void xrDebug::fail(const char* e1, const char* file, gsl::index line, const char* function)
{
    backend("assertion failed", e1, nullptr, nullptr, file, line, function);
    std::unreachable();
}

void xrDebug::fail(const char* e1, const std::string& e2, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2.c_str(), nullptr, nullptr, file, line, function);
    std::unreachable();
}

void xrDebug::fail(const char* e1, const char* e2, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2, nullptr, nullptr, file, line, function);
    std::unreachable();
}

void xrDebug::fail(const char* e1, const char* e2, const char* e3, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2, e3, nullptr, file, line, function);
    std::unreachable();
}

void xrDebug::fail(const char* e1, const char* e2, const char* e3, const char* e4, const char* file, gsl::index line, const char* function)
{
    backend(e1, e2, e3, e4, file, line, function);
    std::unreachable();
}

void xrDebug::fatal(const char* file, gsl::index line, const char* function, const char* F, ...)
{
    string4096 strBuf;
    va_list args;
    va_start(args, F);
    std::vsnprintf(strBuf, sizeof(strBuf), F, args);
    va_end(args);

    backend("FATAL ERROR", strBuf, nullptr, nullptr, file, line, function);
    std::unreachable();
}

static int out_of_memory_handler(size_t size)
{
    Memory.mem_compact();

    const auto process_heap = mem_usage_impl(nullptr, nullptr);
    const auto eco_strings = str_container::stat_economy();
    const auto eco_smem = smem_container::stat_economy();

    Msg("* [x-ray]: process heap[%zd K]", process_heap / 1024);
    Msg("* [x-ray]: economy: strings[%zd K], smem[%zd K]", eco_strings / 1024, eco_smem / 1024);

    FATAL("Out of memory. Memory request: [%zu K]", size / 1024);
}

#ifdef USE_OWN_MINI_DUMP
#include <DbgHelp.h>

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "dbghelp.lib")

static void save_mini_dump(_EXCEPTION_POINTERS* pExceptionInfo)
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
        auto hFile = ::CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            // try to place into current directory
            std::memmove(szDumpPath, szDumpPath + 5, gsl::narrow_cast<size_t>(xr_strlen(szDumpPath)));
            hFile = ::CreateFile(szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        }
        if (hFile != INVALID_HANDLE_VALUE)
        {
            _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

            ExInfo.ThreadId = ::GetCurrentThreadId();
            ExInfo.ExceptionPointers = pExceptionInfo;
            ExInfo.ClientPointers = NULL;

            // write the dump
            auto dump_flags = MINIDUMP_TYPE(MiniDumpNormal | MiniDumpFilterMemory | MiniDumpScanMemory);

            BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dump_flags, &ExInfo, nullptr, nullptr);
            if (bOK)
                Msg("--Saved dump file to [%s]", szDumpPath);
            else
                Msg("!!Failed to save dump file to [%s] (error [%s])", szDumpPath, Debug.error2string(GetLastError()));

            ::CloseHandle(hFile);
        }
        else
        {
            Msg("!!Failed to create dump file [%s] (error [%s])", szDumpPath, Debug.error2string(GetLastError()));
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Msg("Exception catched in function [%s]", __FUNCTION__);
    }
}
#endif

namespace
{
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
        std::unreachable();
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

void format_message(char* buffer)
{
    __try
    {
        auto error_code = GetLastError();
        if (error_code == ERROR_SUCCESS)
        {
            *buffer = 0;
            return;
        }

        void* message{};
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      reinterpret_cast<char*>(&message), 0, nullptr);

        sprintf(buffer, "[error][0x%lx] : [%s]", error_code, static_cast<gsl::czstring>(message));
        LocalFree(message);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Msg("Exception catched in function [%s]", __FUNCTION__);
    }
}

[[nodiscard]] long UnhandledFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
    const auto& rec = *pExceptionInfo->ExceptionRecord;
    const lua_panic_handler panic{rec};

    if (!panic && !is_cpp_exception(rec) && !error_after_dialog)
    {
        string1024 error_message;
        format_message(error_message);
        if (*error_message)
            Msg("\n%s", error_message);

        LogStackTrace("!!Unhandled exception stack trace:\n", pExceptionInfo, true);

        ShowErrorMessage("Fatal error occured\n\nPress OK to abort program execution");
    }

#ifdef USE_OWN_MINI_DUMP
    save_mini_dump(pExceptionInfo);
#endif

    return orig != nullptr ? orig(pExceptionInfo) : EXCEPTION_CONTINUE_SEARCH;
}

// Вызывается при std::terminate()
void _terminate() { Debug.backend("<no expression>", "Unexpected application termination", nullptr, nullptr, DEBUG_INFO); }

void handler_base(const char* reason_string) { Debug.backend("error handler is invoked!", reason_string, nullptr, nullptr, DEBUG_INFO); }

void invalid_parameter_handler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t)
{
    string4096 expression_;
    string4096 function_;
    string4096 file_;
    size_t converted_chars = 0;

    if (expression)
        wcstombs_s(&converted_chars, expression_, sizeof(expression_), expression, (wcslen(expression) + 1) * 2 * sizeof(char));
    else
        strcpy_s(expression_, "");

    if (function)
        wcstombs_s(&converted_chars, function_, sizeof(function_), function, (wcslen(function) + 1) * 2 * sizeof(char));
    else
        strcpy_s(function_, __FUNCTION__);

    if (file)
        wcstombs_s(&converted_chars, file_, sizeof(file_), file, (wcslen(file) + 1) * 2 * sizeof(char));
    else
    {
        line = __LINE__;
        strcpy_s(file_, __FILE__);
    }

    Debug.backend("error handler is invoked!", expression_, nullptr, nullptr, file_, line, function_);
}

void std_out_of_memory_handler() { handler_base("std: out of memory"); }
void pure_call_handler() { handler_base("pure virtual function call"); }
void abort_handler(int) { handler_base("application is aborting"); }
void floating_point_handler(int) { handler_base("floating point error"); }
void illegal_instruction_handler(int) { handler_base("illegal instruction"); }
void termination_handler(int) { handler_base("termination with exit code 3"); }

/*void segment_violation( int signal ) {
  handler_base( "Segment violation error" );
}*/

/*
// http://qaru.site/questions/441696/what-actions-do-i-need-to-take-to-get-a-crash-dump-in-all-error-scenarios
BOOL PreventSetUnhandledExceptionFilter()
{
    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    if (!hKernel32)
        return FALSE;
    void* pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
    if (!pOrgEntry)
        return FALSE;

#ifdef _M_IX86
    // Code for x86:
    // 33 C0                xor         eax,eax
    // C2 04 00             ret         4
    constexpr unsigned char szExecute[] = {0x33, 0xC0, 0xC2, 0x04, 0x00};
#elif _M_X64
    // 33 C0                xor         eax,eax
    // C3                   ret
    constexpr unsigned char szExecute[] = {0x33, 0xC0, 0xC3};
#else
#error "The following code only works for x86 and x64!"
#endif

    SIZE_T bytesWritten = 0;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(), pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);
    return bRet;
}
*/
} // namespace

void xrDebug::_initialize()
{
    std::atexit([] { R_ASSERT(ExitFromWinMain, "Unexpected application exit!"); });

    std::set_terminate(_terminate);

    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    signal(SIGABRT, abort_handler);
    signal(SIGABRT_COMPAT, abort_handler);
    signal(SIGFPE, floating_point_handler);
    signal(SIGILL, illegal_instruction_handler);
    // signal(SIGSEGV, segment_violation);
    signal(SIGINT, nullptr);
    signal(SIGTERM, termination_handler);

    _set_invalid_parameter_handler(&invalid_parameter_handler);

    _set_new_mode(1);
    _set_new_handler(&out_of_memory_handler);
    std::set_new_handler(&std_out_of_memory_handler);

    _set_purecall_handler(&pure_call_handler);

    orig = SetUnhandledExceptionFilter(UnhandledFilter);

    // PreventSetUnhandledExceptionFilter();

    // Выключаем окно "Прекращена работа программы...". У нас своё окно для сообщений об ошибках есть.
    // SetErrorMode(GetErrorMode() | SEM_NOGPFAULTERRORBOX);
}
