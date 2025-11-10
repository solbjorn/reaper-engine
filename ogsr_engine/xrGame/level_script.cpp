////////////////////////////////////////////////////////////////////////////
//	Module 		: level_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Level script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "level.h"
#include "actor.h"
#include "script_game_object.h"
#include "patrol_path_storage.h"
#include "xrServer.h"
#include "client_spawn_manager.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "game_cl_base.h"
#include "ui/UIDialogWnd.h"
#include "date_time.h"
#include "ai_space.h"
#include "level_graph.h"
#include "PHCommander.h"
#include "PHScriptCall.h"
#include "HUDManager.h"
#include "script_engine.h"
#include "game_cl_single.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "map_manager.h"
#include "map_location.h"
#include "phworld.h"
#include "../xrcdb/xr_collide_defs.h"
#include "../xr_3da/Rain.h"
#include "script_rq_result.h"
#include "monster_community.h"
#include "GamePersistent.h"
#include "EffectorBobbing.h"
#include "LevelDebugScript.h"
#include "ui/UIStalkersRankingWnd.h"

#include "ActorEffector.h"
#include "actor_statistic_mgr.h"
#include "ai/monsters/ai_monster_effector.h"
#include "graph_engine.h"
#include "postprocessanimator.h"
#include "relation_registry.h"

#include "UIGameCustom.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UITalkWnd.h"
#include "ui/UICarBodyWnd.h"
#include "UIGameSP.h"
#include "HUDManager.h"
#include "HUDTarget.h"
#include "InventoryBox.h"

#include "../xr_3da/fdemorecord.h"

static LPCSTR command_line() { return (Core.Params); }

#ifdef DEBUG
static void check_object(CScriptGameObject* object)
{
    try
    {
        Msg("check_object %s", object->Name());
    }
    catch (...)
    {
        object = object;
    }
}

static CScriptGameObject* tpfGetActor()
{
    static bool first_time = true;
    if (first_time)
        ai().script_engine().script_log(eLuaMessageTypeError, "Do not use level.actor function!");
    first_time = false;

    CActor* l_tpActor = smart_cast<CActor*>(Level().CurrentEntity());
    if (l_tpActor)
        return (smart_cast<CGameObject*>(l_tpActor)->lua_game_object());
    else
        return (0);
}

static CScriptGameObject* get_object_by_name(LPCSTR caObjectName)
{
    static bool first_time = true;
    if (first_time)
        ai().script_engine().script_log(eLuaMessageTypeError, "Do not use level.object function!");
    first_time = false;

    CGameObject* l_tpGameObject = smart_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
    if (l_tpGameObject)
        return (l_tpGameObject->lua_game_object());
    else
        return (0);
}
#endif

static CScriptGameObject* get_object_by_id(u32 id)
{
    CGameObject* pGameObject = smart_cast<CGameObject*>(Level().Objects.net_Find(id));
    if (!pGameObject || pGameObject->getDestroy())
        return nullptr;

    return pGameObject->lua_game_object();
}

static LPCSTR get_weather() { return (*g_pGamePersistent->Environment().GetWeather()); }

static LPCSTR get_weather_prev() { return (*g_pGamePersistent->Environment().GetPrevWeather()); }

static u32 get_weather_last_shift() { return g_pGamePersistent->Environment().GetWeatherLastShift(); }

static void set_weather(LPCSTR weather_name, bool forced)
{
    if (s_ScriptWeather)
        return;

    // KRodin: ТЧ погоду всегда надо обновлять форсировано, иначе она почему-то не всегда корректно обновляется. А для ЗП погоды так делать нельзя - будут очень резкие переходы!
    if (!g_pGamePersistent->Environment().USED_COP_WEATHER)
        forced = true;

    g_pGamePersistent->Environment().SetWeather(shared_str{weather_name}, forced);
}

static void set_weather_next(LPCSTR weather_name)
{
    if (s_ScriptWeather)
        return;

    g_pGamePersistent->Environment().SetWeatherNext(shared_str{weather_name});
}

static bool set_weather_fx(LPCSTR weather_name)
{
    if (s_ScriptWeather)
        return false;

    return g_pGamePersistent->Environment().SetWeatherFX(shared_str{weather_name});
}

static bool start_weather_fx_from_time(LPCSTR weather_name, float time)
{
    if (s_ScriptWeather)
        return false;

    return g_pGamePersistent->Environment().StartWeatherFXFromTime(shared_str{weather_name}, time);
}

static bool is_wfx_playing() { return (g_pGamePersistent->Environment().IsWFXPlaying()); }

static float get_wfx_time() { return (g_pGamePersistent->Environment().wfx_time); }

static void stop_weather_fx() { g_pGamePersistent->Environment().StopWFX(); }

static void set_time_factor(float time_factor)
{
    Level().Server->game->SetGameTimeFactor(time_factor);
    GamePersistent().Environment().SetGameTime(Level().GetEnvironmentGameDayTimeSec(), Level().game->GetEnvironmentGameTimeFactor());
}

static float get_time_factor() { return (Level().GetGameTimeFactor()); }

static void set_game_difficulty(ESingleGameDifficulty dif)
{
    g_SingleGameDifficulty = dif;
    game_cl_Single* game = smart_cast<game_cl_Single*>(Level().game);
    VERIFY(game);
    game->OnDifficultyChanged();
}

static ESingleGameDifficulty get_game_difficulty() { return g_SingleGameDifficulty; }

