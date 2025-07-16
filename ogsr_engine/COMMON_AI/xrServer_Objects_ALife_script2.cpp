////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script2.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export, the second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects_ALife.h"

void CSE_ALifeObjectProjector::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeObjectProjector>("cse_alife_object_projector", sol::no_constructor, sol::call_constructor,
                                                              sol::factories(std::make_unique<CSE_ALifeObjectProjector, LPCSTR>), sol::base_classes,
                                                              xr_sol_bases<CSE_ALifeObjectProjector>());
}

void CSE_ALifeHelicopter::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeHelicopter>("cse_alife_helicopter", sol::no_constructor, sol::call_constructor,
                                                         sol::factories(std::make_unique<CSE_ALifeHelicopter, LPCSTR>), sol::base_classes, xr_sol_bases<CSE_ALifeHelicopter>());
}

void CSE_ALifeCar::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeCar>("cse_alife_car", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeCar, LPCSTR>),
                                                  sol::base_classes, xr_sol_bases<CSE_ALifeCar>());
}

void CSE_ALifeObjectBreakable::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeObjectBreakable>("cse_alife_object_breakable", sol::no_constructor, sol::call_constructor,
                                                              sol::factories(std::make_unique<CSE_ALifeObjectBreakable, LPCSTR>), sol::base_classes,
                                                              xr_sol_bases<CSE_ALifeObjectBreakable>());
}

void CSE_ALifeObjectClimable::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeObjectClimable>("cse_alife_object_climable", sol::no_constructor, sol::call_constructor,
                                                             sol::factories(std::make_unique<CSE_ALifeObjectClimable, LPCSTR>), sol::base_classes,
                                                             xr_sol_bases<CSE_ALifeObjectClimable>());
}

void CSE_ALifeMountedWeapon::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_ALifeMountedWeapon>("cse_alife_mounted_weapon", sol::no_constructor, sol::call_constructor,
                                                            sol::factories(std::make_unique<CSE_ALifeMountedWeapon, LPCSTR>), sol::base_classes,
                                                            xr_sol_bases<CSE_ALifeMountedWeapon>());
}
