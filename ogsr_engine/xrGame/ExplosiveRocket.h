//////////////////////////////////////////////////////////////////////
// ExplosiveRocket.h:	ракета, которой стреляет RocketLauncher
//						взрывается при столкновении
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CustomRocket.h"
#include "Explosive.h"
#include "inventory_item.h"

class CExplosiveRocket : public CCustomRocket, public CInventoryItem, public CExplosive
{
    RTTI_DECLARE_TYPEINFO(CExplosiveRocket, CCustomRocket, CInventoryItem, CExplosive);

private:
    typedef CCustomRocket inherited;
    friend CRocketLauncher;

public:
    CExplosiveRocket();
    ~CExplosiveRocket() override;

    [[nodiscard]] DLL_Pure* _construct() override;

    [[nodiscard]] CExplosive* cast_explosive() override { return this; }
    [[nodiscard]] CInventoryItem* cast_inventory_item() override { return this; }
    [[nodiscard]] CAttachableItem* cast_attachable_item() override { return this; }
    [[nodiscard]] CWeapon* cast_weapon() override { return nullptr; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }
    [[nodiscard]] IDamageSource* cast_IDamageSource() override { return CExplosive::cast_IDamageSource(); }
    void on_activate_physic_shell() override;

public:
    void Load(gsl::czstring section) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Relcase(CObject* O) override;
    void OnH_A_Independent() override;
    void OnH_B_Independent(bool just_before_destroy) override;
    tmc::task<void> UpdateCL() override;

    void Contact(const Fvector3& pos, const Fvector3& normal) override;

    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;

    void Hit(SHit* pHDS) override { inherited::Hit(pHDS); }

public:
    [[nodiscard]] BOOL UsedAI_Locations() override { return inherited::UsedAI_Locations(); }
    void net_Export(CSE_Abstract* E) override { inherited::net_Export(E); }

    void save(NET_Packet& output_packet) override { inherited::save(output_packet); }
    void load(IReader& input_packet) override { inherited::load(input_packet); }
    [[nodiscard]] BOOL net_SaveRelevant() override { return inherited::net_SaveRelevant(); }

    void OnH_A_Chield() override { inherited::OnH_A_Chield(); }
    void OnH_B_Chield() override { inherited::OnH_B_Chield(); }

#ifdef DEBUG
    virtual void OnRender();
#endif

    void reinit() override;
    void reload(gsl::czstring section) override;
    void activate_physic_shell() override;
    void setup_physic_shell() override;
    void create_physic_shell() override;

public:
    [[nodiscard]] bool Useful() const override;

protected:
    float m_safe_dist_to_explode;
    shared_str real_grenade_name;

public:
    void SetRealGrenadeName(shared_str name) { real_grenade_name = name; }
};
