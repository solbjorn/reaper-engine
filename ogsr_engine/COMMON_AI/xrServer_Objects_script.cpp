////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 23.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Server objects script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xr_3da/NET_Server_Trash/NET_utils.h"
#include "script_ini_file.h"
#include "xrServer_Objects_ALife_Monsters.h"

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

void CSE_Abstract::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_Abstract>("cse_abstract", sol::no_constructor, "priv", &CSE_Abstract::priv, "ops", &CSE_Abstract::ops, "STATE_READ", sol::var(CSE_Abstract::STATE_READ),
                                   "STATE_WRITE", sol::var(CSE_Abstract::STATE_WRITE), "id", sol::readonly(&CSE_Abstract::ID), "parent_id", sol::readonly(&CSE_Abstract::ID_Parent),
                                   "script_version", sol::readonly(&CSE_Abstract::m_script_version), "position", &CSE_Abstract::o_Position, "angle", &CSE_Abstract::o_Angle,
                                   "section_name", &CSE_Abstract::name, "name", &CSE_Abstract::name_replace, "clsid", &CSE_Abstract::script_clsid, "spawn_ini", &get_spawn_ini,
                                   "STATE_Read", &CSE_Abstract::STATE_Read, "STATE_Write", &CSE_Abstract::STATE_Write, "UPDATE_Read", &CSE_Abstract::UPDATE_Read, "UPDATE_Write",
                                   &CSE_Abstract::UPDATE_Write, "custom_data", sol::property(&get_ini_string_script, &set_ini_string_script), "save_spawn_ini", &save_spawn_ini,
                                   "get_dynamic_object", &CSE_Abstract::cast_alife_dynamic_object, "get_trader", &CSE_Abstract::cast_trader, "get_smart_zone",
                                   &CSE_Abstract::cast_smart_zone, "get_human", &CSE_Abstract::cast_human_abstract, "get_monster", &CSE_Abstract::cast_monster_abstract);
}

void CSE_Shape::script_register(sol::state_view& lua) { lua.new_usertype<CSE_Shape>("cse_shape", sol::no_constructor); }

void CSE_Visual::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_Visual>("cse_visual", sol::no_constructor, "visual_name", sol::property(&CSE_Visual::get_visual, &CSE_Visual::set_visual));
}

void CSE_Motion::script_register(sol::state_view& lua) { lua.new_usertype<CSE_Motion>("cse_motion", sol::no_constructor); }

void CSE_Temporary::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_Temporary>("cse_temporary", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_Temporary, LPCSTR>), sol::base_classes,
                                    xr_sol_bases<CSE_Temporary>());
}
