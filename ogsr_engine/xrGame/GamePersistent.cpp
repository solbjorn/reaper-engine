#include "stdafx.h"

#include "gamepersistent.h"

#include "../xr_3da/fmesh.h"
#include "..\xr_3da\XR_IOConsole.h"
#include "../xr_3da/gamemtllib.h"
#include "../Include/xrRender/Kinematics.h"
#include "MainMenu.h"
#include "UICursor.h"
#include "game_base_space.h"
#include "level.h"
#include "ParticlesObject.h"
#include "actor.h"
#include "stalker_animation_data_storage.h"
#include "stalker_velocity_holder.h"
#include "ActorEffector.h"
#include "ui/UILoadingScreen.h"
#include "../xr_3da/x_ray.h"
#include "string_table.h"
#include "HUDManager.h"
#include "..\xr_3da\DiscordRPC.hpp"
#include "holder_custom.h"

#include "embedded_editor/embedded_editor.h"
#include "game_sv_single.h"
#include "xrServer.h"

#include "ai_debug.h"
#include "UI/UIGameTutorial.h"

#include "xr_level_controller.h"

#ifndef MASTER_GOLD
#include "custommonster.h"
#endif // MASTER_GOLD

namespace
{
[[nodiscard]] void* ode_alloc(size_t size) { return xr_malloc(size); }
[[nodiscard]] void* ode_realloc(void* ptr, size_t, size_t newsize) { return xr_realloc(ptr, newsize); }
void ode_free(void* ptr, size_t) { return xr_free(ptr); }
} // namespace

CGamePersistent::CGamePersistent()
{
    m_game_params.m_e_game_type = GAME_ANY;

    m_pMainMenu = nullptr;
    m_intro_event = CallMe::fromMethod<&CGamePersistent::start_logo_intro>(this);

    //
    dSetAllocHandler(ode_alloc);
    dSetReallocHandler(ode_realloc);
    dSetFreeHandler(ode_free);

    //
    if (strstr(Core.Params, "-demomode "))
    {
        string256 fname;
        LPCSTR name = strstr(Core.Params, "-demomode ") + 10;
        sscanf(name, "%s", fname);
        R_ASSERT2(fname[0], "Missing filename for 'demomode'");
        Msg("- playing in demo mode '%s'", fname);
        pDemoFile = FS.r_open(fname);
        Device.seqFrame.Add(this);
        eDemoStart = Engine.Event.Handler_Attach("GAME:demo", this);
        uTime2Change = 0;
    }
    else
    {
        pDemoFile = nullptr;
        eDemoStart = nullptr;
    }

    eQuickLoad = Engine.Event.Handler_Attach("Game:QuickLoad", this);
}

CGamePersistent::~CGamePersistent()
{
    FS.r_close(pDemoFile);
    Device.seqFrame.Remove(this);
    Engine.Event.Handler_Detach(eDemoStart, this);
    Engine.Event.Handler_Detach(eQuickLoad, this);
}

void CGamePersistent::RegisterModel(IRenderVisual* V)
{
    // Check types
    switch (V->getType())
    {
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        u16 def_idx = GMLib.GetMaterialIdx("default_object");
        R_ASSERT2(GMLib.GetMaterialByIdx(def_idx)->Flags.is(SGameMtl::flDynamic), "'default_object' - must be dynamic");
        IKinematics* K = smart_cast<IKinematics*>(V);
        VERIFY(K);
        int cnt = K->LL_BoneCount();
        for (u16 k = 0; k < cnt; k++)
        {
            CBoneData& bd = K->LL_GetData(k);
            if (*(bd.game_mtl_name))
            {
                bd.game_mtl_idx = GMLib.GetMaterialIdx(*bd.game_mtl_name);
                R_ASSERT2(GMLib.GetMaterialByIdx(bd.game_mtl_idx)->Flags.is(SGameMtl::flDynamic), "Required dynamic game material");
            }
            else
            {
                bd.game_mtl_idx = def_idx;
            }
        }
    }
    break;
    }
}

tmc::task<void> CGamePersistent::OnAppStart()
{
    // load game materials
    GMLib.Load();

    auto glob = co_await tmc::fork_clang(init_game_globals(), tmc::current_executor(), xr::tmc_priority_any);
    co_await IGame_Persistent::OnAppStart();

    m_pUI_core = xr_new<ui_core>();
    m_pMainMenu = xr_new<CMainMenu>();
    xr::detail::editor = xr::ingame_editor_create();

    co_await std::move(glob);
}

