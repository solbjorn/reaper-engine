#include "stdafx.h"

void CRenderTarget::phase_smap_spot_clear() { HW.pContext->ClearDepthStencilView(rt_smap_depth->pZRT, D3D_CLEAR_DEPTH, 1.0f, 0L); }

void CRenderTarget::phase_smap_spot(light* L)
{
    // Targets + viewport
    u_setrt(nullptr, nullptr, nullptr, rt_smap_depth->pZRT);

    const D3D_VIEWPORT VP = {(float)L->X.S.posX, (float)L->X.S.posY, (float)L->X.S.size, (float)L->X.S.size, 0, 1};
    // CHK_DX								(HW.pDevice->SetViewport(&VP));
    HW.pContext->RSSetViewports(1, &VP);

    // Misc		- draw only front-faces //back-faces
    RCache.set_CullMode(CULL_CCW);
    RCache.set_Stencil(FALSE);
// no transparency
#pragma todo("can optimize for multi-lights covering more than say 50%...")
    RCache.set_ColorWriteEnable(FALSE);
}
