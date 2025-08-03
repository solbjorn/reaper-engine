#include "stdafx.h"

#include "UIMessageBox.h"
#include "UIMessageBoxEx.h"

void CUIMessageBox::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUIMessageBox>("CUIMessageBox", sol::no_constructor, sol::call_constructor, sol::constructors<CUIMessageBox()>(), "Init", &CUIMessageBox::Init, "SetText",
                                    &CUIMessageBox::SetText, "GetHost", &CUIMessageBox::GetHost, "GetPassword", &CUIMessageBox::GetPassword, sol::base_classes,
                                    xr_sol_bases<CUIMessageBox>());

    lua.new_usertype<CUIMessageBoxEx>("CUIMessageBoxEx", sol::no_constructor, sol::call_constructor, sol::constructors<CUIMessageBoxEx()>(), "Init", &CUIMessageBoxEx::Init,
                                      "SetText", &CUIMessageBoxEx::SetText, "GetHost", &CUIMessageBoxEx::GetHost, "GetPassword", &CUIMessageBoxEx::GetPassword, sol::base_classes,
                                      xr_sol_bases<CUIMessageBoxEx>());
}
