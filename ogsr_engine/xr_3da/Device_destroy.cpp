#include "stdafx.h"

#include "../Include/xrRender/DrawUtils.h"
#include "render.h"
#include "xr_IOConsole.h"
#include "xr_input.h"

void CRenderDevice::_Destroy(BOOL bKeepTextures)
{
    DU->OnDeviceDestroy();

    // before destroy
    b_is_Ready = FALSE;
    Statistic->OnDeviceDestroy();
    ::Render->destroy();
    m_pRender->OnDeviceDestroy(bKeepTextures);

    Memory.mem_compact();
}

void CRenderDevice::Destroy(void)
{
    if (!b_is_Ready)
        return;

    Log("Destroying Direct3D...");

    pInput->clip_cursor(false);

    _Destroy(FALSE);

    // real destroy
    m_pRender->DestroyHW();

    seqRender.Clear();
    seqAppActivate.Clear();
    seqAppDeactivate.Clear();
    seqAppStart.Clear();
    seqAppEnd.Clear();
    seqFrame.Clear();
    seqFrameMT.Clear();
    seqDeviceReset.Clear();
    seqParallel.clear();

    RenderFactory->DestroyRenderDeviceRender(m_pRender);
    m_pRender = nullptr;
    xr_delete(Statistic);
}

void CRenderDevice::Reset(bool precache)
{
    u32 dwWidth_before = dwWidth;
    u32 dwHeight_before = dwHeight;

    pInput->clip_cursor(false);

    u32 tm_start = TimerAsync();

    m_pRender->Reset(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2);

    _SetupStates();

    if (precache)
        PreCache(20, false, false);

    u32 tm_end = TimerAsync();

    Msg("*** RESET [%u ms]", tm_end - tm_start);

    //	TODO: Remove this! It may hide crash
    // TODO: KRodin: ??? Remove this! It may hide crash ???
    Memory.mem_compact();

    seqDeviceReset.Process();

    if (dwWidth_before != dwWidth || dwHeight_before != dwHeight)
    {
        seqResolutionChanged.Process();
    }

    if (g_screenmode == 1)
    {
        u32 w, h;
        GetMonitorResolution(w, h);
        SetWindowLongPtr(Device.m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
        SetWindowPos(Device.m_hWnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
    }

    pInput->clip_cursor(true);
}
