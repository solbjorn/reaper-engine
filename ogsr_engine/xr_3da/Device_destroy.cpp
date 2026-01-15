#include "stdafx.h"

#include "device.h"

#include "render.h"
#include "xr_IOConsole.h"
#include "xr_input.h"

#include "../Include/xrRender/DrawUtils.h"

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

void CRenderDevice::Destroy()
{
    if (!b_is_Ready)
        return;

    Log("Destroying Direct3D...");

    pInput->clip_cursor(false);

    _Destroy(FALSE);

    // real destroy
    m_pRender->DestroyHW();

    seqRender.clear();
    seqAppActivate.clear();
    seqAppDeactivate.clear();
    seqAppStart.clear();
    seqAppEnd.clear();
    seqFrame.clear();
    seqFrameMT.clear();
    seqDeviceReset.clear();
    seqParallel.clear();
    seq_frame_async.clear();

    RenderFactory->DestroyRenderDeviceRender(m_pRender);
    m_pRender = nullptr;
    xr_delete(Statistic);
}

tmc::task<void> CRenderDevice::Reset(bool precache)
{
    u32 dwWidth_before = dwWidth;
    u32 dwHeight_before = dwHeight;

    pInput->clip_cursor(false);

    u32 tm_start = TimerAsync();

    co_await m_pRender->Reset(m_hWnd, dwWidth, dwHeight, fWidth_2, fHeight_2);

    _SetupStates();

    if (precache)
        PreCache(20, false, false);

    u32 tm_end = TimerAsync();

    Msg("*** RESET [%u ms]", tm_end - tm_start);

    //	TODO: Remove this! It may hide crash
    // TODO: KRodin: ??? Remove this! It may hide crash ???
    Memory.mem_compact();

    co_await seqDeviceReset.process();

    if (dwWidth_before != dwWidth || dwHeight_before != dwHeight)
        co_await seqResolutionChanged.process();

    if (g_screenmode == 1)
    {
        co_await tmc::spawn([](auto wnd) -> tmc::task<void> {
            u32 w, h;
            GetMonitorResolution(w, h);

            SetWindowLongPtr(wnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
            SetWindowPos(wnd, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);

            co_return;
        }(m_hWnd))
            .run_on(xr::tmc_cpu_st_executor());
    }

    pInput->clip_cursor(true);
}
