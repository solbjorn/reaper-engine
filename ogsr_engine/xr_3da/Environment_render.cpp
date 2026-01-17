#include "stdafx.h"

#include "Environment.h"

#include "render.h"
#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"
#include "igame_level.h"

//-----------------------------------------------------------------------------
// Environment render
//-----------------------------------------------------------------------------

void CEnvironment::RenderSky()
{
    if (!g_pGameLevel)
        return;

    m_pRender->RenderSky(*this);
}

void CEnvironment::RenderClouds()
{
    if (!g_pGameLevel)
        return;

    // draw clouds
    if (fis_zero(CurrentEnv->clouds_color.w, EPS_L))
        return;

    m_pRender->RenderClouds(*this);
}

void CEnvironment::RenderFlares()
{
    if (!g_pGameLevel)
        return;
    // 1
    eff_LensFlare->Render(false, true, true);
}

tmc::task<void> CEnvironment::RenderLast()
{
    co_await event;

    // 2
    eff_Rain->Render();
    eff_Thunderbolt->Render();
}

tmc::task<void> CEnvironment::OnDeviceCreate()
{
    m_pRender->OnDeviceCreate();

    Invalidate();
    co_await OnFrame();
}

void CEnvironment::OnDeviceDestroy()
{
    m_pRender->OnDeviceDestroy();
    CurrentEnv->put();
}