static u32 get_time_days()
{
    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
    split_time((g_pGameLevel && Level().game) ? Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
    return day;
}

static u32 get_time_hours()
{
    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
    split_time((g_pGameLevel && Level().game) ? Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
    return hours;
}

static u32 get_time_minutes()
{
    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
    split_time((g_pGameLevel && Level().game) ? Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
    return mins;
}

static float cover_in_direction(u32 level_vertex_id, const Fvector& direction)
{
    float y, p;
    direction.getHP(y, p);
    return (ai().level_graph().cover_in_direction(y, level_vertex_id));
}

static float rain_factor() { return g_pGamePersistent->Environment().CurrentEnv->rain_density; }

static float rain_hemi()
{
    CEffect_Rain* rain = g_pGamePersistent->pEnvironment->eff_Rain;

    if (rain)
    {
        return rain->GetRainHemi();
    }
    else
    {
        CObject* E = g_pGameLevel->CurrentViewEntity();
        if (E && E->renderable_ROS())
        {
            float* hemi_cube = E->renderable_ROS()->get_luminocity_hemi_cube();
            float hemi_val = _max(hemi_cube[0], hemi_cube[1]);
            hemi_val = _max(hemi_val, hemi_cube[2]);
            hemi_val = _max(hemi_val, hemi_cube[3]);
            hemi_val = _max(hemi_val, hemi_cube[5]);

            return hemi_val;
        }
        return 0;
    }
}

static void set_rain_wetness(float val)
{
    clamp(val, 0.f, 1.f);
    g_pGamePersistent->Environment().wetness_factor = val;
}

static u32 vertex_in_direction(u32 level_vertex_id, Fvector direction, float max_distance)
{
    direction.normalize_safe();
    direction.mul(max_distance);
    Fvector start_position = ai().level_graph().vertex_position(level_vertex_id);
    Fvector finish_position = Fvector(start_position).add(direction);

    u32 result{std::numeric_limits<u32>::max()};
    ai().level_graph().farthest_vertex_in_direction(level_vertex_id, start_position, finish_position, result, nullptr);

    return (ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

static Fvector vertex_position(u32 level_vertex_id) { return (ai().level_graph().vertex_position(level_vertex_id)); }

static void map_add_object_spot(u16 id, LPCSTR spot_type, LPCSTR text)
{
    CMapLocation* ml = Level().MapManager().AddMapLocation(shared_str{spot_type}, id);
    if (xr_strlen(text))
        ml->SetHint(shared_str{text});
}

static void map_add_object_spot_ser(u16 id, LPCSTR spot_type, LPCSTR text)
{
    CMapLocation* ml = Level().MapManager().AddMapLocation(shared_str{spot_type}, id);
    if (xr_strlen(text))
        ml->SetHint(shared_str{text});

    ml->SetSerializable(true);
}

static u16 map_add_user_spot(u8 level_id, Fvector position, LPCSTR spot_type, LPCSTR text)
{
    shared_str level_name = ai().game_graph().header().level((GameGraph::_LEVEL_ID)level_id).name();

    CMapLocation* ml = Level().MapManager().AddUserLocation(shared_str{spot_type}, level_name, position);
    if (xr_strlen(text))
        ml->SetHint(shared_str{text});

    return ml->ObjectID();
}

static void map_change_spot_hint(u16 id, LPCSTR spot_type, LPCSTR text)
{
    CMapLocation* ml = Level().MapManager().GetMapLocation(shared_str{spot_type}, id);
    if (!ml)
        return;

    ml->SetHint(shared_str{text});
}

static void map_change_spot_ser(u16 id, LPCSTR spot_type, BOOL v)
{
    CMapLocation* ml = Level().MapManager().GetMapLocation(shared_str{spot_type}, id);
    if (!ml)
        return;

    ml->SetSerializable(!!v);
}

static void prefetch_many_sounds(LPCSTR prefix) { Level().PrefetchManySoundsLater(prefix); }

static void map_remove_object_spot(u16 id, LPCSTR spot_type) { Level().MapManager().RemoveMapLocation(shared_str{spot_type}, id); }

static u16 map_has_object_spot(u16 id, LPCSTR spot_type) { return Level().MapManager().HasMapLocation(shared_str{spot_type}, id); }

static bool patrol_path_exists(LPCSTR patrol_path) { return (!!ai().patrol_paths().path(shared_str{patrol_path}, true)); }

static LPCSTR get_name() { return (*Level().name()); }

static void prefetch_sound(LPCSTR name) { Level().PrefetchSound(name); }

static CClientSpawnManager& get_client_spawn_manager() { return (Level().client_spawn_manager()); }

static void start_stop_menu(CUIDialogWnd* pDialog, bool bDoHideIndicators) { HUD().GetUI()->StartStopMenu(pDialog, bDoHideIndicators); }

static void add_dialog_to_render(CUIDialogWnd* pDialog) { HUD().GetUI()->AddDialogToRender(pDialog); }

static void remove_dialog_to_render(CUIDialogWnd* pDialog) { HUD().GetUI()->RemoveDialogToRender(pDialog); }

static CUIDialogWnd* main_input_receiver() { return HUD().GetUI()->MainInputReceiver(); }

static CUIWindow* GetInventoryWindow()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    return (CUIWindow*)pGameSP->InventoryMenu;
}

static CUIWindow* GetTradeWindow()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    return (CUIWindow*)pGameSP->TalkMenu->GetTradeWnd();
}

static CUIWindow* GetTalkWindow()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    return (CUIWindow*)pGameSP->TalkMenu;
}

static CScriptGameObject* GetSecondTalker()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    CUITalkWnd* wnd = pGameSP->TalkMenu;
    if (wnd == nullptr)
        return nullptr;
    return smart_cast<CGameObject*>(wnd->GetSecondTalker())->lua_game_object();
}

static CUIWindow* GetPdaWindow()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    return (CUIWindow*)pGameSP->PdaMenu;
}

static CUIWindow* GetCarBodyWindow()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    return (CUIWindow*)pGameSP->UICarBodyMenu;
}

static CScriptGameObject* GetCarBodyTarget()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    CUICarBodyWnd* wnd = pGameSP->UICarBodyMenu;
    if (wnd == nullptr)
        return nullptr;
    if (wnd->m_pOthersObject != nullptr)
        return smart_cast<CGameObject*>(wnd->m_pOthersObject)->lua_game_object();
    if (wnd->m_pInventoryBox != nullptr)
        return (wnd->m_pInventoryBox->object().lua_game_object());
    return nullptr;
}

static CUIWindow* GetUIChangeLevelWnd()
{
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (!pGameSP)
        return nullptr;
    return (CUIWindow*)pGameSP->UIChangeLevelWnd;
}

