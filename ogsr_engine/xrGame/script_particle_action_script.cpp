////////////////////////////////////////////////////////////////////////////
//	Module 		: script_particle_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script particle action class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "script_particle_action.h"

void CScriptParticleAction::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptParticleAction>(
        "particle", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptParticleAction(), CScriptParticleAction(LPCSTR, LPCSTR), CScriptParticleAction(LPCSTR, LPCSTR, const CParticleParams&),
                          CScriptParticleAction(LPCSTR, LPCSTR, const CParticleParams&, bool), CScriptParticleAction(LPCSTR, const CParticleParams&),
                          CScriptParticleAction(LPCSTR, const CParticleParams&, bool)>(),

        "set_particle", &CScriptParticleAction::SetParticle, "set_bone", &CScriptParticleAction::SetBone, "set_position", &CScriptParticleAction::SetPosition, "set_angles",
        &CScriptParticleAction::SetAngles, "set_velocity", &CScriptParticleAction::SetVelocity, "completed", sol::resolve<bool()>(&CScriptParticleAction::completed));
}
