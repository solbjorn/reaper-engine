#include "stdafx.h"
#include "rgd5.h"

CRGD5::CRGD5(void) { SetSlot(GRENADE_SLOT); }

CRGD5::~CRGD5(void) {}

void CRGD5::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CRGD5>("CRGD5", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CRGD5>), sol::base_classes, xr_sol_bases<CRGD5>());
}
