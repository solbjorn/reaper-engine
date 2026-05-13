#include "stdafx.h"

#include "..\xr_3da\XR_IOConsole.h"
#include "..\xr_3da\xr_ioc_cmd.h"
#include "../xr_3da/customhud.h"
#include "../xr_3da/fdemorecord.h"
#include "../xr_3da/fdemoplay.h"
#include "xrMessages.h"
#include "xrserver.h"
#include "level.h"
#include "ai_debug.h"
#include "alife_simulator.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "game_sv_single.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "Actor_Flags.h"
#include "customzone.h"
#include "script_engine.h"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"
#include "PhysicsGamePars.h"
#include "phworld.h"
#include "string_table.h"
#include "ai_space.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "date_time.h"
#include "mt_config.h"
#include "zone_effector.h"
#include "GameTask.h"
#include "MainMenu.h"
#include "saved_game_wrapper.h"
#include "level_graph.h"
#include "cameralook.h"
#include "ai_object_location.h"
#include "player_hud.h"

#include "HUDManager.h"
#include "UIGameCustom.h"
#include "../xr_3da/x_ray.h"

#include "map_manager.h"
#include "alife_graph_registry.h"
#include "game_graph.h"
#include "GamePersistent.h"

#include "CharacterPhysicsSupport.h"
#include "attachable_item.h"
#include "attachment_owner.h"
#include "InventoryOwner.h"
#include "Inventory.h"
#include "HUDTarget.h"

#ifdef DEBUG
#include "PHDebug.h"
#include "ui/UIDebugFonts.h"
#include "game_graph.h"
#endif // DEBUG

string_path g_last_saved_game;

extern float psSqueezeVelocity;

extern int x_m_x;
extern int x_m_z;

#ifdef DEBUG
extern BOOL g_ShowAnimationInfo;
#endif // DEBUG

extern ESingleGameDifficulty g_SingleGameDifficulty;

//-----------------------------------------------------------
float adj_delta_pos = 0.0005f;
float adj_delta_rot = 0.05f;
//-----------------------------------------------------------

extern BOOL g_enable_memory_debug;

#ifndef MASTER_GOLD
Flags32 g_mt_config = g_mt_default;
#endif

Flags32 dbg_net_Draw_Flags = {0};

#ifdef DEBUG
BOOL g_bDebugNode = FALSE;
u32 g_dwDebugNodeSource = 0;
u32 g_dwDebugNodeDest = 0;
extern BOOL g_bDrawBulletHit;
#endif
#ifdef DEBUG
extern LPSTR dbg_stalker_death_anim;
extern BOOL b_death_anim_velocity;
#endif
int g_AI_inactive_time = 0;

extern void show_animation_stats();

namespace
{
constexpr xr_token lua_gc_method_token[] = {{"gc_disable", 0}, {"gc_step", 1}, {"gc_timeout", 2}, {"gc_full", 3}, {nullptr, -1}};

void get_files_list(xr_vector<shared_str>& files, LPCSTR dir, LPCSTR file_ext)
{
    VERIFY(dir && file_ext);
    files.clear();

    FS_Path* P = FS.get_path(dir);
    P->m_Flags.set(FS_Path::flNeedRescan, TRUE);
    FS.m_Flags.set(CLocatorAPI::flNeedCheck, TRUE);
    FS.rescan_physical_pathes();

    string_path fext;
    xr_strconcat(fext, "*", file_ext);

    FS_FileSet files_set;
    std::ignore = FS.file_list(files_set, dir, FS_ListFiles, fext);
    u32 len_str_ext = xr_strlen(file_ext);

    FS_FileSetIt itb = files_set.begin();
    FS_FileSetIt ite = files_set.end();

    for (; itb != ite; ++itb)
    {
        LPCSTR fn_ext = (*itb).name.c_str();
        VERIFY(xr_strlen(fn_ext) > len_str_ext);

        string_path fn;
        strncpy_s(fn, sizeof(fn), fn_ext, xr_strlen(fn_ext) - len_str_ext);
        files.emplace_back(fn);
    }

    FS.m_Flags.set(CLocatorAPI::flNeedCheck, FALSE);
}

class CCC_MemStats : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_MemStats, IConsole_Command);

public:
    explicit CCC_MemStats(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_MemStats() override = default;

    void Execute(std::string_view) override
    {
        Memory.mem_compact();

        xr::render_memory_usage usage;

        //	Resource check moved to m_pRender
        if (Device.m_pRender != nullptr)
            Device.m_pRender->ResourcesGetMemoryUsage(usage);
        else
            std::memset(&usage, 0, sizeof(usage));

        Log("--------------------------------------------------------------------------------");

        SProcessMemInfo memCounters;
        GetProcessMemInfo(memCounters);

        Msg("[{} Mb] physical memory installed, [{} Mb] available, [{}] percent of memory in use", memCounters.TotalPhysicalMemory / (1024 * 1024),
            memCounters.FreePhysicalMemory / (1024 * 1024), memCounters.MemoryLoad);
        Msg("PageFile total: [{} Mb], free~ [{} Mb]", memCounters.TotalPageFile / (1024 * 1024), memCounters.FreePageFile / (1024 * 1024));

        // PeakWorkingSetSize
        //
        // The peak working set size, in bytes.
        //
        // WorkingSetSize
        //
        // The current working set size, in bytes.

        Msg("Engine memory usage (Working Set): [{} Mb], peak: [{} Mb]", memCounters.WorkingSetSize / (1024 * 1024),
            memCounters.PeakWorkingSetSize / (1024 * 1024));

        // PagefileUsage
        //
        // The Commit Charge value in bytes for this process. Commit Charge is the total amount of memory that the memory manager has committed for a running
        // process.
        //
        // PeakPagefileUsage
        //
        // The peak value in bytes of the Commit Charge during the lifetime of this process.

        Msg("Engine memory usage (Commit Charge): [{} Mb], peak: [{} Mb]", memCounters.PagefileUsage / (1024 * 1024),
            memCounters.PeakPagefileUsage / (1024 * 1024));

        Log("--------------------------------------------------------------------------------");

        const auto _process_heap = mem_usage_impl(nullptr, nullptr);
        const auto _eco_strings = str_container::stat_economy();
        const auto _eco_smem = smem_container::stat_economy();

        Msg("* [ D3D ]: textures count [{}]", usage.c_base + usage.c_lmaps);
        Msg("* [ D3D ]: textures [{} Kb]", (usage.m_base + usage.m_lmaps) / 1024);

        const auto script = xr::script_engine_initialized() ? ai().script_engine().lua().memory_used() : 0uz;
        Msg("* [ Lua ]: render [{} Kb], game [{} Kb]", usage.lua / 1024, script / 1024);

        Msg("* [x-ray]: process heap [{} Kb]", _process_heap / 1024);
        Msg("* [x-ray]: economy: strings [{} Kb], smem [{} Kb]", _eco_strings / 1024, _eco_smem / 1024);

#ifdef DEBUG
        Msg("* [x-ray]: file mapping: memory [{} Kb], count [{}]", g_file_mapped_memory / 1024, g_file_mapped_count);
        dump_file_mappings();
#endif // DEBUG
    }
};

// console commands
class CCC_GameDifficulty : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_GameDifficulty, CCC_Token);

public:
    explicit CCC_GameDifficulty(LPCSTR N) : CCC_Token{N, (u32*)&g_SingleGameDifficulty, difficulty_type_token} {}
    ~CCC_GameDifficulty() override = default;

    void Execute(std::string_view args) override
    {
        if (!Core.Features.test(xrCore::Feature::gd_master_only))
            CCC_Token::Execute(args);

        if (g_pGameLevel && Level().game)
        {
            game_cl_Single* game = smart_cast<game_cl_Single*>(Level().game);
            VERIFY(game);
            game->OnDifficultyChanged();
        }
    }

    [[nodiscard]] xr_string Info() const override { return "game difficulty"; }
};

xr_vector<xr_token> LanguagesToken;
u32 LanguageID{};
} // namespace

namespace xr
{
gsl::czstring GetLanguagesToken() { return LanguagesToken[LanguageID].name; }
} // namespace xr

namespace
{
class CCC_GameLanguage : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_GameLanguage, CCC_Token);

