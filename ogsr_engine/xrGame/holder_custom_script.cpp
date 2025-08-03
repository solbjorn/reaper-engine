#include "stdafx.h"

#include "holder_custom.h"

void CHolderCustom::script_register(sol::state_view& lua)
{
    lua.new_usertype<CHolderCustom>("holder", sol::no_constructor, "engaged", &CHolderCustom::Engaged, "Action", &CHolderCustom::Action, "SetParam",
                                    sol::resolve<void(int, Fvector)>(&CHolderCustom::SetParam));
}