static script_rq_result PerformRayQuery(Fvector start, Fvector dir, float range, collide::rq_target tgt, CScriptGameObject* ignore)
{
    collide::rq_result RQ;
    script_rq_result res;
    CObject* obj = nullptr;
    if (ignore)
        obj = smart_cast<CObject*>(&(ignore->object()));
    if (Level().ObjectSpace.RayPick(start, dir, range, tgt, RQ, obj))
    {
        res.set_result(RQ);
    }
    return res;
}

/*void DisableActorCallbacks(u32 _fl)
{
    psCallbackFlags.set(_fl, true);
}*/

static float GetTargetDist() { return ((CHUDManager*)g_hud)->GetTarget()->GetDist(); }

static script_rq_result GetCurrentRayQuery()
{
    collide::rq_result& RQ = HUD().GetCurrentRayQuery();
    script_rq_result res;
    res.set_result(RQ);
    return res;
}

static CScriptGameObject* GetTargetObj()
{
    CObject* obj = ((CHUDManager*)g_hud)->GetTarget()->GetObj();
    if (!obj)
        return nullptr;
    return smart_cast<CGameObject*>(obj)->lua_game_object();
}

static void hide_indicators()
{
    HUD().GetUI()->UIGame()->HideShownDialogs();

    HUD().GetUI()->HideGameIndicators();
    HUD().GetUI()->HideCrosshair();
}

static void show_indicators()
{
    HUD().GetUI()->ShowGameIndicators();
    HUD().GetUI()->ShowCrosshair();
}

static bool game_indicators_shown() { return HUD().GetUI()->GameIndicatorsShown(); }

static Flags32 get_hud_flags() { return psHUD_Flags; }

static bool is_level_present() { return (!!g_pGameLevel); }

static bool is_removing_objects_script() { return Level().is_removing_objects(); }

static CPHCall* add_call(sol::function condition, sol::function action)
{
    CPHScriptCondition* c = xr_new<CPHScriptCondition>(condition);
    CPHScriptAction* a = xr_new<CPHScriptAction>(action);

    return Level().ph_commander_scripts().add_call(c, a);
}

static void remove_call(sol::function condition, sol::function action)
{
    CPHScriptCondition c(condition);
    CPHScriptAction a(action);

    Level().ph_commander_scripts().remove_call(&c, &a);
}

static CPHCall* add_call(sol::object lua_object, LPCSTR condition, LPCSTR action)
{
    sol::function _condition, _action;

    switch (lua_object.get_type())
    {
    case sol::type::userdata:
        _condition = lua_object.as<sol::userdata>()[condition];
        _action = lua_object.as<sol::userdata>()[action];
        break;
    case sol::type::table:
        _condition = lua_object.as<sol::table>()[condition];
        _action = lua_object.as<sol::table>()[action];
        break;
    default: break;
    }

    R_ASSERT(_condition && _action);

    CPHScriptObjectConditionN* c = xr_new<CPHScriptObjectConditionN>(lua_object, _condition);
    CPHScriptObjectActionN* a = xr_new<CPHScriptObjectActionN>(lua_object, _action);

    return Level().ph_commander_scripts().add_call_unique(c, c, a, a);
}

static void remove_call(sol::object lua_object, LPCSTR condition, LPCSTR action)
{
    CPHScriptObjectCondition c(lua_object, condition);
    CPHScriptObjectAction a(lua_object, action);

    Level().ph_commander_scripts().remove_call(&c, &a);
}

static CPHCall* add_call(sol::object lua_object, sol::function condition, sol::function action)
{
    CPHScriptObjectConditionN* c = xr_new<CPHScriptObjectConditionN>(lua_object, condition);
    CPHScriptObjectActionN* a = xr_new<CPHScriptObjectActionN>(lua_object, action);

    return Level().ph_commander_scripts().add_call(c, a);
}

static void remove_call(sol::object lua_object, sol::function condition, sol::function action)
{
    CPHScriptObjectConditionN c(lua_object, condition);
    CPHScriptObjectActionN a(lua_object, action);

    Level().ph_commander_scripts().remove_call(&c, &a);
}

static void remove_calls_for_object(sol::object lua_object)
{
    CPHSriptReqObjComparer c(lua_object);
    Level().ph_commander_scripts().remove_calls(&c);
}

static CPHWorld* physics_world() { return ph_world; }
static CEnvironment* environment() { return g_pGamePersistent->pEnvironment; }

static void disable_input()
{
    g_bDisableAllInput = true;
    if (Actor())
        Actor()->PickupModeOff();
}

static void enable_input()
{
    g_bDisableAllInput = false;

    Fvector2 pos = GetUICursor()->GetCursorPosition();
    GetUICursor()->SetUICursorPosition(pos);
}

bool g_block_all_except_movement{};
bool g_actor_allow_ladder{true};
bool g_actor_allow_pda{true};

static void block_all_except_movement(bool b) { g_block_all_except_movement = b; }

static bool only_movement_allowed() { return g_block_all_except_movement; }

static void set_actor_allow_ladder(bool b) { g_actor_allow_ladder = b; }

static bool actor_ladder_allowed() { return g_actor_allow_ladder; }

static void set_actor_allow_pda(bool b) { g_actor_allow_pda = b; }

static bool actor_pda_allowed() { return g_actor_allow_pda; }

static void spawn_phantom(const Fvector& position) { Level().spawn_item("m_phantom", position, u32(-1), u16(-1), false); }

static Fbox get_bounding_volume() { return Level().ObjectSpace.GetBoundingVolume(); }

static void iterate_sounds(LPCSTR prefix, u32 max_count, sol::function function, sol::object object)
{
    auto callback = [&function, &object](const char* str) {
        if (object)
            function(str, object);
        else
            function(str);
    };

    for (int j = 0, N = _GetItemCount(prefix); j < N; ++j)
    {
        string_path fn, s;
        LPSTR S = (LPSTR)&s;
        std::ignore = _GetItem(prefix, j, S);
        if (FS.exist(fn, "$game_sounds$", S, ".ogg"))
            callback(prefix);

        for (u32 i = 0; i < max_count; ++i)
        {
            string_path name;
            sprintf_s(name, "%s%d", S, i);
            if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
                callback(name);
        }
    }
}