public:
    explicit CCC_GameLanguage(gsl::czstring N) : CCC_Token{N, &LanguageID, LanguagesToken.data()}
    {
        gsl::czstring str = pSettings->r_string("string_table", "language");

        for (gsl::index i{0}, count{_GetItemCount(str)}; i < count; i++)
        {
            string64 temp;
            std::ignore = _GetItem(str, i, temp);

            LanguagesToken.emplace_back(xr_strdup(temp), i);
        }

        LanguagesToken.emplace_back(nullptr, 0);
        tokens = LanguagesToken.data();
    }

    ~CCC_GameLanguage() override
    {
        for (const auto& tok : LanguagesToken)
        {
            auto name = const_cast<gsl::zstring>(tok.name);
            xr_free(name);
        }

        LanguagesToken.clear();
    }

    void Execute(std::string_view args) override
    {
        auto& arg = Device.add_frame_async(CallMe::fromMethod<&CCC_GameLanguage::execute_async>(this));
        *reinterpret_cast<gsl::zstring*>(&arg) = xr_strdup(args.data());
    }

private:
    tmc::task<void> execute_async(std::array<std::byte, 16>& arg)
    {
        auto args = *reinterpret_cast<gsl::zstring*>(&arg);

        CCC_Token::Execute(args);
        xr_free(args);

        co_await CStringTable::ReloadLanguage();

        if (g_pGamePersistent->IsMainMenuActive())
            MainMenu()->SetLanguageChanged(true);

        if (g_pGameLevel == nullptr)
            co_return;

        if (g_pGamePersistent->IsMainMenuActive())
        {
            co_await MainMenu()->Activate(false);
            co_await MainMenu()->Activate(true);
        }

        for (u32 id{0}; id < std::numeric_limits<ALife::_OBJECT_ID>::max(); id++)
        {
            if (auto gameObj = Level().Objects.net_Find(id); gameObj != nullptr)
            {
                if (auto invItem = smart_cast<CInventoryItem*>(gameObj); invItem != nullptr)
                    invItem->ReloadNames();
            }
        }
    }
};

#ifdef DEBUG
class CCC_ALifePath : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifePath, IConsole_Command);

public:
    explicit CCC_ALifePath(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ALifePath() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_level_graph() == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        const auto res = scn::scan<s32, s32>(args, "{} {}");
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto [id1, id2] = res.values();
        if (std::min(id1, id2) < 0 || std::max(id1, id2) > s64{ai().game_graph().header().vertex_count()} - 1)
        {
        inv:
            InvalidSyntax(xr::format("vertex id(s) out of bounds [0, {}]", ai().game_graph().header().vertex_count() - 1), args);
            return;
        }
    }
};
#endif // DEBUG

#ifndef MASTER_GOLD
class CCC_ALifeTimeFactor : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeTimeFactor, IConsole_Command);

public:
    explicit CCC_ALifeTimeFactor(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ALifeTimeFactor() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        if (res->value() < EPS_L)
        {
            InvalidSyntax("too low time factor", args);
            return;
        }

        Level().Server->game->SetGameTimeFactor(res->value());
    }
};

class CCC_ALifeSwitchDistance : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeSwitchDistance, IConsole_Command);

public:
    explicit CCC_ALifeSwitchDistance(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ALifeSwitchDistance() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_alife() == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        if (res->value() < 2.0f)
        {
            InvalidSyntax("too low online switch distance", args);
            return;
        }

        NET_Packet P;
        P.w_begin(M_SWITCH_DISTANCE);
        P.w_float(res->value());
        Level().Send(P, net_flags(TRUE, TRUE));
    }
};

class CCC_ALifeProcessTime : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeProcessTime, IConsole_Command);

public:
    explicit CCC_ALifeProcessTime(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ALifeProcessTime() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_alife() == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        const auto res = scn::scan_int<s32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        if (res->value() < 1)
        {
            InvalidSyntax("too low process time", args);
            return;
        }

        auto tpGame = smart_cast<game_sv_Single*>(Level().Server->game);
        VERIFY(tpGame);
        tpGame->alife().set_process_time(res->value());
    }
};

class CCC_ALifeObjectsPerUpdate : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeObjectsPerUpdate, IConsole_Command);

public:
    explicit CCC_ALifeObjectsPerUpdate(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ALifeObjectsPerUpdate() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_alife() == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        const auto res = scn::scan_int<s32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        auto tpGame = smart_cast<game_sv_Single*>(Level().Server->game);
        VERIFY(tpGame);
        tpGame->alife().objects_per_update(res->value());
    }
};

class CCC_ALifeSwitchFactor : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeSwitchFactor, IConsole_Command);

public:
    explicit CCC_ALifeSwitchFactor(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ALifeSwitchFactor() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_alife() == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        auto tpGame = smart_cast<game_sv_Single*>(Level().Server->game);
        VERIFY(tpGame);
        tpGame->alife().set_switch_factor(std::clamp(res->value(), 0.1f, 1.0f));
    }
};
#endif // !MASTER_GOLD

class CCC_TimeFactor : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_TimeFactor, IConsole_Command);

public:
    explicit CCC_TimeFactor(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_TimeFactor() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        Device.time_factor(std::clamp(res->value(), 0.001f, 1000.0f));
    }
};

class CCC_DemoRecord : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DemoRecord, IConsole_Command);

public:
    explicit CCC_DemoRecord(gsl::czstring N) : IConsole_Command{N} {}
    ~CCC_DemoRecord() override = default;

    void Execute(std::string_view args) override
    {
        if (g_pGameLevel == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        auto& arg = Device.add_frame_async(CallMe::fromMethod<&CCC_DemoRecord::execute_async>(this));
        *reinterpret_cast<gsl::zstring*>(&arg) = xr_strdup(args.data());
    }

private:
    tmc::task<void> execute_async(std::array<std::byte, 16>& arg)
    {
        co_await Console->Hide();

        if (MainMenu()->IsActive())
            co_await MainMenu()->Activate(false);

        auto args = *reinterpret_cast<gsl::zstring*>(&arg);
        string_path fn;

        xr_strconcat(fn, args, ".xrdemo");
        xr_free(args);
        std::ignore = FS.update_path(fn, "$game_saves$", fn);

        g_pGameLevel->Cameras().AddCamEffector((co_await CDemoRecord::co_create(fn)).release());
    }
};

class CCC_DemoPlay : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DemoPlay, IConsole_Command);

public:
    explicit CCC_DemoPlay(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DemoPlay() override = default;

    void Execute(std::string_view args) override
    {
        if (!g_pGameLevel)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        auto& arg = Device.add_frame_async(CallMe::fromMethod<&CCC_DemoPlay::execute_async>(this));
        *reinterpret_cast<gsl::zstring*>(&arg) = xr_strdup(args.data());
    }

private:
    tmc::task<void> execute_async(std::array<std::byte, 16>& arg)
    {
        co_await Console->Hide();

        auto args = *reinterpret_cast<gsl::zstring*>(&arg);
        u32 loops{};

        if (auto comma = std::strchr(args, ','); comma != nullptr)
        {
            if (const auto res = scn::scan_int<u32>(std::string_view{&comma[1]}); !res)
            {
                InvalidSyntax(res.error().msg(), args);
                co_return;
            }
            else
            {
                loops = res->value();
                comma[0] = '\0';
            }
        }

        string_path fn;
        xr_strconcat(fn, args, ".xrdemo");
        xr_free(args);

        std::ignore = FS.update_path(fn, "$game_saves$", fn);
        g_pGameLevel->Cameras().AddCamEffector(xr_new<CDemoPlay>(fn, 1.0f, loops));
    }
};

bool valid_file_name(LPCSTR file_name)
{
    LPCSTR I = file_name;
    LPCSTR E = file_name + xr_strlen(file_name);

    for (; I != E; ++I)
    {
        if (!strchr("/\\:*?\"<>|", *I))
            continue;

        return (false);
    }

    return (true);
}

class CCC_ALifeSave : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeSave, IConsole_Command);

public:
    explicit CCC_ALifeSave(gsl::czstring N) : IConsole_Command{N, true} {}
    ~CCC_ALifeSave() override = default;

    void Execute(std::string_view args) override
    {
        if (g_actor == nullptr || !Actor()->g_Alive())
        {
            InvalidSyntax("the actor is dead :(", args);
            return;
        }

        const bool named = !args.empty();
        if (named && !valid_file_name(args.data()))
        {
            InvalidSyntax("invalid file name", args);
            return;
        }

        auto& arg = Device.add_frame_async(CallMe::fromMethod<&CCC_ALifeSave::execute_async>(this));
        *reinterpret_cast<gsl::zstring*>(&arg) = xr_strdup(named ? args.data() : "quick");
    }

