#include "stdafx.h"

// startup
void CRenderTarget::phase_scene_prepare()
{
    PIX_EVENT(phase_scene_prepare);

    //	TODO: DX10: Check if we need to set RT here.
    if (RImplementation.o.dx10_msaa)
        u_setrt(RCache, Device.dwWidth, Device.dwHeight, rt_Position->pRT, NULL, NULL, rt_MSAADepth);
    else
        u_setrt(RCache, Device.dwWidth, Device.dwHeight, rt_Position->pRT, NULL, NULL, get_base_zb());

    // Thx to K.D.
    // We need to clean up G-buffer every frame to avoid "ghosting" on sky
    {
        constexpr Fcolor ColorRGBA{};

        RCache.ClearRT(rt_Position, ColorRGBA);
        RCache.ClearRT(rt_Color, ColorRGBA);
        RCache.ClearRT(rt_Accumulator, ColorRGBA);
        RCache.ClearZB(get_base_zb(), 1.0f, 0);

        if (RImplementation.o.dx10_msaa)
            RCache.ClearZB(rt_MSAADepth, 1.0f, 0);
    }

    //	Igor: for volumetric lights
    m_bHasActiveVolumetric = false;
    m_bHasActiveVolumetric_spot = false;
    //	Clear later if try to draw volumetric
}

// begin
void CRenderTarget::phase_scene_begin()
{
    // Enable ANISO
    SSManager.SetMaxAnisotropy(ps_r__tf_Anisotropic);

    // Targets, use accumulator for temporary storage
    if (RImplementation.o.dx10_msaa)
        u_setrt(RCache, rt_Position, rt_Color, NULL, rt_MSAADepth);
    else
        u_setrt(RCache, rt_Position, rt_Color, NULL, get_base_zb());

    // Stencil - write 0x1 at pixel pos
    RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

    // Misc		- draw only front-faces
    //	TODO: DX10: siable two-sided stencil here
    RCache.set_CullMode(CULL_CCW);
    RCache.set_ColorWriteEnable();
}

void CRenderTarget::disable_aniso()
{
    // Disable ANISO
    SSManager.SetMaxAnisotropy(1);
}
