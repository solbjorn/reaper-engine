// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"

#include <mmsystem.h>
#include <objbase.h>

#pragma comment(lib, "winmm.lib")

xrCore Core;

// indicate that we reach WinMain, and all static variables are initialized
bool gModulesLoaded = false;

static u32 init_counter = 0;

void xrCore::_initialize(LPCSTR _ApplicationName, LogCallback cb, BOOL init_fs, LPCSTR fs_fname)
{
    strcpy_s(ApplicationName, _ApplicationName);
    if (0 == init_counter)
    {
        mainThreadId = std::this_thread::get_id();

        /*
            По сути это не рекомендуемый Microsoft, но повсеместно используемый
           способ повышения точности соблюдения и измерения временных интревалов
           функциями Sleep, QueryPerformanceCounter, timeGetTime и GetTickCount.
            Функция действует на всю операционную систему в целом (!) и нет
           необходимости вызывать её при старте нового потока. Вызов
           timeEndPeriod специалисты Microsoft считают обязательным. Есть
           подозрения, что Windows сама устанавливает максимальную точность при
           старте таких приложений как, например, игры. Тогда есть шанс, что
           вызов timeBeginPeriod здесь бессмысленен. Недостатком данного способа
           является то, что он приводит к общему замедлению работы как текущего
           приложения, так и всей операционной системы. Ещё можно посмотреть
           ссылки:
            https://msdn.microsoft.com/en-us/library/vs/alm/dd757624(v=vs.85).aspx
            https://users.livejournal.com/-winnie/151099.html
            https://github.com/tebjan/TimerTool
        */
        timeBeginPeriod(1);

        strcpy_s(Params, sizeof(Params), GetCommandLine());

        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        if (strstr(Params, "-dbg"))
            ParamFlags.set(ParamFlag::dbg, TRUE);

        // application path
        string_path fn, dr, di;
        GetModuleFileName(nullptr, fn, sizeof(fn));
        _splitpath(fn, dr, di, nullptr, nullptr);
        strconcat(sizeof(ApplicationPath), ApplicationPath, dr, di);

        // -use-work-dir используется, когда при отладке в Working Directory указывают путь к ресурсам игры
        // в таком случае не надо вызывать SetCurrentDirectory :)
        if (strstr(Core.Params, "-use-work-dir") == nullptr)
        {
            // Рабочий каталог для процесса надо устанавливать принудительно
            // в папку с движком, независимо откуда запустили. Иначе начинаются
            // чудеса типа игнорирования движком символов для стектрейсинга.
            SetCurrentDirectory(ApplicationPath);
            GetCurrentDirectory(sizeof(WorkingPath), WorkingPath);
        }

        // User/Comp Name
        DWORD sz_user = sizeof(UserName);
        GetUserName(UserName, &sz_user);

        DWORD sz_comp = sizeof(CompName);
        GetComputerName(CompName, &sz_comp);

        Memory._initialize();

        _initialize_cpu();

        xr_FS = std::make_unique<CLocatorAPI>();
    }
    if (init_fs)
    {
        u32 flags = 0;
        if (0 != strstr(Params, "-build"))
            flags |= CLocatorAPI::flBuildCopy;
        if (0 != strstr(Params, "-ebuild"))
            flags |= CLocatorAPI::flBuildCopy | CLocatorAPI::flEBuildCopy;
#ifdef DEBUG
        if constexpr (false) /*(strstr(Params,"-cache"))*/
            flags |= CLocatorAPI::flCacheFiles;
        else
            flags &= ~CLocatorAPI::flCacheFiles;
#endif // DEBUG
        flags |= CLocatorAPI::flScanAppRoot;

        if (0 != strstr(Params, "-file_activity"))
            flags |= CLocatorAPI::flDumpFileActivity;

        FS._initialize(flags, 0, fs_fname);

        Msg("[OGSR Engine (%s)] build date: [" __DATE__ " " __TIME__ "]", GetBuildConfiguration());
        if (strlen(APPVEYOR_BUILD_VERSION))
            Log("[AppVeyor] build version: [" APPVEYOR_BUILD_VERSION "], repo: [" APPVEYOR_REPO_NAME "]");

#ifdef __clang__
        Log("[" _CRT_STRINGIZE_(__clang_version__) "]: [" _CRT_STRINGIZE(__clang_version__) "], [" _CRT_STRINGIZE_(__cplusplus) "]: [" _CRT_STRINGIZE(__cplusplus) "]");
#else
        Log("[" _CRT_STRINGIZE_(_MSC_FULL_VER) "]: [" _CRT_STRINGIZE(_MSC_FULL_VER) "], [" _CRT_STRINGIZE_(_MSVC_LANG) "]: [" _CRT_STRINGIZE(_MSVC_LANG) "]");
#endif

        Msg("Working Directory: [%s]", WorkingPath);
        Msg("CommandLine: [%s]", Core.Params);

#ifdef DEBUG
        Msg("CRT heap 0x%08x", _get_heap_handle());
        Msg("Process heap 0x%08x", GetProcessHeap());
#endif // DEBUG
    }

    SetLogCB(cb);
    init_counter++;
}

void xrCore::_destroy()
{
    --init_counter;
    if (0 == init_counter)
    {
        FS._destroy();

        xr_FS.reset();

        Memory._destroy();

        CoUninitialize();

        timeEndPeriod(1);
    }
}

const char* xrCore::GetEngineVersion()
{
    static string256 buff;
    if (strlen(APPVEYOR_BUILD_VERSION))
        std::snprintf(buff, sizeof(buff), APPVEYOR_BUILD_VERSION " (%s) from repo: [" APPVEYOR_REPO_NAME "]", GetBuildConfiguration());
    else
        std::snprintf(buff, sizeof(buff), "[OGSR Engine %s (build: " __DATE__ " " __TIME__ ")]", GetBuildConfiguration());
    return buff;
}

constexpr const char* xrCore::GetBuildConfiguration()
{
#ifdef _DEBUG
#ifdef _M_X64
    return "x64_Dbg";
#else
    return "x86_Dbg";
#endif
#else
#ifdef _M_X64
    return "x64";
#else
    return "x86";
#endif
#endif
}
