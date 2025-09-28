#include "stdafx.h"

#include "controller.h"

void CController::script_register(sol::state_view& lua)
{
    lua.new_usertype<CController>("CController", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CController>), "factory",
                                  &xr::client_factory<CController>, sol::base_classes, xr::sol_bases<CController>());
}
