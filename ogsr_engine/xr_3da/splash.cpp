#include "stdafx.h"

#include "splash.h"
#include "resource.h"

// какой то говнокод что б подключить <atlimage.h>
#define _FORCENAMELESSUNION

namespace ATL
{
inline errno_t AtlCrtErrorCheck(const char*) { return 0; }
} // namespace ATL

#define __ATLBASE_H__
#include <atlimage.h>

#undef _FORCENAMELESSUNION

#pragma comment(lib, "Windowscodecs")

constexpr const char* c_szSplashClass = "SplashWindow";

static IStream* CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
    IStream* ipStream = nullptr;

    HRSRC hrsrc = FindResource(nullptr, lpName, lpType);
    if (hrsrc == nullptr)
        return nullptr;

    HGLOBAL hglbImage = LoadResource(nullptr, hrsrc);
    if (hglbImage == nullptr)
        return nullptr;

    LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == nullptr)
        return nullptr;

    DWORD dwResourceSize = SizeofResource(nullptr, hrsrc);

    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == nullptr)
        return nullptr;

    LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == nullptr)
    {
        GlobalFree(hgblResourceData);
        return nullptr;
    }

    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);

    GlobalUnlock(hgblResourceData);

    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
        return ipStream;

    GlobalFree(hgblResourceData);
    return nullptr;
}

HWND ShowSplash(HINSTANCE hInstance)
{
    WNDCLASS wc{};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = c_szSplashClass;
    RegisterClass(&wc);

    // image
    CImage img; // объект изображения

    CHAR path[MAX_PATH];

    GetModuleFileName(nullptr, path, MAX_PATH);

    std::string splash_path{path};
    splash_path = splash_path.erase(splash_path.find_last_of('\\'), splash_path.size() - 1);
    splash_path += "\\splash.png";

    img.Load(splash_path.c_str()); // загрузка сплеша

    int splashWidth; // фиксируем ширину картинки
    int splashHeight; // фиксируем высоту картинки

    if (img.IsNull()) // если картинки нет на диске, то грузим из ресурсов
    {
        img.Destroy();

        img.Load(CreateStreamOnResource(MAKEINTRESOURCE(IDB_PNG1), "PNG")); // загружаем сплеш
        splashWidth = img.GetWidth();
        splashHeight = img.GetHeight();
    }
    else
    {
        splashWidth = img.GetWidth();
        splashHeight = img.GetHeight();
    }

    const HWND hwndOwner = CreateWindow(c_szSplashClass, nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);
    const HWND hWnd = CreateWindowEx(WS_EX_LAYERED, c_szSplashClass, nullptr, WS_POPUP, 0, 0, 0, 0, hwndOwner, nullptr, hInstance, nullptr);

    if (!hWnd)
        return nullptr;

    const float scale = float(GetDpiForWindow(hWnd)) / float(USER_DEFAULT_SCREEN_DPI);
    splashWidth = std::lround(float(splashWidth) * scale);
    splashHeight = std::lround(float(splashHeight) * scale);

    const HDC hdcScreen = GetDC(nullptr);
    const HDC hDC = CreateCompatibleDC(hdcScreen);
    const HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, splashWidth, splashHeight);

    const HBITMAP hbmpOld = (HBITMAP)SelectObject(hDC, hBmp);

    // рисуем картиночку
    for (int i = 0; i < img.GetWidth(); i++)
    {
        for (int j = 0; j < img.GetHeight(); j++)
        {
            BYTE* ptr = (BYTE*)img.GetPixelAddress(i, j);
            ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
            ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
            ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
        }
    }

    img.StretchBlt(hDC, 0, 0, splashWidth, splashHeight, 0, 0, img.GetWidth(), img.GetHeight(), SRCCOPY);
    img.AlphaBlend(hDC, 0, 0, splashWidth, splashHeight, 0, 0, splashWidth, splashHeight);

    // alpha
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = 255;

    SIZE sizeWnd{splashWidth, splashHeight};

    POINT ptPos{};
    POINT ptSrc{};

    if (const HWND hDT = GetDesktopWindow())
    {
        RECT rcDT;
        GetWindowRect(hDT, &rcDT);
        ptPos.x = (rcDT.right - splashWidth) / 2;
        ptPos.y = (rcDT.bottom - splashHeight) / 2;
    }

    UpdateLayeredWindow(hWnd, hdcScreen, &ptPos, &sizeWnd, hDC, &ptSrc, 0, &blend, LWA_ALPHA);

    HWND logoInsertPos = IsDebuggerPresent() ? HWND_NOTOPMOST : HWND_TOPMOST;

    // mmccxvii: захардкорил размер битмапа, чтобы не было бага, связанного с увеличенным масштабом интерфейса винды
    SetWindowPos(hWnd, logoInsertPos, 0, 0, splashWidth, splashHeight, SWP_NOMOVE | SWP_SHOWWINDOW);

    // delete temporary objects
    SelectObject(hDC, hbmpOld);
    DeleteDC(hDC);
    ReleaseDC(nullptr, hdcScreen);

    return hWnd;
}
