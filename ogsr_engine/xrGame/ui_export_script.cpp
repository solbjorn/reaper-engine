#include "stdafx.h"

#include "script_ui_registrator.h"
#include "UI\UIMultiTextStatic.h"
#include "MainMenu.h"

CMainMenu* MainMenu();

void UIRegistrator::script_register(sol::state_view& lua)
{
    lua.new_usertype<CGameFont>("CGameFont", sol::no_constructor,
                                // EAligment
                                "alLeft", sol::var(CGameFont::alLeft), "alRight", sol::var(CGameFont::alRight), "alCenter", sol::var(CGameFont::alCenter), "alJustified",
                                sol::var(CGameFont::alJustified),
                                // EVAlignment
                                "valTop", sol::var(EVTextAlignment::valTop), "valCenter", sol::var(EVTextAlignment::valCenter), "valBotton", sol::var(EVTextAlignment::valBotton),

                                "SizeOf", sol::resolve<float(LPCSTR)>(&CGameFont::SizeOf_), "CurrentHeight", &CGameFont::CurrentHeight_);

    lua.new_usertype<CMainMenu>("CMainMenu", sol::no_constructor, "GetGSVer", &CMainMenu::GetGSVer, "PlaySound", &CMainMenu::PlaySound, "IsActive", &CMainMenu::IsActive,
                                sol::base_classes, xr_sol_bases<CMainMenu>());

    lua.create_named_table("main_menu", "get_main_menu", &MainMenu);
}
