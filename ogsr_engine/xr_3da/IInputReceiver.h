// IInputReceiver.h: interface for the IInputReceiver class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class IInputReceiver : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IInputReceiver);

public:
    static void IR_GetLastMouseDelta(Ivector2& p);
    static void IR_GetMousePosScreen(Ivector2& p);
    static void IR_GetMousePosReal(HWND hwnd, Ivector2& p);
    static void IR_GetMousePosReal(Ivector2& p);
    static void IR_GetMousePosIndependent(Fvector2& f);
    static void IR_GetMousePosIndependentCrop(Fvector2& f);
    BOOL IR_GetKeyState(int dik);
    BOOL IR_GetBtnState(int btn);
    void IR_Capture(void);
    void IR_Release(void);

    virtual void IR_OnDeactivate(void);
    virtual void IR_OnActivate(void);

    virtual void IR_OnMousePress(int btn) {}
    virtual void IR_OnMouseRelease(int btn) {}
    virtual void IR_OnMouseHold(int btn) {}
    virtual void IR_OnMouseWheel(int direction) {}
    virtual void IR_OnMouseMove(int x, int y) {}
    virtual void IR_OnMouseStop(int x, int y) {}

    virtual void IR_OnKeyboardPress(int dik) {}
    virtual void IR_OnKeyboardRelease(int dik) {}
    virtual void IR_OnKeyboardHold(int dik) {}
};

extern float psMouseSens;
extern float psMouseSensScale;
extern Flags32 psMouseInvert;
