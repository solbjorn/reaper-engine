#include "stdafx.h"

#include "UIWndCallback.h"

#include "UIWindow.h"
#include "../object_broker.h"

struct event_comparer
{
    shared_str name;
    s16 event;

    constexpr explicit event_comparer(shared_str n, s16 e) : name{n}, event{e} {}

    [[nodiscard]] constexpr bool operator()(const CUIWndCallback::callback& i) const { return name == i.m_controlName && event == i.m_event; }
};

void CUIWndCallback::Register(CUIWindow* pChild) { pChild->SetMessageTarget(smart_cast<CUIWindow*>(this)); }

void CUIWndCallback::OnEvent(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (pWnd == nullptr)
        return;

    const event_comparer ec{pWnd->WindowName(), msg};

    const auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(), ec);
    if (it == m_callbacks.end())
        return;

    it->m_cpp_callback(pWnd, pData);
}

void CUIWndCallback::AddCallback(LPCSTR control_id, s16 event, const void_function& f) { m_callbacks.emplace_back(f, shared_str{control_id}, event); }
void CUIWndCallback::AddCallback(const shared_str& control_id, s16 event, const void_function& f) { m_callbacks.emplace_back(f, control_id, event); }
