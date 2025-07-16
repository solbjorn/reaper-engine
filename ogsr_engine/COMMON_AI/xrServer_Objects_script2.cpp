#include "stdafx.h"

#include "../xrScriptEngine/xr_sol.h"
#include "xrServer_Objects.h"

void CSE_PHSkeleton::script_register(lua_State* L) { sol::state_view(L).new_usertype<CSE_PHSkeleton>("cse_ph_skeleton", sol::no_constructor); }
