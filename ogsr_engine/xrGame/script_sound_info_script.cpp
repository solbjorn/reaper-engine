#include "stdafx.h"
#include "script_sound_info.h"
#include "script_game_object.h"

void CScriptSoundInfo::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScriptSoundInfo>("SoundInfo", sol::no_constructor, "who", &CScriptSoundInfo::who, "danger", &CScriptSoundInfo::dangerous, "position",
                                       &CScriptSoundInfo::position, "power", &CScriptSoundInfo::power, "time", &CScriptSoundInfo::time);
}