tmc::task<void> CGamePersistent::OnAppEnd()
{
    if (m_pMainMenu->IsActive())
        co_await m_pMainMenu->Activate(false);

    xr::ingame_editor_destroy(xr::detail::editor);
    xr_delete(m_pMainMenu);
    xr_delete(m_pUI_core);

    co_await IGame_Persistent::OnAppEnd();

    clean_game_globals();
    GMLib.Unload();
}

void CGamePersistent::PreStart(LPCSTR op)
{
    pApp->SetLoadingScreen(xr_new<UILoadingScreen>());
    IGame_Persistent::PreStart(op);
}

tmc::task<void> CGamePersistent::Start(gsl::czstring op)
{
    co_await IGame_Persistent::Start(op);
    m_intro_event = CallMe::fromMethod<&CGamePersistent::start_game_intro>(this);
}

void CGamePersistent::Disconnect()
{
    // destroy ambient particles
    CParticlesObject::Destroy(ambient_particles);

    __super::Disconnect();
    // stop all played emitters
    ::Sound->stop_emitters();
    m_game_params.m_e_game_type = GAME_ANY;
}

tmc::task<void> CGamePersistent::OnGameStart()
{
    co_await IGame_Persistent::OnGameStart();
    UpdateGameType();
}

void CGamePersistent::UpdateGameType()
{
    __super::UpdateGameType();
    m_game_params.m_e_game_type = GAME_SINGLE;

    // TODO: KRodin: надо подумать, надо ли тут вылетать вообще. Не может ли возникнуть каких-нибудь проблем, если парсер налажал. Он же влияет не только на m_game_type. На данный
    // момент парсер может налажать, если встретит скобочки () в имени сейва.
    ASSERT_FMT_DBG(std::is_eq(xr_strcmp(m_game_params.m_game_type, "single")), "!!failed to parse the name of the save, rename it and try to load again.");
}

void CGamePersistent::OnGameEnd()
{
    __super::OnGameEnd();

    xr_delete(g_stalker_animation_data_storage);
    xr_delete(g_stalker_velocity_holder);
}

