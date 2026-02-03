#ifndef GamePersistentH
#define GamePersistentH

#include "..\xr_3da\IGame_Persistent.h"

class CMainMenu;
class CUICursor;
class CParticlesObject;
class CUISequencer;
class ui_core;

class CGamePersistent : public IGame_Persistent, public IEventReceiver
{
    RTTI_DECLARE_TYPEINFO(CGamePersistent, IGame_Persistent, IEventReceiver);

public:
    // ambient particles
    CParticlesObject* ambient_particles{};
    u32 ambient_sound_next_time[40]{}; // max snd channels
    u32 ambient_sound_next_time_shoc{};
    u32 ambient_effect_next_time{};
    u32 ambient_effect_stop_time;

    float ambient_effect_wind_start{};
    float ambient_effect_wind_in_time{};
    float ambient_effect_wind_end{};
    float ambient_effect_wind_out_time{};
    bool ambient_effect_wind_on{};

    bool GameAutopaused{};

    CUISequencer* m_intro{};
    CEvent* eQuickLoad;

    CallMe::Delegate<tmc::task<void>()> m_intro_event;

    tmc::task<void> start_logo_intro();
    tmc::task<void> update_logo_intro();
    tmc::task<void> start_game_intro();
    tmc::task<void> update_game_intro();

#ifdef DEBUG
    u32 m_frame_counter{};
    u32 m_last_stats_frame{std::numeric_limits<u32>::max() - 1};
#endif

    void WeathersUpdate();

public:
    ui_core* m_pUI_core{};
    IReader* pDemoFile;
    u32 uTime2Change;
    CEvent* eDemoStart;

    CGamePersistent();
    tmc::task<void> co_destroy() override;
    ~CGamePersistent() override;

    void PreStart(LPCSTR op) override;
    tmc::task<void> Start(gsl::czstring op) override;
    tmc::task<void> Disconnect() override;

    tmc::task<void> OnAppActivate() override;
    tmc::task<void> OnAppDeactivate() override;

    tmc::task<void> OnAppStart() override;
    tmc::task<void> OnAppEnd() override;
    tmc::task<void> OnGameStart() override;
    virtual void OnGameEnd();
    tmc::task<void> OnFrame() override;
    tmc::task<void> OnEvent(CEvent* E, u64 P1, u64 P2) override;

    virtual void UpdateGameType();

    virtual void RegisterModel(IRenderVisual* V);
    virtual float MtlTransparent(u32 mtl_idx);
    virtual void Statistics(CGameFont* F);

    virtual bool OnRenderPPUI_query();
    virtual void OnRenderPPUI_main();
    virtual void OnRenderPPUI_PP();
    tmc::task<void> LoadTitle(gsl::czstring title) override;
    virtual void SetTip();

    [[nodiscard]] bool OnKeyboardPress();

protected:
    tmc::task<void> co_init() override;
};

IC CGamePersistent& GamePersistent() { return *smart_cast<CGamePersistent*>(g_pGamePersistent); }

// console_commands.cpp
void CCC_RegisterCommands();

// xrgame_dll_detach.cpp
tmc::task<void> init_game_globals();
void clean_game_globals();

#endif // GamePersistentH
