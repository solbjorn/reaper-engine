#include "stdafx.h"

// startup
void CRenderTarget::phase_scene_prepare()
{
    PIX_EVENT(phase_scene_prepare);

    //	TODO: DX10: Check if we need to set RT here.
    if (!RImplementation.o.dx10_msaa)
        u_setrt(Device.dwWidth, Device.dwHeight, rt_Position->pRT, NULL, NULL, HW.pBaseZB);
    else
        u_setrt(Device.dwWidth, Device.dwHeight, rt_Position->pRT, NULL, NULL, rt_MSAADepth->pZRT);

    // Thx to K.D.
    // We need to clean up G-buffer every frame to avoid "ghosting" on sky
    {
        constexpr float ColorRGBA[]{0.0f, 0.0f, 0.0f, 0.0f};

        HW.pContext->ClearRenderTargetView(rt_Position->pRT, ColorRGBA);
        HW.pContext->ClearRenderTargetView(rt_Color->pRT, ColorRGBA);
        HW.pContext->ClearRenderTargetView(rt_Accumulator->pRT, ColorRGBA);
        HW.pContext->ClearDepthStencilView(HW.pBaseZB, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);

        if (RImplementation.o.dx10_msaa)
            HW.pContext->ClearDepthStencilView(rt_MSAADepth->pZRT, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);
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

    ID3DDepthStencilView* pZB = HW.pBaseZB;

    if (RImplementation.o.dx10_msaa)
        pZB = rt_MSAADepth->pZRT;

    // Targets, use accumulator for temporary storage
    u_setrt(rt_Position, rt_Color, NULL, pZB);

    // Stencil - write 0x1 at pixel pos
    RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

    // Misc		- draw only front-faces
    //	TODO: DX10: siable two-sided stencil here
    // CHK_DX(HW.pDevice->SetRenderState	( D3DRS_TWOSIDEDSTENCILMODE,FALSE				));
    RCache.set_CullMode(CULL_CCW);
    RCache.set_ColorWriteEnable();
}

void CRenderTarget::disable_aniso()
{
    // Disable ANISO
    SSManager.SetMaxAnisotropy(1);
}
