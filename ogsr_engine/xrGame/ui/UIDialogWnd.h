// UIDialogWnd.h: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DIALOG_WND_H_
#define _UI_DIALOG_WND_H_

#pragma once

#include "uiwindow.h"

class CDialogHolder;

class CUIDialogWnd : public CUIWindow
{
    RTTI_DECLARE_TYPEINFO(CUIDialogWnd, CUIWindow);

private:
    typedef CUIWindow inherited;

    CDialogHolder* m_pHolder{};

protected:
    bool IR_process();
    // время прошлого клика мышки
    // для определения DoubleClick
    u32 m_dwLastClickTime;

public:
    bool m_bWorkInPause{};

    CUIDialogWnd();
    ~CUIDialogWnd() override;

    virtual void Show();
    virtual void Hide();
    virtual void Update();

    virtual tmc::task<bool> IR_OnKeyboardPress(xr::key_id dik);
    [[nodiscard]] virtual bool IR_OnKeyboardRelease(xr::key_id dik);
    [[nodiscard]] virtual bool IR_OnMouseMove(int dx, int dy);
    virtual tmc::task<bool> IR_OnMouseWheel(gsl::index direction);
    virtual tmc::task<bool> IR_OnKeyboardHold(xr::key_id dik);
    [[nodiscard]] bool OnKeyboard(xr::key_id dik, EUIMessages keyboard_action) override;
    [[nodiscard]] bool OnKeyboardHold(xr::key_id dik) override;

    CDialogHolder* GetHolder() { return m_pHolder; }
    void SetHolder(CDialogHolder* h) { m_pHolder = h; }
    virtual bool StopAnyMove() { return true; }
    virtual bool NeedCursor() const { return true; }
    virtual bool WorkInPause() const { return m_bWorkInPause; }
};
XR_SOL_BASE_CLASSES(CUIDialogWnd);

#endif // _UI_DIALOG_WND_H_
