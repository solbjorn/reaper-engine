#include "stdafx.h"

void CRenderTarget::phase_accumulator()
{
    // Targets
    if (dwAccumulatorClearMark == Device.dwFrame)
    {
        // normal operation - setup
        if (!RImplementation.o.dx10_msaa)
            u_setrt(rt_Accumulator, NULL, NULL, HW.pBaseZB);
        else
            u_setrt(rt_Accumulator, NULL, NULL, rt_MSAADepth->pZRT);
    }
    else
    {
        // initial setup
        dwAccumulatorClearMark = Device.dwFrame;

        // clear
        if (!RImplementation.o.dx10_msaa)
            u_setrt(rt_Accumulator, NULL, NULL, HW.pBaseZB);
        else
            u_setrt(rt_Accumulator, NULL, NULL, rt_MSAADepth->pZRT);
        // dwLightMarkerID						= 5;					// start from 5, increment in 2 units
        reset_light_marker();
        //	Igor: AMD bug workaround. Should be fixed in 8.7 catalyst
        //	Need for MSAA to work correctly.
        if (RImplementation.o.dx10_msaa)
        {
            HW.pContext->OMSetRenderTargets(1, &(rt_Accumulator->pRT), 0);
        }
        //		u32		clr4clear					= color_rgba(0,0,0,0);	// 0x00
        // CHK_DX	(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));
        constexpr FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        HW.pContext->ClearRenderTargetView(rt_Accumulator->pRT, ColorRGBA);

        // Stencil	- draw only where stencil >= 0x1
        RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_ColorWriteEnable();
    }

    //	Restore viewport after shadow map rendering
    RImplementation.rmNormal();
}

void CRenderTarget::phase_vol_accumulator()
{
    if (RImplementation.o.ssfx_volumetric)
    {
        // SSS does not require the stencil. ( This also fix the MSAA bug )
        if (!m_bHasActiveVolumetric)
        {
            m_bHasActiveVolumetric = true;

            constexpr FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            HW.pContext->ClearRenderTargetView(rt_Generic_2->pRT, ColorRGBA);
        }

        u_setrt(rt_Generic_2, NULL, NULL, NULL);
    }
    else
    {
        if (!m_bHasActiveVolumetric)
        {
            m_bHasActiveVolumetric = true;
            if (!RImplementation.o.dx10_msaa)
                u_setrt(rt_Generic_2, NULL, NULL, HW.pBaseZB);
            else
                u_setrt(rt_Generic_2, NULL, NULL, RImplementation.Target->rt_MSAADepth->pZRT);
            // u32		clr4clearVol				= color_rgba(0,0,0,0);	// 0x00
            // CHK_DX	(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, clr4clearVol, 1.0f, 0L));
            constexpr FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            HW.pContext->ClearRenderTargetView(rt_Generic_2->pRT, ColorRGBA);
        }
        else
        {
            if (!RImplementation.o.dx10_msaa)
                u_setrt(rt_Generic_2, NULL, NULL, HW.pBaseZB);
            else
                u_setrt(rt_Generic_2, NULL, NULL, RImplementation.Target->rt_MSAADepth->pZRT);
        }
    }

    RCache.set_Stencil(FALSE);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_ColorWriteEnable();
}
