#include "stdafx.h"

void CRenderTarget::phase_smap_spot_clear() { RCache.ClearZB(rt_smap_depth, 1.0f); }

void CRenderTarget::phase_smap_spot(light* L)
{
    // Targets + viewport
    u_setrt(RCache, nullptr, nullptr, nullptr, rt_smap_depth);

    const D3D_VIEWPORT VP = {(float)L->X.S.posX, (float)L->X.S.posY, (float)L->X.S.size, (float)L->X.S.size, 0, 1};
    RCache.SetViewport(VP);

    // Misc		- draw only front-faces //back-faces
    RCache.set_CullMode(CULL_CCW);
    RCache.set_Stencil(FALSE);
// no transparency
#pragma todo("can optimize for multi-lights covering more than say 50%...")
    RCache.set_ColorWriteEnable(FALSE);
}
