#include "stdafx.h"
#include "zombie.h"

void CZombie::script_register(sol::state_view& lua)
{
    lua.new_usertype<CZombie>("CZombie", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CZombie>), "factory", &client_factory<CZombie>,
                              sol::base_classes, xr_sol_bases<CZombie>());
}
