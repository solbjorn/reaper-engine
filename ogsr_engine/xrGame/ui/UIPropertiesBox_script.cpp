#include "stdafx.h"

#include "UIPropertiesBox.h"

void CUIPropertiesBox::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUIPropertiesBox>("CUIPropertiesBox", sol::no_constructor, sol::call_constructor, sol::constructors<CUIPropertiesBox()>(), "RemoveItem",
                                       &CUIPropertiesBox::RemoveItemByTAG, "RemoveAll", &CUIPropertiesBox::RemoveAll, "Show", &CUIPropertiesBox::Show, "Hide",
                                       &CUIPropertiesBox::Hide, "AutoUpdateSize", &CUIPropertiesBox::AutoUpdateSize, "AddItem", &CUIPropertiesBox::AddItem_script,
                                       sol::base_classes, xr_sol_bases<CUIPropertiesBox>());
}
