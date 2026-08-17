#include "stdafx.h"

void CRenderTarget::phase_occq()
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, RImplementation.o.dx10_msaa ? nullptr : get_base_rt(), nullptr, nullptr, rt_MSAADepth);

    cmd_list.set_Shader(s_occq);
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
    cmd_list.set_ColorWriteEnable(FALSE);
}