private:
    tmc::task<void> execute_async(std::array<std::byte, 16>& arg)
    {
#ifdef DEBUG
        CTimer timer;
        timer.Start();
#endif

        auto args = *reinterpret_cast<gsl::zstring*>(&arg);
        string_path S;
        S[0] = '\0';

        if (std::is_eq(xr_strcmp(args, "quick")))
        {
            xr_strconcat(S, Core.UserName, "_", "quicksave");

            NET_Packet net_packet;
            net_packet.w_begin(M_SAVE_GAME);
            net_packet.w_stringZ(S);
            net_packet.w_u8(0);

            Level().Send(net_packet, net_flags(TRUE));
        }
        else
        {
            xr_strcpy(S, args);

            NET_Packet net_packet;
            net_packet.w_begin(M_SAVE_GAME);
            net_packet.w_stringZ(S);
            net_packet.w_u8(1);

            Level().Send(net_packet, net_flags(TRUE));
        }

        xr_free(args);

#ifdef DEBUG
        Msg("Game save overhead  : {} milliseconds", timer.GetElapsed_sec() * 1000.f);
#endif

        SDrawStaticStruct* _s = HUD().GetUI()->UIGame()->AddCustomStatic("game_saved", true);
        _s->m_endTime = Device.fTimeGlobal + 3.0f; // 3sec

        string_path save_name;
        strconcat(sizeof(save_name), save_name, *CStringTable().translate(shared_str{"st_game_saved"}), ": ", S);
        _s->wnd()->SetText(save_name);

        xr_strcat(S, ".dds");
        string_path S1;
        std::ignore = FS.update_path(S1, "$game_saves$", S);

#ifdef DEBUG
        timer.Start();
#endif

        co_await MainMenu()->Screenshot(IRender_interface::SM_FOR_GAMESAVE, S1);

#ifdef DEBUG
        Msg("Screenshot overhead : {} milliseconds", timer.GetElapsed_sec() * 1000.f);
#endif
    }
};

class CCC_ALifeLoadFrom : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ALifeLoadFrom, IConsole_Command);

public:
    explicit CCC_ALifeLoadFrom(gsl::czstring N) : IConsole_Command{N, true} {}
    ~CCC_ALifeLoadFrom() override = default;

    void Execute(std::string_view args) override
    {
        if (!CSavedGameWrapper::valid_saved_game(args.data()))
        {
            InvalidSyntax("invalid or corrupted saved game", args);
            return;
        }

        if (ai().get_alife() == nullptr)
        {
            Console->Execute(xr::format("start server({}/single/alife/load)", args).c_str());
            return;
        }

        auto& arg = Device.add_frame_async(CallMe::fromMethod<&CCC_ALifeLoadFrom::execute_async>(this));
        *reinterpret_cast<gsl::zstring*>(&arg) = xr_strdup(args.data());
    }

private:
    tmc::task<void> execute_async(std::array<std::byte, 16>& arg)
    {
        if (MainMenu()->IsActive())
            co_await MainMenu()->Activate(false);

        co_await Console->Hide();
        ::Sound->set_master_volume(0.0f);

        if (Device.Paused())
            Device.Pause(false, true, true, "CCC_ALifeLoadFrom");

        auto args = *reinterpret_cast<gsl::zstring*>(&arg);

        NET_Packet net_packet;
        net_packet.w_begin(M_LOAD_GAME);
        net_packet.w_stringZ(args);

        xr_free(args);
        Level().Send(net_packet, net_flags(true));
    }

    void fill_tips(vecTips& tips) override { get_files_list(tips, "$game_saves$", SAVE_EXTENSION); }
};

class CCC_LoadLastSave : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_LoadLastSave, IConsole_Command);

public:
    explicit CCC_LoadLastSave(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_LoadLastSave() override = default;

    void Execute(std::string_view args) override
    {
        if (!args.empty())
        {
            strcpy_s(g_last_saved_game, args.data());
            return;
        }

        if (!*g_last_saved_game)
        {
            InvalidSyntax("last saved game not set", "(no arguments)");
            return;
        }

        std::string_view pref, post;
        if (ai().get_alife() == nullptr)
        {
            pref = "start server(";
            post = "/single/alife/load)";
        }
        else
        {
            pref = "load ";
            post = "";
        }

        Console->Execute(xr::format("{}{}{}", pref, g_last_saved_game, post).c_str());
    }

    void Save(IWriter* F) override
    {
        if (!*g_last_saved_game)
            return;

        F->w_printf("{} {}\r\n", cName, g_last_saved_game);
    }
};

class CCC_FloatBlock : public CCC_Float
{
    RTTI_DECLARE_TYPEINFO(CCC_FloatBlock, CCC_Float);

public:
    explicit CCC_FloatBlock(LPCSTR N, float* V, float _min = 0, float _max = 1) : CCC_Float{N, V, _min, _max} {}
    ~CCC_FloatBlock() override = default;
};

#ifdef DEBUG
class CCC_DrawGameGraphAll : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DrawGameGraphAll, IConsole_Command);

public:
    explicit CCC_DrawGameGraphAll(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DrawGameGraphAll() override = default;

    void Execute(std::string_view) override
    {
        if (ai().get_level_graph() == nullptr)
        {
            InvalidSyntax("load a level to execute", "(no arguments)");
            return;
        }

        ai().level_graph().setup_current_level(-1);
    }
};

class CCC_DrawGameGraphCurrent : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DrawGameGraphCurrent, IConsole_Command);

public:
    explicit CCC_DrawGameGraphCurrent(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DrawGameGraphCurrent() override = default;

    void Execute(std::string_view) override
    {
        if (ai().get_level_graph() == nullptr)
        {
            InvalidSyntax("load a level to execute", "(no arguments)");
            return;
        }

        ai().level_graph().setup_current_level(ai().level_graph().level_id());
    }
};

class CCC_DrawGameGraphLevel : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DrawGameGraphLevel, IConsole_Command);

public:
    explicit CCC_DrawGameGraphLevel(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_DrawGameGraphLevel() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_level_graph() == nullptr)
        {
            InvalidSyntax("load a level to execute", "(no arguments)");
            return;
        }

        const auto res = scn::scan_value<xr_string>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto level = ai().game_graph().header().level(res->value.c_str(), true);
        if (level == nullptr)
        {
            InvalidSyntax("invalid level name", args);
            return;
        }

        ai().level_graph().setup_current_level(level->id());
    }
};

class CCC_DumpInfos : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DumpInfos, IConsole_Command);

public:
    explicit CCC_DumpInfos(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DumpInfos() override = default;

    void Execute(std::string_view) override
    {
        CActor* A = smart_cast<CActor*>(Level().CurrentEntity());
        if (A)
            A->DumpInfo();
    }

    [[nodiscard]] xr_string Info() const override { return "dumps all infoportions that actor have"; }
};

class CCC_DumpMap : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DumpMap, IConsole_Command);

public:
    explicit CCC_DumpMap(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DumpMap() override = default;

    void Execute(std::string_view) override { Level().MapManager().Dump(); }
    [[nodiscard]] xr_string Info() const override { return "dumps all currentmap locations"; }
};

class CCC_DumpCreatures : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DumpCreatures, IConsole_Command);

public:
    explicit CCC_DumpCreatures(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DumpCreatures() override = default;

    void Execute(std::string_view) override
    {
        typedef CSafeMapIterator<ALife::_OBJECT_ID, CSE_ALifeDynamicObject>::_REGISTRY::const_iterator const_iterator;

        const_iterator I = ai().alife().graph().level().objects().begin();
        const_iterator E = ai().alife().graph().level().objects().end();
        for (; I != E; ++I)
        {
            CSE_ALifeCreatureAbstract* obj = smart_cast<CSE_ALifeCreatureAbstract*>(I->second);
            if (obj)
                Msg("\"{}\",", obj->name_replace());
        }
    }

    [[nodiscard]] xr_string Info() const override { return "dumps all creature names"; }
};

class CCC_DebugFonts : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DebugFonts, IConsole_Command);

public:
    explicit CCC_DebugFonts(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DebugFonts() override = default;

    void Execute(std::string_view) override { HUD().GetUI()->StartStopMenu(xr_new<CUIDebugFonts>(), true); }
};

class CCC_DebugNode : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DebugNode, IConsole_Command);

public:
    explicit CCC_DebugNode(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_DebugNode() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn.scan<u32, u32>(args, "{} {}");
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto [value1, value2] = res->values();
        if ((value1 > 0) && (value2 > 0))
        {
            g_bDebugNode = TRUE;
            g_dwDebugNodeSource = value1;
            g_dwDebugNodeDest = value2;
        }
        else
        {
            g_bDebugNode = FALSE;
        }
    }
};

