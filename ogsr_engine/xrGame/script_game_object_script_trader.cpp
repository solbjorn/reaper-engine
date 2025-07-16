////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script3.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"

void CScriptGameObject::script_register_trader(CScriptGameObject::usertype& lua)
{
    xr_sol_set(lua, "set_trader_global_anim", &CScriptGameObject::set_trader_global_anim, "set_trader_head_anim", &CScriptGameObject::set_trader_head_anim, "set_trader_sound",
               &CScriptGameObject::set_trader_sound, "external_sound_start", &CScriptGameObject::external_sound_start, "external_sound_stop",
               &CScriptGameObject::external_sound_stop);
}
