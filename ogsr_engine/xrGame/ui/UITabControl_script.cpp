#include "stdafx.h"

#include "../../xrScriptEngine/xr_sol.h"

#include "UITabControl.h"
#include "UITabButton.h"

void CUITabControl::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CUITabControl>("CUITabControl", sol::no_constructor, sol::call_constructor, sol::constructors<CUITabControl()>(), "AddItem",
                                    sol::overload(sol::resolve<bool(CUITabButton*)>(&CUITabControl::AddItem),
                                                  sol::resolve<bool(const char*, const char*, float, float, float, float)>(&CUITabControl::AddItem)),
                                    "RemoveItem", &CUITabControl::RemoveItem, "RemoveAll", &CUITabControl::RemoveAll, "GetActiveIndex", &CUITabControl::GetActiveIndex,
                                    "GetTabsCount", &CUITabControl::GetTabsCount, "SetNewActiveTab", &CUITabControl::SetNewActiveTab, "GetButtonByIndex",
                                    &CUITabControl::GetButtonByIndex, sol::base_classes, xr_sol_bases<CUITabControl>());

    lua.new_usertype<CUITabButton>("CUITabButton", sol::no_constructor, sol::call_constructor, sol::constructors<CUITabButton()>(), sol::base_classes,
                                   xr_sol_bases<CUITabButton>());
}