static void iterate_sounds(LPCSTR prefix, u32 max_count, sol::function function) { iterate_sounds(prefix, max_count, function, sol::object{}); }

static float add_cam_effector(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
    CAnimatorCamEffectorScriptCB* e = xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
    e->SetType((ECamEffectorType)id);
    e->SetCyclic(cyclic);
    e->Start(fn);
    Actor()->Cameras().AddCamEffector(e);
    return e->GetAnimatorLength();
}

static float add_cam_effector2(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
    CAnimatorCamEffectorScriptCB* e = xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
    e->m_bAbsolutePositioning = true;
    e->SetType((ECamEffectorType)id);
    e->SetCyclic(cyclic);
    e->Start(fn);
    Actor()->Cameras().AddCamEffector(e);
    return e->GetAnimatorLength();
}

static void remove_cam_effector(int id) { Actor()->Cameras().RemoveCamEffector((ECamEffectorType)id); }

static float get_snd_volume() { return psSoundVFactor; }

static float get_rain_volume()
{
    CEffect_Rain* rain = g_pGamePersistent->pEnvironment->eff_Rain;
    return rain ? rain->GetRainVolume() : 0.0f;
}

static void set_snd_volume(float v)
{
    psSoundVFactor = v;
    clamp(psSoundVFactor, 0.0f, 1.0f);
}

static float get_music_volume() { return psSoundVMusicFactor; }

static void set_music_volume(float v)
{
    psSoundVMusicFactor = v;
    clamp(psSoundVMusicFactor, 0.0f, 1.0f);
}

static void add_actor_points(LPCSTR sect, LPCSTR detail_key, int cnt, int pts) { return Actor()->StatisticMgr().AddPoints(shared_str{sect}, shared_str{detail_key}, cnt, pts); }

static void add_actor_points_str(LPCSTR sect, LPCSTR detail_key, LPCSTR str_value)
{
    return Actor()->StatisticMgr().AddPoints(shared_str{sect}, shared_str{detail_key}, shared_str{str_value});
}

static int get_actor_points(LPCSTR sect) { return Actor()->StatisticMgr().GetSectionPoints(shared_str{sect}); }
static void remove_actor_points(LPCSTR sect, LPCSTR detail_key) { Actor()->StatisticMgr().RemovePoints(shared_str{sect}, shared_str{detail_key}); }

static void add_complex_effector(LPCSTR section, int id) { AddEffector(Actor(), id, shared_str{section}); }
static void remove_complex_effector(int id) { RemoveEffector(Actor(), id); }

static void add_pp_effector(LPCSTR fn, int id, bool cyclic)
{
    CPostprocessAnimator* pp = xr_new<CPostprocessAnimator>(id, cyclic);
    pp->Load(fn);
    Actor()->Cameras().AddPPEffector(pp);
}

static void remove_pp_effector(int id)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    if (pp)
        pp->Stop(1.0f);
}

static void set_pp_effector_factor(int id, float f, float f_sp)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    if (pp)
        pp->SetDesiredFactor(f, f_sp);
}

static void set_pp_effector_factor2(int id, float f)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    if (pp)
        pp->SetCurrentFactor(f);
}

static bool has_pp_effector(int id)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    return !!pp;
}

static void add_monster_cam_effector(float time, float amp, float periods, float power)
{
    CActor* pA = smart_cast<CActor*>(Level().CurrentEntity());

    if (pA)
        Actor()->Cameras().AddCamEffector(xr_new<CMonsterEffectorHit>(time, amp, periods, power));
}

static int g_community_goodwill(LPCSTR _community, int _entity_id)
{
    CHARACTER_COMMUNITY c;
    c.set(shared_str{_community});

    return RELATION_REGISTRY().GetCommunityGoodwill(c.index(), u16(_entity_id));
}

static void g_set_community_goodwill(LPCSTR _community, int _entity_id, int val)
{
    CHARACTER_COMMUNITY c;
    c.set(shared_str{_community});

    RELATION_REGISTRY().SetCommunityGoodwill(c.index(), u16(_entity_id), val);
}

static void g_change_community_goodwill(LPCSTR _community, int _entity_id, int val)
{
    CHARACTER_COMMUNITY c;
    c.set(shared_str{_community});

    RELATION_REGISTRY().ChangeCommunityGoodwill(c.index(), u16(_entity_id), val);
}

static int g_get_personal_goodwill(int _who_id, int _to_whom_id) { return RELATION_REGISTRY().GetGoodwill(u16(_who_id), u16(_to_whom_id)); }
static void g_set_personal_goodwill(int _who_id, int _to_whom_id, int _amount) { RELATION_REGISTRY().SetGoodwill(u16(_who_id), u16(_to_whom_id), _amount); }

static void g_change_personal_goodwill(int _who_id, int _to_whom_id, int _amount)
{
    CHARACTER_GOODWILL gw = RELATION_REGISTRY().GetGoodwill(u16(_who_id), u16(_to_whom_id));
    RELATION_REGISTRY().SetGoodwill(u16(_who_id), u16(_to_whom_id), gw + _amount);
}

static void g_clear_personal_goodwill(int _who_id, int _to_whom_id) { RELATION_REGISTRY().ClearGoodwill(u16(_who_id), u16(_to_whom_id)); }
static void g_clear_personal_relations(int _who_id) { RELATION_REGISTRY().ClearRelations(u16(_who_id)); }
static void set_ignore_game_state_update() { Game().m_need_to_update = false; }

static void g_set_artefact_position(const u32 i, const float x, const float y, const float z)
{
    if (fis_zero(x) && fis_zero(y) && fis_zero(z))
        shader_exports.set_artefact_position(i, Fvector2{});
    else
    {
        Fvector pos{x, y, z};
        Device.mView.transform_tiny(pos);
        shader_exports.set_artefact_position(i, Fvector2{pos.x, pos.z});
    }
}

static void g_set_anomaly_position(const u32 i, const float x, const float y, const float z)
{
    if (fis_zero(x) && fis_zero(y) && fis_zero(z))
        shader_exports.set_anomaly_position(i, Fvector2{});
    else
    {
        Fvector pos{x, y, z};
        Device.mView.transform_tiny(pos);
        shader_exports.set_anomaly_position(i, Fvector2{pos.x, pos.z});
    }
}

