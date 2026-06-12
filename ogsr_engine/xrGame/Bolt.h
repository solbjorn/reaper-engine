#pragma once

#include "missile.h"
#include "DamageSource.h"

class CBolt : public CMissile, public IDamageSource
{
    RTTI_DECLARE_TYPEINFO(CBolt, CMissile, IDamageSource);

public:
    typedef CMissile inherited;
    u16 m_thrower_id;

    CBolt();
    ~CBolt() override;

    void OnH_A_Chield() override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    bool Activate(bool = false) override;
    void Deactivate(bool = false) override;

    void SetInitiator(u16 id) override;
    [[nodiscard]] u16 Initiator() override;

    void Throw() override;
    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;
    [[nodiscard]] bool Useful() const override;
    void Destroy() override;
    void activate_physic_shell() override;
    void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) override;

    [[nodiscard]] BOOL UsedAI_Locations() override { return FALSE; }
    [[nodiscard]] IDamageSource* cast_IDamageSource() override { return this; }

    [[nodiscard]] bool StopSprintOnFire() override { return false; }

protected:
    [[nodiscard]] size_t GetWeaponTypeForCollision() const override { return Bolt; }
};