class CCC_ShowMonsterInfo : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ShowMonsterInfo, IConsole_Command);

public:
    explicit CCC_ShowMonsterInfo(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_ShowMonsterInfo() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan<xr_string, u32>(args, "{} {}");
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto& [name, val] = res->values();

        auto monster = smart_cast<CBaseMonster*>(Level().Objects.FindObjectByName(name.c_str()));
        if (monster == nullptr)
        {
            InvalidSyntax("invalid creature name", name);
            return;
        }

        monster->set_show_debug_info(val);
    }
};

class CCC_DbgPhTrackObj : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgPhTrackObj, IConsole_Command);

public:
    explicit CCC_DbgPhTrackObj(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_DbgPhTrackObj() override = default;

    void Execute(std::string_view args) override
    {
        ph_dbg_draw_mask1.set(ph_m1_DbgTrackObject, TRUE);
        PH_DBG_SetTrackObject(args.data());
    }
};
#endif

class CCC_PHIterations : public CCC_Integer
{
    RTTI_DECLARE_TYPEINFO(CCC_PHIterations, CCC_Integer);

public:
    explicit CCC_PHIterations(LPCSTR N) : CCC_Integer{N, &phIterations, 15, 50} {}
    ~CCC_PHIterations() override = default;

    void Execute(std::string_view args) override
    {
        CCC_Integer::Execute(args);
        dWorldSetQuickStepNumIterations(nullptr, phIterations);
    }
};

class CCC_PHGravity : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_PHGravity, IConsole_Command);

public:
    explicit CCC_PHGravity(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_PHGravity() override = default;

    void Execute(std::string_view args) override
    {
        if (ph_world == nullptr)
            return;

        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        ph_world->SetGravity(res->value());
    }

    [[nodiscard]] xr_string Status() const override { return xr::format("{}", ph_world != nullptr ? ph_world->Gravity() : default_world_gravity); }
};

class CCC_PHFps : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_PHFps, IConsole_Command);

public:
    explicit CCC_PHFps(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_PHFps() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        CPHWorld::SetStep(1.0f / std::clamp(res->value(), 50.0f, 200.0f));
    }

    [[nodiscard]] xr_string Status() const override { return xr::format("{}", 1.0f / fixed_step); }
};

struct CCC_JumpToLevel : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_JumpToLevel, IConsole_Command);

public:
    explicit CCC_JumpToLevel(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_JumpToLevel() override = default;

    void Execute(std::string_view args) override
    {
        if (ai().get_alife() == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        const auto res = scn::scan_value<xr_string>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto& level = res->value();

        for (const auto& lvl : ai().game_graph().header().levels())
        {
            if (std::is_eq(xr_strcmp(lvl.second.name(), level)))
            {
                ai().alife().jump_to_level(level.c_str());
                return;
            }
        }

        InvalidSyntax("invalid level name", level);
    }

    void fill_tips(vecTips& tips) override
    {
        if (!ai().get_alife())
        {
            Log("! ALife simulator is needed to perform specified command!");
            return;
        }

        for (const auto& lvl : ai().game_graph().header().levels())
            tips.emplace_back(lvl.second.name());
    }
};

class CCC_Spawn : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Spawn, IConsole_Command);

public:
    explicit CCC_Spawn(LPCSTR N) : IConsole_Command(N) {}
    ~CCC_Spawn() override = default;

    void Execute(std::string_view args) override
    {
        if (g_pGameLevel == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        if (!pSettings->section_exist(args.data()))
        {
            InvalidSyntax("invalid section", args);
            return;
        }

        if (auto tpGame = smart_cast<game_sv_Single*>(Level().Server->game))
            tpGame->alife().spawn_item(args.data(), Actor()->Position(), Actor()->ai_location().level_vertex_id(), Actor()->ai_location().game_vertex_id(),
                                       ALife::_OBJECT_ID(-1));
    }

    void fill_tips(vecTips& tips) override
    {
        if (!ai().get_alife())
        {
            Log("! ALife simulator is needed to perform specified command!");
            return;
        }

        for (const auto& it : pSettings->sections())
        {
            auto& section = it.first;

            if (pSettings->line_exist(section, "class"))
            {
                tips.push_back(section);
            }
        }

        std::sort(tips.begin(), tips.end());
    }
};

class CCC_SpawnToInventory : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_SpawnToInventory, IConsole_Command);

public:
    explicit CCC_SpawnToInventory(LPCSTR N) : IConsole_Command(N) {}
    ~CCC_SpawnToInventory() override = default;

    void Execute(std::string_view args) override
    {
        if (g_pGameLevel == nullptr)
        {
            InvalidSyntax("load a level to execute", args);
            return;
        }

        if (!pSettings->section_exist(args.data()))
        {
            InvalidSyntax("invalid section", args);
            return;
        }

        if (auto tpGame = smart_cast<game_sv_Single*>(Level().Server->game))
        {
            NET_Packet packet;
            packet.w_begin(M_SPAWN);
            packet.w_stringZ(args.data());

            CSE_Abstract* item = tpGame->alife().spawn_item(args.data(), Actor()->Position(), Actor()->ai_location().level_vertex_id(),
                                                            Actor()->ai_location().game_vertex_id(), 0, false);
            item->Spawn_Write(packet, FALSE);
            tpGame->alife().server().FreeID(item->ID, 0);
            F_entity_Destroy(item);

            ClientID clientID;
            clientID.set(0xffff);

            u16 dummy;
            std::ignore = packet.r_begin(dummy);
            VERIFY(dummy == M_SPAWN);

            tpGame->alife().server().Process_spawn(packet, clientID);
        }
    }

    void fill_tips(vecTips& tips) override
    {
        if (!ai().get_alife())
        {
            Log("! ALife simulator is needed to perform specified command!");
            return;
        }

        for (const auto& it : pSettings->sections())
        {
            auto& section = it.first;

            if (pSettings->line_exist(section, "class"))
                tips.push_back(section);
        }

        std::sort(tips.begin(), tips.end());
    }
};

class CCC_LuaGCMethod : public CCC_Token
{
    RTTI_DECLARE_TYPEINFO(CCC_LuaGCMethod, CCC_Token);

public:
    explicit CCC_LuaGCMethod(gsl::czstring name) : CCC_Token{name, &ps_lua_gc_method, lua_gc_method_token} {}
    ~CCC_LuaGCMethod() override = default;

    void Execute(std::string_view args) override
    {
        const auto prev = *value;
        CCC_Token::Execute(args);

        if (!xr::script_engine_initialized())
            return;

        auto& lua = ai().script_engine().lua();
        switch (*value)
        {
        case 0: lua.stop_gc(); break;
        case 1:
        case 2:
            if (prev == 0)
                lua.restart_gc();

            break;
        case 3:
            // Perform a full garbage collection cycle and return to previous strategy.
            lua.collect_gc();
            *value = prev;

            break;
        }
    }
};

class CCC_MainMenu : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_MainMenu, IConsole_Command);

public:
    explicit CCC_MainMenu(gsl::czstring N) : IConsole_Command{N, true} {}
    ~CCC_MainMenu() override = default;

    void Execute(std::string_view args) override
    {
        auto& arg = Device.add_frame_async(CallMe::fromMethod<&CCC_MainMenu::execute_async>(this));
        *reinterpret_cast<gsl::zstring*>(&arg) = xr_strdup(!args.empty() ? args.data() : "toggle");
    }

private:
    tmc::task<void> execute_async(std::array<std::byte, 16>& arg)
    {
        auto args = *reinterpret_cast<gsl::zstring*>(&arg);
        bool bWhatToDo{true};

        if (std::is_eq(xr_strcmp(args, "toggle")))
            bWhatToDo = !MainMenu()->IsActive();
        else if (std::is_eq(xr_strcmp(args, "on")) || std::is_eq(xr_strcmp(args, "1")))
            bWhatToDo = true;
        else if (std::is_eq(xr_strcmp(args, "off")) || std::is_eq(xr_strcmp(args, "0")))
            bWhatToDo = false;

        xr_free(args);
        co_await MainMenu()->Activate(bWhatToDo);
    }
};

#ifndef MASTER_GOLD
struct CCC_StartTimeSingle : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_StartTimeSingle, IConsole_Command);

