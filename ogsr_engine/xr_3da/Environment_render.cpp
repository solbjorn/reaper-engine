#include "stdafx.h"

#include "Environment.h"
#include "render.h"
#include "xr_efflensflare.h"
#include "rain.h"
#include "thunderbolt.h"
#include "igame_level.h"
#include "xr_task.h"

//-----------------------------------------------------------------------------
// Environment render
//-----------------------------------------------------------------------------
// BOOL bNeed_re_create_env = FALSE;
void CEnvironment::RenderSky()
{
    if (0 == g_pGameLevel)
        return;

    m_pRender->RenderSky(*this);
}

void CEnvironment::RenderClouds()
{
    if (0 == g_pGameLevel)
        return;

    // draw clouds
    if (fis_zero(CurrentEnv->clouds_color.w, EPS_L))
        return;

    m_pRender->RenderClouds(*this);
}

void CEnvironment::RenderFlares()
{
    if (0 == g_pGameLevel)
        return;
    // 1
    eff_LensFlare->Render(FALSE, TRUE, TRUE);
}

void CEnvironment::RenderLast()
{
    tg->wait();

    // 2
    eff_Rain->Render();
    eff_Thunderbolt->Render();
}

void CEnvironment::OnDeviceCreate()
{
    m_pRender->OnDeviceCreate();
    tg = &xr_task_group_get();

    Invalidate();
    OnFrame();
}

void CEnvironment::OnDeviceDestroy()
{
    tg->cancel();
    tg->put();

    m_pRender->OnDeviceDestroy();
    CurrentEnv->destroy();
}
