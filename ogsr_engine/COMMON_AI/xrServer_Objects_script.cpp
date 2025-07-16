////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 23.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Server objects script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "../xr_3da/NET_Server_Trash/NET_utils.h"

#include "script_ini_file.h"
#include "xrServer_Objects.h"

LPCSTR get_section_name(const CSE_Abstract* abstract) { return (abstract->name()); }

LPCSTR get_name(const CSE_Abstract* abstract) { return (abstract->name_replace()); }

CScriptIniFile* get_spawn_ini(CSE_Abstract* abstract) { return ((CScriptIniFile*)&abstract->spawn_ini()); }

void save_spawn_ini(CSE_Abstract* abstract)
{
    auto str = abstract->spawn_ini().get_as_string();
    abstract->m_ini_string = shared_str(str.c_str());
}

LPCSTR get_ini_string_script(const CSE_Abstract* abstract) { return abstract->m_ini_string.c_str(); }

void set_ini_string_script(CSE_Abstract* abstract, LPCSTR cd)
{
    abstract->m_ini_string = cd;
    abstract->m_ini_file = nullptr;
}

void CSE_Abstract::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_Abstract>("cse_abstract", sol::no_constructor, "id", sol::readonly(&CSE_Abstract::ID), "parent_id", sol::readonly(&CSE_Abstract::ID_Parent),
                                                  "script_version", sol::readonly(&CSE_Abstract::m_script_version), "position", &CSE_Abstract::o_Position, "angle",
                                                  &CSE_Abstract::o_Angle, "section_name", &get_section_name, "name", &get_name, "clsid", &CSE_Abstract::script_clsid, "spawn_ini",
                                                  &get_spawn_ini, "STATE_Read", &CSE_Abstract::STATE_Read, "STATE_Write", &CSE_Abstract::STATE_Write, "UPDATE_Read",
                                                  &CSE_Abstract::UPDATE_Read, "UPDATE_Write", &CSE_Abstract::UPDATE_Write, "custom_data",
                                                  sol::property(&get_ini_string_script, &set_ini_string_script), "save_spawn_ini", &save_spawn_ini);
}

void CSE_Shape::script_register(lua_State* L) { sol::state_view(L).new_usertype<CSE_Shape>("cse_shape", sol::no_constructor); }

void CSE_Visual::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_Visual>("cse_visual", sol::no_constructor, "visual_name", sol::property(&CSE_Visual::get_visual, &CSE_Visual::set_visual));
}

void CSE_Motion::script_register(lua_State* L) { sol::state_view(L).new_usertype<CSE_Motion>("cse_motion", sol::no_constructor); }

void CSE_Temporary::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CSE_Temporary>("cse_temporary", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_Temporary, LPCSTR>),
                                                   sol::base_classes, xr_sol_bases<CSE_Temporary>());
}
