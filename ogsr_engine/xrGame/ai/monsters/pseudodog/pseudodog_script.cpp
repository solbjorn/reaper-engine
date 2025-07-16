#include "stdafx.h"
#include "pseudodog.h"
#include "psy_dog.h"

void CAI_PseudoDog::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CAI_PseudoDog>("CAI_PseudoDog", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_PseudoDog>), sol::base_classes,
                                                   xr_sol_bases<CAI_PseudoDog>());
}

void CPsyDog::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPsyDog>("CPsyDog", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPsyDog>), sol::base_classes,
                                             xr_sol_bases<CPsyDog>());
}

void CPsyDogPhantom::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CPsyDogPhantom>("CPsyDogPhantom", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CPsyDogPhantom>),
                                                    sol::base_classes, xr_sol_bases<CPsyDogPhantom>());
}
