#include "stdafx.h"

#include "MosquitoBald.h"

void CMosquitoBald::script_register(sol::state_view& lua)
{
    lua.new_usertype<CMosquitoBald>("CMosquitoBald", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CMosquitoBald>), "factory",
                                    &xr::client_factory<CMosquitoBald>, sol::base_classes, xr::sol_bases<CMosquitoBald>());
}
