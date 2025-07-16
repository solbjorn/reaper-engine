#include "stdafx.h"
#include "scope.h"

CScope::CScope() {}

CScope::~CScope() {}

void CScope::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CScope>("CScope", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CScope>), sol::base_classes,
                                            xr_sol_bases<CScope>());
}
