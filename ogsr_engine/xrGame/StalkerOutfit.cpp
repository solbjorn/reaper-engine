#include "stdafx.h"
#include "StalkerOutfit.h"

CStalkerOutfit::CStalkerOutfit() {}

CStalkerOutfit::~CStalkerOutfit() {}

void CStalkerOutfit::script_register(sol::state_view& lua)
{
    lua.new_usertype<CStalkerOutfit>("CStalkerOutfit", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CStalkerOutfit>), "factory",
                                     &client_factory<CStalkerOutfit>, sol::base_classes, xr_sol_bases<CStalkerOutfit>());
}
