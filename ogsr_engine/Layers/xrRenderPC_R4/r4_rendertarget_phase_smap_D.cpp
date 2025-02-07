#include "stdafx.h"

void CRenderTarget::phase_smap_direct(light* L, u32 sub_phase)
{
    // Targets
    u_setrt(nullptr, nullptr, nullptr, rt_smap_depth->pZRT);

    HW.pContext->ClearDepthStencilView(rt_smap_depth->pZRT, D3D_CLEAR_DEPTH, 1.0f, 0L);

    //	Prepare viewport for shadow map rendering
    if (sub_phase != SE_SUN_RAIN_SMAP)
        RImplementation.rmNormal();
    else
    {
        const D3D_VIEWPORT VP = {(float)L->X.D.minX, (float)L->X.D.minY, (float)(L->X.D.maxX - L->X.D.minX), (float)(L->X.D.maxY - L->X.D.minY), 0, 1};
        // CHK_DX								(HW.pDevice->SetViewport(&VP));
        HW.pContext->RSSetViewports(1, &VP);
    }

    // Stencil	- disable
    RCache.set_Stencil(FALSE);
}