void CGamePersistent::WeathersUpdate()
{
    if (g_pGamePersistent->Environment().USED_COP_WEATHER)
    {
        if (g_pGameLevel)
        {
            const bool bIndoor = g_pGamePersistent->IsActorInHideout();

            int data_set = (Random.randF() < (1.f - Environment().CurrentEnv->weight)) ? 0 : 1;

            CEnvDescriptor* const current_env = Environment().Current[0];
            VERIFY(current_env);

            CEnvDescriptor* const _env = Environment().Current[data_set];
            VERIFY(_env);

            CEnvAmbient* env_amb = _env->env_ambient;
            if (env_amb)
            {
                CEnvAmbient::SSndChannelVec& vec = current_env->env_ambient->get_snd_channels();
                CEnvAmbient::SSndChannelVecIt I = vec.begin();
                CEnvAmbient::SSndChannelVecIt E = vec.end();

                for (u32 idx = 0; I != E; ++I, ++idx)
                {
                    CEnvAmbient::SSndChannel& ch = **I;
                    R_ASSERT(idx < 40);
                    if (ambient_sound_next_time[idx] == 0) // first
                    {
                        ambient_sound_next_time[idx] = Device.dwTimeGlobal + ch.get_rnd_sound_first_time();
                    }
                    else if (Device.dwTimeGlobal > ambient_sound_next_time[idx])
                    {
                        ref_sound& snd = ch.get_rnd_sound();

                        Fvector pos;
                        float angle = ::Random.randF(PI_MUL_2);
                        pos.x = _cos(angle);
                        pos.y = 0;
                        pos.z = _sin(angle);
                        pos.normalize().mul(ch.get_rnd_sound_dist()).add(Device.vCameraPosition);
                        pos.y += 10.f;
                        snd.play_at_pos(nullptr, pos, (!ch.m_sound_period.x && !ch.m_sound_period.y && !ch.m_sound_period.z && !ch.m_sound_period.w) ? sm_2D : 0);

#ifdef DEBUG
                        if (!snd._handle() && strstr(Core.Params, "-nosound"))
                            continue;
#endif // DEBUG

                        VERIFY(snd._handle());
                        u32 _length_ms = iFloor(snd.get_length_sec() * 1000.0f);
                        ambient_sound_next_time[idx] = Device.dwTimeGlobal + _length_ms + ch.get_rnd_sound_time();
                        //					Msg("- Playing ambient sound channel [%s] file[%s]",ch.m_load_section.c_str(),snd._handle()->file_name());
                    }
                }
                /*
                            if (Device.dwTimeGlobal > ambient_sound_next_time)
                            {
                                ref_sound* snd			= env_amb->get_rnd_sound();
                                ambient_sound_next_time	= Device.dwTimeGlobal + env_amb->get_rnd_sound_time();
                                if (snd)
                                {
                                    Fvector	pos;
                                    float	angle		= ::Random.randF(PI_MUL_2);
                                    pos.x				= _cos(angle);
                                    pos.y				= 0;
                                    pos.z				= _sin(angle);
                                    pos.normalize		().mul(env_amb->get_rnd_sound_dist()).add(Device.vCameraPosition);
                                    pos.y				+= 10.f;
                                    snd->play_at_pos	(0,pos);
                                }
                            }
                */
                // start effect
                if (!bIndoor && !ambient_particles && Device.dwTimeGlobal > ambient_effect_next_time)
                {
                    CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect();
                    if (eff)
                    {
                        Environment().wind_gust_factor = eff->wind_gust_factor;
                        ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time();
                        ambient_effect_stop_time = Device.dwTimeGlobal + eff->life_time;
                        ambient_effect_wind_start = Device.fTimeGlobal;
                        ambient_effect_wind_in_time = Device.fTimeGlobal + eff->wind_blast_in_time;
                        ambient_effect_wind_end = Device.fTimeGlobal + eff->life_time / 1000.f;
                        ambient_effect_wind_out_time = Device.fTimeGlobal + eff->life_time / 1000.f + eff->wind_blast_out_time;
                        ambient_effect_wind_on = true;

                        ambient_particles = CParticlesObject::Create(eff->particles.c_str(), FALSE, false);
                        Fvector pos;
                        pos.add(Device.vCameraPosition, eff->offset);
                        ambient_particles->play_at_pos(pos);
                        if (eff->sound._handle())
                            eff->sound.play_at_pos(nullptr, pos);

                        Environment().wind_blast_strength_start_value = Environment().wind_strength_factor;
                        Environment().wind_blast_strength_stop_value = eff->wind_blast_strength;

                        if (Environment().wind_blast_strength_start_value == 0.f)
                        {
                            Environment().wind_blast_start_time.set(0.f, eff->wind_blast_direction.x, eff->wind_blast_direction.y, eff->wind_blast_direction.z);
                        }
                        else
                        {
                            Environment().wind_blast_start_time.set(0.f, Environment().wind_blast_direction.x, Environment().wind_blast_direction.y,
                                                                    Environment().wind_blast_direction.z);
                        }
                        Environment().wind_blast_stop_time.set(0.f, eff->wind_blast_direction.x, eff->wind_blast_direction.y, eff->wind_blast_direction.z);
                    }
                }
                else if (!ambient_particles && Device.dwTimeGlobal > ambient_effect_next_time)
                {
                    CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect();
                    if (eff)
                        ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time();
                }
            }
            if (Device.fTimeGlobal >= ambient_effect_wind_start && Device.fTimeGlobal <= ambient_effect_wind_in_time && ambient_effect_wind_on)
            {
                float delta = ambient_effect_wind_in_time - ambient_effect_wind_start;
                float t;
                if (delta != 0.f)
                {
                    float cur_in = Device.fTimeGlobal - ambient_effect_wind_start;
                    t = cur_in / delta;
                }
                else
                {
                    t = 0.f;
                }
                Environment().wind_blast_current.slerp(Environment().wind_blast_start_time, Environment().wind_blast_stop_time, t);

                Environment().wind_blast_direction.set(Environment().wind_blast_current.x, Environment().wind_blast_current.y, Environment().wind_blast_current.z);
                Environment().wind_strength_factor =
                    Environment().wind_blast_strength_start_value + t * (Environment().wind_blast_strength_stop_value - Environment().wind_blast_strength_start_value);
            }

            // stop if time exceed or indoor
            if (bIndoor || Device.dwTimeGlobal >= ambient_effect_stop_time)
            {
                if (ambient_particles)
                    ambient_particles->Stop();

                Environment().wind_gust_factor = 0.f;
            }

            if (Device.fTimeGlobal >= ambient_effect_wind_end && ambient_effect_wind_on)
            {
                Environment().wind_blast_strength_start_value = Environment().wind_strength_factor;
                Environment().wind_blast_strength_stop_value = 0.f;

                ambient_effect_wind_on = false;
            }

            if (Device.fTimeGlobal >= ambient_effect_wind_end && Device.fTimeGlobal <= ambient_effect_wind_out_time)
            {
                float delta = ambient_effect_wind_out_time - ambient_effect_wind_end;
                float t;
                if (delta != 0.f)
                {
                    float cur_in = Device.fTimeGlobal - ambient_effect_wind_end;
                    t = cur_in / delta;
                }
                else
                {
                    t = 0.f;
                }
                Environment().wind_strength_factor =
                    Environment().wind_blast_strength_start_value + t * (Environment().wind_blast_strength_stop_value - Environment().wind_blast_strength_start_value);
            }
            if (Device.fTimeGlobal > ambient_effect_wind_out_time && ambient_effect_wind_out_time != 0.f)
            {
                Environment().wind_strength_factor = 0.0;
            }

            // if particles not playing - destroy
            if (ambient_particles && !ambient_particles->IsPlaying())
                CParticlesObject::Destroy(ambient_particles);
        }
    }
    else
    {
        if (g_pGameLevel)
        {
            const bool bIndoor = g_pGamePersistent->IsActorInHideout();

            int data_set = (Random.randF() < (1.f - Environment().CurrentEnv->weight)) ? 0 : 1;

            CEnvDescriptor* const _env = Environment().Current[data_set];
            VERIFY(_env);

            CEnvAmbient* env_amb = _env->env_ambient;

            // start sound
            if (env_amb)
            {
                if (Device.dwTimeGlobal > ambient_sound_next_time_shoc)
                {
                    ref_sound* snd = env_amb->get_rnd_sound();
                    ambient_sound_next_time_shoc = Device.dwTimeGlobal + env_amb->get_rnd_sound_time();
                    if (snd)
                    {
                        Fvector pos;
                        float angle = ::Random.randF(PI_MUL_2);
                        pos.x = _cos(angle);
                        pos.y = 0;
                        pos.z = _sin(angle);
                        pos.normalize().mul(env_amb->get_rnd_sound_dist()).add(Device.vCameraPosition);
                        pos.y += 10.f;
                        snd->play_at_pos(nullptr, pos);
                    }
                }

                // start effect
                if (!bIndoor && !ambient_particles && Device.dwTimeGlobal > ambient_effect_next_time)
                {
                    CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect();
                    if (eff)
                    {
                        Environment().wind_gust_factor = eff->wind_gust_factor;
                        ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time_shoc();
                        ambient_effect_stop_time = Device.dwTimeGlobal + eff->life_time;
                        ambient_particles = CParticlesObject::Create(eff->particles.c_str(), FALSE, false);
                        Fvector pos;
                        pos.add(Device.vCameraPosition, eff->offset);
                        ambient_particles->play_at_pos(pos);
                        if (eff->sound._handle())
                            eff->sound.play_at_pos(nullptr, pos);
                    }
                }
                else if (!ambient_particles && Device.dwTimeGlobal > ambient_effect_next_time)
                {
                    CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect();
                    if (eff)
                        ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time_shoc();
                }
            }
            // stop if time exceed or indoor
            if (bIndoor || Device.dwTimeGlobal >= ambient_effect_stop_time)
            {
                if (ambient_particles)
                    ambient_particles->Stop();
                Environment().wind_gust_factor = 0.f;
            }
            // if particles not playing - destroy
            if (ambient_particles && !ambient_particles->IsPlaying())
                CParticlesObject::Destroy(ambient_particles);
        }
    }
}

