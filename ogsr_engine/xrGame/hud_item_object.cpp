////////////////////////////////////////////////////////////////////////////
//	Module 		: hud_item_object.cpp
//	Created 	: 24.03.2003
//  Modified 	: 27.12.2004
//	Author		: Yuri Dobronravin
//	Description : HUD item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "hud_item_object.h"

CHudItemObject::CHudItemObject() = default;
CHudItemObject::~CHudItemObject() = default;

DLL_Pure* CHudItemObject::_construct()
{
    CInventoryItemObject::_construct();
    CHudItem::_construct();
    return (this);
}

void CHudItemObject::Load(LPCSTR section)
{
    CInventoryItemObject::Load(section);
    CHudItem::Load(section);
}

bool CHudItemObject::Action(EGameActions cmd, u32 flags)
{
    if (CInventoryItemObject::Action(cmd, flags))
        return (true);

    return (CHudItem::Action(cmd, flags));
}

void CHudItemObject::SwitchState(u32 S) { CHudItem::SwitchState(S); }
void CHudItemObject::OnStateSwitch(u32 S, u32 oldState) { CHudItem::OnStateSwitch(S, oldState); }

tmc::task<void> CHudItemObject::OnEvent(NET_Packet& P, u16 type)
{
    co_await CInventoryItemObject::OnEvent(P, type);
    co_await CHudItem::OnEvent(P, type);
}

void CHudItemObject::OnH_A_Chield()
{
    CHudItem::OnH_A_Chield();
    CInventoryItemObject::OnH_A_Chield();
}

void CHudItemObject::OnH_B_Chield()
{
    CInventoryItemObject::OnH_B_Chield();
    CHudItem::OnH_B_Chield();
}

void CHudItemObject::OnH_B_Independent(bool just_before_destroy)
{
    CHudItem::OnH_B_Independent(just_before_destroy);
    CInventoryItemObject::OnH_B_Independent(just_before_destroy);
}

void CHudItemObject::OnH_A_Independent()
{
    CHudItem::OnH_A_Independent();
    CInventoryItemObject::OnH_A_Independent();
}

tmc::task<bool> CHudItemObject::net_Spawn(CSE_Abstract* DC) { co_return co_await CInventoryItemObject::net_Spawn(DC) && co_await CHudItem::net_Spawn(DC); }

tmc::task<void> CHudItemObject::net_Destroy()
{
    co_await CHudItem::net_Destroy();
    co_await CInventoryItemObject::net_Destroy();
}

bool CHudItemObject::Activate(bool now) { return CHudItem::Activate(now); }
void CHudItemObject::Deactivate(bool now) { CHudItem::Deactivate(now); }

tmc::task<void> CHudItemObject::UpdateCL()
{
    co_await CInventoryItemObject::UpdateCL();
    co_await CHudItem::UpdateCL();
}

void CHudItemObject::renderable_Render(u32 context_id, IRenderable* root) { CHudItem::renderable_Render(context_id, root); }
void CHudItemObject::on_renderable_Render(u32 context_id, IRenderable* root) { CInventoryItemObject::renderable_Render(context_id, root); }
