#include "stdafx.h"
#include "fracture.h"

void CFracture::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CFracture>("CFracture", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CFracture>), sol::base_classes,
                                               xr_sol_bases<CFracture>());
}
