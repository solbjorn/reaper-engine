////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_script.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"

#include "script_binder_object.h"
#include "script_export_space.h"
#include "script_game_object.h"

#include "xrServer_Objects_ALife.h"

void CScriptBinderObject::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptBinderObject>(
        "object_binder", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptBinderObject(CScriptGameObject*)>(), "object",
        sol::readonly(&CScriptBinderObject::m_object), "reinit", &CScriptBinderObject::reinit, "reload", &CScriptBinderObject::reload, "net_spawn", &CScriptBinderObject::net_Spawn,
        "net_destroy", &CScriptBinderObject::net_Destroy, "update", &CScriptBinderObject::shedule_Update, "save", &CScriptBinderObject::save, "load", &CScriptBinderObject::load,
        "net_save_relevant", &CScriptBinderObject::net_SaveRelevant, "net_Relcase", &CScriptBinderObject::net_Relcase);
}
