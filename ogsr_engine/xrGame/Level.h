// Level.h: interface for the CLevel class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\xr_3da\igame_level.h"
#include "../xr_3da/NET_Server_Trash/net_client.h"
#include "script_export_space.h"
#include "..\xr_3da\StatGraph.h"
#include "xrMessages.h"
#include "alife_space.h"
#include "xrDebug.h"
#include "xrServer.h"
#include "xr_level_controller.h"

#include "../xr_3da/feel_touch.h"

class CHUDManager;
class CParticlesObject;
class xrServer;
class game_cl_GameState;
class NET_Queue_Event;
class CSE_Abstract;
class CSpaceRestrictionManager;
class CSeniorityHierarchyHolder;
class CClientSpawnManager;
class CGameObject;
class CPHCommander;
class CPHWorld;
class CLevelDebug;
class CLevelSoundManager;

class CDebugRenderer;

extern float g_fov;

const int maxRP = 64;
const int maxTeams = 32;

// class CFogOfWar;
// class CFogOfWarMngr;
class CBulletManager;
class CMapManager;

class DBG_ScriptObject;

class GlobalFeelTouch : public Feel::Touch
{
    RTTI_DECLARE_TYPEINFO(GlobalFeelTouch, Feel::Touch);

public:
    GlobalFeelTouch();
    ~GlobalFeelTouch() override;

    void update();
    bool is_object_denied(CObject const* O);
};

class CLevel : public IGame_Level, public IPureClient
{
    RTTI_DECLARE_TYPEINFO(CLevel, IGame_Level, IPureClient);

private:
#ifdef DEBUG
    bool m_bSynchronization{};
#endif

protected:
    typedef IGame_Level inherited;

    CLevelSoundManager* m_level_sound_manager;

    // movement restriction manager
    CSpaceRestrictionManager* m_space_restriction_manager;
    // seniority hierarchy holder
    CSeniorityHierarchyHolder* m_seniority_hierarchy_holder;
    // client spawn_manager
    CClientSpawnManager* m_client_spawn_manager;
    // debug renderer
    CDebugRenderer* m_debug_renderer;

    CPHCommander* m_ph_commander;
    CPHCommander* m_ph_commander_scripts;

    // level name
    shared_str m_name;
    // Local events
    EVENT eChangeRP;
    EVENT eDemoPlay;
    EVENT eChangeTrack;
    EVENT eEnvironment;
    EVENT eEntitySpawn;
    //---------------------------------------------
    CStatGraph* pStatGraphS{};
    u32 m_dwSPC; // SendedPacketsCount
    u32 m_dwSPS; // SendedPacketsSize
    CStatGraph* pStatGraphR{};
    u32 m_dwRPC; // ReceivedPacketsCount
    u32 m_dwRPS; // ReceivedPacketsSize
    //---------------------------------------------

public:
#ifdef DEBUG
    // level debugger
    CLevelDebug* m_level_debug;
#endif

public:
    ////////////// network ////////////////////////
    static constexpr u32 GetInterpolationSteps() { return 0; }
    static void PhisStepsCallback(u32 Time0, u32 Time1);

    virtual void OnMessage(void* data, u32 size);
    virtual void OnConnectRejected();

private:
    CObject* pCurrentControlEntity{};
    xrServer::EConnect m_connect_server_err;

public:
    CObject* CurrentControlEntity(void) const { return pCurrentControlEntity; }
    void SetControlEntity(CObject* O) { pCurrentControlEntity = O; }

private:
    tmc::task<bool> Connect2Server(gsl::czstring options);

    bool m_bConnectResultReceived;
    bool m_bConnectResult;
    xr_string m_sConnectResult;

public:
    void OnBuildVersionChallenge();
    void OnConnectResult(NET_Packet* P);

public:
    //////////////////////////////////////////////
    // static particles
    DEFINE_VECTOR(CParticlesObject*, POVec, POIt);
    POVec m_StaticParticles;

    game_cl_GameState* game{};
    BOOL m_bGameConfigStarted{};
    BOOL game_configured{};
    NET_Queue_Event* game_events;
    xr_deque<CSE_Abstract*> game_spawn_queue;
    xrServer* Server{};
    GlobalFeelTouch m_feel_deny;
    xr_vector<u16> m_just_destroyed;

private:
    // preload sounds registry
    xr_map<shared_str, ref_sound, absl::container_internal::StringBtreeDefaultLess> sound_registry;
    xr_deque<std::string> sound_registry_defer;

