#include "stdafx.h"

#include "../../xrScriptEngine/xr_sol.h"

#include "UIButton.h"
#include "UIWindow.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIDialogWnd.h"
#include "../HUDManager.h"
#include "../GamePersistent.h"
#include "UILabel.h"
#include "UIMMShniaga.h"
#include "UITextureMaster.h"
#include "UIScrollView.h"
#include "UIIconParams.h"

static CFontManager& mngr() { return *(UI()->Font()); }

// hud font
static CGameFont* GetFontSmall() { return mngr().pFontSmall; }
static CGameFont* GetFontMedium() { return mngr().pFontMedium; }

static CGameFont* GetFontDI() { return mngr().pFontDI; }

// шрифты для интерфейса
static CGameFont* GetFontGraffiti19Russian() { return mngr().pFontGraffiti19Russian; }
static CGameFont* GetFontGraffiti22Russian() { return mngr().pFontGraffiti22Russian; }

static CGameFont* GetFontLetterica16Russian() { return mngr().pFontLetterica16Russian; }
static CGameFont* GetFontLetterica18Russian() { return mngr().pFontLetterica18Russian; }

static CGameFont* GetFontGraffiti32Russian() { return mngr().pFontGraffiti32Russian; }
static CGameFont* GetFontGraffiti40Russian() { return mngr().pFontGraffiti40Russian; }
static CGameFont* GetFontGraffiti50Russian() { return mngr().pFontGraffiti50Russian; }

static CGameFont* GetFontLetterica25() { return mngr().pFontLetterica25; }

static CGameFont* GetFontArial14() { return mngr().pFontArial14; }
static CGameFont* GetFontArial21() { return mngr().pFontArial21; }

static CGameFont* GetFontCustom(LPCSTR section) { return mngr().InitializeCustomFont(section); }

static int GetARGB(u16 a, u16 r, u16 g, u16 b) { return color_argb(a, r, g, b); }

static Frect get_texture_rect(LPCSTR icon_name) { return CUITextureMaster::GetTextureRect(icon_name); }

static LPCSTR get_texture_name(LPCSTR icon_name) { return CUITextureMaster::GetTextureFileName(icon_name); }

static TEX_INFO get_texture_info(LPCSTR name, LPCSTR def_name) { return CUITextureMaster::FindItem(name, def_name); }

static LPCSTR CIconParams__get_name(CIconParams* self) { return self->name.c_str(); }

template <typename T>
static T* wnd_object_cast(CUIWindow* wnd)
{
    return smart_cast<T*>(wnd);
}

