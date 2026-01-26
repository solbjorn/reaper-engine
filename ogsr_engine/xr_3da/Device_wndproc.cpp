#include "stdafx.h"

#include "device.h"

bool CRenderDevice::on_message(UINT uMsg, WPARAM wParam, LRESULT& result)
{
    switch (uMsg)
    {
    case WM_SYSKEYDOWN: {
        return true;
    }
    case WM_ACTIVATE: {
        auto& arg = add_frame_async(CallMe::fromMethod<&CRenderDevice::OnWM_Activate>(this));
        *reinterpret_cast<u64*>(&arg) = wParam;

        return false;
    }
    case WM_SETCURSOR: {
        result = 1;
        return (true);
    }
    case WM_SYSCOMMAND: {
        // Prevent moving/sizing and power loss in fullscreen mode
        switch (wParam)
        {
        case SC_MOVE:
        case SC_SIZE:
        case SC_MAXIMIZE:
        case SC_MONITORPOWER: result = 1; return (true);
        }
        return (false);
    }
    case WM_CLOSE: {
        result = 0;
        return (true);
    }
    }

    return (false);
}

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    if (Device.on_message(uMsg, wParam, result))
        return result;

    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
