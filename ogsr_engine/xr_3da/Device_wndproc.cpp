#include "stdafx.h"

#include "device.h"

#include "xr_input.h"

std::pair<bool, s32> CRenderDevice::on_message(u32 msg, std::size_t wp, std::ptrdiff_t lp)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: pInput->keyboard_event(msg, wp, lp); return {msg != WM_SYSKEYDOWN && msg != WM_SYSKEYUP, 0};
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEWHEEL: pInput->mouse_event(msg, wp); return {true, 0};
    case WM_ACTIVATE: {
        auto& arg = add_frame_async(CallMe::fromMethod<&CRenderDevice::OnWM_Activate>(this));
        *reinterpret_cast<u64*>(&arg) = wp;

        return {false, 0};
    }
    case WM_SETCURSOR: return {true, 1};
    case WM_SYSCOMMAND:
        // Bits [0, 3] are reserved for internal usage
        switch (wp & 0xfff0uz)
        {
        // Don't steal the focus on Alt or F10
        case SC_KEYMENU: return {true, 0};
        // Prevent moving/sizing and power loss in fullscreen mode
        case SC_MAXIMIZE:
        case SC_MONITORPOWER:
        case SC_MOVE:
        case SC_SIZE: return {true, 1};
        default: return {false, 0};
        }
    case WM_CLOSE: return {true, 0};
    default: return {false, 0};
    }
}

CALLBACK std::ptrdiff_t CRenderDevice::wnd_proc(HWND hWnd, u32 msg, std::size_t wp, std::ptrdiff_t lp)
{
    if (const auto ret = Device.on_message(msg, wp, lp); ret.first)
        return ret.second;

    return ::DefWindowProcW(hWnd, msg, wp, lp);
}
