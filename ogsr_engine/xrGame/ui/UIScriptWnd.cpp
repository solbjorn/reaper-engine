#include "stdafx.h"

#include "UIScriptWnd.h"

#include "../HudManager.h"
#include "../object_broker.h"

struct event_comparer
{
    shared_str name;
    s16 event;

    constexpr explicit event_comparer(shared_str n, s16 e) : name{n}, event{e} {}

    [[nodiscard]] constexpr bool operator()(const CUIDialogWndEx::callback& i) const { return name == i.m_controlName && event == i.m_event; }
};

CUIDialogWndEx::CUIDialogWndEx() { Hide(); }
CUIDialogWndEx::~CUIDialogWndEx() { ClearCallbacks(); }

void CUIDialogWndEx::Register(CUIWindow* pChild) { pChild->SetMessageTarget(this); }

void CUIDialogWndEx::Register(CUIWindow* pChild, LPCSTR name)
{
    pChild->SetWindowName(name);
    pChild->SetMessageTarget(this);
}

void CUIDialogWndEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    const event_comparer ec{pWnd->WindowName(), msg};

    auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(), ec);
    if (it == m_callbacks.end())
        return inherited::SendMessage(pWnd, msg, pData);

    it->fn(this);
}

bool CUIDialogWndEx::Load(LPCSTR) { return true; }

void CUIDialogWndEx::AddCallback(LPCSTR control_id, s16 event, sol::function function) { m_callbacks.emplace_back(std::move(function), shared_str{control_id}, event); }

bool CUIDialogWndEx::OnKeyboard(xr::key_id dik, EUIMessages keyboard_action)
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
    priv = sol::object{};
    ops.clear();

    m_callbacks.clear();
}
