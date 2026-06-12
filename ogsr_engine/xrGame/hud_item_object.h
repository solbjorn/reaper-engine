////////////////////////////////////////////////////////////////////////////
//	Module 		: hud_item_object.h
//	Created 	: 24.03.2003
//  Modified 	: 27.12.2004
//	Author		: Yuri Dobronravin
//	Description : HUD item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_item_object.h"
#include "huditem.h"

class CHudItemObject : public CInventoryItemObject, public CHudItem
{
    RTTI_DECLARE_TYPEINFO(CHudItemObject, CInventoryItemObject, CHudItem);

public:
    CHudItemObject();
    ~CHudItemObject() override;

    [[nodiscard]] DLL_Pure* _construct() override;
    [[nodiscard]] CHudItem* cast_hud_item() override { return this; }

    void Load(gsl::czstring section) override;
    [[nodiscard]] bool Action(EGameActions cmd, u32 flags) override;
    void SwitchState(u32 S) override;
    void OnStateSwitch(u32 S, u32 oldState) override;
    tmc::task<void> OnEvent(NET_Packet& P, u16 type) override;
    void OnH_A_Chield() override;
    void OnH_B_Chield() override;
    void OnH_B_Independent(bool just_before_destroy) override;
    void OnH_A_Independent() override;
    tmc::task<bool> net_Spawn(CSE_Abstract* DC) override;
    tmc::task<void> net_Destroy() override;
    [[nodiscard]] bool Activate(bool = false) override;
    void Deactivate(bool = false) override;
    tmc::task<void> UpdateCL() override;
    void renderable_Render(u32 context_id, IRenderable* root) override;
    void on_renderable_Render(u32 context_id, IRenderable* root) override;

    [[nodiscard]] bool use_parent_ai_locations() const override { return Device.dwFrame != dwXF_Frame; }
};
