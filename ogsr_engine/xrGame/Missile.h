#pragma once

#include "hud_item_object.h"
#include "HudSound.h"

class CUIProgressShape;
struct dContact;
struct SGameMtl;

class CMissile : public CHudItemObject
{
    RTTI_DECLARE_TYPEINFO(CMissile, CHudItemObject);

public:
    friend class CWeaponScript;
    typedef CHudItemObject inherited;

    CMissile();
    ~CMissile() override;

    [[nodiscard]] BOOL AlwaysTheCrow() override { return TRUE; }
    void OnDrawUI() override;

    void reinit() override;
    [[nodiscard]] CMissile* cast_missile() override { return this; }

    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;

    tmc::task<void> UpdateCL() override;
    tmc::task<void> shedule_Update(u32 dt) override;

    void OnH_A_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    void OnAnimationEnd(u32 state) override;
    void OnMotionMark(u32 state, const motion_marks& M) override;

protected:
    void DeviceUpdate() override;

public:
    void Show(bool = false) override;
    void Hide(bool = false) override;

    virtual void Throw();
    virtual void Destroy();

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;

    virtual void State(u32 state, u32 oldState);
    void OnStateSwitch(u32 S, u32 oldState) override;
    void PlayAnimIdle() override;
    void PlayAnimDeviceSwitch() override;
    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;

protected:
    void UpdateXForm() override;
    void UpdatePosition(const Fmatrix& trans);
    void spawn_fake_missile();

    // инициализация если вещь в активном слоте или спрятана на OnH_B_Chield
    void OnActiveItem() override;
    void OnHiddenItem() override;

    // для сети
    void net_Relcase(CObject* O) override;

protected:
    bool m_throw;

    bool HeadLampSwitch{}, NightVisionSwitch{};

    // время уничтожения
    u32 m_dwDestroyTime;
    u32 m_dwDestroyTimeMax;

    Fvector m_throw_direction;
    Fmatrix m_throw_matrix;

    // указатель на владельца RocketLauncher - который кидал гранату
    CGameObject* m_pOwner{};

    CMissile* m_fake_missile;

    // параметры броска

    float m_fMinForce, m_fConstForce, m_fMaxForce, m_fForceGrowSpeed;

    bool m_constpower;
    float m_fThrowForce;

    bool m_kick_on_explode{};
    bool m_explode_by_timer_on_safe_dist{true};

    float m_safe_dist_to_explode{};

    bool has_already_contact{};

protected:
    // относительная точка и направление вылета гранаты
    Fvector m_vThrowPoint;
    Fvector m_vThrowDir;
    // для HUD
    Fvector m_vHudThrowPoint;
    Fvector m_vHudThrowDir;

    // звук анимации "играния"
    HUD_SOUND sndPlaying;
    HUD_SOUND sndItemOn;

    bool m_throwMotionMarksAvailable{};

protected:
    void setup_throw_params();

public:
    Fvector const& throw_point_offset() const { return m_vThrowPoint; }
    void activate_physic_shell() override;
    void setup_physic_shell() override;
    void create_physic_shell() override;
    IC void set_destroy_time(u32 delta_destroy_time) { m_dwDestroyTime = delta_destroy_time + Device.dwTimeGlobal; }

protected:
    u32 m_ef_weapon_type;

public:
    [[nodiscard]] u32 ef_weapon_type() const override;
    [[nodiscard]] u32 destroy_time() const { return m_dwDestroyTime; }
    static void ExitContactCallback(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/);
};
XR_SOL_BASE_CLASSES(CMissile);

extern CUIProgressShape* g_MissileForceShape;
