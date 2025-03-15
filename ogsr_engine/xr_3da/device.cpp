#include "stdafx.h"

#include <mmsystem.h>

#include "x_ray.h"
#include "render.h"
#include "xr_input.h"

#include "igame_persistent.h"

#include "imgui.h"
#include "..\Layers\xrRenderDX10\imgui_impl_dx11.h"
#include <winternl.h>

#include <oneapi/tbb/parallel_invoke.h>

CRenderDevice Device;
CLoadScreenRenderer load_screen_renderer;

BOOL g_bRendering = FALSE;

int g_3dscopes_fps_factor = 2; // На каком кадре с момента прошлого рендера во второй вьюпорт мы начнём новый (не может быть меньше 2 - каждый второй кадр, чем больше
                               // тем более низкий FPS во втором вьюпорте)

BOOL g_bLoaded = FALSE;
float refresh_rate = 0;

bool CRenderDevice::RenderBegin()
{
    switch (m_pRender->GetDeviceState())
    {
    case IRenderDeviceRender::dsOK: break;

    case IRenderDeviceRender::dsLost:
        // If the device was lost, do not render until we get it back
        Sleep(33);
        return false;

    case IRenderDeviceRender::dsNeedReset:
        // Check if the device is ready to be reset
        Reset();
        return false;

    default: R_ASSERT(0);
    }

    m_pRender->Begin();
    g_bRendering = true;

    return true;
}

void CRenderDevice::Clear() { m_pRender->Clear(); }

void CRenderDevice::RenderEnd(void)
{
    if (dwPrecacheFrame)
    {
        ::Sound->set_master_volume(0.f);
        dwPrecacheFrame--;

        if (!load_screen_renderer.b_registered)
            m_pRender->ClearTarget();

        if (0 == dwPrecacheFrame)
        {
            m_pRender->updateGamma();
            ::Sound->set_master_volume(1.f);

            Memory.mem_compact();

            Msg("* MEMORY USAGE: %d K", Memory.mem_usage() / 1024);
            Msg("* End of synchronization A[%d] R[%d]", b_is_Active, b_is_Ready);
        }
    }

    g_bRendering = false;

    extern BOOL g_appLoaded;
    if (g_appLoaded)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    m_pRender->End();

    vCameraDirectionSaved = vCameraDirection;
}

#include "igame_level.h"

void CRenderDevice::PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
    if (m_pRender->GetForceGPU_REF())
        amount = 0;

    dwPrecacheFrame = dwPrecacheTotal = amount;

    if (amount && b_draw_loadscreen && load_screen_renderer.b_registered == false)
    {
        load_screen_renderer.start(b_wait_user_input);
    }
}

void GetMonitorResolution(u32& horizontal, u32& vertical)
{
    HMONITOR hMonitor = MonitorFromWindow(Device.m_hWnd, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoA(hMonitor, &mi))
    {
        horizontal = mi.rcMonitor.right - mi.rcMonitor.left;
        vertical = mi.rcMonitor.bottom - mi.rcMonitor.top;
    }
    else
    {
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        horizontal = desktop.right - desktop.left;
        vertical = desktop.bottom - desktop.top;
    }
}

float GetMonitorRefresh()
{
    DEVMODE lpDevMode;
    memset(&lpDevMode, 0, sizeof(DEVMODE));
    lpDevMode.dmSize = sizeof(DEVMODE);
    lpDevMode.dmDriverExtra = 0;

    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode) == 0)
    {
        return 60.f;
    }
    else
        return lpDevMode.dmDisplayFrequency;
}

extern int ps_framelimiter;
extern u32 g_screenmode;

xr_list<CallMe::Delegate<bool()>> g_loading_events;

bool CRenderDevice::BeforeFrame()
{
    if (!b_is_Ready)
    {
        Sleep(100);
        return false;
    }

    if (psDeviceFlags.test(rsStatistic))
        g_bEnableStatGather = TRUE;
    else
        g_bEnableStatGather = FALSE;

    if (!g_loading_events.empty())
    {
        if (g_loading_events.front()())
            g_loading_events.pop_front();

        pApp->LoadDraw();
        return false;
    }

    return true;
}

