#include "stdafx.h"

#include "UsableScriptObject.h"

#include "GameObject.h"
#include "script_game_object.h"
#include "game_object_space.h"

CUsableScriptObject::CUsableScriptObject() = default;
CUsableScriptObject::~CUsableScriptObject() = default;

bool CUsableScriptObject::use(CGameObject* who_use)
{
    VERIFY(who_use);
    CGameObject* pThis = smart_cast<CGameObject*>(this);
    VERIFY(pThis);

    pThis->callback(GameObject::eUseObject)(pThis->lua_game_object(), who_use->lua_game_object());

    return true;
}

LPCSTR CUsableScriptObject::tip_text() { return m_sTipText.c_str(); }
void CUsableScriptObject::set_tip_text(LPCSTR new_text) { m_sTipText._set(new_text); }
void CUsableScriptObject::set_tip_text_default() { m_sTipText._set(nullptr); }

bool CUsableScriptObject::nonscript_usable() { return m_bNonscriptUsable; }
void CUsableScriptObject::set_nonscript_usable(bool usable) { m_bNonscriptUsable = usable; }
