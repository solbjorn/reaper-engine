#ifndef IGame_PersistentH
#define IGame_PersistentH

#include "../xrCDB/ISpatial.h"
#include "Environment.h"
#include "IGame_ObjectPool.h"

#include "ShadersExternalData.h" //--#SM+#--

class IRenderVisual;
class IMainMenu;
class CPS_Instance;

//-----------------------------------------------------------------------------------------------------------

class XR_NOVTABLE IGame_Persistent : public DLL_Pure, public pureAppStart, public pureAppEnd, public pureAppActivate, public pureAppDeactivate, public pureFrame
{
    RTTI_DECLARE_TYPEINFO(IGame_Persistent, DLL_Pure, pureAppStart, pureAppEnd, pureAppActivate, pureAppDeactivate, pureFrame);

public:
    union params
    {
        struct
        {
            string256 m_game_or_spawn;
            string256 m_game_type;
            string256 m_alife;
            string256 m_new_or_load;
            u32 m_e_game_type;
        };
        string256 m_params[4];

        constexpr params() { reset(); }

        constexpr void reset()
        {
            for (auto& param : m_params)
                param[0] = '\0';
        }

        void parse_cmd_line(LPCSTR cmd_line)
        {
            reset();

            const auto n = _min(4z, _GetItemCount(cmd_line, '/'));
            for (gsl::index i{}; i < n; ++i)
            {
                std::ignore = _GetItem(cmd_line, i, m_params[i], '/');
                _strlwr(m_params[i]);
            }
        }
    };
    params m_game_params;

    xr_set<CPS_Instance*> ps_active, ps_destroy;
    xr_vector<CPS_Instance*> ps_needtoplay;

    enum GrassBenders_Anim : s8
    {
        BENDER_ANIM_EXPLOSION = 0,
        BENDER_ANIM_DEFAULT = 1,
        BENDER_ANIM_WAVY = 2,
        BENDER_ANIM_SUCK = 3,
        BENDER_ANIM_BLOW = 4,
        BENDER_ANIM_PULSE = 5,
    };

    void GrassBendersUpdateAnimations();
    void GrassBendersAddExplosion(const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius);
    void GrassBendersAddShot(const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity, const float radius);
    void GrassBendersRemoveById(const u16 id);
    void GrassBendersRemoveByIndex(size_t& idx);
    void GrassBendersUpdate(const u16 id, size_t& data_idx, u32& data_frame, const Fvector& position, const float init_radius, const float init_str, bool CheckDistance);
    void GrassBendersReset(const size_t idx);
    void GrassBendersSet(const size_t idx, const u16 id, const Fvector& position, const Fvector3& dir, const float fade, const float speed, const float intensity,
                         const float radius, const GrassBenders_Anim anim, const bool resetTime);
    float GrassBenderToValue(float& current, const float go_to, const float intensity, const bool use_easing);
    CPerlinNoise1D* PerlinNoise1D{};

    bool IsActorInHideout() const;

    void destroy_particles(const bool& all_particles);

    virtual void PreStart(LPCSTR op);
    virtual tmc::task<void> Start(gsl::czstring op);
    virtual void Disconnect();

    IGame_ObjectPool ObjectPool;
    IMainMenu* m_pMainMenu{};

    bool IsMainMenuActive() const;
    bool MainMenuActiveOrLevelNotExist() const;

    CEnvironment* pEnvironment;
    CEnvironment& Environment() { return *pEnvironment; }

    ShadersExternalData m_pGShaderConstants; //--#SM+#--

    virtual bool OnRenderPPUI_query() { return FALSE; } // should return true if we want to have second function called
    virtual void OnRenderPPUI_main() {}
    virtual void OnRenderPPUI_PP() {}

    tmc::task<void> OnAppStart() override;
    tmc::task<void> OnAppEnd() override;
    tmc::task<void> OnAppActivate() override { co_return; }
    tmc::task<void> OnAppDeactivate() override { co_return; }
    tmc::task<void> OnFrame() override;

    // вызывается только когда изменяется тип игры
    virtual tmc::task<void> OnGameStart();
    virtual void OnGameEnd();

    virtual void UpdateGameType() {}
    virtual void OnSectorChanged(sector_id_t) {}

    virtual void RegisterModel(IRenderVisual* V) = 0;
    virtual float MtlTransparent(u32 mtl_idx) = 0;

    IGame_Persistent();
    ~IGame_Persistent() override;

    u32 GameType() { return m_game_params.m_e_game_type; }
    virtual void Statistics(CGameFont* F) = 0;
    virtual tmc::task<void> LoadTitle(gsl::czstring title) = 0;
    virtual void SetTip() = 0;

    virtual void models_savePrefetch();
};

class XR_NOVTABLE IMainMenu : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IMainMenu);

public:
    ~IMainMenu() override = 0;

    virtual tmc::task<void> Activate(bool bActive) = 0;
    virtual bool IsActive() = 0;
    virtual bool CanSkipSceneRendering() = 0;
    virtual void DestroyInternal(bool bForce) = 0;
};

inline IMainMenu::~IMainMenu() = default;

namespace xr
{
class XR_NOVTABLE ingame_editor : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(xr::ingame_editor);

public:
    ~ingame_editor() override = 0;

    [[nodiscard]] virtual bool opened() = 0;
    virtual void update() = 0;

    [[nodiscard]] virtual bool key_hold(gsl::index key) = 0;
    [[nodiscard]] virtual bool key_press(gsl::index key) = 0;
    [[nodiscard]] virtual bool key_release(gsl::index key) = 0;

    [[nodiscard]] virtual bool mouse_move(gsl::index dx, gsl::index dy) = 0;
    [[nodiscard]] virtual bool mouse_wheel(gsl::index dir) = 0;

    [[nodiscard]] virtual bool script_mixer() const = 0;
    [[nodiscard]] virtual bool script_time() const = 0;
    [[nodiscard]] virtual bool script_weather() const = 0;
};

inline ingame_editor::~ingame_editor() = default;

namespace detail
{
extern std::unique_ptr<xr::ingame_editor> editor;
}

[[nodiscard]] inline xr::ingame_editor* editor() { return xr::detail::editor.get(); }
} // namespace xr

extern IGame_Persistent* g_pGamePersistent;
extern BOOL g_prefetch;

#endif // IGame_PersistentH
