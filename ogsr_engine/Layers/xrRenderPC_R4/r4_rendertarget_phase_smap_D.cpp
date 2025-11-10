#include "stdafx.h"

void CRenderTarget::phase_smap_direct(CBackend& cmd_list, const light* L, u32 sub_phase)
{
    // Targets
    rt_smap_depth->set_slice_write(cmd_list.context_id, cmd_list.context_id);
    u_setrt(cmd_list, {}, {}, {}, rt_smap_depth);
    cmd_list.ClearZB(rt_smap_depth, 1.0f);

    //	Prepare viewport for shadow map rendering
    if (sub_phase != SE_SUN_RAIN_SMAP)
    {
        RImplementation.rmNormal(cmd_list);
    }
    else
    {
        const D3D_VIEWPORT VP{(float)L->X.D.minX, (float)L->X.D.minY, (float)(L->X.D.maxX - L->X.D.minX), (float)(L->X.D.maxY - L->X.D.minY), 0, 1};
        cmd_list.SetViewport(VP);
    }

    // Stencil	- disable
    cmd_list.set_Stencil(FALSE);
}
