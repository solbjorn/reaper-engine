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

    virtual void Load(LPCSTR section);

    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    virtual void net_Relcase(CObject* O);

    virtual void OnH_B_Independent(bool just_before_destroy);
    virtual void OnH_A_Independent();
    virtual void OnH_A_Chield();

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    virtual void OnAnimationEnd(u32 state);
    tmc::task<void> UpdateCL() override;

    virtual void Throw();
    virtual void Destroy();

    virtual bool Action(s32 cmd, u32 flags);
    virtual bool Useful() const;
    virtual void State(u32 state, u32 oldState);

    virtual void OnH_B_Chield() { inherited::OnH_B_Chield(); }

    virtual void Hit(SHit* pHDS);

    void PutNextToSlot();

    virtual void Deactivate(bool = false);
    virtual void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);
    virtual bool StopSprintOnFire() { return false; }

    virtual bool CanTake() const override;

protected:
    HUD_SOUND sndCheckout;
    ESoundTypes m_eSoundCheckout;

    size_t GetWeaponTypeForCollision() const override { return Knife_and_other; }

private:
    f32 m_grenade_detonation_threshold_hit{};
    bool m_thrown{};

    void TrySwitchGrenade(CGrenade* grenade = nullptr);

protected:
    virtual void UpdateXForm() { CMissile::UpdateXForm(); }

public:
    virtual BOOL UsedAI_Locations();
    virtual CExplosive* cast_explosive() { return this; }
    virtual CMissile* cast_missile() { return this; }
    virtual CHudItem* cast_hud_item() { return this; }
    virtual CGameObject* cast_game_object() { return this; }
    virtual IDamageSource* cast_IDamageSource() { return CExplosive::cast_IDamageSource(); }

    using destroy_callback = CallMe::Delegate<void(CGrenade*)>;

    void set_destroy_callback(destroy_callback callback) { m_destroy_callback = callback; }

private:
    destroy_callback m_destroy_callback;
};
