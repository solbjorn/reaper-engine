#include "stdafx.h"
#include "UIScriptWnd.h"
#include "../HudManager.h"
#include "../object_broker.h"
#include "../callback_info.h"

struct event_comparer
{
    shared_str name;
    s16 event;

    event_comparer(shared_str n, s16 e) : name(n), event(e) {}
    bool operator()(CUIDialogWndEx::callback& i) { return ((i.m_controlName == name) && (i.m_event == event)); }
};

CUIDialogWndEx::CUIDialogWndEx() : inherited() { Hide(); }

void CUIDialogWndEx::Register(CUIWindow* pChild) { pChild->SetMessageTarget(this); }

void CUIDialogWndEx::Register(CUIWindow* pChild, LPCSTR name)
{
    pChild->SetWindowName(name);
    pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    event_comparer ec(pWnd->WindowName(), msg);

    CALLBACK_IT it = std::find_if(m_callbacks.begin(), m_callbacks.end(), ec);
    if (it == m_callbacks.end())
        return inherited::SendMessage(pWnd, msg, pData);

    it->fn(this);
}

bool CUIDialogWndEx::Load(LPCSTR xml_name) { return true; }

void CUIDialogWndEx::AddCallback(LPCSTR control_id, s16 event, sol::function function) { m_callbacks.emplace_back(control_id, event, function); }

bool CUIDialogWndEx::OnKeyboard(int dik, EUIMessages keyboard_action)
{
    bool ret = inherited::OnKeyboard(dik, keyboard_action);

    auto op = ops.find(wnd_ops::ON_KEYBOARD);
    if (op == ops.end())
        return ret;

    return op->second(this, dik, keyboard_action);
}

void CUIDialogWndEx::Update() { inherited::Update(); }

void CUIDialogWndEx::ClearCallbacks()
{
    priv = sol::object();
    ops.clear();

    m_callbacks.clear();
}
