#include "stdafx.h"

void CRenderTarget::phase_smap_direct(light* L, u32 sub_phase)
{
    // Targets
    u_setrt(RCache, nullptr, nullptr, nullptr, rt_smap_depth);

    RCache.ClearZB(rt_smap_depth, 1.0f);

    //	Prepare viewport for shadow map rendering
    if (sub_phase != SE_SUN_RAIN_SMAP)
        RImplementation.rmNormal(RCache);
    else
    {
        const D3D_VIEWPORT VP = {(float)L->X.D.minX, (float)L->X.D.minY, (float)(L->X.D.maxX - L->X.D.minX), (float)(L->X.D.maxY - L->X.D.minY), 0, 1};
        RCache.SetViewport(VP);
    }

    // Stencil	- disable
    RCache.set_Stencil(FALSE);
}
