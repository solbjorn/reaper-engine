#include "stdafx.h"

// #include "resourcemanager.h"
#include "../Include/xrRender/DrawUtils.h"
// #include "xr_effgamma.h"
#include "render.h"

// #include "../xrcdb/xrxrc.h"

namespace
{
void SetupGPU(IRenderDeviceRender* pRender)
{
    // Command line
    char* lpCmdLine = Core.Params;

    BOOL bForceGPU_SW;
    BOOL bForceGPU_NonPure;
    BOOL bForceGPU_REF;

    if (strstr(lpCmdLine, "-gpu_sw"))
        bForceGPU_SW = TRUE;
    else
        bForceGPU_SW = FALSE;

    if (strstr(lpCmdLine, "-gpu_nopure"))
        bForceGPU_NonPure = TRUE;
    else
        bForceGPU_NonPure = FALSE;

    if (strstr(lpCmdLine, "-gpu_ref"))
        bForceGPU_REF = TRUE;
    else
        bForceGPU_REF = FALSE;

    pRender->SetupGPU(bForceGPU_SW, bForceGPU_NonPure, bForceGPU_REF);
}
} // namespace

void CRenderDevice::_SetupStates()
{
    // General Render States
    vCameraPosition.set(0, 0, 0);
    vCameraDirection.set(0, 0, 1);
    vCameraTop.set(0, 1, 0);
    vCameraRight.set(1, 0, 0);

    mView.identity();
    mProject.identity();
    mFullTransform.identity();

    mViewHud.identity();
    mProjectHud.identity();

    mInvView.identity();
    mInvFullTransform.identity();

    vCameraDirectionSaved = vCameraPosition;

    m_pRender->SetupStates();
}

void CRenderDevice::_Create()
{
    Memory.mem_compact();

    // after creation
    b_is_Ready = TRUE;

    _SetupStates();

    m_pRender->OnDeviceCreate();

    dwFrame = 0;
}

void CRenderDevice::ConnectToRender()
{
    if (!m_pRender)
        m_pRender = RenderFactory->CreateRenderDeviceRender();
}

void CRenderDevice::Create()
{
    if (b_is_Ready)
        return; // prevent double call

    Statistic = xr_new<CStats>();

#ifdef DEBUG
    cdb_clRAY = &Statistic->clRAY; // total: ray-testing
    cdb_clBOX = &Statistic->clBOX; // total: box query
    cdb_clFRUSTUM = &Statistic->clFRUSTUM; // total: frustum query
#endif

    if (!m_pRender)
        m_pRender = RenderFactory->CreateRenderDeviceRender();

    SetupGPU(m_pRender);
    Log("Starting RENDER device...");

    fFOV = 90.f;
    fASPECT = 1.f;
    m_pRender->Create(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2);

    if (g_screenmode == 1)
    {
        u32 w, h;
        GetMonitorResolution(w, h);
        SetWindowLongPtr(Device.m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(Device.m_hWnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
    }

    DisableProcessWindowsGhosting();

    RECT winRect;
    GetClientRect(m_hWnd, &winRect);
    MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<LPPOINT>(&winRect), 2);
    ClipCursor(&winRect);
    SetActiveWindow(m_hWnd);

    _Create();

    PreCache(0, false, false);
}