public:
    explicit CCC_StartTimeSingle(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_StartTimeSingle() override = default;

    void Execute(std::string_view args) override
    {
        const auto res = scn::scan<u32, u32, u32, u32, u32, u32, u32>(args, "{}.{}.{} {}:{}:{}.{}");
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        const auto& [year, month, day, hours, mins, secs, milisecs] = res->values();
        g_qwStartGameTime = generate_time(std::max(year, 1u), std::max(month, 1u), std::max(day, 1u), hours, mins, secs, milisecs);

        if (g_pGameLevel == nullptr || Level().Server == nullptr || Level().Server->game == nullptr)
            return;

        Level().Server->game->SetGameTimeFactor(g_qwStartGameTime, g_fTimeFactor);
    }

    [[nodiscard]] xr_string Status() const override
    {
        u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
        split_time(g_qwStartGameTime, year, month, day, hours, mins, secs, milisecs);

        return xr::format("{}.{}.{} {}:{}:{}.{}", year, month, day, hours, mins, secs, milisecs);
    }
};

struct CCC_TimeFactorSingle : public CCC_Float
{
    RTTI_DECLARE_TYPEINFO(CCC_TimeFactorSingle, CCC_Float);

public:
    explicit CCC_TimeFactorSingle(LPCSTR N, float* V, float _min = 0.f, float _max = 1.f) : CCC_Float{N, V, _min, _max} {}
    ~CCC_TimeFactorSingle() override = default;

    void Execute(std::string_view args) override
    {
        CCC_Float::Execute(args);

        if (g_pGameLevel == nullptr || Level().Server == nullptr || Level().Server->game == nullptr)
            return;

        Level().Server->game->SetGameTimeFactor(g_fTimeFactor);
    }
};
#endif // !MASTER_GOLD

#ifdef DEBUG
class CCC_RadioGroupMask2;

class CCC_RadioMask : public CCC_Mask
{
    RTTI_DECLARE_TYPEINFO(CCC_RadioGroupMask, CCC_Mask);

private:
    CCC_RadioGroupMask2* group{};

public:
    explicit CCC_RadioMask(LPCSTR N, Flags32* V, u32 M) : CCC_Mask{N, V, M} {}
    ~CCC_RadioMask() override = default;

    void SetGroup(CCC_RadioGroupMask2* G) { group = G; }
    void Execute(std::string_view args) override;

    IC void Set(BOOL V) { value->set(mask, V); }
};

class CCC_RadioGroupMask2
{
    CCC_RadioMask* mask0;
    CCC_RadioMask* mask1;

public:
    explicit CCC_RadioGroupMask2(CCC_RadioMask* m0, CCC_RadioMask* m1)
    {
        mask0 = m0;
        mask1 = m1;
        mask0->SetGroup(this);
        mask1->SetGroup(this);
    }

    ~CCC_RadioGroupMask2() override = default;

    void Execute(CCC_RadioMask& m, std::string_view)
    {
        BOOL value = m.GetValue();
        if (value)
        {
            mask0->Set(!value);
            mask1->Set(!value);
        }

        m.Set(value);
    }
};

void CCC_RadioMask::Execute(std::string_view args)
{
    CCC_Mask::Execute(args);
    VERIFY2(group, "CCC_RadioMask: group not set");
    group->Execute(*this, args);
}

#define CMD_RADIOGROUPMASK2(p1, p2, p3, p4, p5, p6) \
    { \
        static CCC_RadioMask x##CCC_RadioMask1(p1, p2, p3); \
        Console->AddCommand(&x##CCC_RadioMask1); \
        static CCC_RadioMask x##CCC_RadioMask2(p4, p5, p6); \
        Console->AddCommand(&x##CCC_RadioMask2); \
        static CCC_RadioGroupMask2 x##CCC_RadioGroupMask2(&x##CCC_RadioMask1, &x##CCC_RadioMask2); \
    } \
    XR_MACRO_END()

struct CCC_DbgBullets : public CCC_Integer
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgBullets, CCC_Integer);

public:
    explicit CCC_DbgBullets(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer{N, V, _min, _max} {}
    ~CCC_DbgBullets() override = default;

    void Execute(std::string_view args) override
    {
        extern FvectorVec g_hit[];
        g_hit[0].clear();
        g_hit[1].clear();
        g_hit[2].clear();
        CCC_Integer::Execute(args);
    }
};
#endif // DEBUG

class CCC_TuneAttachableItem : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_TuneAttachableItem, IConsole_Command);

public:
    explicit CCC_TuneAttachableItem(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_TuneAttachableItem() override = default;

    void Execute(std::string_view args) override
    {
        if (!g_pGameLevel) // level not loaded
            return;

        if (CAttachableItem::m_dbgItem)
        {
            CAttachableItem::m_dbgItem = nullptr;
            Msg("~~[{}] switched to off", __FUNCTION__);
            return;
        }

        if (!g_hud)
            return;

        CObject* obj = ((CHUDManager*)g_hud)->GetTarget()->GetObj();
        auto owner = smart_cast<CAttachmentOwner*>(obj);
        if (!owner)
        {
            obj = Level().CurrentViewEntity();
            owner = smart_cast<CAttachmentOwner*>(obj);
        }

        if (!owner)
            return;

        if (auto itm = owner->attachedItem(shared_str{args}))
        {
            CAttachableItem::m_dbgItem = itm;
        }
        else
        {
            auto iowner = smart_cast<CInventoryOwner*>(obj);
            if (iowner != nullptr)
            {
                auto active_item = iowner->m_inventory->ActiveItem();
                if (active_item != nullptr && std::is_eq(xr_strcmp(active_item->object().cNameSect(), args)))
                    CAttachableItem::m_dbgItem = active_item->cast_attachable_item();
            }
        }

        if (CAttachableItem::m_dbgItem)
            Msg("--[{}] switched to ON for [{}]", __FUNCTION__, args);
        else
            Msg("!![{}] cannot find attached item [{}]", __FUNCTION__, args);
    }

    [[nodiscard]] xr_string Info() const override
    {
        return "allows to change bind rotation and position offsets for attached item, <section_name> given as arguments";
    }
};

class CCC_TuneAttachableItemInSlot : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_TuneAttachableItemInSlot, IConsole_Command);

public:
    explicit CCC_TuneAttachableItemInSlot(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_TuneAttachableItemInSlot() override = default;

    void Execute(std::string_view args) override
    {
        if (!g_pGameLevel) // level not loaded
            return;

        if (CAttachableItem::m_dbgItem)
        {
            CAttachableItem::m_dbgItem = nullptr;
            Msg("~~[{}] switched to off", __FUNCTION__);
            return;
        }

        if (!g_hud)
            return;

        CObject* obj = ((CHUDManager*)g_hud)->GetTarget()->GetObj();
        auto owner = smart_cast<CAttachmentOwner*>(obj);
        if (!owner)
        {
            obj = Level().CurrentViewEntity();
            owner = smart_cast<CAttachmentOwner*>(obj);
        }

        if (!owner)
            return;

        auto iowner = smart_cast<CInventoryOwner*>(obj);
        if (iowner)
        {
            const auto res = scn::scan_int<u32>(args);
            if (!res)
            {
                InvalidSyntax(res.error().msg(), args);
                return;
            }

            if (res->value() >= SLOTS_TOTAL)
            {
                InvalidSyntax("invalid slot", args);
                return;
            }

            auto active_item = iowner->m_inventory->ItemFromSlot(res->value());
            if (active_item != nullptr)
                CAttachableItem::m_dbgItem = active_item->cast_attachable_item();
        }

        if (CAttachableItem::m_dbgItem)
            Msg("--[{}] switched to ON for item in slot [{}]", __FUNCTION__, args);
        else
            Msg("!![{}] cannot find attached item in slot [{}]", __FUNCTION__, args);
    }

    [[nodiscard]] xr_string Info() const override
    {
        return "allows to change bind rotation and position offsets for attached item, <section_name> given as arguments";
    }
};

#ifdef DEBUG
class CCC_Crash : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_Crash, IConsole_Command);

public:
    explicit CCC_Crash(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_Crash() override = default;

    void Execute(std::string_view) override
    {
        VERIFY3(false, "This is a test crash", "Do not post it as a bug");
        int* pointer = nullptr;
        *pointer = 0;
    }
};

class CCC_DumpModelBones : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DumpModelBones, IConsole_Command);

