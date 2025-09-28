#include "stdafx.h"

#include "pseudodog.h"
#include "psy_dog.h"

void CAI_PseudoDog::script_register(sol::state_view& lua)
{
    lua.new_usertype<CAI_PseudoDog>("CAI_PseudoDog", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_PseudoDog>), "factory",
                                    &xr::client_factory<CAI_PseudoDog>, sol::base_classes, xr::sol_bases<CAI_PseudoDog>());
}

void CPsyDog::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPsyDog>("CPsyDog", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPsyDog>), "factory", &xr::client_factory<CPsyDog>,
                              sol::base_classes, xr::sol_bases<CPsyDog>());
}

void CPsyDogPhantom::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPsyDogPhantom>("CPsyDogPhantom", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPsyDogPhantom>), "factory",
                                     &xr::client_factory<CPsyDogPhantom>, sol::base_classes, xr::sol_bases<CPsyDogPhantom>());
}
