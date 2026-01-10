#include "stdafx.h"

#include "device.h"

#include "resource.h"

extern LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

tmc::task<void> CRenderDevice::Initialize()
{
    Log("Initializing Engine...");
    TimerGlobal.Start();
    TimerMM.Start();

    // Unless a substitute hWnd has been specified, create a window to render into
    if (!m_hWnd)
    {
        const char* wndclass = "_XRAY_1.5";

        // Register the windows class
        HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(nullptr);
        WNDCLASS wndClass = {
            0, WndProc, 0, 0, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)), LoadCursor(nullptr, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), nullptr, wndclass};
        RegisterClass(&wndClass);

        // Set the window's initial style
        m_dwWindowStyle = WS_BORDER | WS_DLGFRAME;

        // Set the window's initial width
        RECT rc;
        SetRect(&rc, 0, 0, 640, 480);
        AdjustWindowRect(&rc, m_dwWindowStyle, FALSE);

        // Create the render window
        m_hWnd = CreateWindow /*Ex*/ ( // WS_EX_TOPMOST,
            wndclass, "OGSR Engine", m_dwWindowStyle,
            /*rc.left, rc.top, */ CW_USEDEFAULT, CW_USEDEFAULT, (rc.right - rc.left), (rc.bottom - rc.top), nullptr, nullptr, hInstance, nullptr);
        gGameWindow = m_hWnd;
    }

    // Save window properties
    m_dwWindowStyle = u32(GetWindowLongPtr(m_hWnd, GWL_STYLE));
    GetWindowRect(m_hWnd, &m_rcWindowBounds);
    GetClientRect(m_hWnd, &m_rcWindowClient);

    co_return;
}