tmc::task<void> CGamePersistent::start_logo_intro()
{
    if (!strstr(Core.Params, "-intro"))
    {
        m_intro_event = CallMe::Delegate<tmc::task<void>()>{};
        co_await Console->Show();
        Console->Execute("main_menu on");

        co_return;
    }

    if (Device.dwPrecacheFrame > 0)
        co_return;

    m_intro_event = CallMe::fromMethod<&CGamePersistent::update_logo_intro>(this);

    if (!xr_strlen(m_game_params.m_game_or_spawn) && !g_pGameLevel)
    {
        VERIFY(!m_intro);
        m_intro = xr_new<CUISequencer>();

        co_await m_intro->Start("intro_logo");
        co_await Console->Hide();
    }
}

tmc::task<void> CGamePersistent::update_logo_intro()
{
    if (m_intro && (false == m_intro->IsActive()))
    {
        m_intro_event = CallMe::Delegate<tmc::task<void>()>{};
        xr_delete(m_intro);
        Console->Execute("main_menu on");
    }

    co_return;
}

tmc::task<void> CGamePersistent::start_game_intro()
{
    if (g_pGameLevel && g_pGameLevel->bReady && Device.dwPrecacheFrame <= 2)
    {
        m_intro_event = CallMe::fromMethod<&CGamePersistent::update_game_intro>(this);

        if (std::is_eq(xr::strcasecmp(m_game_params.m_new_or_load, "new")))
        {
            VERIFY(m_intro == nullptr);

            m_intro = xr_new<CUISequencer>();
            co_await m_intro->Start("intro_game");
        }
    }
}

