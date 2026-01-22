//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "x_ray.h"

#include "igame_level.h"
#include "igame_persistent.h"

#include "xr_input.h"
#include "xr_ioconsole.h"
#include "std_classes.h"
#include "LightAnimLibrary.h"
#include "../xrcdb/ispatial.h"
#include "ILoadingScreen.h"
#include "DiscordRPC.hpp"
#include "splash.h"

#include "xrcpuid.h"

#define CORE_FEATURE_SET(feature, section) Core.Features.set(xrCore::Feature::feature, READ_IF_EXISTS(pSettings, r_bool, section, #feature, false))

bool IS_OGSR_GA{};
CInifile* pGameIni = nullptr;

namespace
{
struct SoundProcessor final : public pureFrame
{
    RTTI_DECLARE_TYPEINFO(SoundProcessor, pureFrame);

public:
    tmc::task<void> OnFrame() override
    {
        ::Sound->update(Device.vCameraPosition, Device.vCameraDirection, Device.vCameraTop, Device.vCameraRight);
        co_return;
    }
} g_sound_processor;

struct SoundRenderer final : public pureFrame
{
    RTTI_DECLARE_TYPEINFO(SoundRenderer, pureFrame);

public:
    tmc::task<void> OnFrame() override
    {
        ::Sound->render();
        co_return;
    }
} g_sound_renderer;
} // namespace

// global variables
CApplication* pApp{};
static HWND logoWindow{};

bool g_bBenchmark = false;
string512 g_sBenchmarkName;

