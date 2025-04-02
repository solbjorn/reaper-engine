#include "stdafx.h"

void CRenderTarget::phase_accumulator(CBackend& cmd_list)
{
    // Targets
    if (dwAccumulatorClearMark == Device.dwFrame)
    {
        // normal operation - setup
        u_setrt(cmd_list, rt_Accumulator, NULL, NULL, rt_MSAADepth);
    }
    else
    {
        // initial setup
        dwAccumulatorClearMark = Device.dwFrame;

        // clear
        u_setrt(cmd_list, rt_Accumulator, NULL, NULL, rt_MSAADepth);
        reset_light_marker(cmd_list);

        //	Igor: AMD bug workaround. Should be fixed in 8.7 catalyst
        //	Need for MSAA to work correctly.
        if (RImplementation.o.dx10_msaa)
            cmd_list.context()->OMSetRenderTargets(1, &rt_Accumulator->pRT, 0);

        cmd_list.ClearRT(rt_Accumulator, {});

        // Stencil	- draw only where stencil >= 0x1
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_ColorWriteEnable();
    }

    //	Restore viewport after shadow map rendering
    RImplementation.rmNormal(cmd_list);
}

void CRenderTarget::phase_vol_accumulator(CBackend& cmd_list)
{
    if (RImplementation.o.ssfx_volumetric)
    {
        // SSS does not require the stencil. ( This also fix the MSAA bug )
        if (!m_bHasActiveVolumetric)
        {
            m_bHasActiveVolumetric = true;
            cmd_list.ClearRT(rt_Generic_2, {});
        }

        u_setrt(cmd_list, rt_Generic_2, NULL, NULL, NULL);
    }
    else
    {
        if (!m_bHasActiveVolumetric)
        {
            m_bHasActiveVolumetric = true;

            u_setrt(cmd_list, rt_Generic_2, NULL, NULL, rt_MSAADepth);
            cmd_list.ClearRT(rt_Generic_2, {});
        }
        else
        {
            u_setrt(cmd_list, rt_Generic_2, NULL, NULL, rt_MSAADepth);
        }
    }

    cmd_list.set_Stencil(FALSE);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_ColorWriteEnable();
}
