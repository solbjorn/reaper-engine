// File:        UIComboBox_script.cpp
// Description: exports CUIComobBox to LUA environment
// Created:     11.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#include "stdafx.h"
#include "UIComboBox.h"

void CUIComboBox::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CUIComboBox>(
        "CUIComboBox", sol::no_constructor, sol::call_constructor, sol::constructors<CUIComboBox()>(), "Init",
        sol::overload(sol::resolve<void(float, float, float)>(&CUIComboBox::Init), sol::resolve<void(float, float, float, float)>(&CUIComboBox::Init)), "SetVertScroll",
        &CUIComboBox::SetVertScroll, "SetListLength", &CUIComboBox::SetListLength, "CurrentID", &CUIComboBox::CurrentID, "SetCurrentID", &CUIComboBox::SetItem, "AddItem",
        sol::resolve<CUIListBoxItem*(LPCSTR)>(&CUIComboBox::AddItem_), "GetText", &CUIComboBox::GetText, sol::base_classes, xr_sol_bases<CUIComboBox>());
}