tmc::task<void> CGamePersistent::update_game_intro()
{
    if (!m_intro)
    {
        m_intro_event = CallMe::Delegate<tmc::task<void>()>{};
    }
    else if (!m_intro->IsActive())
    {
        xr_delete(m_intro);
        m_intro_event = CallMe::Delegate<tmc::task<void>()>{};
    }

    co_return;
}

tmc::task<void> CGamePersistent::OnFrame()
{
    if (g_tutorial2)
    {
        co_await g_tutorial2->Destroy();
        xr_delete(g_tutorial2);
    }

    if (g_tutorial && !g_tutorial->IsActive())
        xr_delete(g_tutorial);

#ifdef DEBUG
    ++m_frame_counter;
#endif

    if (m_intro_event != CallMe::Delegate<tmc::task<void>()>{} && !load_screen_renderer.b_registered)
        co_await m_intro_event();

    if (Device.dwPrecacheFrame == 0 && load_screen_renderer.b_registered && !GameAutopaused)
    {
        if (psActorFlags.test(AF_KEYPRESS_ON_START))
        {
            Device.Pause(TRUE, TRUE, TRUE, "AUTOPAUSE_START");
            pApp->LoadForceFinish();

            co_await LoadTitle("st_press_any_key");
            GameAutopaused = true;
        }
        else
        {
            load_screen_renderer.stop();
        }

        Discord.Update(CStringTable().translate(Level().name()).c_str(), Level().name().c_str());
    }

    if (!m_pMainMenu->IsActive())
        m_pMainMenu->DestroyInternal(false);

    if (g_pGameLevel == nullptr || !g_pGameLevel->bReady)
        co_return;

    if (Device.Paused())
    {
#ifndef MASTER_GOLD
        if (Level().CurrentViewEntity())
        {
            if (!g_actor || (g_actor->ID() != Level().CurrentViewEntity()->ID()))
            {
                CCustomMonster* custom_monster = smart_cast<CCustomMonster*>(Level().CurrentViewEntity());
                if (custom_monster) // can be spectator in multiplayer
                    custom_monster->UpdateCamera();
            }
            else
            {
                CCameraBase* C{};
                if (g_actor)
                {
                    if (!Actor()->Holder())
                        C = Actor()->cam_Active();
                    else
                        C = Actor()->Holder()->Camera();

                    Actor()->Cameras().UpdateFromCamera(C);
                    Actor()->Cameras().ApplyDevice();
                }
            }
        }
#else // MASTER_GOLD
        if (g_actor)
        {
            CCameraBase* C{};
            if (!Actor()->Holder())
                C = Actor()->cam_Active();
            else
                C = Actor()->Holder()->Camera();

            Actor()->Cameras().UpdateFromCamera(C);
            Actor()->Cameras().ApplyDevice();
        }
#endif // MASTER_GOLD
    }

    co_await IGame_Persistent::OnFrame();

    if (!Device.Paused())
        Engine.Sheduler.Update();

    // update weathers ambient
    if (!Device.Paused())
        WeathersUpdate();

    if (pDemoFile)
    {
        if (Device.dwTimeGlobal > uTime2Change)
        {
            // Change level + play demo
            if (pDemoFile->elapsed() < 3)
                pDemoFile->seek(0); // cycle

            // Read params
            string512 params;
            pDemoFile->r_string(params, sizeof(params));
            string256 o_server, o_client, o_demo;
            u32 o_time;
            sscanf(params, "%[^,],%[^,],%[^,],%u", o_server, o_client, o_demo, &o_time);

            // Start _new level + demo
            Engine.Event.Defer("KERNEL:disconnect");
            Engine.Event.Defer("KERNEL:start", size_t(xr_strdup(_Trim(o_server))), size_t(xr_strdup(_Trim(o_client))));
            Engine.Event.Defer("GAME:demo", size_t(xr_strdup(_Trim(o_demo))), u64(o_time));
            uTime2Change = 0xffffffff; // Block changer until Event received
        }
    }

#ifdef DEBUG
    if ((m_last_stats_frame + 1) < m_frame_counter)
        profiler().clear();
#endif
}

