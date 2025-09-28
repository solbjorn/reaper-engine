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
                                          "priv", &CScriptBinderObject::priv, "ops", &CScriptBinderObject::ops, "LOAD", sol::var(CScriptBinderObject::binder_ops::LOAD),
                                          "NET_DESTROY", sol::var(CScriptBinderObject::binder_ops::NET_DESTROY), "NET_RELCASE",
                                          sol::var(CScriptBinderObject::binder_ops::NET_RELCASE), "NET_SAVE_RELEVANT", sol::var(CScriptBinderObject::binder_ops::NET_SAVE_RELEVANT),
                                          "NET_SPAWN", sol::var(CScriptBinderObject::binder_ops::NET_SPAWN), "REINIT", sol::var(CScriptBinderObject::binder_ops::REINIT), "RELOAD",
                                          sol::var(CScriptBinderObject::binder_ops::RELOAD), "SAVE", sol::var(CScriptBinderObject::binder_ops::SAVE), "UPDATE",
                                          sol::var(CScriptBinderObject::binder_ops::UPDATE), "object", sol::readonly(&CScriptBinderObject::m_object));
}
