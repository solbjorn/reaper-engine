////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "script_particles.h"

void CScriptParticles::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScriptParticles>("particles_object", sol::no_constructor, sol::call_constructor, sol::constructors<CScriptParticles(LPCSTR)>(), "play",
                                                      &CScriptParticles::Play, "play_at_pos", &CScriptParticles::PlayAtPos, "stop", &CScriptParticles::Stop, "stop_deffered",
                                                      &CScriptParticles::StopDeffered, "playing", &CScriptParticles::IsPlaying, "looped", &CScriptParticles::IsLooped, "move_to",
                                                      &CScriptParticles::MoveTo, "set_position", &CScriptParticles::XFORMMoveTo, "set_direction", &CScriptParticles::SetDirection,
                                                      "set_orientation", &CScriptParticles::SetOrientation, "last_position", &CScriptParticles::LastPosition, "load_path",
                                                      &CScriptParticles::LoadPath, "start_path", &CScriptParticles::StartPath, "stop_path", &CScriptParticles::StopPath,
                                                      "pause_path", &CScriptParticles::PausePath, "life_time", &CScriptParticles::LifeTime, "length", &CScriptParticles::Length);
}