public:
    explicit CCC_DumpModelBones(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_DumpModelBones() override = default;

    void Execute(std::string_view args) override
    {
        if (args.empty())
        {
            Log("! no arguments passed");
            return;
        }

        string_path name;
        string_path fn;

        if (0 == strext(args.data()))
            strconcat(sizeof(name), name, args.data(), ".ogf");
        else
            strcpy_s(name, sizeof(name), args.data());

        if (!FS.exist(args.data()) && !FS.exist(fn, "$level$", name) && !FS.exist(fn, "$game_meshes$", name))
        {
            Msg("! Cannot find visual \"{}\"", args);
            return;
        }

        IRenderVisual* visual = Render->model_Create(args.data());
        IKinematics* kinematics = smart_cast<IKinematics*>(visual);
        if (!kinematics)
        {
            Render->model_Delete(visual);
            InvalidSyntax("! Invalid visual type (not a IKinematics)", args);
            return;
        }

        Msg("bones for model \"{}\"", args);

        for (u16 i = 0, n = kinematics->LL_BoneCount(); i < n; ++i)
            Log(kinematics->LL_GetData(i).name);

        Render->model_Delete(visual);
    }
};

class CCC_ShowAnimationStats : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_ShowAnimationStats, IConsole_Command);

public:
    explicit CCC_ShowAnimationStats(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_ShowAnimationStats() override = default;

    void Execute(std::string_view) override { show_animation_stats(); }
};

class CCC_DumpObjects : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DumpObjects, IConsole_Command);

public:
    explicit CCC_DumpObjects(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DumpObjects() override = default;

    void Execute(std::string_view) override { Level().Objects.dump_all_objects(); }
};
#endif // DEBUG

// Change weather immediately
class CCC_SetWeather : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_SetWeather, IConsole_Command);

public:
    explicit CCC_SetWeather(LPCSTR N) : IConsole_Command{N} {}
    ~CCC_SetWeather() override = default;

    void Execute(std::string_view args) override
    {
        if (args.empty())
        {
            InvalidSyntax("insufficient args", "(no arguments)");
            return;
        }

        if (!g_pGameLevel)
            return;
        if (!g_pGamePersistent)
            return;

        g_pGamePersistent->Environment().SetWeather(shared_str{args}, true);
    }

    void fill_tips(vecTips& tips) override
    {
        if (!g_pGameLevel)
            return;
        if (!g_pGamePersistent)
            return;

        for (auto& [name, cycle] : g_pGamePersistent->Environment().WeatherCycles)
            tips.push_back(name);
    }
};

#ifdef USE_MEMORY_VALIDATOR
class CCC_DbgMemoryDump : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgMemoryDump, IConsole_Command);

public:
    explicit CCC_DbgMemoryDump(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DbgMemoryDump() override = default;

    void Execute(std::string_view args) override
    {
        f32 thresholdInKb{1.0f};

        if (args.empty())
        {
        dump:
            PointerRegistryDump(thresholdInKb);
            return;
        }

        const auto res = scn::scan_value<f32>(args);
        if (!res)
        {
            InvalidSyntax(res.error().msg(), args);
            return;
        }

        thresholdInKb = res->value();
        goto dump;
    }
};

class CCC_DbgMemoryClear : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgMemoryClear, IConsole_Command);

public:
    explicit CCC_DbgMemoryClear(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DbgMemoryClear() override = default;

    void Execute(std::string_view) override { PointerRegistryClear(); }
};

class CCC_DbgMemoryInfo : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_DbgMemoryInfo, IConsole_Command);

public:
    explicit CCC_DbgMemoryInfo(LPCSTR N) : IConsole_Command{N, true} {}
    ~CCC_DbgMemoryInfo() override = default;

    void Execute(std::string_view) override { PointerRegistryInfo(); }
};
#endif // USE_MEMORY_VALIDATOR

class CCC_UI_Reload : public IConsole_Command
{
    RTTI_DECLARE_TYPEINFO(CCC_UI_Reload, IConsole_Command);

public:
    explicit CCC_UI_Reload(gsl::czstring N) : IConsole_Command{N, true} {}

    void Execute(std::string_view) override
    {
        if (g_pGamePersistent != nullptr && g_pGameLevel != nullptr && Level().game != nullptr)
            HUD().OnScreenRatioChanged();
    }
};
} // namespace

