#include "stdafx.h"

#include "pseudo_gigant.h"

void CPseudoGigant::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPseudoGigant>("CPseudoGigant", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPseudoGigant>), "factory",
                                    &xr::client_factory<CPseudoGigant>, sol::base_classes, xr::sol_bases<CPseudoGigant>());
}