    xr_set<EGameActions> m_blocked_actions; // Вектор с заблокированными действиями. Real Wolf. 14.10.2014.

public:
    bool PrefetchSound(LPCSTR name);
    bool PrefetchManySounds(LPCSTR prefix);
    bool PrefetchManySoundsLater(LPCSTR prefix);
    tmc::task<void> PrefetchDeferredSounds();
    void CancelPrefetchManySounds(LPCSTR prefix);

protected:
    BOOL net_start_result_total;
    BOOL connected_to_server;

    tmc::task<bool> net_start1();
    tmc::task<bool> net_start2();
    tmc::task<bool> net_start3();
    tmc::task<bool> net_start4();
    tmc::task<bool> net_start5();
    tmc::task<bool> net_start6();

    tmc::task<bool> net_start_client1();
    tmc::task<bool> net_start_client2();
    tmc::task<bool> net_start_client3();
    tmc::task<bool> net_start_client4();
    tmc::task<bool> net_start_client5();
    tmc::task<bool> net_start_client6();

    void net_OnChangeSelfName(NET_Packet* P);

public:
    // startup options
    shared_str m_caServerOptions;
    shared_str m_caClientOptions;

    // Starting/Loading
    virtual BOOL net_Start(LPCSTR op_server, LPCSTR op_client);
    void net_Load(LPCSTR) override;
    virtual void net_Save(LPCSTR name);
    tmc::task<void> net_Stop() override;
    virtual void net_Update();

    tmc::task<bool> Load_GameSpecific_Before() override;
    tmc::task<bool> Load_GameSpecific_After() override;

    void Load_GameSpecific_CFORM_Serialize(IWriter& writer) override;
    bool Load_GameSpecific_CFORM_Deserialize(IReader& reader) override;
    void Load_GameSpecific_CFORM(std::span<CDB::TRI> T) override;

    // Events
    tmc::task<void> OnEvent(EVENT E, u64 P1, u64) override;
    tmc::task<void> OnFrame() override;
    tmc::task<void> OnRender() override;
    tmc::task<void> cl_Process_Event(u16 dest, u16 type, NET_Packet& P);
    tmc::task<void> cl_Process_Spawn(NET_Packet& P);
    tmc::task<void> ProcessGameEvents();
    tmc::task<void> ProcessGameSpawns();
    void ProcessGameSpawnsDestroy(u16 dest, u16 type);

    // Input
    tmc::task<void> IR_OnKeyboardPress(gsl::index btn) override;
    virtual void IR_OnKeyboardRelease(int btn);
    tmc::task<void> IR_OnKeyboardHold(gsl::index btn) override;
    tmc::task<void> IR_OnMousePress(gsl::index btn) override;
    virtual void IR_OnMouseRelease(int btn);
    tmc::task<void> IR_OnMouseHold(gsl::index btn) override;
    virtual void IR_OnMouseMove(int, int);
    virtual void IR_OnMouseStop(int, int);
    tmc::task<void> IR_OnMouseWheel(gsl::index direction) override;
    tmc::task<void> IR_OnActivate() override;

    // Real Wolf. Start. 14.10.2014
    void block_action(EGameActions cmd);
    void unblock_action(EGameActions cmd);
    // Real Wolf. End. 14.10.2014

    int get_RPID(LPCSTR name);

    // Game
    tmc::task<void> InitializeClientGame(NET_Packet& P);
    tmc::task<void> ClientReceive();
    void ClientSend();
    void ClientSave();

    void Send(NET_Packet& P, u32 = DPNSEND_GUARANTEED, u32 = 0) override;

    void g_cl_Spawn(LPCSTR name, u8 rp, u16 flags, Fvector pos); // only ask server
    tmc::task<void> g_sv_Spawn(CSE_Abstract* E); // server reply/command spawning

    IC CSpaceRestrictionManager& space_restriction_manager();
    IC CSeniorityHierarchyHolder& seniority_holder();
    IC CClientSpawnManager& client_spawn_manager();
    IC CDebugRenderer& debug_renderer();
    tmc::task<void> script_gc(); // GC-cycle

    IC CPHCommander& ph_commander();
    IC CPHCommander& ph_commander_scripts();

    // C/D
    CLevel();
    ~CLevel() override;

