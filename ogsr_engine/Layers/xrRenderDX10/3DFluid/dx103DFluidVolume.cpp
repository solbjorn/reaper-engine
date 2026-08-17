#include "stdafx.h"

#include "dx103DFluidVolume.h"
#include "dx103DFluidManager.h"

dx103DFluidVolume::dx103DFluidVolume() = default;
dx103DFluidVolume::~dx103DFluidVolume() = default;

void dx103DFluidVolume::Load(LPCSTR, IReader* data, u32)
{
    //	Uncomment this if choose to read from OGF
    //	dxRender_Visual::Load		(N,data,dwFlags);

    //	Create shader for correct sort while rendering
    //	shader name can't start from a digit
    shader.create("fluid3d_stub", "water\\water_ryaska1");

    Type = MT_3DFLUIDVOLUME;

    //	Version 3>
    m_FluidData.Load(data);

    //	Prepare transform
    const Fmatrix& Transform = m_FluidData.GetTransform();

    //	Update visibility data
    vis.box.min.set(-0.5f, -0.5f, -0.5f);
    vis.box.max.set(0.5f, 0.5f, 0.5f);

    vis.box.xform(Transform);

    vis.box.getcenter(vis.sphere.P);
    vis.sphere.R = vis.box.getradius();
}

void dx103DFluidVolume::Render(CBackend& cmd_list, f32, bool)
{
    if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;

    XR_TRACY_ZONE_SCOPED();

    FluidManager.Update(cmd_list, m_FluidData, 2.0f);
    FluidManager.RenderFluid(cmd_list, m_FluidData);
}

void dx103DFluidVolume::Copy(dxRender_Visual* pFrom) { dxRender_Visual::Copy(pFrom); }
void dx103DFluidVolume::Release() { dxRender_Visual::Release(); }