void CUIWindow::script_register(lua_State* L)
{
    auto lua = sol::state_view(L);

    lua.set_function("GetARGB", &GetARGB);
    lua.set_function("GetFontSmall", &GetFontSmall);
    lua.set_function("GetFontMedium", &GetFontMedium);
    lua.set_function("GetFontDI", &GetFontDI);
    lua.set_function("GetFontGraffiti19Russian", &GetFontGraffiti19Russian);
    lua.set_function("GetFontGraffiti22Russian", &GetFontGraffiti22Russian);
    lua.set_function("GetFontLetterica16Russian", &GetFontLetterica16Russian);
    lua.set_function("GetFontLetterica18Russian", &GetFontLetterica18Russian);
    lua.set_function("GetFontGraffiti32Russian", &GetFontGraffiti32Russian);
    lua.set_function("GetFontGraffiti40Russian", &GetFontGraffiti40Russian);
    lua.set_function("GetFontGraffiti50Russian", &GetFontGraffiti50Russian);
    lua.set_function("GetFontArial14", &GetFontArial14);
    lua.set_function("GetFontArial21", &GetFontArial21);
    lua.set_function("GetFontLetterica25", &GetFontLetterica25);
    lua.set_function("GetFontCustom", &GetFontCustom);

    lua.new_usertype<TEX_INFO>("TEX_INFO", sol::no_constructor, "get_file_name", &TEX_INFO::get_file_name, "get_rect", &TEX_INFO::get_rect);

    lua.set_function("GetTextureName", &get_texture_name);
    lua.set_function("GetTextureRect", &get_texture_rect);
    lua.set_function("GetTextureInfo", &get_texture_info);

    lua.new_usertype<CUIWindow>(
        "CUIWindow", sol::no_constructor, sol::call_constructor, sol::constructors<CUIWindow()>(), "AttachChild", &CUIWindow::AttachChild, "DetachChild", &CUIWindow::DetachChild,
        "DetachAll", &CUIWindow::DetachAll, "SetAutoDelete", &CUIWindow::SetAutoDelete, "IsAutoDelete", &CUIWindow::IsAutoDelete, "SetWndRect",
        sol::overload(sol::resolve<void(Frect)>(&CUIWindow::SetWndRect_script), sol::resolve<void(float, float, float, float)>(&CUIWindow::SetWndRect_script)), "Init",
        sol::overload(sol::resolve<void(float, float, float, float)>(&CUIWindow::Init), sol::resolve<void(Frect*)>(&CUIWindow::Init)), "GetWndPos", &CUIWindow::GetWndPos,
        "SetWndPos", sol::resolve<void(float, float)>(&CUIWindow::SetWndPos), "SetWndSize", [](CUIWindow& self, float w, float h) -> void { self.SetWndSize({w, h}); }, "GetWidth",
        &CUIWindow::GetWidth, "SetWidth", &CUIWindow::SetWidth, "GetHeight", &CUIWindow::GetHeight, "SetHeight", &CUIWindow::SetHeight, "GetPosTop", &CUIWindow::GetPosTop,
        "GetPosLeft", &CUIWindow::GetPosLeft, "Enable", &CUIWindow::Enable, "IsEnabled", &CUIWindow::IsEnabled, "Show", &CUIWindow::Show, "IsShown", &CUIWindow::IsShown, "SetFont",
        &CUIWindow::SetFont, "GetFont", &CUIWindow::GetFont, "DetachFromParent", &CUIWindow::DetachFromParent, "WindowName", &CUIWindow::WindowName_script, "SetWindowName",
        &CUIWindow::SetWindowName, "SetPPMode", &CUIWindow::SetPPMode, "ResetPPMode", &CUIWindow::ResetPPMode, "GetMousePosX", &CUIWindow::GetMousePosX, "GetMousePosY",
        &CUIWindow::GetMousePosY, "GetParent", &CUIWindow::GetParent, "GetWndRect", sol::resolve<void(Frect&)>(&CUIWindow::GetWndRect_script), "IsChild", &CUIWindow::IsChild,
        "FindChild", sol::resolve<CUIWindow*(LPCSTR)>(&CUIWindow::FindChild), "GetButton", &wnd_object_cast<CUIButton>, "GetCUIStatic", &wnd_object_cast<CUIStatic>,
        "GetAbsoluteRect", sol::resolve<void(Frect&)>(&CUIWindow::GetAbsoluteRect), sol::base_classes, xr_sol_bases<CUIWindow>());

    lua.new_usertype<CDialogHolder>("CDialogHolder", sol::no_constructor, "MainInputReceiver", &CDialogHolder::MainInputReceiver, "start_stop_menu", &CDialogHolder::StartStopMenu,
                                    "AddDialogToRender", &CDialogHolder::AddDialogToRender, "RemoveDialogToRender", &CDialogHolder::RemoveDialogToRender, sol::base_classes,
                                    xr_sol_bases<CDialogHolder>());

    lua.new_usertype<CUIDialogWnd>("CUIDialogWnd", sol::no_constructor, "GetHolder", &CUIDialogWnd::GetHolder, "SetHolder", &CUIDialogWnd::SetHolder, sol::base_classes,
                                   xr_sol_bases<CUIDialogWnd>());

    lua.new_usertype<CUIFrameWindow>("CUIFrameWindow", sol::no_constructor, sol::call_constructor, sol::constructors<CUIFrameWindow()>(), "SetWidth", &CUIFrameWindow::SetWidth,
                                     "SetHeight", &CUIFrameWindow::SetHeight, "SetColor", &CUIFrameWindow::SetColor, "GetTitleStatic", &CUIFrameWindow::GetTitleStatic, "Init",
                                     sol::resolve<void(LPCSTR, float, float, float, float)>(&CUIFrameWindow::Init), sol::base_classes, xr_sol_bases<CUIFrameWindow>());

    lua.new_usertype<CUIFrameLineWnd>("CUIFrameLineWnd", sol::no_constructor, sol::call_constructor, sol::constructors<CUIFrameLineWnd()>(), "SetWidth", &CUIFrameLineWnd::SetWidth,
                                      "SetHeight", &CUIFrameLineWnd::SetHeight, "SetOrientation", &CUIFrameLineWnd::SetOrientation, "SetColor", &CUIFrameLineWnd::SetColor,
                                      "GetTitleStatic", &CUIFrameLineWnd::GetTitleStatic, "Init",
                                      sol::resolve<void(LPCSTR, float, float, float, float, bool)>(&CUIFrameLineWnd::Init), sol::base_classes, xr_sol_bases<CUIFrameLineWnd>());

    lua.new_usertype<CUILabel>("CUILabel", sol::no_constructor, sol::call_constructor, sol::constructors<CUILabel()>(), "SetText", &CUILabel::SetText, "GetText",
                               &CUILabel::GetText, sol::base_classes, xr_sol_bases<CUILabel>());

    lua.new_usertype<CUIMMShniaga>("CUIMMShniaga", sol::no_constructor, "SetVisibleMagnifier", &CUIMMShniaga::SetVisibleMagnifier, sol::base_classes, xr_sol_bases<CUIMMShniaga>());

    lua.new_usertype<CUIScrollView>("CUIScrollView", sol::no_constructor, sol::call_constructor, sol::constructors<CUIScrollView()>(), "AddWindow", &CUIScrollView::AddWindow,
                                    "RemoveWindow", &CUIScrollView::RemoveWindow, "Clear", &CUIScrollView::Clear, "ScrollToBegin", &CUIScrollView::ScrollToBegin, "ScrollToEnd",
                                    &CUIScrollView::ScrollToEnd, "GetMinScrollPos", &CUIScrollView::GetMinScrollPos, "GetMaxScrollPos", &CUIScrollView::GetMaxScrollPos,
                                    "GetCurrentScrollPos", &CUIScrollView::GetCurrentScrollPos, "SetScrollPos", &CUIScrollView::SetScrollPos, "ForceUpdate",
                                    &CUIScrollView::ForceUpdate, sol::base_classes, xr_sol_bases<CUIScrollView>());

    lua.new_usertype<CIconParams>("CIconParams", sol::no_constructor, sol::call_constructor, sol::constructors<CIconParams(LPCSTR)>(), "icon_group",
                                  sol::readonly(&CIconParams::icon_group), "grid_width", sol::readonly(&CIconParams::grid_width), "grid_height",
                                  sol::readonly(&CIconParams::grid_height), "grid_x", sol::readonly(&CIconParams::grid_x), "grid_y", sol::readonly(&CIconParams::grid_y),
                                  "icon_name", sol::property(&CIconParams__get_name), "original_rect", &CIconParams::original_rect, "set_shader",
                                  sol::resolve<void(CUIStatic*)>(&CIconParams::set_shader));

    lua.new_enum("ui_events",
                 // CUIWindow
                 "WINDOW_LBUTTON_DOWN", WINDOW_LBUTTON_DOWN, "WINDOW_RBUTTON_DOWN", WINDOW_RBUTTON_DOWN, "WINDOW_LBUTTON_UP", WINDOW_LBUTTON_UP, "WINDOW_RBUTTON_UP",
                 WINDOW_RBUTTON_UP, "WINDOW_MOUSE_MOVE", WINDOW_MOUSE_MOVE, "WINDOW_LBUTTON_DB_CLICK", WINDOW_LBUTTON_DB_CLICK, "WINDOW_KEY_PRESSED", WINDOW_KEY_PRESSED,
                 "WINDOW_KEY_RELEASED", WINDOW_KEY_RELEASED, "WINDOW_MOUSE_CAPTURE_LOST", WINDOW_MOUSE_CAPTURE_LOST, "WINDOW_KEYBOARD_CAPTURE_LOST", WINDOW_KEYBOARD_CAPTURE_LOST,

                 // CUIStatic
                 "STATIC_FOCUS_RECEIVED", STATIC_FOCUS_RECEIVED, "STATIC_FOCUS_LOST", STATIC_FOCUS_LOST,

                 // CUIButton
                 "BUTTON_CLICKED", BUTTON_CLICKED, "BUTTON_DOWN", BUTTON_DOWN,

                 // CUITabControl
                 "TAB_CHANGED", TAB_CHANGED,

                 // CUICustomEdit
                 "EDIT_TEXT_CHANGED", EDIT_TEXT_CHANGED, "EDIT_TEXT_COMMIT", EDIT_TEXT_COMMIT,

                 // CUICheckButton
                 "CHECK_BUTTON_SET", CHECK_BUTTON_SET, "CHECK_BUTTON_RESET", CHECK_BUTTON_RESET,

                 // CUITrackBar
                 "TRACKBAR_CHANGED", TRACKBAR_CHANGED,

#pragma todo("KRodin: ивент RADIOBUTTON_SET нигде не вызывается. Надо доделать по необходимости.")
                 // CUIRadioButton
                 "RADIOBUTTON_SET", RADIOBUTTON_SET,

                 // CUIdragDropItem
                 "DRAG_DROP_ITEM_DRAG", DRAG_DROP_ITEM_DRAG, "DRAG_DROP_ITEM_DROP", DRAG_DROP_ITEM_DROP, "DRAG_DROP_ITEM_DB_CLICK", DRAG_DROP_ITEM_DB_CLICK,
                 "DRAG_DROP_ITEM_RBUTTON_CLICK", DRAG_DROP_ITEM_RBUTTON_CLICK,

                 // CUIScrollBox
                 "SCROLLBOX_MOVE", SCROLLBOX_MOVE,

                 // CUIScrollBar
                 "SCROLLBAR_VSCROLL", SCROLLBAR_VSCROLL, "SCROLLBAR_HSCROLL", SCROLLBAR_HSCROLL,

                 // CUIListWnd
                 "LIST_ITEM_CLICKED", LIST_ITEM_CLICKED, "LIST_ITEM_SELECT", LIST_ITEM_SELECT,

                 // UIPropertiesBox
                 "PROPERTY_CLICKED", PROPERTY_CLICKED,

                 // CUIMessageBox
                 "MESSAGE_BOX_OK_CLICKED", MESSAGE_BOX_OK_CLICKED, "MESSAGE_BOX_YES_CLICKED", MESSAGE_BOX_YES_CLICKED, "MESSAGE_BOX_NO_CLICKED", MESSAGE_BOX_NO_CLICKED,
                 "MESSAGE_BOX_CANCEL_CLICKED", MESSAGE_BOX_CANCEL_CLICKED, "MESSAGE_BOX_QUIT_GAME_CLICKED", MESSAGE_BOX_QUIT_GAME_CLICKED, "MESSAGE_BOX_QUIT_WIN_CLICKED",
                 MESSAGE_BOX_QUIT_WIN_CLICKED,

                 // CUITalkDialogWnd
                 "TALK_DIALOG_TRADE_BUTTON_CLICKED", TALK_DIALOG_TRADE_BUTTON_CLICKED, "TALK_DIALOG_QUESTION_CLICKED", TALK_DIALOG_QUESTION_CLICKED,

#pragma todo( \
    "KRodin: ивент PDA_CONTACTS_WND_CONTACT_SELECTED нигде не вызывается. Надо доделать по необходимости. Хотя я не очень представляю, для чего он может понадобиться в скриптах.")
                 // CUIPdaContactsWnd
                 "PDA_CONTACTS_WND_CONTACT_SELECTED", PDA_CONTACTS_WND_CONTACT_SELECTED,

                 // CUITradeWnd
                 "TRADE_WND_CLOSED", TRADE_WND_CLOSED,

                 // CUIInventroyWnd
                 "INVENTORY_DROP_ACTION", INVENTORY_DROP_ACTION, "INVENTORY_EAT_ACTION", INVENTORY_EAT_ACTION, "INVENTORY_TO_BELT_ACTION", INVENTORY_TO_BELT_ACTION,
                 "INVENTORY_TO_SLOT_ACTION", INVENTORY_TO_SLOT_ACTION, "INVENTORY_TO_BAG_ACTION", INVENTORY_TO_BAG_ACTION, "INVENTORY_ATTACH_ADDON", INVENTORY_ATTACH_ADDON,
                 "INVENTORY_DETACH_SCOPE_ADDON", INVENTORY_DETACH_SCOPE_ADDON, "INVENTORY_DETACH_SILENCER_ADDON", INVENTORY_DETACH_SILENCER_ADDON,
                 "INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON", INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
}
