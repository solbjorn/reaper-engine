#include "stdafx.h"

#include "xrServer_Objects.h"

void CSE_PHSkeleton::script_register(sol::state_view& lua) { lua.new_usertype<CSE_PHSkeleton>("cse_ph_skeleton", sol::no_constructor); }
