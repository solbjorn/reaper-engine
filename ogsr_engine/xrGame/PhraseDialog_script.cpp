#include "stdafx.h"

#include "PhraseDialog_script.h"
#include "PhraseDialog.h"

void CPhraseScript::AddPrecondition(LPCSTR str) { m_Preconditions.push_back(str); }
void CPhraseScript::AddAction(LPCSTR str) { m_ScriptActions.push_back(str); }
void CPhraseScript::AddHasInfo(LPCSTR str) { m_HasInfo.push_back(str); }
void CPhraseScript::AddDontHasInfo(LPCSTR str) { m_DontHasInfo.push_back(str); }
void CPhraseScript::AddGiveInfo(LPCSTR str) { m_GiveInfo.push_back(str); }
void CPhraseScript::AddDisableInfo(LPCSTR str) { m_DisableInfo.push_back(str); }

void CPhraseDialogExporter::script_register(sol::state_view& lua)
{
    lua.new_usertype<CPhrase>("CPhrase", sol::no_constructor, "GetPhraseScript", &CPhrase::GetPhraseScript, "GetText", &CPhrase::GetText, "SetText", &CPhrase::SetText);

    lua.new_usertype<CPhraseDialog>("CPhraseDialog", sol::no_constructor, "AddPhrase", &CPhraseDialog::AddPhrase_script, "SetPriority", &CPhraseDialog::SetPriority, "SetCaption",
                                    &CPhraseDialog::SetCaption, "SetForceReload", &CPhraseDialog::SetForceReload, "GetPhrase", &CPhraseDialog::GetPhrase, "GetPhraseScript",
                                    &CPhraseDialog::GetPhraseScript);

    lua.new_usertype<CPhraseScript>("CPhraseScript", sol::no_constructor, "AddPrecondition", &CPhraseScript::AddPrecondition, "AddAction", &CPhraseScript::AddAction, "AddHasInfo",
                                    &CPhraseScript::AddHasInfo, "AddDontHasInfo", &CPhraseScript::AddDontHasInfo, "AddGiveInfo", &CPhraseScript::AddGiveInfo, "AddDisableInfo",
                                    &CPhraseScript::AddDisableInfo, "SetScriptText", &CPhraseScript::SetScriptText);
}
