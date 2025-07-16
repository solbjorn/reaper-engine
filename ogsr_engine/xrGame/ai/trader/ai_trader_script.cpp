#include "stdafx.h"
#include "ai_trader.h"

void CAI_Trader::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CAI_Trader>("CAI_Trader", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CAI_Trader>), sol::base_classes,
                                                xr_sol_bases<CAI_Trader>());
}
