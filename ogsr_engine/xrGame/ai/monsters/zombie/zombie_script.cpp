#include "stdafx.h"
#include "zombie.h"

void CZombie::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CZombie>("CZombie", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CZombie>), sol::base_classes,
                                             xr_sol_bases<CZombie>());
}
