#pragma once

#include "weapon.h"
#include "hudsound.h"
#include "ai_sounds.h"

class CMotionDef;

// размер очереди считается бесконечность
// заканчиваем стрельбу, только, если кончились патроны
#define WEAPON_ININITE_QUEUE -1

class CBinocularsVision;

class CWeaponMagazined : public CWeapon
{
    RTTI_DECLARE_TYPEINFO(CWeaponMagazined, CWeapon);

public:
    friend class CWeaponScript;

private:
    typedef CWeapon inherited;

protected:
    // Media :: sounds
    HUD_SOUND sndShow;
    HUD_SOUND sndHide;
    HUD_SOUND sndShot;
    HUD_SOUND sndEmptyClick;
    HUD_SOUND sndReload, sndReloadPartly, sndReloadJammed, sndReloadJammedLast;
    HUD_SOUND sndFireModes;
    HUD_SOUND sndZoomChange;
    HUD_SOUND sndTactItemOn;
    HUD_SOUND sndAimStart, sndAimEnd;
    HUD_SOUND sndItemOn;
    // звук текущего выстрела
    HUD_SOUND* m_pSndShotCurrent{};

    void StopHUDSounds() override;

    // дополнительная информация о глушителе
    LPCSTR m_sSilencerFlameParticles{};
    LPCSTR m_sSilencerSmokeParticles{};
    HUD_SOUND sndSilencerShot;

    ESoundTypes m_eSoundShow;
    ESoundTypes m_eSoundHide;
    ESoundTypes m_eSoundShot;
    ESoundTypes m_eSoundEmptyClick;
    ESoundTypes m_eSoundReload;

    // General
    // кадр момента пересчета UpdateSounds
    u32 dwUpdateSounds_Frame;

protected:
    void OnMagazineEmpty() override;

    virtual void switch2_Idle();
    virtual void switch2_Fire();
    virtual void switch2_Fire2() {}
    void switch2_Empty(const bool empty_click_anim_play);
    virtual void switch2_Reload();
    virtual void switch2_Hiding();
    virtual void switch2_Hidden();
    virtual void switch2_Showing();

    void OnShot() override;
    virtual void OnEmptyClick();

    void OnAnimationEnd(u32 state) override;
    void OnStateSwitch(u32 S, u32 oldState) override;

    virtual void UpdateSounds();
    bool TryReload();

protected:
    virtual void ReloadMagazine();
    void ApplySilencerKoeffs();

    virtual void state_Fire(float);

public:
    explicit CWeaponMagazined(ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
    ~CWeaponMagazined() override;

    void Load(gsl::czstring section) override;
    [[nodiscard]] CWeaponMagazined* cast_weapon_magazined() override { return this; }

    void SetDefaults() override;
    void FireStart() override;
    void FireEnd() override;
    void Reload() override;
    void Misfire() override;
    void DeviceSwitch() override;

protected:
    void DeviceUpdate() override;

public:
    tmc::task<void> UpdateCL() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;

    void OnH_A_Chield() override;

    [[nodiscard]] bool Attach(PIItem pIItem, bool b_send_event) override;
    [[nodiscard]] bool Detach(gsl::czstring item_section_name, bool b_spawn_item) override;
    [[nodiscard]] bool CanAttach(PIItem pIItem) override;
    [[nodiscard]] bool CanDetach(gsl::czstring item_section_name) override;

    void InitAddons() override;
    virtual void InitZoomParams(LPCSTR section, bool useTexture);

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;
    virtual void UnloadMagazine(bool spawn_ammo = true);

    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;

    //////////////////////////////////////////////
    // для стрельбы очередями или одиночными
    //////////////////////////////////////////////
public:
    [[nodiscard]] virtual bool SwitchMode();
    [[nodiscard]] virtual bool SingleShotMode() { return 1 == m_iQueueSize; }
    virtual void SetQueueSize(int size);
    [[nodiscard]] s32 GetQueueSize() const { return m_iQueueSize; }
    [[nodiscard]] virtual bool StopedAfterQueueFired() { return m_bStopedAfterQueueFired; }
    virtual void StopedAfterQueueFired(bool value) { m_bStopedAfterQueueFired = value; }

protected:
    // максимальный размер очереди, которой можно стрельнуть
    int m_iQueueSize{WEAPON_ININITE_QUEUE};
    // количество реально выстреляных патронов
    int m_iShotNum{};
    //  [7/20/2005]
    // после какого патрона, при непрерывной стрельбе, начинается отдача (сделано из-зи Абакана)
    int m_iShootEffectorStart;
    Fvector m_vStartPos, m_vStartDir;
    //  [7/20/2005]
    // флаг того, что мы остановились после того как выстреляли
    // ровно столько патронов, сколько было задано в m_iQueueSize
    bool m_bStopedAfterQueueFired;
    // флаг того, что хотя бы один выстрел мы должны сделать
    //(даже если очень быстро нажали на курок и вызвалось FireEnd)
    bool m_bFireSingleShot{};
    // режимы стрельбы
    bool m_bHasDifferentFireModes;
    xr_vector<int> m_aFireModes;
    int m_iCurFireMode;
    string16 m_sCurFireMode;
    int m_iPrefferedFireMode;
    u32 m_fire_zoomout_time = u32(-1);

    // переменная блокирует использование
    // только разных типов патронов
    bool m_bLockType{};

    const char* m_str_count_tmpl;

    // режим выделения рамкой противников
protected:
    CBinocularsVision* m_binoc_vision{};
    bool m_bVision{};

    //////////////////////////////////////////////
    // режим приближения
    //////////////////////////////////////////////
public:
    void OnZoomIn() override;
    void OnZoomOut() override;
    void OnZoomChanged() override;
    virtual void OnNextFireMode(bool = false);
    virtual void OnPrevFireMode(bool = false);
    [[nodiscard]] virtual bool HasFireModes() { return m_bHasDifferentFireModes; }
    [[nodiscard]] virtual s32 GetCurrentFireMode() { return m_bHasDifferentFireModes ? m_aFireModes[m_iCurFireMode] : 1; }
    [[nodiscard]] virtual gsl::czstring GetCurrentFireModeStr() { return m_sCurFireMode; }

    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;

protected:
    [[nodiscard]] virtual bool AllowFireWhileWorking() { return false; }

    // виртуальные функции для проигрывания анимации HUD
    virtual void PlayAnimShow();
    virtual void PlayAnimHide();
    virtual void PlayAnimReload();
    void PlayAnimIdle() override;

    bool LaserSwitch{}, TorchSwitch{}, HeadLampSwitch{}, NightVisionSwitch{};
    bool CartridgeInTheChamberEnabled{};
    u32 CartridgeInTheChamber{};

private:
    string128 guns_aim_anm;

protected:
    [[nodiscard]] virtual gsl::czstring GetAnimAimName();

    virtual void PlayAnimAim();
    virtual void PlayAnimShoot();
    virtual void PlayAnimFakeShoot();
    void PlayAnimDeviceSwitch() override;
    virtual void PlayAnimCheckMisfire();
    virtual void PlayReloadSound();

    [[nodiscard]] s32 ShotsFired() override { return m_iShotNum; }
    [[nodiscard]] f32 GetWeaponDeterioration() override;

    void OnDrawUI() override;
    void net_Relcase(CObject* object) override;

    [[nodiscard]] bool ScopeRespawn(PIItem);

    void OnMotionMark(u32 state, const motion_marks& M) override;
    [[nodiscard]] s32 CheckAmmoBeforeReload(u32& v_ammoType);
};
XR_SOL_BASE_CLASSES(CWeaponMagazined);