void CCC_RegisterCommands()
{
    CMD1(CCC_MemStats, "stat_memory");
    CMD1(CCC_UI_Reload, "ui_reload");

    // game
    CMD1(CCC_GameDifficulty, "g_game_difficulty");
    CMD1(CCC_GameLanguage, "g_language");

    CMD3(CCC_Mask, "wpn_aim_toggle", &psActorFlags, AF_WPN_AIM_TOGGLE);

    // alife
#ifdef DEBUG
    CMD1(CCC_ALifePath, "al_path"); // build path
#endif // DEBUG

    CMD1(CCC_ALifeSave, "save"); // save game
    CMD1(CCC_ALifeLoadFrom, "load"); // load game from ...
    CMD1(CCC_LoadLastSave, "load_last_save"); // load last saved game from ...

#ifndef MASTER_GOLD
    CMD1(CCC_ALifeTimeFactor, "al_time_factor"); // set time factor
    CMD1(CCC_ALifeSwitchDistance, "al_switch_distance"); // set switch distance
    CMD1(CCC_ALifeProcessTime, "al_process_time"); // set process time
    CMD1(CCC_ALifeObjectsPerUpdate, "al_objects_per_update"); // set process time
    CMD1(CCC_ALifeSwitchFactor, "al_switch_factor"); // set switch factor
#endif // MASTER_GOLD

    CMD3(CCC_Mask, "hud_info", &psHUD_Flags, HUD_INFO);
    CMD3(CCC_Mask, "hud_draw", &psHUD_Flags, HUD_DRAW);
    CMD3(CCC_Mask, "hud_crosshair_build", &psHUD_Flags, HUD_CROSSHAIR_BUILD); // билдокурсор

    if (IS_OGSR_GA)
        psHUD_Flags.set(HUD_CROSSHAIR_HARD, TRUE);
    else
        CMD3(CCC_Mask, "hud_crosshair_hard", &psHUD_Flags, HUD_CROSSHAIR_HARD);

    CMD3(CCC_Mask, "hud_crosshair", &psHUD_Flags, HUD_CROSSHAIR);
    CMD3(CCC_Mask, "hud_crosshair_dist", &psHUD_Flags, HUD_CROSSHAIR_DIST);

    if (IS_OGSR_GA)
        psHUD_FOV_def = 0.65f;
    CMD4(CCC_Float, "hud_fov", &psHUD_FOV_def, 0.1f, 1.0f);

    CMD4(CCC_Float, "fov", &g_fov, 5.0f, 140.0f);

    // Demo
    CMD1(CCC_DemoPlay, "demo_play");
    CMD1(CCC_DemoRecord, "demo_record");

#ifndef MASTER_GOLD
    // ai
    CMD3(CCC_Mask, "mt_ai_vision", &g_mt_config, mtAiVision);
    CMD3(CCC_Mask, "mt_level_path", &g_mt_config, mtLevelPath);
    CMD3(CCC_Mask, "mt_detail_path", &g_mt_config, mtDetailPath);
    CMD3(CCC_Mask, "mt_object_handler", &g_mt_config, mtObjectHandler);
    CMD3(CCC_Mask, "mt_sound_player", &g_mt_config, mtSoundPlayer);
    CMD3(CCC_Mask, "mt_bullets", &g_mt_config, mtBullets);
    CMD3(CCC_Mask, "mt_script_gc", &g_mt_config, mtLUA_GC);
    CMD3(CCC_Mask, "mt_level_sounds", &g_mt_config, mtLevelSounds);
    CMD3(CCC_Mask, "mt_alife", &g_mt_config, mtALife);
    CMD3(CCC_Mask, "mt_map", &g_mt_config, mtMap);
#endif // MASTER_GOLD

    CMD1(CCC_LuaGCMethod, "lua_gc_method");
    CMD4(CCC_Integer, "lua_gc_step", &psLUA_GCSTEP, 1, 1000);
    CMD4(CCC_Integer, "lua_gc_timeout", &psLUA_GCTIMEOUT, 1000, 16000);

#ifdef DEBUG
    CMD3(CCC_Mask, "ai_debug", &psAI_Flags, aiDebug);
    CMD3(CCC_Mask, "ai_dbg_brain", &psAI_Flags, aiBrain);
    CMD3(CCC_Mask, "ai_dbg_motion", &psAI_Flags, aiMotion);
    CMD3(CCC_Mask, "ai_dbg_frustum", &psAI_Flags, aiFrustum);
    CMD3(CCC_Mask, "ai_dbg_funcs", &psAI_Flags, aiFuncs);
    CMD3(CCC_Mask, "ai_dbg_alife", &psAI_Flags, aiALife);
    CMD3(CCC_Mask, "ai_dbg_lua", &psAI_Flags, aiLua);
    CMD3(CCC_Mask, "ai_dbg_goap", &psAI_Flags, aiGOAP);
    CMD3(CCC_Mask, "ai_dbg_goap_script", &psAI_Flags, aiGOAPScript);
    CMD3(CCC_Mask, "ai_dbg_goap_object", &psAI_Flags, aiGOAPObject);
    CMD3(CCC_Mask, "ai_dbg_cover", &psAI_Flags, aiCover);
    CMD3(CCC_Mask, "ai_dbg_anim", &psAI_Flags, aiAnimation);
    CMD3(CCC_Mask, "ai_dbg_vision", &psAI_Flags, aiVision);
    CMD3(CCC_Mask, "ai_dbg_monster", &psAI_Flags, aiMonsterDebug);
    CMD3(CCC_Mask, "ai_dbg_stalker", &psAI_Flags, aiStalker);
    CMD3(CCC_Mask, "ai_stats", &psAI_Flags, aiStats);
    CMD3(CCC_Mask, "ai_dbg_destroy", &psAI_Flags, aiDestroy);
    CMD3(CCC_Mask, "ai_dbg_serialize", &psAI_Flags, aiSerialize);
    CMD3(CCC_Mask, "ai_dbg_dialogs", &psAI_Flags, aiDialogs);
    CMD3(CCC_Mask, "ai_dbg_infoportion", &psAI_Flags, aiInfoPortion);

    CMD3(CCC_Mask, "ai_draw_game_graph", &psAI_Flags, aiDrawGameGraph);
    CMD3(CCC_Mask, "ai_draw_game_graph_stalkers", &psAI_Flags, aiDrawGameGraphStalkers);
    CMD3(CCC_Mask, "ai_draw_game_graph_objects", &psAI_Flags, aiDrawGameGraphObjects);

    CMD3(CCC_Mask, "ai_nil_object_access", &psAI_Flags, aiNilObjectAccess);

    CMD3(CCC_Mask, "ai_draw_visibility_rays", &psAI_Flags, aiDrawVisibilityRays);
    CMD3(CCC_Mask, "ai_animation_stats", &psAI_Flags, aiAnimationStats);

    CMD1(CCC_DumpModelBones, "debug_dump_model_bones");

    CMD1(CCC_DrawGameGraphAll, "ai_draw_game_graph_all");
    CMD1(CCC_DrawGameGraphCurrent, "ai_draw_game_graph_current_level");
    CMD1(CCC_DrawGameGraphLevel, "ai_draw_game_graph_level");

    CMD4(CCC_Integer, "ai_dbg_inactive_time", &g_AI_inactive_time, 0, 1000000);

    CMD1(CCC_DebugNode, "ai_dbg_node");

    CMD1(CCC_ShowMonsterInfo, "ai_monster_info");
    CMD1(CCC_DebugFonts, "debug_fonts");
#endif
    CMD1(CCC_TuneAttachableItem, "dbg_adjust_attachable_item");
    CMD1(CCC_TuneAttachableItemInSlot, "dbg_adjust_attachable_item_in_slot");
    // adjust mode support
    CMD4(CCC_Integer, "hud_adjust_mode", &g_bHudAdjustMode, 0, 11);
    CMD4(CCC_Float, "hud_adjust_delta_value", &g_bHudAdjustDeltaPos, 0.00005f, 1.f);
    CMD4(CCC_Float, "hud_adjust_delta_rot", &g_bHudAdjustDeltaRot, 0.00005f, 10.f);

    CMD4(CCC_Float, "adjust_delta_pos", &adj_delta_pos, -10.f, 10.f);
    CMD4(CCC_Float, "adjust_delta_rot", &adj_delta_rot, -10.f, 10.f);

#ifdef DEBUG
    CMD1(CCC_ShowAnimationStats, "ai_show_animation_stats");
#endif // DEBUG

#ifndef MASTER_GOLD
    CMD3(CCC_Mask, "ai_ignore_actor", &psAI_Flags, aiIgnoreActor);
#endif // MASTER_GOLD

    // Physics
    CMD1(CCC_PHFps, "ph_frequency");
    CMD1(CCC_PHIterations, "ph_iterations");

    CMD1(CCC_PHGravity, "ph_gravity");
    CMD4(CCC_FloatBlock, "ph_timefactor", &phTimefactor, 0.0001f, 1000.f);

#ifdef DEBUG
    CMD4(CCC_FloatBlock, "ph_break_common_factor", &phBreakCommonFactor, 0.f, 1000000000.f);
    CMD4(CCC_FloatBlock, "ph_rigid_break_weapon_factor", &phRigidBreakWeaponFactor, 0.f, 1000000000.f);
    CMD4(CCC_Integer, "ph_tri_clear_disable_count", &ph_tri_clear_disable_count, 0, 255);
    CMD4(CCC_FloatBlock, "ph_tri_query_ex_aabb_rate", &ph_tri_query_ex_aabb_rate, 1.01f, 3.f);
#endif // DEBUG

    // #ifndef MASTER_GOLD
    CMD1(CCC_JumpToLevel, "jump_to_level");
    CMD1(CCC_Spawn, "g_spawn");
    CMD1(CCC_SpawnToInventory, "g_spawn_to_inventory");
    CMD3(CCC_Mask, "g_god", &psActorFlags, AF_GODMODE);
    CMD3(CCC_Mask, "g_unlimitedammo", &psActorFlags, AF_UNLIMITEDAMMO);
    CMD3(CCC_Mask, "g_ammunition_on_belt", &psActorFlags, AF_AMMO_ON_BELT);
    CMD3(CCC_Mask, "g_3d_scopes", &psActorFlags, AF_3D_SCOPES);
    CMD4(CCC_Integer, "g_3d_scopes_fps_factor", &g_3dscopes_fps_factor, 2, 5);
    CMD3(CCC_Mask, "g_crosshair_dbg", &psActorFlags, AF_CROSSHAIR_DBG);
    CMD3(CCC_Mask, "g_camera_collision", &psActorFlags, AF_CAM_COLLISION);

    CMD3(CCC_Mask, "g_mouse_wheel_switch_slot", &psActorFlags, AF_MOUSE_WHEEL_SWITCH_SLOTS);

    CMD3(CCC_Mask, "g_3d_pda", &psActorFlags, AF_3D_PDA);

    CMD3(CCC_Mask, "g_first_person_death", &psActorFlags, AF_FIRST_PERSON_DEATH);

    CMD1(CCC_TimeFactor, "time_factor");
    CMD1(CCC_SetWeather, "set_weather");
    // #endif // MASTER_GOLD

    CMD3(CCC_Mask, "g_music_tracks", &psActorFlags, AF_MUSIC_TRACKS);

    CMD3(CCC_Mask, "g_zones_dbg", &psActorFlags, AF_ZONES_DBG);
    CMD3(CCC_Mask, "g_vertex_dbg", &psActorFlags, AF_VERTEX_DBG);
    CMD3(CCC_Mask, "keypress_on_start", &psActorFlags, AF_KEYPRESS_ON_START);
    CMD3(CCC_Mask, "g_effects_on_demorecord", &psActorFlags, AF_EFFECTS_ON_DEMORECORD);
    CMD3(CCC_Mask, "g_lock_reload", &psActorFlags, AF_LOCK_RELOAD);

    CMD4(CCC_Integer, "g_cop_death_anim", &g_bCopDeathAnim, 0, 1);

#ifdef DEBUG
    CMD3(CCC_Mask, "dbg_draw_actor_alive", &dbg_net_Draw_Flags, (1 << 0));
    CMD3(CCC_Mask, "dbg_draw_actor_dead", &dbg_net_Draw_Flags, (1 << 1));
    CMD3(CCC_Mask, "dbg_draw_invitem", &dbg_net_Draw_Flags, (1 << 4));
    CMD3(CCC_Mask, "dbg_draw_actor_phys", &dbg_net_Draw_Flags, (1 << 5));
    CMD3(CCC_Mask, "dbg_draw_customdetector", &dbg_net_Draw_Flags, (1 << 6));
    CMD3(CCC_Mask, "dbg_destroy", &dbg_net_Draw_Flags, (1 << 7));
    CMD3(CCC_Mask, "dbg_draw_autopickupbox", &dbg_net_Draw_Flags, (1 << 8));
    CMD3(CCC_Mask, "dbg_draw_climbable", &dbg_net_Draw_Flags, (1 << 10));

    CMD3(CCC_Mask, "dbg_draw_ph_contacts", &ph_dbg_draw_mask, phDbgDrawContacts);
    CMD3(CCC_Mask, "dbg_draw_ph_enabled_aabbs", &ph_dbg_draw_mask, phDbgDrawEnabledAABBS);
    CMD3(CCC_Mask, "dbg_draw_ph_intersected_tries", &ph_dbg_draw_mask, phDBgDrawIntersectedTries);
    CMD3(CCC_Mask, "dbg_draw_ph_saved_tries", &ph_dbg_draw_mask, phDbgDrawSavedTries);
    CMD3(CCC_Mask, "dbg_draw_ph_tri_trace", &ph_dbg_draw_mask, phDbgDrawTriTrace);
    CMD3(CCC_Mask, "dbg_draw_ph_positive_tries", &ph_dbg_draw_mask, phDBgDrawPositiveTries);
    CMD3(CCC_Mask, "dbg_draw_ph_negative_tries", &ph_dbg_draw_mask, phDBgDrawNegativeTries);
    CMD3(CCC_Mask, "dbg_draw_ph_tri_test_aabb", &ph_dbg_draw_mask, phDbgDrawTriTestAABB);
    CMD3(CCC_Mask, "dbg_draw_ph_tries_changes_sign", &ph_dbg_draw_mask, phDBgDrawTriesChangesSign);
    CMD3(CCC_Mask, "dbg_draw_ph_tri_point", &ph_dbg_draw_mask, phDbgDrawTriPoint);
    CMD3(CCC_Mask, "dbg_draw_ph_explosion_position", &ph_dbg_draw_mask, phDbgDrawExplosionPos);
    CMD3(CCC_Mask, "dbg_draw_ph_statistics", &ph_dbg_draw_mask, phDbgDrawObjectStatistics);
    CMD3(CCC_Mask, "dbg_draw_ph_mass_centres", &ph_dbg_draw_mask, phDbgDrawMassCenters);
    CMD3(CCC_Mask, "dbg_draw_ph_death_boxes", &ph_dbg_draw_mask, phDbgDrawDeathActivationBox);
    CMD3(CCC_Mask, "dbg_draw_ph_hit_app_pos", &ph_dbg_draw_mask, phHitApplicationPoints);
    CMD3(CCC_Mask, "dbg_draw_ph_cashed_tries_stats", &ph_dbg_draw_mask, phDbgDrawCashedTriesStat);
    CMD3(CCC_Mask, "dbg_draw_ph_car_dynamics", &ph_dbg_draw_mask, phDbgDrawCarDynamics);
    CMD3(CCC_Mask, "dbg_draw_ph_car_plots", &ph_dbg_draw_mask, phDbgDrawCarPlots);
    CMD3(CCC_Mask, "dbg_ph_ladder", &ph_dbg_draw_mask, phDbgLadder);
    CMD3(CCC_Mask, "dbg_draw_ph_explosions", &ph_dbg_draw_mask, phDbgDrawExplosions);
    CMD3(CCC_Mask, "dbg_draw_car_plots_all_trans", &ph_dbg_draw_mask, phDbgDrawCarAllTrnsm);
    CMD3(CCC_Mask, "dbg_draw_ph_zbuffer_disable", &ph_dbg_draw_mask, phDbgDrawZDisable);
    CMD3(CCC_Mask, "dbg_ph_obj_collision_damage", &ph_dbg_draw_mask, phDbgDispObjCollisionDammage);
    CMD_RADIOGROUPMASK2("dbg_ph_ai_always_phmove", &ph_dbg_draw_mask, phDbgAlwaysUseAiPhMove, "dbg_ph_ai_never_phmove", &ph_dbg_draw_mask,
                        phDbgNeverUseAiPhMove);
    CMD3(CCC_Mask, "dbg_ph_ik", &ph_dbg_draw_mask, phDbgIK);
    CMD3(CCC_Mask, "dbg_ph_ik_off", &ph_dbg_draw_mask1, phDbgIKOff);
    CMD3(CCC_Mask, "dbg_draw_ph_ik_goal", &ph_dbg_draw_mask, phDbgDrawIKGoal);
    CMD3(CCC_Mask, "dbg_ph_ik_limits", &ph_dbg_draw_mask, phDbgIKLimits);
    CMD3(CCC_Mask, "dbg_ph_character_control", &ph_dbg_draw_mask, phDbgCharacterControl);
    CMD3(CCC_Mask, "dbg_draw_ph_ray_motions", &ph_dbg_draw_mask, phDbgDrawRayMotions);
    CMD4(CCC_Float, "dbg_ph_vel_collid_damage_to_display", &dbg_vel_collid_damage_to_display, 0.f, 1000.f);
    CMD4(CCC_DbgBullets, "dbg_draw_bullet_hit", &g_bDrawBulletHit, 0, 1);
    CMD1(CCC_DbgPhTrackObj, "dbg_track_obj");
    CMD3(CCC_Mask, "dbg_ph_actor_restriction", &ph_dbg_draw_mask1, ph_m1_DbgActorRestriction);
    CMD3(CCC_Mask, "dbg_draw_ph_hit_anims", &ph_dbg_draw_mask1, phDbgHitAnims);
    CMD3(CCC_Mask, "dbg_draw_ph_ik_limits", &ph_dbg_draw_mask1, phDbgDrawIKLimits);
#endif

    CMD3(CCC_Mask, "dbg_draw_skeleton", &dbg_net_Draw_Flags, (1 << 11));

    CMD4(CCC_Integer, "dbg_string_table_error_msg", &CStringTable::WriteErrorsToLog, 0, 1);

#ifdef DEBUG
    CMD1(CCC_DumpInfos, "dump_infos");
    CMD1(CCC_DumpMap, "dump_map");
    CMD1(CCC_DumpCreatures, "dump_creatures");
#endif

    CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
    CMD1(CCC_MainMenu, "main_menu");

#ifndef MASTER_GOLD
    CMD1(CCC_StartTimeSingle, "start_time_single");
    CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.f, flt_max);
