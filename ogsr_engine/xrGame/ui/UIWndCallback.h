#pragma once

#include "../callback_info.h"

class CUIWindow;

class CUIWndCallback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CUIWndCallback);

public:
    using void_function = CallMe::Delegate<void(CUIWindow*, void*)>;

private:
    typedef xr_vector<SCallbackInfo> CALLBACKS;
    typedef CALLBACKS::iterator CALLBACK_IT;

private:
    CALLBACKS m_callbacks;
    SCallbackInfo* NewCallback();

public:
    ~CUIWndCallback() override = default;

    virtual void OnEvent(CUIWindow* pWnd, s16 msg, void* pData = nullptr);
    void Register(CUIWindow* pChild);
    void AddCallback(LPCSTR control_id, s16 event, const void_function& f);
    void AddCallback(const shared_str& control_id, s16 event, const void_function& f);
};
