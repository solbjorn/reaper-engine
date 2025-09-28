#include "stdafx.h"

// UI-controls
#include "UIScriptWnd.h"
#include "UIButton.h"
#include "UIMessageBox.h"
#include "UIPropertiesBox.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UIStatic.h"
#include "UIEditBox.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIListWnd.h"
#include "UIProgressBar.h"
#include "UITabControl.h"

template <typename T>
IC T* CUIDialogWndEx::GetControl(LPCSTR name)
{
    shared_str n = name;
    CUIWindow* pWnd = FindChild(n);
    if (!pWnd)
        return nullptr;

    return smart_cast<T*>(pWnd);
}

void CUIDialogWndEx::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUIDialogWndEx>(
        "CUIScriptWnd", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CUIDialogWndEx>), "factory", &xr::client_factory<CUIDialogWndEx>, "priv",
        &CUIDialogWndEx::priv, "ops", &CUIDialogWndEx::ops, "ON_KEYBOARD", sol::var(CUIDialogWndEx::wnd_ops::ON_KEYBOARD), "AddCallback", &CUIDialogWndEx::AddCallback,
        "ClearCallbacks", &CUIDialogWndEx::ClearCallbacks, "Register",
        sol::overload(sol::resolve<void(CUIWindow*)>(&CUIDialogWndEx::Register), sol::resolve<void(CUIWindow*, LPCSTR)>(&CUIDialogWndEx::Register)), "GetButton",
        sol::resolve<CUIButton*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIButton>), "GetMessageBox", sol::resolve<CUIMessageBox*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIMessageBox>),
        "GetPropertiesBox", sol::resolve<CUIPropertiesBox*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIPropertiesBox>), "GetCheckButton",
        sol::resolve<CUICheckButton*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUICheckButton>), "GetRadioButton",
        sol::resolve<CUIRadioButton*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIRadioButton>), "GetStatic", sol::resolve<CUIStatic*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIStatic>),
        "GetEditBox", sol::resolve<CUIEditBox*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIEditBox>), "GetDialogWnd",
        sol::resolve<CUIDialogWnd*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIDialogWnd>), "GetFrameWindow",
        sol::resolve<CUIFrameWindow*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIFrameWindow>), "GetFrameLineWnd",
        sol::resolve<CUIFrameLineWnd*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIFrameLineWnd>), "GetProgressBar",
        sol::resolve<CUIProgressBar*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIProgressBar>), "GetTabControl",
        sol::resolve<CUITabControl*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUITabControl>), "GetListWnd", sol::resolve<CUIListWnd*(LPCSTR)>(&CUIDialogWndEx::GetControl<CUIListWnd>),
        "Load", &CUIDialogWndEx::Load, sol::base_classes, xr::sol_bases<CUIDialogWndEx>());
}
