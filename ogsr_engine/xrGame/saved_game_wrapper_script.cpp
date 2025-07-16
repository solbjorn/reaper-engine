////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper_script.cpp
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"

#include "saved_game_wrapper.h"
#include "ai_space.h"
#include "game_graph.h"
#include "xr_time.h"

static xrTime CSavedGameWrapper__game_time(const CSavedGameWrapper* self) { return xrTime(self->game_time()); }

void CSavedGameWrapper::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CSavedGameWrapper>("CSavedGameWrapper", sol::no_constructor, sol::call_constructor, sol::constructors<CSavedGameWrapper(LPCSTR)>(), "game_time",
                                        &CSavedGameWrapper__game_time, "level_name", &CSavedGameWrapper::level_name, "level_id", &CSavedGameWrapper::level_id, "actor_health",
                                        &CSavedGameWrapper::actor_health);

    lua.set_function("valid_saved_game", sol::resolve<bool(LPCSTR)>(&valid_saved_game));
}
