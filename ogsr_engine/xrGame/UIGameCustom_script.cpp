#include "stdafx.h"

#include "UIGameCustom.h"

#include "level.h"
#include "hudmanager.h"
#include "ui/uistatic.h"

namespace
{
SDrawStaticStruct* AddCustomStatic(CUIGameCustom& self, gsl::czstring id) { return self.AddCustomStatic(id, false); }
} // namespace

void CUIGameCustom::script_register(sol::state_view& lua)
{
    lua.new_usertype<SDrawStaticStruct>("SDrawStaticStruct", sol::no_constructor, "m_endTime", &SDrawStaticStruct::m_endTime, "wnd", &SDrawStaticStruct::wnd);

    lua.new_usertype<CUIGameCustom>("CUIGameCustom", sol::no_constructor, "AddDialogToRender", &CUIGameCustom::AddDialogToRender, "RemoveDialogToRender",
                                    &CUIGameCustom::RemoveDialogToRender, "AddCustomMessage",
                                    sol::overload(sol::resolve<void(LPCSTR, float, float, float, CGameFont*, u16, u32, float)>(&CUIGameCustom::AddCustomMessage),
                                                  sol::resolve<void(LPCSTR, float, float, float, CGameFont*, u16, u32)>(&CUIGameCustom::AddCustomMessage)),
                                    "CustomMessageOut", &CUIGameCustom::CustomMessageOut, "RemoveCustomMessage", &CUIGameCustom::RemoveCustomMessage, "AddCustomStatic",
                                    sol::overload(&CUIGameCustom::AddCustomStatic, &::AddCustomStatic), "RemoveCustomStatic", &CUIGameCustom::RemoveCustomStatic, "GetCustomStatic",
                                    &CUIGameCustom::GetCustomStatic);

    lua.set_function("get_hud", [] { return HUD().GetUI()->UIGame(); });
}
