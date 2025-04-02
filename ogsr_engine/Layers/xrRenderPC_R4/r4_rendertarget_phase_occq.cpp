#include "stdafx.h"

void CRenderTarget::phase_occq()
{
    u_setrt(RCache, Device.dwWidth, Device.dwHeight, RImplementation.o.dx10_msaa ? nullptr : get_base_rt(), nullptr, nullptr, rt_MSAADepth);

    RCache.set_Shader(s_occq);
    RCache.set_CullMode(CULL_CCW);
    RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
    RCache.set_ColorWriteEnable(FALSE);
}
