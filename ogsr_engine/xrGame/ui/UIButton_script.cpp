#include "stdafx.h"

#include "../../xrScriptEngine/xr_sol.h"

#include "UIButton.h"
#include "UI3tButton.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UISpinNum.h"
#include "UISpinText.h"
#include "UITrackBar.h"

void CUIButton::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.new_usertype<CUIButton>(
        "CUIButton", sol::no_constructor, sol::call_constructor, sol::constructors<CUIButton()>(), "Init",
        sol::overload(sol::resolve<void(float, float, float, float)>(&CUIButton::Init), sol::resolve<void(LPCSTR, float, float, float, float)>(&CUIButton::Init)),
        "SetHighlightColor", &CUIButton::SetHighlightColor, "EnableTextHighlighting", &CUIButton::EnableTextHighlighting, "SetAccelerator", &CUIButton::SetAccelerator,
        sol::base_classes, xr_sol_bases<CUIButton>());

    lua.new_usertype<CUI3tButton>("CUI3tButton", sol::no_constructor, sol::call_constructor, sol::constructors<CUI3tButton()>(), sol::base_classes, xr_sol_bases<CUI3tButton>());

    lua.new_usertype<CUICheckButton>("CUICheckButton", sol::no_constructor, sol::call_constructor, sol::constructors<CUICheckButton()>(), "GetCheck", &CUICheckButton::GetCheck,
                                     "SetCheck", &CUICheckButton::SetCheck, "SetDependControl", &CUICheckButton::SetDependControl, sol::base_classes,
                                     xr_sol_bases<CUICheckButton>());

    lua.new_usertype<CUICustomSpin>("CUICustomSpin", sol::no_constructor, "Init", &CUICustomSpin::Init, "GetText", &CUICustomSpin::GetText, sol::base_classes,
                                    xr_sol_bases<CUICustomSpin>());

    lua.new_usertype<CUISpinNum>("CUISpinNum", sol::no_constructor, sol::call_constructor, sol::constructors<CUISpinNum()>(), sol::base_classes, xr_sol_bases<CUISpinNum>());
    lua.new_usertype<CUISpinFlt>("CUISpinFlt", sol::no_constructor, sol::call_constructor, sol::constructors<CUISpinFlt()>(), sol::base_classes, xr_sol_bases<CUISpinFlt>());
    lua.new_usertype<CUISpinText>("CUISpinText", sol::no_constructor, sol::call_constructor, sol::constructors<CUISpinText()>(), sol::base_classes, xr_sol_bases<CUISpinText>());

    lua.new_usertype<CUITrackBar>("CUITrackBar", sol::no_constructor, sol::call_constructor, sol::constructors<CUITrackBar()>(), "GetCheck", &CUITrackBar::GetCheck, "SetCheck",
                                  &CUITrackBar::SetCheck, "GetTrackValue", &CUITrackBar::GetTrackValue, sol::base_classes, xr_sol_bases<CUITrackBar>());
}