    // название текущего уровня
    virtual shared_str name() const;

    // gets the time from the game simulation

    // возвращает время в милисекундах относительно начала игры
    ALife::_TIME_ID GetGameTime();
    // возвращает время для энвайронмента в милисекундах относительно начала игры
    ALife::_TIME_ID GetEnvironmentGameTime();
    // игровое время в отформатированном виде
    void GetGameDateTime(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs);

    float GetGameTimeFactor();
    void SetGameTimeFactor(const float fTimeFactor);
    void SetGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor);
    virtual void SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor);

    void GetGameTimeForShaders(u32& hours, u32& minutes, u32& seconds, u32& milliseconds) override;

    // gets current daytime [0..23]
    u8 GetDayTime();
    u32 GetGameDayTimeMS();
    float GetGameDayTimeSec();
    float GetEnvironmentGameDayTimeSec();

    xr_map<u16, DBG_ScriptObject*>* getScriptRenderQueue() { return &m_debug_render_queue; }

protected:
    CMapManager* m_map_manager;

    xr_map<u16, DBG_ScriptObject*> m_debug_render_queue;

    void ScriptDebugRender();

public:
    CMapManager& MapManager() { return *m_map_manager; }
    //	CFogOfWarMngr&			FogOfWarMngr				()	{return *m_pFogOfWarMngr;}

    // работа с пулями
protected:
    CBulletManager* m_pBulletManager;

public:
    IC CBulletManager& BulletManager() { return *m_pBulletManager; }

    CSE_Abstract* spawn_item(LPCSTR section, const Fvector& position, u32 level_vertex_id, u16 parent_id, bool return_item = false);

protected:
    u32 m_dwCL_PingDeltaSend{1000};
    u32 m_dwCL_PingLastSendTime{};
    u32 m_dwRealPing{};

public:
    virtual u32 GetRealPing() { return m_dwRealPing; }

private:
    bool m_is_removing_objects{};

public:
    bool is_removing_objects() { return m_is_removing_objects; }
    tmc::task<void> remove_objects();
    virtual void OnSessionTerminate(LPCSTR reason);
    void OnDestroyObject(u16 id) override;
    virtual void OnChangeCurrentWeather(const char* sect) override;

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};

add_to_type_list(CLevel);
#undef script_type_list
#define script_type_list save_type_list(CLevel)

struct CKeyBinding
{
public:
    bool ignore{};
};

IC CLevel& Level() { return *((CLevel*)g_pGameLevel); }
IC game_cl_GameState& Game() { return *Level().game; }
u32 GameID();

IC CHUDManager& HUD() { return *((CHUDManager*)Level().pHUD); }

#ifdef DEBUG
IC CLevelDebug& DBG() { return *((CLevelDebug*)Level().m_level_debug); }
#endif

IC CSpaceRestrictionManager& CLevel::space_restriction_manager()
{
    VERIFY(m_space_restriction_manager);
    return (*m_space_restriction_manager);
}

IC CSeniorityHierarchyHolder& CLevel::seniority_holder()
{
    VERIFY(m_seniority_hierarchy_holder);
    return (*m_seniority_hierarchy_holder);
}

IC CClientSpawnManager& CLevel::client_spawn_manager()
{
    VERIFY(m_client_spawn_manager);
    return (*m_client_spawn_manager);
}

IC CDebugRenderer& CLevel::debug_renderer()
{
    VERIFY(m_debug_renderer);
    return (*m_debug_renderer);
}

IC CPHCommander& CLevel::ph_commander()
{
    VERIFY(m_ph_commander);
    return *m_ph_commander;
}

IC CPHCommander& CLevel::ph_commander_scripts()
{
    VERIFY(m_ph_commander_scripts);
    return *m_ph_commander_scripts;
}

// Level.cpp
extern BOOL g_bDebugEvents;
extern CPHWorld* ph_world;

extern u32 ps_lua_gc_method;
extern int psLUA_GCSTEP;
extern int psLUA_GCTIMEOUT;

// Level_network.cpp
extern pureFrame* g_pNetProcessor;

// level_script.cpp
extern bool g_bDisableAllInput;
extern bool g_actor_allow_ladder;
extern bool g_actor_allow_pda;
extern bool g_block_all_except_movement;

void update_inventory_window();

// console_commands.cpp
extern Flags32 dbg_net_Draw_Flags;
