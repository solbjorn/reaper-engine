////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_sound.h"
#include "script_game_object.h"

void CScriptSound::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CSound_params>("sound_params", sol::no_constructor, "position", &CSound_params::position, "volume", &CSound_params::volume, "frequency", &CSound_params::freq,
                                    "min_distance", &CSound_params::min_distance, "max_distance", &CSound_params::max_distance);

    lua.new_usertype<CScriptSound>(
        "sound_object", sol::no_constructor, sol::call_constructor,
        sol::constructors<CScriptSound(LPCSTR), CScriptSound(LPCSTR, ESoundTypes), CScriptSound(LPCSTR, ESoundTypes, esound_type)>(), "looped", sol::var(sm_Looped), "s2d",
        sol::var(sm_2D), "s3d", sol::var(0), "effect", sol::var(st_Effect), "music", sol::var(st_Music), "frequency",
        sol::property(&CScriptSound::GetFrequency, &CScriptSound::SetFrequency), "min_distance", sol::property(&CScriptSound::GetMinDistance, &CScriptSound::SetMinDistance),
        "max_distance", sol::property(&CScriptSound::GetMaxDistance, &CScriptSound::SetMaxDistance), "volume", sol::property(&CScriptSound::GetVolume, &CScriptSound::SetVolume),
        "get_position", &CScriptSound::GetPosition, "set_position", &CScriptSound::SetPosition, "play",
        sol::overload(sol::resolve<void(CScriptGameObject*)>(&CScriptSound::Play), sol::resolve<void(CScriptGameObject*, float)>(&CScriptSound::Play),
                      sol::resolve<void(CScriptGameObject*, float, int)>(&CScriptSound::Play)),
        "play_at_pos",
        sol::overload(sol::resolve<void(CScriptGameObject*, const Fvector&)>(&CScriptSound::PlayAtPos),
                      sol::resolve<void(CScriptGameObject*, const Fvector&, float)>(&CScriptSound::PlayAtPos),
                      sol::resolve<void(CScriptGameObject*, const Fvector&, float, int)>(&CScriptSound::PlayAtPos)),
        "play_no_feedback", &CScriptSound::PlayNoFeedback, "stop", &CScriptSound::Stop, "stop_deffered", &CScriptSound::StopDeffered, "playing", &CScriptSound::IsPlaying, "length",
        &CScriptSound::Length, "set_start_time", &CScriptSound::SetTime);
}
