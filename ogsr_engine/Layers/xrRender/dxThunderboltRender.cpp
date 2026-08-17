#include "stdafx.h"

#include "dxThunderboltRender.h"

#include "../../xr_3da/thunderbolt.h"
#include "dxThunderboltDescRender.h"
#include "dxLensFlareRender.h"

dxThunderboltRender::dxThunderboltRender()
{
    // geom
    hGeom_model.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, SGeometry::default_vb(), SGeometry::default_ib());
    XR_ASSERT(hGeom_model.stride() == sizeof(IRender_DetailModel::fvfVertexOut));

    hGeom_gradient.create(FVF::F_LIT, SGeometry::default_vb(), RImplementation.QuadIB);
    XR_ASSERT(hGeom_gradient.stride() == sizeof(FVF::LIT));
}

dxThunderboltRender::~dxThunderboltRender()
{
    hGeom_model.destroy();
    hGeom_gradient.destroy();
}

void dxThunderboltRender::Copy(IThunderboltRender& _in)
{
    auto& in{*smart_cast<const dxThunderboltRender*>(&_in)};

    hGeom_model = in.hGeom_model;
    hGeom_gradient = in.hGeom_gradient;
}

void dxThunderboltRender::Render(CEffect_Thunderbolt& owner)
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // lightning model
    cmd_list.set_CullMode(CULL_NONE);

    float dv = owner.lightning_phase * 0.5f;
    dv = (owner.lightning_phase > 0.5f) ? Random.randI(2) * 0.5f : dv;

    auto& model = *smart_cast<dxThunderboltDescRender*>(&*XR_ASSERT_VAL(owner.current != nullptr)->m_pRender)->l_model;
    const auto vCount_Lock = model.vertices.size();
    const auto iCount_Lock = model.indices.size();

    const auto verts = cmd_list.Vertex.Lock<IRender_DetailModel::fvfVertexOut>(vCount_Lock);
    const auto indices = cmd_list.Index.Lock(iCount_Lock);

    // XForm verts
    model.transfer(owner.current_xform, verts, 0xffffffff, indices, 0, 0.0f, dv);

    // Flush if needed
    const auto v_offset = cmd_list.Vertex.Unlock<IRender_DetailModel::fvfVertexOut>(vCount_Lock);
    const auto i_offset = cmd_list.Index.Unlock(iCount_Lock);

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Shader(model.shader);
    cmd_list.set_Geometry(hGeom_model);
    cmd_list.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, iCount_Lock / 3);

    // gradient
    cmd_list.set_CullMode(CULL_CCW);

    const auto verts_grad = cmd_list.Vertex.Lock<FVF::LIT>(8);

    // top
    u32 c_val = iFloor(owner.current->m_GradientTop->fOpacity * owner.lightning_phase * 255.f);
    u32 c = color_rgba(c_val, c_val, c_val, c_val);

    Fvector3 vecSx;
    vecSx.mul(Device.vCameraRight, owner.current->m_GradientTop->fRadius.x * owner.lightning_size);
    Fvector3 vecSy;
    vecSy.mul(Device.vCameraTop, -owner.current->m_GradientTop->fRadius.y * owner.lightning_size);

    verts_grad[0].set(owner.current_xform.c.x + vecSx.x - vecSy.x, owner.current_xform.c.y + vecSx.y - vecSy.y, owner.current_xform.c.z + vecSx.z - vecSy.z, c,
                      0, 0);
    verts_grad[1].set(owner.current_xform.c.x + vecSx.x + vecSy.x, owner.current_xform.c.y + vecSx.y + vecSy.y, owner.current_xform.c.z + vecSx.z + vecSy.z, c,
                      0, 1);
    verts_grad[2].set(owner.current_xform.c.x - vecSx.x - vecSy.x, owner.current_xform.c.y - vecSx.y - vecSy.y, owner.current_xform.c.z - vecSx.z - vecSy.z, c,
                      1, 0);
    verts_grad[3].set(owner.current_xform.c.x - vecSx.x + vecSy.x, owner.current_xform.c.y - vecSx.y + vecSy.y, owner.current_xform.c.z - vecSx.z + vecSy.z, c,
                      1, 1);

    // center
    c_val = iFloor(owner.current->m_GradientCenter->fOpacity * owner.lightning_phase * 255.f);
    c = color_rgba(c_val, c_val, c_val, c_val);

    vecSx.mul(Device.vCameraRight, owner.current->m_GradientCenter->fRadius.x * owner.lightning_size);
    vecSy.mul(Device.vCameraTop, -owner.current->m_GradientCenter->fRadius.y * owner.lightning_size);

    verts_grad[4].set(owner.lightning_center.x + vecSx.x - vecSy.x, owner.lightning_center.y + vecSx.y - vecSy.y, owner.lightning_center.z + vecSx.z - vecSy.z,
                      c, 0, 0);
    verts_grad[5].set(owner.lightning_center.x + vecSx.x + vecSy.x, owner.lightning_center.y + vecSx.y + vecSy.y, owner.lightning_center.z + vecSx.z + vecSy.z,
                      c, 0, 1);
    verts_grad[6].set(owner.lightning_center.x - vecSx.x - vecSy.x, owner.lightning_center.y - vecSx.y - vecSy.y, owner.lightning_center.z - vecSx.z - vecSy.z,
                      c, 1, 0);
    verts_grad[7].set(owner.lightning_center.x - vecSx.x + vecSy.x, owner.lightning_center.y - vecSx.y + vecSy.y, owner.lightning_center.z - vecSx.z + vecSy.z,
                      c, 1, 1);

    const auto VS_Offset = cmd_list.Vertex.Unlock<FVF::LIT>(8);

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Geometry(hGeom_gradient);

    cmd_list.set_Shader(smart_cast<dxFlareRender*>(&*owner.current->m_GradientTop->m_pFlare)->hShader);

    //	Hack. Since lightning gradient uses sun shader override z write settings manually
    cmd_list.set_Z(TRUE);
    cmd_list.set_ZFunc(D3DCMP_LESSEQUAL);
    cmd_list.Render(D3DPT_TRIANGLELIST, VS_Offset, 0, 4, 0, 2);

    cmd_list.set_Shader(smart_cast<dxFlareRender*>(&*owner.current->m_GradientCenter->m_pFlare)->hShader);

    //	Hack. Since lightning gradient uses sun shader override z write settings manually
    cmd_list.set_Z(TRUE);
    cmd_list.set_ZFunc(D3DCMP_LESSEQUAL);
    cmd_list.Render(D3DPT_TRIANGLELIST, VS_Offset + 4, 0, 4, 0, 2);
}
