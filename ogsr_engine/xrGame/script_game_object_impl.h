////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_impl.h
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"

#include "ai_space.h"
#include "script_engine.h"

IC CGameObject& CScriptGameObject::object() const
{
    XR_ASSERT(m_game_object != nullptr && m_game_object->lua_game_object() == this, "trying to use a destroyed object");
    return *m_game_object;
}
