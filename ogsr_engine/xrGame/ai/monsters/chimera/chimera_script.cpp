#include "stdafx.h"
#include "chimera.h"

void CChimera::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CChimera>("CChimera", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CChimera>), sol::base_classes,
                                              xr_sol_bases<CChimera>());
}
