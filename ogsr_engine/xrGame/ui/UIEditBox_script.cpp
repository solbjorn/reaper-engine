#include "stdafx.h"

#include "UIEditBox.h"
#include "UIEditBoxEx.h"

void CUIEditBox::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUICustomEdit>("CUICustomEdit", sol::no_constructor, "SetText", &CUICustomEdit::SetText, "GetText", &CUICustomEdit::GetText, "SetTextColor",
                                    &CUICustomEdit::SetTextColor, "GetTextColor", &CUICustomEdit::GetTextColor, "SetFont", &CUICustomEdit::SetFont, "GetFont",
                                    &CUICustomEdit::GetFont, "SetTextAlignment", &CUICustomEdit::SetTextAlignment, "GetTextAlignment", &CUICustomEdit::GetTextAlignment,
                                    "SetTextPosX", &CUICustomEdit::SetTextPosX, "SetTextPosY", &CUICustomEdit::SetTextPosY, "SetNumbersOnly", &CUICustomEdit::SetNumbersOnly,
                                    sol::base_classes, xr::sol_bases<CUICustomEdit>());

    lua.new_usertype<CUIEditBox>("CUIEditBox", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CUIEditBox>), "factory", &xr::ui_factory<CUIEditBox>,
                                 "InitTexture", &CUIEditBox::InitTexture, sol::base_classes, xr::sol_bases<CUIEditBox>());

    lua.new_usertype<CUIEditBoxEx>("CUIEditBoxEx", sol::no_constructor, sol::call_constructor, sol::constructors<CUIEditBoxEx()>(), "InitTexture", &CUIEditBoxEx::InitTexture,
                                   sol::base_classes, xr::sol_bases<CUIEditBoxEx>());
}