void CRenderDevice::OnCameraUpdated()
{
    if (camFrame == dwFrame)
        return;

    // Precache
    if (dwPrecacheFrame)
    {
        const float factor = float(dwPrecacheFrame) / float(dwPrecacheTotal);
        float sin, cos;

        DirectX::XMScalarSinCos(&sin, &cos, PI_MUL_2 * factor);
        vCameraDirection.set(sin, 0, cos);
        vCameraDirection.normalize();
        vCameraTop.set(0, 1, 0);
        vCameraRight.crossproduct(vCameraTop, vCameraDirection);

        mView.build_camera_dir(vCameraPosition, vCameraDirection, vCameraTop);
    }

    // Matrices
    mFullTransform.mul(mProject, mView);
    mInvFullTransform.invert_44(mFullTransform);
    ::Render->OnCameraUpdated();
    m_pRender->SetCacheXform(mView, mProject);

    camFrame = dwFrame;
}

void CRenderDevice::ProcessFrame()
{
    if (!BeforeFrame())
        return;

    const auto FrameStartTime = std::chrono::high_resolution_clock::now();

    ImGui_ImplDX11_NewFrame(); // должно быть перед FrameMove

    FrameMove();
    OnCameraUpdated();

    std::chrono::time_point<std::chrono::high_resolution_clock> FrameEndTime;
    std::chrono::duration<double, std::milli> SecondThreadTasksElapsedTime;

    oneapi::tbb::parallel_invoke(
        [this, &FrameEndTime] {
            Statistic->RenderTOTAL_Real.FrameStart();
            Statistic->RenderTOTAL_Real.Begin();

            if (b_is_Active && RenderBegin())
            {
                seqRender.Process();

                if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic) || Statistic->errors.size())
                    Statistic->Show();

                Statistic->Show_HW_Stats();

                RenderEnd();
            }

            ImGui::EndFrame();

            Statistic->RenderTOTAL_Real.End();
            Statistic->RenderTOTAL_Real.FrameEnd();
            Statistic->RenderTOTAL.accum = Statistic->RenderTOTAL_Real.accum;

            FrameEndTime = std::chrono::high_resolution_clock::now();
        },
        [this, &SecondThreadTasksElapsedTime] {
            const auto SecondThreadTasksStartTime = std::chrono::high_resolution_clock::now();

            auto size = seqParallel.size();
            while (size-- > 0)
            {
                seqParallel.front()();
                seqParallel.pop_front();
            }

            seqFrameMT.Process();

            const auto SecondThreadTasksEndTime = std::chrono::high_resolution_clock::now();
            SecondThreadTasksElapsedTime = SecondThreadTasksEndTime - SecondThreadTasksStartTime;
        });

    const std::chrono::duration<double, std::milli> FrameElapsedTime = FrameEndTime - FrameStartTime;

    float curFPSLimit = ps_framelimiter;
    if (g_pGamePersistent->IsMainMenuActive() || Paused())
    {
        if (!refresh_rate) [[unlikely]]
            refresh_rate = GetMonitorRefresh();

        curFPSLimit = ps_framelimiter ? std::min<float>(ps_framelimiter, refresh_rate) : refresh_rate;
    }

    if (curFPSLimit > 0 && !m_SecondViewport.IsSVPFrame())
    {
        const std::chrono::duration<double, std::milli> FpsLimitMs{std::floor(1000.f / (curFPSLimit + 1))};
        const std::chrono::duration<double, std::milli> total = std::chrono::high_resolution_clock::now() - FrameStartTime;

        if (total < FpsLimitMs)
        {
            const auto TimeToSleep = FpsLimitMs - total;
            Sleep(iFloor(TimeToSleep.count()));
        }
    }

    bool show_stats = SecondThreadTasksElapsedTime > FrameElapsedTime;
    if (show_stats && !dwPrecacheFrame) [[unlikely]]
    {
        const std::chrono::duration<double, std::milli> SecondThreadFreeTime = FrameElapsedTime - SecondThreadTasksElapsedTime;

        Msg("##[%s] Second thread work time is too long! Avail: [%f]ms, used: [%f]ms, free: [%f]ms", __FUNCTION__, FrameElapsedTime.count(), SecondThreadTasksElapsedTime.count(),
            SecondThreadFreeTime.count());
    }

    if (!b_is_Active)
        Sleep(1);
}

