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
    void IR_Capture();
    void IR_Release();

    virtual void IR_OnDeactivate();
    virtual void IR_OnActivate();

    virtual void IR_OnMousePress(int) {}
    virtual void IR_OnMouseRelease(int) {}
    virtual void IR_OnMouseHold(int) {}
    virtual void IR_OnMouseWheel(int) {}
    virtual void IR_OnMouseMove(int, int) {}
    virtual void IR_OnMouseStop(int, int) {}

    virtual void IR_OnKeyboardPress(int) {}
    virtual void IR_OnKeyboardRelease(int) {}
    virtual void IR_OnKeyboardHold(int) {}
};

extern float psMouseSens;
extern float psMouseSensScale;
extern Flags32 psMouseInvert;