static void g_set_detector_params(const int _one, const int _two) { shader_exports.set_detector_params(Ivector2{_one, _two}); }

#include "game_sv_single.h"

static void AdvanceGameTime(u32 _ms)
{
    auto game = smart_cast<game_sv_Single*>(Level().Server->game);
    game->alife().time_manager().advance_game_time(_ms);

    Level().game->SetGameTimeFactor(ai().get_alife() ? ai().alife().time().game_time() : Level().GetGameTime(), Level().game->GetGameTimeFactor());
    GamePersistent().Environment().SetGameTime(Level().GetEnvironmentGameDayTimeSec(), Level().game->GetEnvironmentGameTimeFactor());
}

//
static void send_event_key_press(int dik) // Нажатие клавиши
{
    Level().IR_OnKeyboardPress(dik);
}

static void send_event_key_release(int dik) // Отпускание клавиши
{
    Level().IR_OnKeyboardRelease(dik);
}

static void send_event_key_hold(int dik) // Удержание клавиши.
{
    Level().IR_OnKeyboardHold(dik);
}

static void send_event_mouse_wheel(int vol) // Вращение колеса мыши
{
    Level().IR_OnMouseWheel(vol);
}
//

// Real Wolf 07.07.2014
static u32 vertex_id(const Fvector& vec) { return ai().level_graph().vertex_id(vec); }

static u32 vertex_id(u32 node, const Fvector& vec) { return ai().level_graph().vertex(node, vec); }

static u32 nearest_vertex_id(const Fvector& vec) { return ai().level_graph().vertex(vec); }

// Also referenced in ui\UIInventoryWnd.cpp
void update_inventory_window() { HUD().GetUI()->UIGame()->ReInitShownUI(); }

static void change_level(GameGraph::_GRAPH_ID game_vertex_id, u32 level_vertex_id, Fvector pos, Fvector dir)
{
    NET_Packet p;
    p.w_begin(M_CHANGE_LEVEL);
    p.w(&game_vertex_id, sizeof(game_vertex_id));
    p.w(&level_vertex_id, sizeof(level_vertex_id));
    p.w_vec3(pos);
    p.w_vec3(dir);
    Level().Send(p, net_flags(TRUE));
}

static void set_cam_inert(float v)
{
    psCamInert = v;
    clamp(psCamInert, 0.0f, 1.0f);
}

static void set_monster_relation(LPCSTR from, LPCSTR to, int rel) { MONSTER_COMMUNITY::set_relation((MONSTER_COMMUNITY_ID)from, (MONSTER_COMMUNITY_ID)to, rel); }

static void patrol_path_add(LPCSTR patrol_path, CPatrolPath* path) { ai().patrol_paths_raw().add_path(shared_str(patrol_path), path); }

static void patrol_path_remove(LPCSTR patrol_path) { ai().patrol_paths_raw().remove_path(shared_str(patrol_path)); }

//
static float set_blender_mode_main(float blender_num = 0.f) //--#SM+#--
{
    g_pGamePersistent->m_pGShaderConstants.m_blender_mode.x = blender_num;
    return g_pGamePersistent->m_pGShaderConstants.m_blender_mode.x;
}

static float get_blender_mode_main() //--#SM+#--
{
    return g_pGamePersistent->m_pGShaderConstants.m_blender_mode.x;
}

static Fmatrix get_shader_params() //--#SM+#--
{
    return g_pGamePersistent->m_pGShaderConstants.m_script_params;
}

static void set_shader_params(const Fmatrix& m_params) //--#SM+#--
{
    g_pGamePersistent->m_pGShaderConstants.m_script_params = m_params;
}
//

static bool valid_vertex_id(u32 level_vertex_id) { return ai().level_graph().valid_vertex_id(level_vertex_id); }

static u32 vertex_count() { return ai().level_graph().header().vertex_count(); }

static void disable_vertex(u32 vertex_id) { ai().level_graph().set_mask(vertex_id); }
static void enable_vertex(u32 vertex_id) { ai().level_graph().clear_mask(vertex_id); }

static bool is_accessible_vertex_id(u32 level_vertex_id) { return ai().level_graph().is_accessible(level_vertex_id); }

static IC Fvector construct_position(u32 level_vertex_id, float x, float z) { return Fvector().set(x, ai().level_graph().vertex_plane_y(level_vertex_id, x, z), z); }

static IC bool CSpaceRestrictionBase_inside(Fsphere m_sphere, const Fvector& position, const float& radius)
{
    Fsphere sphere;
    sphere.P = position;
    sphere.R = radius;
    return sphere.intersect(m_sphere);
}

static bool CSpaceRestrictionBase_inside(Fsphere m_sphere, u32 level_vertex_id, bool partially_inside, float radius = EPS_L)
{
    const Fvector& position = ai().level_graph().vertex_position(level_vertex_id);
    float offset = ai().level_graph().header().cell_size() * .5f - EPS_L;
    if (partially_inside)
        return (CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x + offset, position.z + offset), radius) ||
                CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x + offset, position.z - offset), radius) ||
                CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x - offset, position.z + offset), radius) ||
                CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x - offset, position.z - offset), radius) ||
                CSpaceRestrictionBase_inside(m_sphere, Fvector().set(position.x, position.y, position.z), radius));
    else
        return (CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x + offset, position.z + offset), radius) &&
                CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x + offset, position.z - offset), radius) &&
                CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x - offset, position.z + offset), radius) &&
                CSpaceRestrictionBase_inside(m_sphere, construct_position(level_vertex_id, position.x - offset, position.z - offset), radius) &&
                CSpaceRestrictionBase_inside(m_sphere, Fvector().set(position.x, position.y, position.z), radius));
}

