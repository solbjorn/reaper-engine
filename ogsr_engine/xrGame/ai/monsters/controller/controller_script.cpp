#include "stdafx.h"
#include "controller.h"

void CController::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CController>("CController", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CController>), sol::base_classes,
                                                 xr_sol_bases<CController>());
}
