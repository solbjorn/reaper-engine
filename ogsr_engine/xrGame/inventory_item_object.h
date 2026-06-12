////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item_object.h
//	Created 	: 24.03.2003
//  Modified 	: 27.12.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item object implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "physic_item.h"
#include "inventory_item.h"

class CInventoryItemObject : public CInventoryItem, public CPhysicItem
{
    RTTI_DECLARE_TYPEINFO(CInventoryItemObject, CInventoryItem, CPhysicItem);

public:
    CInventoryItemObject();
    ~CInventoryItemObject() override;

    [[nodiscard]] DLL_Pure* _construct() override;

    [[nodiscard]] CPhysicsShellHolder* cast_physics_shell_holder() override { return this; }
    [[nodiscard]] CInventoryItem* cast_inventory_item() override { return this; }
    [[nodiscard]] CAttachableItem* cast_attachable_item() override { return this; }
    [[nodiscard]] CWeapon* cast_weapon() override { return nullptr; }
    [[nodiscard]] CFoodItem* cast_food_item() override { return nullptr; }
    [[nodiscard]] CMissile* cast_missile() override { return nullptr; }
    [[nodiscard]] CHudItem* cast_hud_item() override { return nullptr; }
    [[nodiscard]] CWeaponAmmo* cast_weapon_ammo() override { return nullptr; }
    [[nodiscard]] CGameObject* cast_game_object() override { return this; }

public:
    void Load(gsl::czstring section) override;
    [[nodiscard]] gsl::czstring Name() override;
    [[nodiscard]] gsl::czstring NameShort() override;
    void Hit(SHit* pHDS) override;

    void OnH_B_Independent(bool just_before_destroy) override;
    void OnH_A_Independent() override;
    void OnH_B_Chield() override;
    void OnH_A_Chield() override;
    tmc::task<void> UpdateCL() override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    void net_Export(CSE_Abstract* E) override;
    void save(NET_Packet& output_packet) override;
    void load(IReader& input_packet) override;
    [[nodiscard]] BOOL net_SaveRelevant() override { return TRUE; }
    void renderable_Render(u32 context_id, IRenderable* root) override;
    void reload(gsl::czstring section) override;
    void reinit() override;
    void activate_physic_shell() override;
    void on_activate_physic_shell() override;
    void modify_holder_params(f32& range, f32& fov) const override;

#ifdef DEBUG
protected:
    virtual void OnRender();
#endif

public:
    [[nodiscard]] bool Useful() const override;
    [[nodiscard]] u32 ef_weapon_type() const override;
};
XR_SOL_BASE_CLASSES(CInventoryItemObject);

#include "inventory_item_inline.h"