tmc::task<void> CGamePersistent::OnEvent(EVENT E, u64 P1, u64 P2)
{
    if (E == eQuickLoad)
    {
        if (Device.Paused())
            Device.Pause(FALSE, TRUE, TRUE, "eQuickLoad");

        LPSTR saved_name = (LPSTR)(P1);

        co_await Level().remove_objects();

        game_sv_Single* game = smart_cast<game_sv_Single*>(Level().Server->game);
        R_ASSERT(game);
        co_await game->restart_simulator(saved_name);

        xr_free(saved_name);
        co_return;
    }
    else if (E == eDemoStart)
    {
        string256 cmd;
        LPCSTR demo = LPCSTR(P1);
        sprintf_s(cmd, "demo_play %s", demo);
        Console->Execute(cmd);

        auto dmem = const_cast<gsl::zstring>(demo);
        xr_free(dmem);
        uTime2Change = Device.TimerAsync() + u32(P2) * 1000;
    }
}

void CGamePersistent::Statistics([[maybe_unused]] CGameFont* F)
{
#ifdef DEBUG
    m_last_stats_frame = m_frame_counter;
    profiler().show_stats(F, !!psAI_Flags.test(aiStats));
#endif
}

float CGamePersistent::MtlTransparent(u32 mtl_idx) { return GMLib.GetMaterialByIdx((u16)mtl_idx)->fVisTransparencyFactor; }
static BOOL bRestorePause = FALSE;
static BOOL bEntryFlag = TRUE;

tmc::task<void> CGamePersistent::OnAppActivate()
{
    Device.Pause(FALSE, !bRestorePause, TRUE, "CGP::OnAppActivate");
    bEntryFlag = TRUE;

    co_return;
}

tmc::task<void> CGamePersistent::OnAppDeactivate()
{
    if (!bEntryFlag)
        co_return;

    bRestorePause = Device.Paused();
    Device.Pause(TRUE, TRUE, TRUE, "CGP::OnAppDeactivate");
    bEntryFlag = FALSE;
}

bool CGamePersistent::OnRenderPPUI_query()
{
    return MainMenu()->OnRenderPPUI_query();
    // enable PP or not
}

void CGamePersistent::OnRenderPPUI_main()
{
    // always
    MainMenu()->OnRenderPPUI_main();
}

void CGamePersistent::OnRenderPPUI_PP() { MainMenu()->OnRenderPPUI_PP(); }

tmc::task<void> CGamePersistent::LoadTitle(gsl::czstring title)
{
    title = CStringTable().translate(shared_str{title}).c_str();

    pApp->SetLoadStageTitle(title);
    co_await pApp->LoadStage();

    Discord.Update(title);
}

void CGamePersistent::SetTip() { pApp->LoadTitleInt(); }

bool CGamePersistent::OnKeyboardPress()
{
    if (psActorFlags.test(AF_KEYPRESS_ON_START) && GameAutopaused)
    {
        Device.Pause(FALSE, TRUE, TRUE, "AUTOPAUSE_END");
        load_screen_renderer.stop();
        GameAutopaused = false;
        return true;
    }

    return false;
}
