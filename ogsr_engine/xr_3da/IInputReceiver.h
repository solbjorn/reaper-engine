// IInputReceiver.h: interface for the IInputReceiver class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class IInputReceiver : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IInputReceiver);

public:
    ~IInputReceiver() override = default;

    static void IR_GetLastMouseDelta(Ivector2& p);
    static void IR_GetMousePosScreen(Ivector2& p);
    static void IR_GetMousePosReal(HWND hwnd, Ivector2& p);
    static void IR_GetMousePosReal(Ivector2& p);
    static void IR_GetMousePosIndependent(Fvector2& f);
    static void IR_GetMousePosIndependentCrop(Fvector2& f);

    BOOL IR_GetKeyState(int dik);
    BOOL IR_GetBtnState(int btn);
    tmc::task<void> IR_Capture();
    tmc::task<void> IR_Release();

    virtual tmc::task<void> IR_OnActivate() { co_return; }
    virtual void IR_OnDeactivate();

    virtual tmc::task<void> IR_OnMousePress(gsl::index) { co_return; }
    virtual void IR_OnMouseRelease(int) {}
    virtual tmc::task<void> IR_OnMouseHold(gsl::index) { co_return; }
    virtual tmc::task<void> IR_OnMouseWheel(gsl::index) { co_return; }
    virtual void IR_OnMouseMove(int, int) {}
    virtual void IR_OnMouseStop(int, int) {}

    virtual tmc::task<void> IR_OnKeyboardPress(gsl::index) { co_return; }
    virtual void IR_OnKeyboardRelease(int) {}
    virtual tmc::task<void> IR_OnKeyboardHold(gsl::index) { co_return; }
};

extern float psMouseSens;
extern float psMouseSensScale;
extern Flags32 psMouseInvert;
