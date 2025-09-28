#include "stdafx.h"

#include "scope.h"

CScope::CScope() {}
CScope::~CScope() {}

void CScope::script_register(sol::state_view& lua)
{
    lua.new_usertype<CScope>("CScope", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CScope>), "factory", &xr::client_factory<CScope>,
                             sol::base_classes, xr::sol_bases<CScope>());
}
