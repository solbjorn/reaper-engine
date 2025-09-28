////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrServer_Objects_ALife_Monsters.h"

extern u32 get_level_id(u32 gvid);
extern LPCSTR get_level_name_by_id(u32 level_id);

u32 se_obj_level_id(CSE_ALifeObject* O) { return get_level_id(O->m_tGraphID); }
LPCSTR se_obj_level_name(CSE_ALifeObject* O) { return get_level_name_by_id(se_obj_level_id(O)); }

bool se_obj_is_alive(CSE_ALifeObject* O)
{
    CSE_ALifeCreatureAbstract* cr = smart_cast<CSE_ALifeCreatureAbstract*>(O);
    if (cr)
        return cr->g_Alive();
    else
        return false;
}

void CSE_ALifeSchedulable::script_register(sol::state_view& lua) { lua.new_usertype<CSE_ALifeSchedulable>("cse_alife_schedulable", sol::no_constructor); }

void CSE_ALifeGraphPoint::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeGraphPoint>("cse_alife_graph_point", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeGraphPoint, LPCSTR>),
                                          sol::base_classes, xr::sol_bases<CSE_ALifeGraphPoint>());
}

Flags32& get_flags_ref(CSE_ALifeObject* sobj) { return sobj->m_flags; }

void cse_obj_set_position(CSE_ALifeObject* o, const Fvector& pos) { o->position().set(pos); }

void CSE_ALifeObject::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeObject>(
        "cse_alife_object", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeObject, LPCSTR>), "CAN_SWITCH_OFFLINE",
        sol::var(CSE_ALifeObject::server_ops::CAN_SWITCH_OFFLINE), "CAN_SWITCH_ONLINE", sol::var(CSE_ALifeObject::server_ops::CAN_SWITCH_ONLINE), "KEEP_SAVED_DATA_ANYWAY",
        sol::var(CSE_ALifeObject::server_ops::KEEP_SAVED_DATA_ANYWAY), "online", sol::readonly(&CSE_ALifeObject::m_bOnline), "move_offline",
        sol::overload(sol::resolve<bool() const>(&CSE_ALifeObject::move_offline), sol::resolve<void(bool)>(&CSE_ALifeObject::move_offline)), "visible_for_map",
        sol::overload(sol::resolve<bool() const>(&CSE_ALifeObject::visible_for_map), sol::resolve<void(bool)>(&CSE_ALifeObject::visible_for_map)), "can_switch_online",
        sol::overload(sol::resolve<bool() const>(&CSE_ALifeObject::can_switch_online), sol::resolve<void(bool)>(&CSE_ALifeObject::can_switch_online)), "can_switch_offline",
        sol::overload(sol::resolve<bool() const>(&CSE_ALifeObject::can_switch_offline), sol::resolve<void(bool)>(&CSE_ALifeObject::can_switch_offline)), "used_ai_locations",
        sol::overload(sol::resolve<bool() const>(&CSE_ALifeObject::used_ai_locations), sol::resolve<void(bool)>(&CSE_ALifeObject::used_ai_locations)),
        // alpet: для коррекции позиции в оффлайне
        "set_position", &cse_obj_set_position, "m_level_vertex_id", &CSE_ALifeObject::m_tNodeID, "m_game_vertex_id", &CSE_ALifeObject::m_tGraphID, "m_story_id",
        sol::readonly(&CSE_ALifeObject::m_story_id), "m_flags", sol::property(&get_flags_ref), "level_id", sol::property(&se_obj_level_id), "level_name",
        sol::property(&se_obj_level_name), "is_alive", sol::property(&se_obj_is_alive), sol::base_classes, xr::sol_bases<CSE_ALifeObject>());
}

void CSE_ALifeGroupAbstract::script_register(sol::state_view& lua) { lua.new_usertype<CSE_ALifeGroupAbstract>("cse_alife_group_abstract", sol::no_constructor); }

void CSE_ALifeDynamicObject::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeDynamicObject>(
        "cse_alife_dynamic_object", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeDynamicObject, LPCSTR>), "ON_BEFORE_REGISTER",
        sol::var(CSE_ALifeDynamicObject::server_ops::ON_BEFORE_REGISTER), "ON_REGISTER", sol::var(CSE_ALifeDynamicObject::server_ops::ON_REGISTER), "ON_UNREGISTER",
        sol::var(CSE_ALifeDynamicObject::server_ops::ON_UNREGISTER), "on_before_register", &CSE_ALifeDynamicObject::on_before_register, sol::base_classes,
        xr::sol_bases<CSE_ALifeDynamicObject>());
}

void CSE_ALifeDynamicObjectVisual::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeDynamicObjectVisual>("cse_alife_dynamic_object_visual", sol::no_constructor, sol::call_constructor,
                                                   sol::factories(std::make_unique<CSE_ALifeDynamicObjectVisual, LPCSTR>), sol::base_classes,
                                                   xr::sol_bases<CSE_ALifeDynamicObjectVisual>());
}

void CSE_ALifePHSkeletonObject::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifePHSkeletonObject>("cse_alife_ph_skeleton_object", sol::no_constructor, sol::call_constructor,
                                                sol::factories(std::make_unique<CSE_ALifePHSkeletonObject, LPCSTR>), sol::base_classes, xr::sol_bases<CSE_ALifePHSkeletonObject>());
}

u8 cse_get_restrictor_type(CSE_ALifeDynamicObject* se_obj)
{
    CSE_ALifeSpaceRestrictor* SR = smart_cast<CSE_ALifeSpaceRestrictor*>(se_obj);
    if (SR)
        return SR->m_space_restrictor_type;
    return 0;
}

void CSE_ALifeSpaceRestrictor::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeSpaceRestrictor>("cse_alife_space_restrictor", sol::no_constructor, sol::call_constructor,
                                               sol::factories(std::make_unique<CSE_ALifeSpaceRestrictor, LPCSTR>), "factory", &xr::server_factory<CSE_ALifeSpaceRestrictor>,
                                               "restrictor_type", &CSE_ALifeSpaceRestrictor::m_space_restrictor_type, "cse_get_restrictor_type", &cse_get_restrictor_type,
                                               sol::base_classes, xr::sol_bases<CSE_ALifeSpaceRestrictor>());
}

void CSE_ALifeLevelChanger::script_register(sol::state_view& lua)
{
    lua.new_usertype<CSE_ALifeLevelChanger>("cse_alife_level_changer", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CSE_ALifeLevelChanger, LPCSTR>),
                                            "dest_game_vertex_id", &CSE_ALifeLevelChanger::m_tNextGraphID, "dest_level_vertex_id", &CSE_ALifeLevelChanger::m_dwNextNodeID,
                                            "dest_position", &CSE_ALifeLevelChanger::m_tNextPosition, "dest_direction", &CSE_ALifeLevelChanger::m_tAngles, "silent_mode",
                                            &CSE_ALifeLevelChanger::m_SilentMode, sol::base_classes, xr::sol_bases<CSE_ALifeLevelChanger>());
}