static void InitSettings()
{
    string_path fname;
    std::ignore = FS.update_path(fname, "$game_config$", "system.ltx");
    pSettings = xr_new<CInifile>(fname, TRUE);
    CHECK_OR_EXIT(!pSettings->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

    std::ignore = FS.update_path(fname, "$game_config$", "game.ltx");
    pGameIni = xr_new<CInifile>(fname, TRUE);
    CHECK_OR_EXIT(!pGameIni->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

    IS_OGSR_GA = strstr(READ_IF_EXISTS(pSettings, r_string, "mod_ver", "mod_ver", "nullptr"), "OGSR");
}

static void InitConsole()
{
    Console = xr_new<CConsole>();
    Console->Initialize();

    strcpy_s(Console->ConfigFile, "user.ltx");
    if (strstr(Core.Params, "-ltx "))
    {
        string64 c_name;
        sscanf(strstr(Core.Params, "-ltx ") + 5, "%[^ ] ", c_name);
        strcpy_s(Console->ConfigFile, c_name);
    }

    CORE_FEATURE_SET(colorize_ammo, "dragdrop");
    CORE_FEATURE_SET(colorize_untradable, "dragdrop");
    CORE_FEATURE_SET(equipped_untradable, "dragdrop");
    CORE_FEATURE_SET(select_mode_1342, "dragdrop");
    CORE_FEATURE_SET(highlight_equipped, "dragdrop");
    CORE_FEATURE_SET(af_radiation_immunity_mod, "features");
    CORE_FEATURE_SET(condition_jump_weight_mod, "features");
    CORE_FEATURE_SET(forcibly_equivalent_slots, "features");
    CORE_FEATURE_SET(slots_extend_menu, "features");
    CORE_FEATURE_SET(dynamic_sun_movement, "features");
    CORE_FEATURE_SET(wpn_bobbing, "features");
    CORE_FEATURE_SET(show_inv_item_condition, "features");
    CORE_FEATURE_SET(remove_alt_keybinding, "features");
    CORE_FEATURE_SET(binoc_firing, "features");
    CORE_FEATURE_SET(busy_actor_restrictions, "features");
    CORE_FEATURE_SET(stop_anim_playing, "features");
    CORE_FEATURE_SET(corpses_collision, "features");
    CORE_FEATURE_SET(more_hide_weapon, "features");
    CORE_FEATURE_SET(keep_inprogress_tasks_only, "features");
    CORE_FEATURE_SET(show_dialog_numbers, "features");
    CORE_FEATURE_SET(objects_radioactive, "features");
    CORE_FEATURE_SET(af_zero_condition, "features");
    CORE_FEATURE_SET(af_satiety, "features");
    CORE_FEATURE_SET(af_psy_health, "features");
    CORE_FEATURE_SET(outfit_af, "features");
    CORE_FEATURE_SET(gd_master_only, "features");
    CORE_FEATURE_SET(scope_textures_autoresize, "features");
    CORE_FEATURE_SET(ogse_new_slots, "features");
    CORE_FEATURE_SET(ogse_wpn_zoom_system, "features");
    CORE_FEATURE_SET(wpn_cost_include_addons, "features");
    CORE_FEATURE_SET(new_pda_info, "features");
    CORE_FEATURE_SET(hard_ammo_reload, "features");
    CORE_FEATURE_SET(engine_ammo_repacker, "features");
    CORE_FEATURE_SET(ruck_flag_preferred, "features");
    CORE_FEATURE_SET(pickup_bolts, "features");
    CORE_FEATURE_SET(old_outfit_slot_style, "features");
    CORE_FEATURE_SET(npc_simplified_shooting, "features");
    CORE_FEATURE_SET(use_trade_deficit_factor, "features");
    CORE_FEATURE_SET(show_objectives_ondemand, "features");
    CORE_FEATURE_SET(pickup_check_overlaped, "features");
    CORE_FEATURE_SET(actor_thirst, "features");
    CORE_FEATURE_SET(autoreload_wpn, "features");
    CORE_FEATURE_SET(no_progress_bar_animation, "features");
    CORE_FEATURE_SET(disable_dialog_break, "features");
}

namespace xr
{
namespace
{
tmc::task<void> exec_user_script()
{
    Console->Execute("unbindall");

    if (FS.exist("$app_data_root$", Console->ConfigFile))
    {
        Console->ExecuteScript(Console->ConfigFile);
    }
    else
    {
        string_path default_full_name;
        std::ignore = FS.update_path(default_full_name, "$game_config$", "rspec_default.ltx");
        Console->ExecuteScript(default_full_name);
    }

    co_await Device.process_frame_async();
}

tmc::task<void> pre_startup()
{
    pApp = (co_await CApplication::co_create()).release();
    g_pGamePersistent = smart_cast<IGame_Persistent*>(NEW_INSTANCE(CLSID_GAME_PERSISTANT));

    co_await g_pGamePersistent->Environment().OnDeviceCreate();
}

void destroy_splash()
{
    // Destroy LOGO
    DestroyWindow(logoWindow);
    logoWindow = nullptr;
}

constexpr xr::tmc_atomic_wait_t code_start{std::numeric_limits<xr::tmc_atomic_wait_t>::min()};
constexpr xr::tmc_atomic_wait_t code_splash{std::numeric_limits<xr::tmc_atomic_wait_t>::min() / 2};
constexpr xr::tmc_atomic_wait_t code_exit{0};

tmc::task<void> startup(std::atomic<xr::tmc_atomic_wait_t>& code)
{
    code = xr::code_splash;
    code.notify_all();

    Discord.Init();

    // Main cycle
    std::ignore = Memory.mem_usage();
    co_await Device.Run();

    // Destroy APP
    xr_delete(g_SpatialSpacePhysic);
    xr_delete(g_SpatialSpace);

    DEL_INSTANCE(g_pGamePersistent);
    co_await pApp->co_destroy();
    Engine.Event.Dump();

    // Destroying
    xr_delete(pInput);

    if (!g_bBenchmark)
    {
        xr_delete(pSettings);
        xr_delete(pGameIni);
    }

    LALib.OnDestroy();

    if (!g_bBenchmark)
        xr_delete(Console);
    else
        co_await Console->OnScreenResolutionChanged();

    CSound_manager_interface::_destroy();

    co_await Device.Destroy();
    Engine.Destroy();
}
} // namespace
} // namespace xr

constexpr auto dwStickyKeysStructSize = sizeof(STICKYKEYS);
constexpr auto dwFilterKeysStructSize = sizeof(FILTERKEYS);
constexpr auto dwToggleKeysStructSize = sizeof(TOGGLEKEYS);

struct damn_keys_filter
{
    BOOL bScreenSaverState;

    // Sticky & Filter & Toggle keys

    STICKYKEYS StickyKeysStruct;
    FILTERKEYS FilterKeysStruct;
    TOGGLEKEYS ToggleKeysStruct;

    DWORD dwStickyKeysFlags;
    DWORD dwFilterKeysFlags;
    DWORD dwToggleKeysFlags;

    damn_keys_filter()
    {
        // Screen saver stuff

        bScreenSaverState = FALSE;

        // Saveing current state
        SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, (PVOID)&bScreenSaverState, 0);

        if (bScreenSaverState)
            // Disable screensaver
            SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, nullptr, 0);

        dwStickyKeysFlags = 0;
        dwFilterKeysFlags = 0;
        dwToggleKeysFlags = 0;

        std::memset(&StickyKeysStruct, 0, dwStickyKeysStructSize);
        std::memset(&FilterKeysStruct, 0, dwFilterKeysStructSize);
        std::memset(&ToggleKeysStruct, 0, dwToggleKeysStructSize);

        StickyKeysStruct.cbSize = dwStickyKeysStructSize;
        FilterKeysStruct.cbSize = dwFilterKeysStructSize;
        ToggleKeysStruct.cbSize = dwToggleKeysStructSize;

        // Saving current state
        SystemParametersInfo(SPI_GETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
        SystemParametersInfo(SPI_GETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
        SystemParametersInfo(SPI_GETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);

        if (StickyKeysStruct.dwFlags & SKF_AVAILABLE)
        {
            // Disable StickyKeys feature
            dwStickyKeysFlags = StickyKeysStruct.dwFlags;
            StickyKeysStruct.dwFlags = 0;
            SystemParametersInfo(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
        }

        if (FilterKeysStruct.dwFlags & FKF_AVAILABLE)
        {
            // Disable FilterKeys feature
            dwFilterKeysFlags = FilterKeysStruct.dwFlags;
            FilterKeysStruct.dwFlags = 0;
            SystemParametersInfo(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
        }

        if (ToggleKeysStruct.dwFlags & TKF_AVAILABLE)
        {
            // Disable FilterKeys feature
            dwToggleKeysFlags = ToggleKeysStruct.dwFlags;
            ToggleKeysStruct.dwFlags = 0;
            SystemParametersInfo(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
        }
    }

    ~damn_keys_filter()
    {
        if (bScreenSaverState)
            // Restoring screen saver
            SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, nullptr, 0);

        if (dwStickyKeysFlags)
        {
            // Restore StickyKeys feature
            StickyKeysStruct.dwFlags = dwStickyKeysFlags;
            SystemParametersInfo(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
        }

        if (dwFilterKeysFlags)
        {
            // Restore FilterKeys feature
            FilterKeysStruct.dwFlags = dwFilterKeysFlags;
            SystemParametersInfo(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
        }

        if (dwToggleKeysFlags)
        {
            // Restore FilterKeys feature
            ToggleKeysStruct.dwFlags = dwToggleKeysFlags;
            SystemParametersInfo(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
        }
    }
};

namespace xr
{
namespace
{
[[nodiscard]] s32 main(gsl::czstring cmdline, void* handle);
tmc::task<void> main_async(gsl::czstring cmdline, void* handle, std::atomic<xr::tmc_atomic_wait_t>& code);
} // namespace
} // namespace xr

namespace
{
int WinMain_impl(HINSTANCE hInstance, char* lpCmdLine)
{
    HANDLE hCheckPresenceMutex = INVALID_HANDLE_VALUE;
    if (!strstr(lpCmdLine, "-multi_instances"))
    { // Check for another instance
        constexpr const char* STALKER_PRESENCE_MUTEX = "STALKER-SoC";
        hCheckPresenceMutex = OpenMutex(READ_CONTROL, FALSE, STALKER_PRESENCE_MUTEX);
        if (hCheckPresenceMutex == nullptr)
        {
            // New mutex
            hCheckPresenceMutex = CreateMutex(nullptr, FALSE, STALKER_PRESENCE_MUTEX);
            if (hCheckPresenceMutex == nullptr)
                // Shit happens
                return 2;
        }
        else
        {
            // Already running
            CloseHandle(hCheckPresenceMutex);
            return 1;
        }
    }

    // Title window

    DisableProcessWindowsGhosting();
    logoWindow = ShowSplash(hInstance);

    return xr::main(lpCmdLine, hCheckPresenceMutex);
}
} // namespace

namespace xr
{
namespace
{
s32 main(gsl::czstring cmdline, void* handle)
{
    size_t cpus{};

    if (gsl::czstring key{"-max-threads"}, param = std::strstr(cmdline, key); param != nullptr)
    {
        if (sscanf_s(param + xr_strlen(key) + 1, "%zu", &cpus) == 1)
            cpus = std::clamp(cpus, 1uz, TMC_PLATFORM_BITS);
    }

    CPU::ID.topo = tmc::topology::query();
    auto& cpu = tmc::cpu_executor();

    if (CPU::ID.topo.is_hybrid())
    {
        tmc::topology::topology_filter p_cores, e_cores;
        p_cores.set_cpu_kinds(tmc::topology::cpu_kind::PERFORMANCE);
        e_cores.set_cpu_kinds(tmc::topology::cpu_kind::EFFICIENCY1);

        cpu.add_partition(p_cores, xr::tmc_priority_high, xr::tmc_priority_any + 1).add_partition(e_cores, xr::tmc_priority_any, xr::tmc_priority_low + 1);
    }

    if (cpus > 0)
        cpu.set_thread_count(cpus);

    cpu.fill_thread_occupancy().set_thread_init_hook([](tmc::topology::thread_info info) { CPU::ID.threads.emplace_back(std::move(info)); }).init();
    std::ranges::sort(CPU::ID.threads, [](const auto& a, const auto& b) { return a.index < b.index; });

    std::atomic<xr::tmc_atomic_wait_t> code{xr::code_start};
    tmc::post(cpu, xr::main_async(cmdline, handle, code), xr::tmc_priority_high, 0);
    code.wait(xr::code_start);

    if (code == xr::code_splash)
    {
        destroy_splash();
        code.wait(xr::code_splash);
    }

    return gsl::narrow_cast<s32>(code);
}

tmc::task<void> main_async(gsl::czstring cmdline, void* handle, std::atomic<xr::tmc_atomic_wait_t>& code)
{
    xr::tmc_cpu_st_executor().init();

    auto in = co_await tmc::fork_clang(CInput::co_create(), tmc::current_executor(), xr::tmc_priority_any);
    auto snd = co_await tmc::fork_clang(CSound_manager_interface::_create(0), tmc::current_executor(), xr::tmc_priority_any);

    string_path fsgame;
    fsgame[0] = '\0';

    if (gsl::czstring key{"-fsltx"}, param = std::strstr(cmdline, key); param != nullptr)
        sscanf(param + xr_strlen(key) + 1, "%[^ ] ", fsgame);

    Core._initialize("xray", nullptr, TRUE, fsgame[0] ? fsgame : nullptr);
    InitSettings();

    {
        damn_keys_filter filter;
        (void)filter;

        Engine.Initialize();
        co_await Device.Initialize();

        co_await std::move(in);
        pInput->Attach();
        InitConsole();

        Engine.External.CreateRendererList();
        Engine.External.Initialize();

        Console->Execute("stat_memory");
        co_await xr::exec_user_script();

        co_await std::move(snd);
        auto sp = tmc::fork_group();

        co_await sp.fork_clang(
            [] -> tmc::task<void> {
                g_SpatialSpace = xr_new<ISpatial_DB>();
                co_return;
            }(),
            tmc::current_executor(), xr::tmc_priority_any);

        co_await sp.fork_clang(
            [] -> tmc::task<void> {
                g_SpatialSpacePhysic = xr_new<ISpatial_DB>();
                co_return;
            }(),
            tmc::current_executor(), xr::tmc_priority_any);

        co_await CSound_manager_interface::_create(1);

        // ...command line for auto start
        LPCSTR pStartup = strstr(Core.Params, "-start ");
        if (pStartup)
            Console->Execute(pStartup + 1);

        pStartup = strstr(Core.Params, "-load ");
        if (pStartup)
            Console->Execute(pStartup + 1);

        co_await Device.process_frame_async();
        auto la = co_await tmc::fork_clang(LALib.OnCreate(), tmc::current_executor(), xr::tmc_priority_any);

        // Initialize APP
        co_await Device.Create();

        co_await std::move(la);
        co_await xr::pre_startup();

        co_await std::move(sp);
        co_await xr::startup(code);

        Core._destroy();

        if (std::strstr(cmdline, "-multi_instances") == nullptr)
            // Delete application presence mutex
            CloseHandle(handle);
    }
    // here damn_keys_filter class instanse will be destroyed

    code = xr::code_exit;
    code.notify_all();
}
} // namespace
} // namespace xr

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, char* lpCmdLine, int)
{
    gModulesLoaded = true;
    Debug._initialize();

    int ret = WinMain_impl(hInstance, lpCmdLine);
    ExitFromWinMain = true;

    return ret;
}

CApplication::CApplication()
{
    ll_dwReference = 0;

    // events
    eQuit = Engine.Event.Handler_Attach("KERNEL:quit", this);
    eStart = Engine.Event.Handler_Attach("KERNEL:start", this);
    eStartLoad = Engine.Event.Handler_Attach("KERNEL:load", this);
    eDisconnect = Engine.Event.Handler_Attach("KERNEL:disconnect", this);

    // levels
    Level_Current = 0;
    Level_Scan();

    // Register us
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 1000);

    Device.seqFrame.Add(&g_sound_processor, REG_PRIORITY_NORMAL - 1000); // Place it after Level update
    Device.seqFrameMT.Add(&g_sound_renderer);
}

tmc::task<std::unique_ptr<CApplication>> CApplication::co_create()
{
    auto app = absl::WrapUnique(new (xr_alloc<CApplication>(1)) CApplication{});

    co_await Console->Show();
    co_return app;
}

tmc::task<void> CApplication::co_destroy()
{
    co_await Console->Hide();

    auto app = this;
    xr_delete(app);
}

CApplication::~CApplication()
{
    Device.seqFrameMT.Remove(&g_sound_renderer);
    Device.seqFrame.Remove(&g_sound_processor);
    Device.seqFrame.Remove(this);

    // events
    Engine.Event.Handler_Detach(eDisconnect, this);
    Engine.Event.Handler_Detach(eStartLoad, this);
    Engine.Event.Handler_Detach(eStart, this);
    Engine.Event.Handler_Detach(eQuit, this);
}

tmc::task<void> CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
    if (E == eQuit)
    {
        auto scope = co_await tmc::enter(xr::tmc_cpu_st_executor());
        PostQuitMessage(0);
        co_await scope.exit();

        for (auto& level : Levels)
            xr_free(level.folder);
    }
    else if (E == eStart)
    {
        LPSTR op_server = LPSTR(P1);
        LPSTR op_client = LPSTR(P2);
        R_ASSERT(!g_pGameLevel);
        R_ASSERT(g_pGamePersistent);

        Console->Execute("main_menu off");
        co_await Console->Hide();

        g_pGamePersistent->PreStart(op_server);
        g_pGameLevel = smart_cast<IGame_Level*>(NEW_INSTANCE(CLSID_GAME_LEVEL));

        pApp->LoadBegin();
        co_await g_pGamePersistent->Start(op_server);
        g_pGameLevel->net_Start(op_server, op_client);
        pApp->LoadEnd();

        xr_free(op_server);
        xr_free(op_client);
    }
    else if (E == eDisconnect)
    {
        if (g_pGameLevel)
        {
            Console->Execute("main_menu off");
            co_await Console->Hide();

            co_await g_pGameLevel->net_Stop();
            DEL_INSTANCE(g_pGameLevel);
            co_await Console->Show();

            if ((FALSE == Engine.Event.Peek("KERNEL:quit")) && (FALSE == Engine.Event.Peek("KERNEL:start")))
            {
                Console->Execute("main_menu on");
            }
        }

        R_ASSERT(g_pGamePersistent);
        g_pGamePersistent->Disconnect();
    }
}

static CTimer phase_timer;
BOOL g_appLoaded = FALSE;

void CApplication::LoadBegin()
{
    ll_dwReference++;
    if (1 == ll_dwReference)
    {
        g_appLoaded = FALSE;

        phase_timer.Start();
        load_stage = 0;
    }
}

void CApplication::LoadEnd()
{
    if (--ll_dwReference == 0)
    {
        Msg("* phase time: %lld ms", phase_timer.GetElapsed_ms());
        Msg("* phase cmem: %zd K", Memory.mem_usage() / 1024);

        Console->Execute("stat_memory");
        g_appLoaded = TRUE;
    }
}

void CApplication::SetLoadingScreen(ILoadingScreen* newScreen)
{
    if (loadingScreen)
    {
        Log("! Trying to create new loading screen, but there is already one..");
        xr_delete(newScreen);
        return;
    }

    loadingScreen = newScreen;
}

void CApplication::DestroyLoadingScreen() { xr_delete(loadingScreen); }

tmc::task<void> CApplication::LoadDraw()
{
    if (g_appLoaded)
        co_return;

    Device.dwFrame++;

    if (!co_await Device.RenderBegin())
        co_return;

    load_draw_internal();

    co_await Device.RenderEnd();
}

void CApplication::LoadForceFinish() { loadingScreen->ForceFinish(); }

void CApplication::SetLoadStageTitle(pcstr _ls_title) { loadingScreen->SetStageTitle(_ls_title); }
void CApplication::LoadTitleInt() { loadingScreen->SetStageTip(); }

tmc::task<void> CApplication::LoadStage()
{
    VERIFY(ll_dwReference);

    Msg("* phase time: %lld ms", phase_timer.GetElapsed_ms());
    Msg("* phase cmem: %zd K", Memory.mem_usage() / 1024);

    phase_timer.Start();

    // if (g_pGamePersistent->GameType() == 1 && strstr(Core.Params, "alife"))
    max_load_stage = 16; // 17; //KRodin: пересчитал кол-во стадий, у нас их 15 при создании НИ + 1 на автопаузу
    // else
    //	max_load_stage = 14;

    co_await LoadDraw();

    ++load_stage;
    // Msg("--LoadStage is [%d]", load_stage);
}

// Sequential
tmc::task<void> CApplication::OnFrame()
{
    co_await Engine.Event.OnFrame();

    g_SpatialSpace->update();
    g_SpatialSpacePhysic->update();

    if (g_pGameLevel)
        g_pGameLevel->SoundEvent_Dispatch();
}

void CApplication::Level_Append(LPCSTR folder)
{
    string_path N1, N2, N3, N4;
    strconcat(sizeof(N1), N1, folder, "level");
    strconcat(sizeof(N2), N2, folder, "level.ltx");
    strconcat(sizeof(N3), N3, folder, "level.geom");
    strconcat(sizeof(N4), N4, folder, "level.cform");
    if (FS.exist("$game_levels$", N1) && FS.exist("$game_levels$", N2) && FS.exist("$game_levels$", N3) && FS.exist("$game_levels$", N4))
    {
        Levels.emplace_back(sLevelInfo{xr_strdup(folder)});
    }
}

void CApplication::Level_Scan()
{
    xr_vector<char*>* folder = FS.file_list_open("$game_levels$", FS_ListFolders | FS_RootOnly);
    R_ASSERT(folder && folder->size());
    for (u32 i = 0; i < folder->size(); i++)
        Level_Append((*folder)[i]);
    FS.file_list_close(folder);
#ifdef DEBUG
    folder = FS.file_list_open("$game_levels$", "$debug$\\", FS_ListFolders | FS_RootOnly);
    if (folder)
    {
        string_path tmp_path;
        for (u32 i = 0; i < folder->size(); i++)
        {
            strconcat(sizeof(tmp_path), tmp_path, "$debug$\\", (*folder)[i]);
            Level_Append(tmp_path);
        }

        FS.file_list_close(folder);
    }
#endif
}

// Taken from OpenXray/xray-16 and refactored
static void generate_logo_path(string_path& path, pcstr level_name, int num = -1)
{
    strconcat(sizeof(path), path, "intro\\intro_", level_name);

    if (num < 0)
        return;

    string16 buff;
    xr_strcat(path, sizeof(path), "_");
    xr_strcat(path, sizeof(path), _itoa(num + 1, buff, 10));
}

// Taken from OpenXray/xray-16 and refactored
// Return true if logo exists
// Always sets the path even if logo doesn't exist
static bool validate_logo_path(string_path& path, pcstr level_name, int num = -1)
{
    generate_logo_path(path, level_name, num);
    string_path temp;
    return FS.exist(temp, "$game_textures$", path, ".dds") || FS.exist(temp, "$level$", path, ".dds");
}

void CApplication::Level_Set(u32 L)
{
    if (L >= Levels.size())
        return;

    Level_Current = L;
    FS.get_path("$level$")->_set(Levels[L].folder);

    std::string temp = Levels[L].folder;
    temp.pop_back();
    const char* level_name = temp.c_str();

    static string_path path;
    path[0] = 0;

    int count = 0;
    while (true)
    {
        if (validate_logo_path(path, level_name, count))
            count++;
        else
            break;
    }

    if (count)
    {
        const int curr = ::Random.randI(count);
        generate_logo_path(path, level_name, curr);
    }
    else if (!validate_logo_path(path, level_name))
    {
        if (!validate_logo_path(path, "no_start_picture"))
            path[0] = 0;
    }

    if (path[0])
        loadingScreen->SetLevelLogo(path);

    loadingScreen->SetLevelText(level_name);
}

gsl::index CApplication::Level_ID(gsl::czstring name) const
{
    char buffer[256];
    strconcat(sizeof(buffer), buffer, name, "\\");

    for (auto [id, level] : xr::views_enumerate(Levels))
    {
        if (std::is_eq(xr::strcasecmp(buffer, level.folder)))
            return id;
    }

    return -1;
}

void CApplication::load_draw_internal() { loadingScreen->Update(load_stage, max_load_stage); }
