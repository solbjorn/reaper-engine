#pragma once

#include "../script_export_space.h"
#include "UIDialogWnd.h"

class CUIDialogWndEx : public CUIDialogWnd, public DLL_Pure
{
    RTTI_DECLARE_TYPEINFO(CUIDialogWndEx, CUIDialogWnd, DLL_Pure);

public:
    struct callback
    {
        sol::function fn;
        shared_str m_controlName;
        s16 m_event;
    };

    typedef CUIDialogWnd inherited;
    typedef xr_vector<callback> CALLBACKS;
    typedef CALLBACKS::iterator CALLBACK_IT;

private:
    enum class wnd_ops : s32
    {
        ON_KEYBOARD
    };

    sol::object priv;
    xr_map<wnd_ops, sol::function> ops;

    CALLBACKS m_callbacks;

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

protected:
    [[nodiscard]] bool Load(LPCSTR);

public:
    CUIDialogWndEx();
    ~CUIDialogWndEx() override;

    void Register(CUIWindow* pChild);
    void Register(CUIWindow* pChild, LPCSTR name);
    void AddCallback(LPCSTR control_id, s16 event, sol::function function);
    void ClearCallbacks();

    virtual void Update();
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;

    template <typename T>
    IC T* GetControl(LPCSTR name);

    DECLARE_SCRIPT_REGISTER_FUNCTION();
};
XR_SOL_BASE_CLASSES(CUIDialogWndEx);

add_to_type_list(CUIDialogWndEx);
#undef script_type_list
#define script_type_list save_type_list(CUIDialogWndEx)
