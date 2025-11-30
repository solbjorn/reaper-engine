#include "stdafx.h"

#include "UITabControl.h"

#include "UITabButton.h"

namespace
{
[[nodiscard]] bool AddItem_script(CUITabControl& self, std::unique_ptr<CUITabButton>& button) { return self.AddItem(button.release()); }
} // namespace

void CUITabControl::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUITabControl>("CUITabControl", sol::no_constructor, sol::call_constructor, sol::constructors<CUITabControl()>(), "AddItem",
                                    sol::overload(&AddItem_script, sol::resolve<bool(const char*, const char*, float, float, float, float)>(&CUITabControl::AddItem)), "RemoveItem",
                                    &CUITabControl::RemoveItem, "RemoveAll", &CUITabControl::RemoveAll, "GetActiveIndex", &CUITabControl::GetActiveIndex, "GetTabsCount",
                                    &CUITabControl::GetTabsCount, "SetNewActiveTab", &CUITabControl::SetNewActiveTab, "GetButtonByIndex", &CUITabControl::GetButtonByIndex,
                                    sol::base_classes, xr::sol_bases<CUITabControl>());

    lua.new_usertype<CUITabButton>("CUITabButton", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CUITabButton>), sol::base_classes,
                                   xr::sol_bases<CUITabButton>());
}
