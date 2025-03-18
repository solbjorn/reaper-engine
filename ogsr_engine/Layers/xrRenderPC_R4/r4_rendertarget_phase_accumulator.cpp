#include "stdafx.h"

void CRenderTarget::phase_accumulator()
{
    // Targets
    if (dwAccumulatorClearMark == Device.dwFrame)
    {
        // normal operation - setup
        if (!RImplementation.o.dx10_msaa)
            u_setrt(RCache, rt_Accumulator, NULL, NULL, get_base_zb());
        else
            u_setrt(RCache, rt_Accumulator, NULL, NULL, rt_MSAADepth);
    }
    else
    {
        // initial setup
        dwAccumulatorClearMark = Device.dwFrame;

        // clear
        if (!RImplementation.o.dx10_msaa)
            u_setrt(RCache, rt_Accumulator, NULL, NULL, get_base_zb());
        else
            u_setrt(RCache, rt_Accumulator, NULL, NULL, rt_MSAADepth);

        reset_light_marker(RCache);

        //	Igor: AMD bug workaround. Should be fixed in 8.7 catalyst
        //	Need for MSAA to work correctly.
        if (RImplementation.o.dx10_msaa)
            RCache.context()->OMSetRenderTargets(1, &rt_Accumulator->pRT, 0);

        RCache.ClearRT(rt_Accumulator, {});

        // Stencil	- draw only where stencil >= 0x1
        RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_ColorWriteEnable();
    }

    //	Restore viewport after shadow map rendering
    RImplementation.rmNormal(RCache);
}

void CRenderTarget::phase_vol_accumulator()
{
    if (RImplementation.o.ssfx_volumetric)
    {
        // SSS does not require the stencil. ( This also fix the MSAA bug )
        if (!m_bHasActiveVolumetric)
        {
            m_bHasActiveVolumetric = true;
            RCache.ClearRT(rt_Generic_2, {});
        }

        u_setrt(RCache, rt_Generic_2, NULL, NULL, NULL);
    }
    else
    {
        if (!m_bHasActiveVolumetric)
        {
            m_bHasActiveVolumetric = true;
            if (!RImplementation.o.dx10_msaa)
                u_setrt(RCache, rt_Generic_2, NULL, NULL, get_base_zb());
            else
                u_setrt(RCache, rt_Generic_2, NULL, NULL, rt_MSAADepth);

            RCache.ClearRT(rt_Generic_2, {});
        }
        else
        {
            if (!RImplementation.o.dx10_msaa)
                u_setrt(RCache, rt_Generic_2, NULL, NULL, get_base_zb());
            else
                u_setrt(RCache, rt_Generic_2, NULL, NULL, rt_MSAADepth);
        }
    }

    RCache.set_Stencil(FALSE);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_ColorWriteEnable();
}
