#include "stdafx.h"

#include "Explosive.h"

void CExplosive::script_register(sol::state_view& lua) { lua.new_usertype<CExplosive>("explosive", sol::no_constructor, "explode", &CExplosive::Explode); }