static void iterate_vertices_inside(Fvector P, float R, bool partially_inside, sol::function funct)
{
    Fvector start, dest;
    start.sub(Fvector().set(0, 0, 0), Fvector().set(R, 0.f, R));
    dest.add(Fvector().set(0, 0, 0), Fvector().set(R, 0.f, R));
    start.add(P);
    dest.add(P);
    Fsphere m_sphere;
    m_sphere.P = P;
    m_sphere.R = R;
    ai().level_graph().iterate_vertices(start, dest, [&](const auto& vertex) {
        if (CSpaceRestrictionBase_inside(m_sphere, ai().level_graph().vertex_id(&vertex), partially_inside))
            funct(ai().level_graph().vertex_id(&vertex));
    });
}

static void iterate_vertices_border(Fvector P, float R, sol::function funct)
{
    Fvector start, dest;
    start.sub(Fvector().set(0, 0, 0), Fvector().set(R, 0.f, R));
    dest.add(Fvector().set(0, 0, 0), Fvector().set(R, 0.f, R));
    start.add(P);
    dest.add(P);
    Fsphere m_sphere;
    m_sphere.P = P;
    m_sphere.R = R;
    ai().level_graph().iterate_vertices(start, dest, [&](const auto& vertex) {
        if (CSpaceRestrictionBase_inside(m_sphere, ai().level_graph().vertex_id(&vertex), true) &&
            !CSpaceRestrictionBase_inside(m_sphere, ai().level_graph().vertex_id(&vertex), false))
            funct(ai().level_graph().vertex_id(&vertex));
    });
}

static int get_character_community_team(LPCSTR comm)
{
    CHARACTER_COMMUNITY community;
    community.set(shared_str{comm});

    return community.team();
}

static void demo_record_start()
{
    string_path fn{};
    g_pGameLevel->Cameras().AddCamEffector(xr_new<CDemoRecord>(fn));
}

static void demo_record_stop() { g_pGameLevel->Cameras().RemoveCamEffector(cefDemo); }

static Fvector demo_record_get_position()
{
    CDemoRecord* demo = (CDemoRecord*)g_pGameLevel->Cameras().GetCamEffector(cefDemo);
    return demo->m_Position;
}

static void demo_record_set_position(Fvector p)
{
    CDemoRecord* demo = (CDemoRecord*)g_pGameLevel->Cameras().GetCamEffector(cefDemo);
    demo->m_Position = p;
}

static Fvector demo_record_get_HPB()
{
    CDemoRecord* demo = (CDemoRecord*)g_pGameLevel->Cameras().GetCamEffector(cefDemo);
    return demo->m_HPB;
}

static void demo_record_set_HPB(Fvector p)
{
    CDemoRecord* demo = (CDemoRecord*)g_pGameLevel->Cameras().GetCamEffector(cefDemo);
    demo->m_HPB = p;
}

static void demo_record_set_direct_input(bool f)
{
    CDemoRecord* demo = (CDemoRecord*)g_pGameLevel->Cameras().GetCamEffector(cefDemo);
    demo->m_b_redirect_input_to_level = f;
}

static CEffectorBobbing* get_effector_bobbing() { return Actor()->GetEffectorBobbing(); }

static void iterate_nearest(const Fvector& pos, float radius, sol::function function)
{
    xr_vector<CObject*> m_nearest;
    Level().ObjectSpace.GetNearest(m_nearest, pos, radius, nullptr);

    if (m_nearest.empty())
        return;

    for (auto item : m_nearest)
    {
        CGameObject* obj = smart_cast<CGameObject*>(item);
        if (!obj)
            continue;
        if (function(obj->lua_game_object()))
            break;
    }
}

static float is_ray_intersect_sphere(Fvector pos, Fvector dir, Fvector C, float R)
{
    Fsphere sphere;
    sphere.P = C;
    sphere.R = R;
    dir.normalize_safe();
    float dist;
    return sphere.intersect_ray(pos, dir, dist) == Fsphere::rpNone ? -1.0f : dist;
}

static DBG_ScriptObject* get_object(u16 id)
{
    xr_map<u16, DBG_ScriptObject*>::iterator it = Level().getScriptRenderQueue()->find(id);
    if (it == Level().getScriptRenderQueue()->end())
        return nullptr;

    return it->second;
}

static void remove_object(u16 id)
{
    DBG_ScriptObject* dbg_obj = get_object(id);
    if (!dbg_obj)
        return;

    xr_delete(dbg_obj);
    Level().getScriptRenderQueue()->erase(id);
}

static DBG_ScriptObject* add_object(u16 id, DebugRenderType type)
{
    remove_object(id);
    DBG_ScriptObject* dbg_obj;

    switch (type)
    {
    case eDBGSphere: dbg_obj = xr_new<DBG_ScriptSphere>(); break;
    case eDBGBox: dbg_obj = xr_new<DBG_ScriptBox>(); break;
    case eDBGLine: dbg_obj = xr_new<DBG_ScriptLine>(); break;
    default: dbg_obj = nullptr; break;
    }

    R_ASSERT(dbg_obj);
    Level().getScriptRenderQueue()->try_emplace(id, dbg_obj);

    return dbg_obj;
}

