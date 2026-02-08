#include "stdafx.h"

#include "IInputReceiver.h"

#include "xr_input.h"

#include <SFML/Window/Joystick.hpp>

tmc::task<void> IInputReceiver::IR_Capture()
{
    VERIFY(pInput);
    co_await pInput->iCapture(this);
}

tmc::task<void> IInputReceiver::IR_Release()
{
    VERIFY(pInput);
    co_await pInput->iRelease(this);
}

void IInputReceiver::IR_GetLastMouseDelta(Ivector2& p)
{
    VERIFY(pInput);
    pInput->iGetLastMouseDelta(p);
}

void IInputReceiver::IR_OnDeactivate()
{
    for (s32 i{0}; i < s32{sf::Keyboard::ScancodeCount}; ++i)
    {
        if (IR_GetKeyState(xr::key_id{sf::Keyboard::Scancode{i}}))
            IR_OnKeyboardRelease(xr::key_id{sf::Keyboard::Scancode{i}});
    }

    for (s32 i{0}; i < s32{sf::Mouse::ButtonCount}; ++i)
    {
        if (IR_GetKeyState(xr::key_id{sf::Mouse::Button{i}}))
            IR_OnKeyboardRelease(xr::key_id{sf::Mouse::Button{i}});
    }

    for (s32 i{0}; i < s32{sf::Joystick::ButtonCount}; ++i)
    {
        if (IR_GetKeyState(xr::key_id{xr::key_id::joystick{i}}))
            IR_OnKeyboardRelease(xr::key_id{xr::key_id::joystick{i}});
    }

    IR_OnMouseStop(0, 0);
    IR_OnMouseStop(4, 0);
}

bool IInputReceiver::IR_GetKeyState(xr::key_id dik) const
{
    VERIFY(pInput);
    return pInput->iGetAsyncKeyState(dik);
}

void IInputReceiver::IR_GetMousePosScreen(Ivector2& p) { GetCursorPos((LPPOINT)&p); }

void IInputReceiver::IR_GetMousePosReal(HWND hwnd, Ivector2& p)
{
    IR_GetMousePosScreen(p);
    if (hwnd)
        ScreenToClient(hwnd, (LPPOINT)&p);
}

void IInputReceiver::IR_GetMousePosReal(Ivector2& p) { IR_GetMousePosReal(Device.m_hWnd, p); }

void IInputReceiver::IR_GetMousePosIndependent(Fvector2& f)
{
    Ivector2 p;
    IR_GetMousePosReal(p);
    f.set(2.f * float(p.x) / float(Device.dwWidth) - 1.f, 2.f * float(p.y) / float(Device.dwHeight) - 1.f);
}

void IInputReceiver::IR_GetMousePosIndependentCrop(Fvector2& f)
{
    IR_GetMousePosIndependent(f);
    if (f.x < -1.f)
        f.x = -1.f;
    if (f.x > 1.f)
        f.x = 1.f;
    if (f.y < -1.f)
        f.y = -1.f;
    if (f.y > 1.f)
        f.y = 1.f;
}