void CRenderDevice::message_loop()
{
    MSG msg;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ProcessFrame();
    }
}

static void LogOsVersion()
{
    static auto RtlGetVersion = reinterpret_cast<NTSTATUS(WINAPI*)(LPOSVERSIONINFOEXW)>(GetProcAddress(GetModuleHandle("ntdll"), "RtlGetVersion"));

    if (RtlGetVersion)
    {
        OSVERSIONINFOEXW osInfo{};
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);

        if (NT_SUCCESS(RtlGetVersion(&osInfo)))
        {
            Msg("--OS Version major: [%d] minor: [%d], build: [%d]. Server OS: [%s]", osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber,
                osInfo.wProductType != VER_NT_WORKSTATION ? "yes" : "no");
            return;
        }
    }
    Msg("!![%s] Can't get RtlGetVersion", __FUNCTION__);
}

void CRenderDevice::Run()
{
    //	DUMP_PHASE;
    g_bLoaded = FALSE;

    LogOsVersion();

    Log("Starting engine...");
    set_current_thread_name("X-RAY Primary thread");
    Msg("Main thread id: [%u]", _Thrd_id());

    // Startup timers and calculate timer delta
    dwTimeGlobal = 0;
    Timer_MM_Delta = 0;
    {
        u32 time_mm = timeGetTime();
        while (timeGetTime() == time_mm)
            ; // wait for next tick
        u32 time_system = timeGetTime();
        u32 time_local = TimerAsync();
        Timer_MM_Delta = time_system - time_local;
    }

    // Message cycle
    seqAppStart.Process();

    m_pRender->ClearTarget();

    message_loop();

    seqAppEnd.Process();
}

u32 app_inactive_time = 0;
u32 app_inactive_time_start = 0;

void CRenderDevice::FrameMove()
{
    dwFrame++;
    dwTimeContinual = TimerMM.GetElapsed_ms() - app_inactive_time;

    // Timer
    float fPreviousFrameTime = Timer.GetElapsed_sec();
    Timer.Start(); // previous frame
    fTimeDelta = 0.1f * fTimeDelta + 0.9f * fPreviousFrameTime; // smooth random system activity - worst case ~7% error

    if (fTimeDelta > .1f)
        fTimeDelta = .1f; // limit to 15fps minimum

    if (fTimeDelta <= 0.f)
        fTimeDelta = EPS_S + EPS_S; // limit to 15fps minimum

    if (Paused())
        fTimeDelta = 0.0f;

    fTimeGlobal = TimerGlobal.GetElapsed_sec();
    const u32 _old_global = dwTimeGlobal;
    dwTimeGlobal = TimerGlobal.GetElapsed_ms();
    dwTimeDelta = dwTimeGlobal - _old_global;

    // Frame move
    Statistic->EngineTOTAL.Begin();

    seqFrame.Process();
    g_bLoaded = TRUE;

    Statistic->EngineTOTAL.End();
}

BOOL bShowPauseString = TRUE;

void CRenderDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{
    static int snd_emitters_ = -1;

    if (g_bBenchmark)
        return;

#ifdef DEBUG
//	Msg("pause [%s] timer=[%s] sound=[%s] reason=%s",bOn?"ON":"OFF", bTimer?"ON":"OFF", bSound?"ON":"OFF", reason);
#endif // DEBUG

    if (bOn)
    {
        if (!Paused())
            bShowPauseString =
#ifdef DEBUG
                !xr_strcmp(reason, "li_pause_key_no_clip") ? FALSE :
#endif // DEBUG
                                                             TRUE;

        if (bTimer)
        {
            g_pauseMngr->Pause(TRUE);
#ifdef DEBUG
            if (!xr_strcmp(reason, "li_pause_key_no_clip"))
                TimerGlobal.Pause(FALSE);
#endif // DEBUG
        }

        if (bSound && ::Sound)
        {
            snd_emitters_ = ::Sound->pause_emitters(true);
#ifdef DEBUG
//			Log("snd_emitters_[true]",snd_emitters_);
#endif // DEBUG
        }
    }
    else
    {
        if (bTimer && g_pauseMngr->Paused())
        {
            fTimeDelta = EPS_S + EPS_S;
            g_pauseMngr->Pause(FALSE);
        }

        if (bSound)
        {
            if (snd_emitters_ > 0) // avoid crash
            {
                snd_emitters_ = ::Sound->pause_emitters(false);
#ifdef DEBUG
//				Log("snd_emitters_[false]",snd_emitters_);
#endif // DEBUG
            }
            else
            {
#ifdef DEBUG
                Log("Sound->pause_emitters underflow");
#endif // DEBUG
            }
        }
    }
}

BOOL CRenderDevice::Paused() { return g_pauseMngr->Paused(); }

void CRenderDevice::OnWM_Activate(WPARAM wParam, LPARAM lParam)
{
    const u16 fActive = LOWORD(wParam);
    const BOOL fMinimized = (BOOL)HIWORD(wParam);
    const BOOL bActive = ((fActive != WA_INACTIVE) && (!fMinimized)) ? TRUE : FALSE;
    const BOOL isGameActive = ((psDeviceFlags.is(rsAlwaysActive) && g_screenmode != 2) || bActive) ? TRUE : FALSE;

    pInput->clip_cursor(fActive != WA_INACTIVE);

    if (isGameActive != b_is_Active)
    {
        b_is_Active = isGameActive;

        if (b_is_Active)
        {
            seqAppActivate.Process();
            app_inactive_time += TimerMM.GetElapsed_ms() - app_inactive_time_start;
        }
        else
        {
            app_inactive_time_start = TimerMM.GetElapsed_ms();
            seqAppDeactivate.Process();
        }
    }
}

CLoadScreenRenderer::CLoadScreenRenderer() : b_registered(false) {}

void CLoadScreenRenderer::start(bool b_user_input)
{
    Device.seqRender.Add(this, 0);
    b_registered = true;
    b_need_user_input = b_user_input;
}

void CLoadScreenRenderer::stop()
{
    if (!b_registered)
        return;
    Device.seqRender.Remove(this);
    pApp->DestroyLoadingScreen();
    b_registered = false;
    b_need_user_input = false;
}

void CLoadScreenRenderer::OnRender() { pApp->load_draw_internal(); }

void CRenderDevice::CSecondVPParams::SetSVPActive(bool bState) //--#SM+#-- +SecondVP+
{
    m_bIsActive = bState;
    if (g_pGamePersistent)
        g_pGamePersistent->m_pGShaderConstants.m_blender_mode.z = (m_bIsActive ? 1.0f : 0.0f);
}

bool CRenderDevice::CSecondVPParams::IsSVPFrame() //--#SM+#-- +SecondVP+
{
    bool cond = IsSVPActive() && ((Device.dwFrame % g_3dscopes_fps_factor) == 0);
    if (g_pGamePersistent)
        g_pGamePersistent->m_pGShaderConstants.m_blender_mode.y = cond ? 1.0f : 0.0f;
    return cond;
}

void CRenderDevice::time_factor(const float& time_factor)
{
    Timer.time_factor(time_factor);
    TimerGlobal.time_factor(time_factor);
    psSoundTimeFactor = time_factor; //--#SM+#--
}
