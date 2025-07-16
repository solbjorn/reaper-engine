#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "Explosive.h"

void CExplosive::script_register(lua_State* L) { sol::state_view(L).new_usertype<CExplosive>("explosive", sol::no_constructor, "explode", &CExplosive::Explode); }
