#include "stdafx.h"

#include "../../xrScriptEngine/xr_sol.h"
#include "UIProgressBar.h"

void CUIProgressBar::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CUIProgressBar>("CUIProgressBar", sol::no_constructor, sol::call_constructor, sol::constructors<CUIProgressBar()>(), "SetProgressPos",
                                                    &CUIProgressBar::SetProgressPos, "GetProgressPos", &CUIProgressBar::GetProgressPos, "GetRange_min",
                                                    &CUIProgressBar::GetRange_min, "GetRange_max", &CUIProgressBar::GetRange_max, sol::base_classes,
                                                    xr_sol_bases<CUIProgressBar>());
}
