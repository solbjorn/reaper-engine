#include "stdafx.h"

void CRenderTarget::phase_smap_spot_clear(CBackend& cmd_list)
{
    rt_smap_depth->set_slice_write(cmd_list.context_id, cmd_list.context_id);
    u_setrt(cmd_list, {}, {}, {}, rt_smap_depth);
    cmd_list.ClearZB(rt_smap_depth, 1.0f);
}

void CRenderTarget::phase_smap_spot(CBackend& cmd_list, const light* L)
{
    // Targets + viewport
    rt_smap_depth->set_slice_write(cmd_list.context_id, cmd_list.context_id);
    u_setrt(cmd_list, {}, {}, {}, rt_smap_depth);

    const D3D_VIEWPORT VP{(float)L->X.S.posX, (float)L->X.S.posY, (float)L->X.S.size, (float)L->X.S.size, 0, 1};
    cmd_list.SetViewport(VP);

    // Misc - draw only front-faces //back-faces
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(FALSE);
    // no transparency
    cmd_list.set_ColorWriteEnable(FALSE);
}
