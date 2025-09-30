#include "stdafx.h"

#include "rgd5.h"

CRGD5::CRGD5() { SetSlot(GRENADE_SLOT); }
CRGD5::~CRGD5() = default;

void CRGD5::script_register(sol::state_view& lua)
{
    lua.new_usertype<CRGD5>("CRGD5", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CRGD5>), sol::base_classes, xr::sol_bases<CRGD5>());
}
