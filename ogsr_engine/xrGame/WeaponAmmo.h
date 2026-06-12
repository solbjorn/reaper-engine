#pragma once

#include "inventory_item_object.h"

class CCartridge
{
public:
    CCartridge();

    void Load(LPCSTR section, u8 LocalAmmoType);
    float Weight() const;

    shared_str m_ammoSect;
    enum
    {
        cfTracer = (1 << 0),
        cfRicochet = (1 << 1),
        cfCanBeUnlimited = (1 << 2),
        cfExplosive = (1 << 3),
    };
    float m_kDist, m_kDisp, m_kHit, m_kImpulse, m_kPierce, m_kAP, m_kAirRes, m_kSpeed;
    int m_buckShot;
    float m_impair;
    float fWallmarkSize{};

    u8 m_u8ColorID{};
    u8 m_LocalAmmoType{};

    u16 bullet_material_idx;
    Flags8 m_flags;

    shared_str m_InvShortName;
    RStringVec m_ExplodeParticles;
};

class CWeaponAmmo : public CInventoryItemObject
{
    RTTI_DECLARE_TYPEINFO(CWeaponAmmo, CInventoryItemObject);

public:
    typedef CInventoryItemObject inherited;

    CWeaponAmmo();
    ~CWeaponAmmo() override;

    [[nodiscard]] CWeaponAmmo* cast_weapon_ammo() override { return this; }
    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;
    void OnH_B_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;
    tmc::task<void> UpdateCL() override;
    void renderable_Render(u32 context_id, IRenderable* root) override;

    [[nodiscard]] bool Useful() const override;
    [[nodiscard]] f32 Weight() const override;
    [[nodiscard]] u32 Cost() const override;

    bool Get(CCartridge& cartridge);

    float m_kDist, m_kDisp, m_kHit, m_kImpulse, m_kPierce, m_kAP, m_kAirRes, m_kSpeed;
    int m_buckShot;
    float m_impair;
    float fWallmarkSize;
    u8 m_u8ColorID;

    u16 m_boxSize;
    u16 m_boxCurr;
    bool m_tracer;

public:
    [[nodiscard]] CInventoryItem* can_make_killing(const CInventory* inventory) const override;
};
