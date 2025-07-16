////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_Items_script3.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server items for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects_ALife_Items.h"

void CSE_ALifeItemWeaponMagazinedWGL::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeItemWeaponMagazinedWGL>(
        "cse_alife_item_weapon_magazined_w_gl", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeItemWeaponMagazinedWGL, LPCSTR>),
        "ammo_type_2", &CSE_ALifeItemWeaponMagazinedWGL::ammo_type2, "ammo_elapsed_2", &CSE_ALifeItemWeaponMagazinedWGL::a_elapsed2, "gl_mode",
        &CSE_ALifeItemWeaponMagazinedWGL::m_bGrenadeMode, sol::base_classes, xr_sol_bases<CSE_ALifeItemWeaponMagazinedWGL>());
}
