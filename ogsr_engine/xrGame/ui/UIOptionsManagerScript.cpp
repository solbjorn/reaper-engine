#include "stdafx.h"

#include "../../xrScriptEngine/xr_sol.h"

#include "UIOptionsItem.h"
#include "UIOptionsManagerScript.h"

void CUIOptionsManagerScript::SetCurrentValues(const char* group) { CUIOptionsItem::GetOptionsManager()->SetCurrentValues(group); }

void CUIOptionsManagerScript::SaveBackupValues(const char* group) { CUIOptionsItem::GetOptionsManager()->SeveBackupValues(group); }

void CUIOptionsManagerScript::SaveValues(const char* group) { CUIOptionsItem::GetOptionsManager()->SaveValues(group); }

bool CUIOptionsManagerScript::IsGroupChanged(const char* group) { return CUIOptionsItem::GetOptionsManager()->IsGroupChanged(group); }

void CUIOptionsManagerScript::UndoGroup(const char* group) { CUIOptionsItem::GetOptionsManager()->UndoGroup(group); }

void CUIOptionsManagerScript::OptionsPostAccept() { CUIOptionsItem::GetOptionsManager()->OptionsPostAccept(); }

void CUIOptionsManagerScript::SendMessage2Group(const char* group, const char* message) { CUIOptionsItem::GetOptionsManager()->SendMessage2Group(group, message); }

void CUIOptionsManagerScript::script_register(lua_State* L)
{
    sol::state_view(L).new_usertype<CUIOptionsManagerScript>("COptionsManager", sol::no_constructor, sol::call_constructor, sol::constructors<CUIOptionsManagerScript()>(),
                                                             "SaveBackupValues", &CUIOptionsManagerScript::SaveBackupValues, "SetCurrentValues",
                                                             &CUIOptionsManagerScript::SetCurrentValues, "SaveValues", &CUIOptionsManagerScript::SaveValues, "IsGroupChanged",
                                                             &CUIOptionsManagerScript::IsGroupChanged, "UndoGroup", &CUIOptionsManagerScript::UndoGroup, "OptionsPostAccept",
                                                             &CUIOptionsManagerScript::OptionsPostAccept, "SendMessage2Group", &CUIOptionsManagerScript::SendMessage2Group);
}
