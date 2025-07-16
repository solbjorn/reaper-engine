////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script3.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export, the third part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects_ALife_Monsters.h"

#ifdef XRGAME_EXPORTS
#include "alife_smart_terrain_task.h"
#endif

void CSE_ALifeObjectHangingLamp::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeObjectHangingLamp>("cse_alife_object_hanging_lamp", sol::no_constructor, sol::call_constructor,
                                                                sol::factories(std::make_unique<CSE_ALifeObjectHangingLamp, LPCSTR>), sol::base_classes,
                                                                xr_sol_bases<CSE_ALifeObjectHangingLamp>());
}

void CSE_ALifeObjectPhysic::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeObjectPhysic>(
        "cse_alife_object_physic", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeObjectPhysic, LPCSTR>), "mass",
        &CSE_ALifeObjectPhysic::mass, "fixed_bones",
        sol::property([](CSE_ALifeObjectPhysic* self) { return self->fixed_bones.c_str(); }, [](CSE_ALifeObjectPhysic* self, const char* name) { self->fixed_bones = name; }),
        sol::base_classes, xr_sol_bases<CSE_ALifeObjectPhysic>());
}

void CSE_ALifeSmartZone::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeSmartZone>("cse_alife_smart_zone", sol::no_constructor, sol::call_constructor,
                                                        sol::factories(std::make_unique<CSE_ALifeSmartZone, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeSmartZone>());
}