void CLevel::script_register(sol::state_view& lua)
{
    lua.new_usertype<CEnvDescriptor>("CEnvDescriptor", sol::no_constructor, "fog_density", &CEnvDescriptor::fog_density, "fog_distance", &CEnvDescriptor::fog_distance, "far_plane",
                                     &CEnvDescriptor::far_plane, "sun_dir", &CEnvDescriptor::sun_dir, "wind_velocity", &CEnvDescriptor::wind_velocity, "wind_direction",
                                     &CEnvDescriptor::wind_direction, "m_fTreeAmplitudeIntensity", &CEnvDescriptor::m_fTreeAmplitudeIntensity, "m_fSunShaftsIntensity",
                                     &CEnvDescriptor::m_fSunShaftsIntensity, "m_identifier", sol::property([](CEnvDescriptor* self) { return self->m_identifier.c_str(); }),
                                     "set_env_ambient", &CEnvDescriptor::setEnvAmbient, "clouds_color", &CEnvDescriptor::clouds_color, "sky_color", &CEnvDescriptor::sky_color,
                                     "fog_color", &CEnvDescriptor::fog_color, "rain_color", &CEnvDescriptor::rain_color, "ambient", &CEnvDescriptor::ambient, "hemi_color",
                                     &CEnvDescriptor::hemi_color, "sun_color", &CEnvDescriptor::sun_color, "set_sun", &CEnvDescriptor::set_sun);

    lua.new_usertype<CEnvironment>("CEnvironment", sol::no_constructor, "getCurrentWeather", [](CEnvironment* self, const size_t idx) {
        R_ASSERT(idx < 2);
        return self->Current[idx];
    });

    lua.new_usertype<CPHCall>("CPHCall", sol::no_constructor, "set_pause", &CPHCall::setPause);

    lua.new_usertype<CEffectorBobbing>("CEffectorBobbing", sol::no_constructor, "run_amplitude", &CEffectorBobbing::m_fAmplitudeRun, "walk_amplitude",
                                       &CEffectorBobbing::m_fAmplitudeWalk, "limp_amplitude", &CEffectorBobbing::m_fAmplitudeLimp, "run_speed", &CEffectorBobbing::m_fSpeedRun,
                                       "walk_speed", &CEffectorBobbing::m_fSpeedWalk, "limp_speed", &CEffectorBobbing::m_fSpeedLimp);

    lua.new_usertype<DBG_ScriptObject>("DBG_ScriptObject", sol::no_constructor,
                                       // dbg_type
                                       "line", sol::var(DebugRenderType::eDBGLine), "sphere", sol::var(DebugRenderType::eDBGSphere), "box", sol::var(DebugRenderType::eDBGBox),

                                       "cast_dbg_sphere", &DBG_ScriptObject::cast_dbg_sphere, "cast_dbg_box", &DBG_ScriptObject::cast_dbg_box, "cast_dbg_line",
                                       &DBG_ScriptObject::cast_dbg_line, "color", &DBG_ScriptObject::m_color, "hud", &DBG_ScriptObject::m_hud, "visible",
                                       &DBG_ScriptObject::m_visible);

    lua.new_usertype<DBG_ScriptSphere>("DBG_ScriptSphere", sol::no_constructor, "matrix", &DBG_ScriptSphere::m_mat, sol::base_classes, xr::sol_bases<DBG_ScriptSphere>());
    lua.new_usertype<DBG_ScriptBox>("DBG_ScriptBox", sol::no_constructor, "matrix", &DBG_ScriptBox::m_mat, "size", &DBG_ScriptBox::m_size, sol::base_classes,
                                    xr::sol_bases<DBG_ScriptBox>());
    lua.new_usertype<DBG_ScriptLine>("DBG_ScriptLine", sol::no_constructor, "point_a", &DBG_ScriptLine::m_point_a, "point_b", &DBG_ScriptLine::m_point_b, sol::base_classes,
                                     xr::sol_bases<DBG_ScriptLine>());

    lua.new_usertype<CKeyBinding>("CKeyBinding", sol::no_constructor, "ignore", &CKeyBinding::ignore);

    lua.create_named_table("debug_render", "add_object", &add_object, "remove_object", &remove_object, "get_object", &get_object);

    lua.create_named_table(
        "level",
        // obsolete\deprecated
        "object_by_id", &get_object_by_id, "is_removing_objects", &is_removing_objects_script,
#ifdef DEBUG
        "debug_object", &get_object_by_name, "debug_actor", &tpfGetActor, "check_object", &check_object,
#endif

        "get_weather", &get_weather, "get_weather_prev", &get_weather_prev, "get_weather_last_shift", &get_weather_last_shift, "set_weather", &set_weather, "set_weather_next",
        &set_weather_next, "set_weather_fx", &set_weather_fx, "start_weather_fx_from_time", &start_weather_fx_from_time, "is_wfx_playing", &is_wfx_playing, "get_wfx_time",
        &get_wfx_time, "stop_weather_fx", &stop_weather_fx, "environment", &environment, "set_time_factor", &set_time_factor, "get_time_factor", &get_time_factor,
        "set_game_difficulty", &set_game_difficulty, "get_game_difficulty", &get_game_difficulty, "get_time_days", &get_time_days, "get_time_hours", &get_time_hours,
        "get_time_minutes", &get_time_minutes, "cover_in_direction", &cover_in_direction, "vertex_in_direction", &vertex_in_direction, "rain_factor", &rain_factor, "rain_hemi",
        &rain_hemi, "rain_wetness", [] { return g_pGamePersistent->Environment().wetness_factor; }, "set_rain_wetness", set_rain_wetness, "patrol_path_exists", &patrol_path_exists,
        "vertex_position", &vertex_position, "name", &get_name, "prefetch_sound", &prefetch_sound, "prefetch_sound", prefetch_sound, "prefetch_many_sounds", prefetch_many_sounds,
        "client_spawn_manager", &get_client_spawn_manager, "map_add_object_spot_ser", &map_add_object_spot_ser, "map_add_object_spot", &map_add_object_spot,
        "map_remove_object_spot", &map_remove_object_spot, "map_has_object_spot", &map_has_object_spot, "map_change_spot_hint", &map_change_spot_hint, "map_change_spot_ser",
        &map_change_spot_ser, "map_add_user_spot", &map_add_user_spot, "start_stop_menu", &start_stop_menu, "add_dialog_to_render", &add_dialog_to_render,
        "remove_dialog_to_render", &remove_dialog_to_render, "main_input_receiver", &main_input_receiver, "hide_indicators", &hide_indicators, "show_indicators", &show_indicators,
        "game_indicators_shown", &game_indicators_shown, "get_hud_flags", &get_hud_flags, "add_call",
        sol::overload(sol::resolve<CPHCall*(sol::function, sol::function)>(&add_call), sol::resolve<CPHCall*(sol::object, sol::function, sol::function)>(&add_call),
                      sol::resolve<CPHCall*(sol::object, LPCSTR, LPCSTR)>(&add_call)),
        "remove_call",
        sol::overload(sol::resolve<void(sol::function, sol::function)>(&remove_call), sol::resolve<void(sol::object, sol::function, sol::function)>(&remove_call),
                      sol::resolve<void(sol::object, LPCSTR, LPCSTR)>(&remove_call)),
        "remove_calls_for_object", &remove_calls_for_object, "present", &is_level_present, "disable_input", &disable_input, "enable_input", &enable_input, "only_allow_movekeys",
        &block_all_except_movement, "only_movekeys_allowed", &only_movement_allowed, "set_actor_allow_ladder", &set_actor_allow_ladder, "actor_ladder_allowed",
        &actor_ladder_allowed, "set_actor_allow_pda", &set_actor_allow_pda, "actor_pda_allowed", &actor_pda_allowed, "spawn_phantom", &spawn_phantom, "get_bounding_volume",
        &get_bounding_volume, "iterate_sounds",
        sol::overload(sol::resolve<void(LPCSTR, u32, sol::function, sol::object)>(&iterate_sounds), sol::resolve<void(LPCSTR, u32, sol::function)>(&iterate_sounds)),
        "physics_world", &physics_world, "get_snd_volume", &get_snd_volume, "get_rain_volume", &get_rain_volume, "set_snd_volume", &set_snd_volume, "add_cam_effector",
        &add_cam_effector, "add_cam_effector2", &add_cam_effector2, "remove_cam_effector", &remove_cam_effector, "add_pp_effector", &add_pp_effector, "set_pp_effector_factor",
        &set_pp_effector_factor, "set_pp_effector_factor", &set_pp_effector_factor2, "remove_pp_effector", &remove_pp_effector, "has_pp_effector", &has_pp_effector,
        "add_monster_cam_effector", &add_monster_cam_effector, "get_music_volume", &get_music_volume, "set_music_volume", &set_music_volume, "demo_record_start",
        &demo_record_start, "demo_record_stop", &demo_record_stop, "demo_record_get_position", &demo_record_get_position, "demo_record_set_position", &demo_record_set_position,
        "demo_record_get_HPB", &demo_record_get_HPB, "demo_record_set_HPB", &demo_record_set_HPB, "demo_record_set_direct_input", &demo_record_set_direct_input,
        "add_complex_effector", &add_complex_effector, "remove_complex_effector", &remove_complex_effector, "game_id", &GameID, "set_ignore_game_state_update",
        &set_ignore_game_state_update, "get_inventory_wnd", &GetInventoryWindow, "get_talk_wnd", &GetTalkWindow, "get_trade_wnd", &GetTradeWindow, "get_pda_wnd", &GetPdaWindow,
        "get_car_body_wnd", &GetCarBodyWindow, "get_second_talker", &GetSecondTalker, "get_car_body_target", &GetCarBodyTarget, "get_change_level_wnd", &GetUIChangeLevelWnd,
        "ray_query", &PerformRayQuery,

        // Real Wolf 07.07.2014
        "vertex_id", sol::overload(sol::resolve<u32(const Fvector&)>(&vertex_id), sol::resolve<u32(u32, const Fvector&)>(&vertex_id)),

        "nearest_vertex_id", &nearest_vertex_id, "advance_game_time", &AdvanceGameTime, "get_target_dist", &GetTargetDist, "get_target_obj", &GetTargetObj, "get_current_ray_query",
        &GetCurrentRayQuery,

        //
        "send_event_key_press", &send_event_key_press, "send_event_key_release", &send_event_key_release, "send_event_key_hold", &send_event_key_hold, "send_event_mouse_wheel",
        &send_event_mouse_wheel,

        "iterate_nearest", &iterate_nearest, "change_level", &change_level, "set_cam_inert", &set_cam_inert, "set_monster_relation", &set_monster_relation, "patrol_path_add",
        &patrol_path_add, "patrol_path_remove", &patrol_path_remove, "valid_vertex_id", &valid_vertex_id, "vertex_count", &vertex_count, "disable_vertex", &disable_vertex,
        "enable_vertex", &enable_vertex, "is_accessible_vertex_id", &is_accessible_vertex_id, "iterate_vertices_inside", &iterate_vertices_inside, "iterate_vertices_border",
        &iterate_vertices_border, "get_character_community_team", &get_character_community_team, "get_effector_bobbing", &get_effector_bobbing, "is_ray_intersect_sphere",
        &is_ray_intersect_sphere,

        //--#SM+# Begin --
        "set_blender_mode_main", &set_blender_mode_main, "get_blender_mode_main", &get_blender_mode_main, "set_shader_params", &set_shader_params, "get_shader_params",
        &get_shader_params
        //--#SM+# End --
    );

    lua.create_named_table("actor_stats", "add_points", &add_actor_points, "add_points_str", &add_actor_points_str, "get_points", &get_actor_points, "remove_points",
                           &remove_actor_points, "add_to_ranking", &add_human_to_top_list, "remove_from_ranking", &remove_human_from_top_list, "get_actor_ranking",
                           &get_actor_ranking);

    lua.set_function("command_line", &command_line);

    lua.create_named_table("relation_registry", "community_goodwill", &g_community_goodwill, "set_community_goodwill", &g_set_community_goodwill, "change_community_goodwill",
                           &g_change_community_goodwill, "get_personal_goodwill", &g_get_personal_goodwill, "set_personal_goodwill", &g_set_personal_goodwill,
                           "change_personal_goodwill", &g_change_personal_goodwill, "clear_personal_goodwill", &g_clear_personal_goodwill, "clear_personal_relations",
                           &g_clear_personal_relations);

    // установка параметров для шейдеров из скриптов
    lua.set_function("set_artefact_slot", &g_set_artefact_position);
    lua.set_function("set_anomaly_slot", &g_set_anomaly_position);
    lua.set_function("set_detector_mode", &g_set_detector_params);
    lua.set_function("set_pda_params", [](const Fvector& p) { shader_exports.set_pda_params(p); });
    lua.set_function("update_inventory_window", &update_inventory_window);

    lua.new_enum("rq_target", "rqtNone", collide::rqtNone, "rqtObject", collide::rqtObject, "rqtStatic", collide::rqtStatic, "rqtShape", collide::rqtShape, "rqtObstacle",
                 collide::rqtObstacle, "rqtBoth", collide::rqtBoth, "rqtDyn", collide::rqtDyn);

    lua.create_named_table("lib", "random_f64_below", &xr::random_f64_below, "random_s64", &xr::random_s64, "random_u64", &xr::random_u64);
}
