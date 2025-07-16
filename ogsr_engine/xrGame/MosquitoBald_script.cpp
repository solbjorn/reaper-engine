#include "stdafx.h"
#include "MosquitoBald.h"

void CMosquitoBald::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CMosquitoBald>("CMosquitoBald", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CMosquitoBald>), sol::base_classes,
                                                   xr_sol_bases<CMosquitoBald>());
}
