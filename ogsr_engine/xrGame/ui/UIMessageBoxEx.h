#pragma once

#include "UIDialogWnd.h"
#include "UIWndCallback.h"

class CUIMessageBox;

class CUIMessageBoxEx : public CUIDialogWnd, public CUIWndCallback
{
    RTTI_DECLARE_TYPEINFO(CUIMessageBoxEx, CUIDialogWnd, CUIWndCallback);

public:
    CUIMessageBoxEx();
    virtual ~CUIMessageBoxEx();
    void SetText(LPCSTR text);
    LPCSTR GetText();
    virtual void Init(LPCSTR xml_template);
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = nullptr);

    LPCSTR GetHost();
    LPCSTR GetPassword();

    CUIMessageBox* m_pMessageBox;
};
XR_SOL_BASE_CLASSES(CUIMessageBoxEx);
