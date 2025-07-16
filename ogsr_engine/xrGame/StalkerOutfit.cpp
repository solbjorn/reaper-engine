#include "stdafx.h"
#include "StalkerOutfit.h"

CStalkerOutfit::CStalkerOutfit() {}

CStalkerOutfit::~CStalkerOutfit() {}

void CStalkerOutfit::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CStalkerOutfit>("CStalkerOutfit", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CStalkerOutfit>),
                                                    sol::base_classes, xr_sol_bases<CStalkerOutfit>());
}
