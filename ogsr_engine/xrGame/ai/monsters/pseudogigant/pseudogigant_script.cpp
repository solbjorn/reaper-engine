#include "stdafx.h"
#include "pseudo_gigant.h"

void CPseudoGigant::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPseudoGigant>("CPseudoGigant", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPseudoGigant>), sol::base_classes,
                                                   xr_sol_bases<CPseudoGigant>());
}
