#include "stdafx.h"

#include "UIListWnd.h"
#include "UIListItemEx.h"
#include "UISpinText.h"
#include "UIComboBox.h"

bool CUIListWnd::AddItem_script(std::unique_ptr<CUIListItemEx>& ptr) { return AddItem(ptr.release(), -1); }

void CUIListWnd::script_register(sol::state_view& lua)
{
    lua.new_usertype<CUIListWnd>("CUIListWnd", sol::no_constructor, sol::call_constructor, sol::constructors<CUIListWnd()>(), "AddItem", &CUIListWnd::AddItem_script, "RemoveItem",
                                 &CUIListWnd::RemoveItem, "RemoveAll", &CUIListWnd::RemoveAll, "EnableScrollBar", &CUIListWnd::EnableScrollBar, "IsScrollBarEnabled",
                                 &CUIListWnd::IsScrollBarEnabled, "SetItemHeight", &CUIListWnd::SetItemHeight, "GetItem", &CUIListWnd::GetItem, "GetItemPos",
                                 &CUIListWnd::GetItemPos, "GetSize", &CUIListWnd::GetItemsCount, "ScrollToBegin", &CUIListWnd::ScrollToBegin, "ScrollToEnd",
                                 &CUIListWnd::ScrollToEnd, "ScrollToPos", &CUIListWnd::ScrollToPos, "SetWidth", &CUIListWnd::SetWidth, "SetTextColor", &CUIListWnd::SetTextColor,
                                 "ActivateList", &CUIListWnd::ActivateList, "IsListActive", &CUIListWnd::IsListActive, "SetVertFlip", &CUIListWnd::SetVertFlip, "GetVertFlip",
                                 &CUIListWnd::GetVertFlip, "SetFocusedItem", &CUIListWnd::SetFocusedItem, "GetFocusedItem", &CUIListWnd::GetFocusedItem, "ShowSelectedItem",
                                 &CUIListWnd::ShowSelectedItem, "GetSelectedItem", &CUIListWnd::GetSelectedItem, "SetSelectedItem", &CUIListWnd::SetSelectedItem,
                                 "ResetFocusCapture", &CUIListWnd::ResetFocusCapture, sol::base_classes, xr::sol_bases<CUIListWnd>());

    lua.new_usertype<CUIListItem>("CUIListItem", sol::no_constructor, sol::call_constructor, sol::constructors<CUIListItem()>(), "priv", &CUIListItem::priv, sol::base_classes,
                                  xr::sol_bases<CUIListItem>());

    lua.new_usertype<CUIListItemEx>("CUIListItemEx", sol::no_constructor, sol::call_constructor, sol::factories(std::make_unique<CUIListItemEx>), "SetSelectionColor",
                                    &CUIListItemEx::SetSelectionColor, sol::base_classes, xr::sol_bases<CUIListItemEx>());
}
