#pragma once

class CUIWindow;

class CUIWndCallback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(CUIWndCallback);

public:
    using void_function = CallMe::Delegate<void(CUIWindow*, void*)>;

    struct callback
    {
        CallMe::Delegate<void(CUIWindow*, void*)> m_cpp_callback;
        shared_str m_controlName;
        s16 m_event;
    };

private:
    xr_vector<callback> m_callbacks;

public:
    CUIWndCallback();
    ~CUIWndCallback() override;

    virtual void OnEvent(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

    void Register(CUIWindow* pChild);
    void AddCallback(LPCSTR control_id, s16 event, const void_function& f);
    void ClearCallbacks();
};
