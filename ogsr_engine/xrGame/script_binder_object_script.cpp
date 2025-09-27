////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_script.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xr_3da/NET_Server_Trash/net_utils.h"

#include "script_binder_object.h"
#include "script_export_space.h"
#include "script_game_object.h"

#include "xrServer_Objects_ALife.h"

void CScriptBinderObject::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptBinderObject>("object_binder", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CScriptBinderObject, CScriptGameObject*>),
                                          "priv", &CScriptBinderObject::priv, "ops", &CScriptBinderObject::ops, "LOAD", sol::var(CScriptBinderObject::LOAD), "NET_DESTROY",
                                          sol::var(CScriptBinderObject::NET_DESTROY), "NET_RELCASE", sol::var(CScriptBinderObject::NET_RELCASE), "NET_SAVE_RELEVANT",
                                          sol::var(CScriptBinderObject::NET_SAVE_RELEVANT), "NET_SPAWN", sol::var(CScriptBinderObject::NET_SPAWN), "REINIT",
                                          sol::var(CScriptBinderObject::REINIT), "RELOAD", sol::var(CScriptBinderObject::RELOAD), "SAVE", sol::var(CScriptBinderObject::SAVE),
                                          "UPDATE", sol::var(CScriptBinderObject::UPDATE), "object", sol::readonly(&CScriptBinderObject::m_object));
}
