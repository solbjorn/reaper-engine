#pragma once

#include "missile.h"
#include "explosive.h"
#include "..\xr_3da\feel_touch.h"

#define SND_RIC_COUNT 5

class CGrenade : public CMissile, public CExplosive
{
    RTTI_DECLARE_TYPEINFO(CGrenade, CMissile, CExplosive);

private:
    typedef CMissile inherited;

public:
    CGrenade();
    ~CGrenade() override;

    void Load(gsl::czstring section) override;

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Relcase(CObject* O) override;

    void OnH_B_Independent(bool just_before_destroy) override;
    void OnH_A_Independent() override;
    void OnH_A_Chield() override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    void OnAnimationEnd(u32 state) override;
    tmc::task<void> UpdateCL() override;

    void Throw() override;
    void Destroy() override;

    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;
    [[nodiscard]] bool Useful() const override;
    void State(u32 state, u32 oldState) override;

    void OnH_B_Chield() override { inherited::OnH_B_Chield(); }

    void Hit(SHit* pHDS) override;

    void PutNextToSlot();

    void Deactivate(bool = false) override;
    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;
    [[nodiscard]] bool StopSprintOnFire() override { return false; }

    [[nodiscard]] bool CanTake() const override;

protected:
    HUD_SOUND sndCheckout;
    ESoundTypes m_eSoundCheckout;

    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return Knife_and_other; }

private:
    f32 m_grenade_detonation_threshold_hit{};
    bool m_thrown{};

    void TrySwitchGrenade(CGrenade* grenade = nullptr);

protected:
    void UpdateXForm() override { CMissile::UpdateXForm(); }

public:
    [[nodiscard]] BOOL UsedAI_Locations() override;
    [[nodiscard]] CExplosive* cast_explosive() override { return this; }
    [[nodiscard]] CMissile* cast_missile() override { return this; }
    [[nodiscard]] CHudItem* cast_hud_item() override { return this; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }
    [[nodiscard]] IDamageSource* cast_IDamageSource() override { return CExplosive::cast_IDamageSource(); }

    using destroy_callback = CallMe::Delegate<void(CGrenade*)>;

    void set_destroy_callback(destroy_callback callback) { m_destroy_callback = callback; }

private:
    destroy_callback m_destroy_callback;
};