#endif // MASTER_GOLD

    CMD3(CCC_Mask, "ai_use_torch_dynamic_lights", &psActorFlags, AF_AI_VOLUMETRIC_LIGHTS);
    CMD3(CCC_Mask, "r2_exp_force_shadowed", &psActorFlags, AF_FORCE_LIGHTS_SHADOWED);

#ifndef MASTER_GOLD
    CMD4(CCC_Vector3, "psp_cam_offset", &CCameraLook2::m_cam_offset, Fvector().set(-1000, -1000, -1000), Fvector().set(1000, 1000, 1000));
#endif // MASTER_GOLD

#ifdef DEBUG
    CMD1(CCC_DumpObjects, "dump_all_objects");
    CMD3(CCC_String, "stalker_death_anim", dbg_stalker_death_anim, 32);
    CMD4(CCC_Integer, "death_anim_velocity", &b_death_anim_velocity, FALSE, TRUE);
    CMD1(CCC_Crash, "crash");
    CMD4(CCC_Integer, "dbg_show_ani_info", &g_ShowAnimationInfo, 0, 1);
    CMD4(CCC_Integer, "dbg_dump_physics_step", &g_bDebugDumpPhysicsStep, 0, 1);
#endif

    CMD4(CCC_Integer, "show_wnd_rect", &g_show_wnd_rect, 0, 1);
    CMD4(CCC_Integer, "show_wnd_rect_all", &g_show_wnd_rect2, 0, 1);
    CMD4(CCC_Integer, "show_wnd_rect_names", &g_show_wnd_rect_text, 0, 1);
    CMD4(CCC_Integer, "g_console_show_always", &g_console_show_always, 0, 1);

    *g_last_saved_game = 0;

    CMD4(CCC_Float, "g_cam_height_speed", &cam_HeightInterpolationSpeed, 4.0f, 16.0f);
    CMD4(CCC_Float, "g_cam_lookout_speed", &cam_LookoutSpeed, 1.0f, 4.0f);

#ifdef USE_MEMORY_VALIDATOR
    CMD4(CCC_Integer, "g_enable_memory_debug", &g_enable_memory_debug, 0, 1);
    CMD1(CCC_DbgMemoryDump, "dbg_memory_dump");
    CMD1(CCC_DbgMemoryInfo, "dbg_memory_info");
    CMD1(CCC_DbgMemoryClear, "dbg_memory_clear");
    if (!g_enable_memory_debug)
        PointerRegistryClear();
#endif
}
