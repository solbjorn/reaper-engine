// file: D3DUtils.cpp

#include "stdafx.h"

#include "D3DUtils.h"

#include "../../xr_3da/GameFont.h"
#include "du_box.h"
#include "du_sphere.h"
#include "du_sphere_part.h"
#include "du_cone.h"
#include "du_cylinder.h"

#include "dxRenderDeviceRender.h"

CDrawUtilities DUImpl;

namespace
{
#define LINE_DIVISION 32 // не меньше 6!!!!!
// for drawing sphere
Fvector circledef1[LINE_DIVISION];
Fvector circledef2[LINE_DIVISION];
Fvector circledef3[LINE_DIVISION];

constexpr u32 boxcolor{D3DCOLOR_RGBA(255, 255, 255, 0)};

constexpr u32 boxvertcount{48};
Fvector boxvert[boxvertcount];

#define DU_DRAW_SH_C(cl, sh, c) \
    { \
        cl.set_Shader(sh); \
        Fvector4 tfactor{gsl::narrow_cast<f32>(color_get_R(c)), gsl::narrow_cast<f32>(color_get_G(c)), gsl::narrow_cast<f32>(color_get_B(c)), \
                         gsl::narrow_cast<f32>(color_get_A(c))}; \
        constexpr Fvector4 divisor{255.0f, 255.0f, 255.0f, 255.0f}; \
        tfactor.div(divisor); \
        cl.set_c("tfactor", tfactor); \
    } \
    XR_MACRO_END()

#define DU_DRAW_SH(cl, sh) \
    { \
        cl.set_Shader(sh); \
        constexpr Fvector4 tfactor{1.0f, 1.0f, 1.0f, 1.0f}; \
        cl.set_c("tfactor", tfactor); \
    } \
    XR_MACRO_END()

#define FILL_MODE D3DFILL_SOLID
#define SHADE_MODE D3DSHADE_GOURAUD
#define SCREEN_QUALITY 1.f

// identity box
constexpr u32 identboxwirecount{24};
constexpr Fvector identboxwire[identboxwirecount]{
    Fvector{-0.5f, -0.5f, -0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, -0.5f},
    Fvector{+0.5f, -0.5f, -0.5f}, Fvector{+0.5f, -0.5f, -0.5f}, Fvector{-0.5f, -0.5f, -0.5f}, Fvector{-0.5f, +0.5f, +0.5f}, Fvector{+0.5f, +0.5f, +0.5f},
    Fvector{+0.5f, +0.5f, +0.5f}, Fvector{+0.5f, -0.5f, +0.5f}, Fvector{+0.5f, -0.5f, +0.5f}, Fvector{-0.5f, -0.5f, +0.5f}, Fvector{-0.5f, -0.5f, +0.5f},
    Fvector{-0.5f, +0.5f, +0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{-0.5f, +0.5f, +0.5f}, Fvector{+0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, +0.5f},
    Fvector{+0.5f, -0.5f, -0.5f}, Fvector{+0.5f, -0.5f, +0.5f}, Fvector{-0.5f, -0.5f, -0.5f}, Fvector{-0.5f, -0.5f, +0.5f}};

#define SIGN(x) ((x < 0) ? -1 : 1)

xr_vector<FVF::L> m_GridPoints;

constexpr u32 m_ColorGrid{0xff909090};
constexpr u32 m_ColorGridTh{0xffb4b4b4};
constexpr u32 m_SelectionRect{D3DCOLOR_RGBA(127, 255, 127, 64)};
} // namespace

void SPrimitiveBuffer::Destroy()
{
    if (pGeom)
    {
        HW.stats_manager.decrement_stats_vb(pGeom->vb);
        HW.stats_manager.decrement_stats_ib(pGeom->ib);
        _RELEASE(pGeom->vb);
        _RELEASE(pGeom->ib);
        pGeom.destroy();
    }
}

void CDrawUtilities::UpdateGrid(int number_of_cell, float square_size, int subdiv)
{
    m_GridPoints.clear();
    // grid
    int m_GridSubDiv[2];
    int m_GridCounts[2];
    Fvector2 m_GridStep;

    m_GridStep.set(square_size, square_size);
    m_GridSubDiv[0] = subdiv;
    m_GridSubDiv[1] = subdiv;
    m_GridCounts[0] = number_of_cell; // iFloor(size/step)*subdiv;
    m_GridCounts[1] = number_of_cell; // iFloor(size/step)*subdiv;

    FVF::L left, right;
    left.p.y = right.p.y = 0;

    for (int thin = 0; thin < 2; thin++)
    {
        for (int i = -m_GridCounts[0]; i <= m_GridCounts[0]; i++)
        {
            if ((!!thin) != !!(i % m_GridSubDiv[0]))
            {
                left.p.z = -m_GridCounts[1] * m_GridStep.y;
                right.p.z = m_GridCounts[1] * m_GridStep.y;
                left.p.x = i * m_GridStep.x;
                right.p.x = left.p.x;
                left.color = (i % m_GridSubDiv[0]) ? m_ColorGrid : m_ColorGridTh;
                right.color = left.color;
                m_GridPoints.push_back(left);
                m_GridPoints.push_back(right);
            }
        }
        for (int i = -m_GridCounts[1]; i <= m_GridCounts[1]; i++)
        {
            if ((!!thin) != !!(i % m_GridSubDiv[1]))
            {
                left.p.x = -m_GridCounts[0] * m_GridStep.x;
                right.p.x = m_GridCounts[0] * m_GridStep.x;
                left.p.z = i * m_GridStep.y;
                right.p.z = left.p.z;
                left.color = (i % m_GridSubDiv[1]) ? m_ColorGrid : m_ColorGridTh;
                right.color = left.color;
                m_GridPoints.push_back(left);
                m_GridPoints.push_back(right);
            }
        }
    }
}

void CDrawUtilities::OnDeviceCreate()
{
    Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);

    m_SolidBox.CreateFromData(D3DPT_TRIANGLELIST, DU_BOX_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_box_vertices.data(), DU_BOX_NUMVERTEX, du_box_faces.data(),
                              DU_BOX_NUMFACES * 3);
    m_SolidCone.CreateFromData(D3DPT_TRIANGLELIST, DU_CONE_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cone_vertices.data(), DU_CONE_NUMVERTEX,
                               du_cone_faces.data(), DU_CONE_NUMFACES * 3);
    m_SolidSphere.CreateFromData(D3DPT_TRIANGLELIST, DU_SPHERE_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_vertices.data(), DU_SPHERE_NUMVERTEX,
                                 du_sphere_faces.data(), DU_SPHERE_NUMFACES * 3);
    m_SolidSpherePart.CreateFromData(D3DPT_TRIANGLELIST, DU_SPHERE_PART_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_part_vertices.data(),
                                     DU_SPHERE_PART_NUMVERTEX, du_sphere_part_faces.data(), DU_SPHERE_PART_NUMFACES * 3);
    m_SolidCylinder.CreateFromData(D3DPT_TRIANGLELIST, DU_CYLINDER_NUMFACES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cylinder_vertices.data(), DU_CYLINDER_NUMVERTEX,
                                   du_cylinder_faces.data(), DU_CYLINDER_NUMFACES * 3);
    m_WireBox.CreateFromData(D3DPT_LINELIST, DU_BOX_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_box_vertices.data(), DU_BOX_NUMVERTEX, du_box_lines.data(),
                             DU_BOX_NUMLINES * 2);
    m_WireCone.CreateFromData(D3DPT_LINELIST, DU_CONE_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cone_vertices.data(), DU_CONE_NUMVERTEX, du_cone_lines.data(),
                              DU_CONE_NUMLINES * 2);
    m_WireSphere.CreateFromData(D3DPT_LINELIST, DU_SPHERE_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_verticesl.data(), DU_SPHERE_NUMVERTEXL,
                                du_sphere_lines.data(), DU_SPHERE_NUMLINES * 2);
    m_WireSpherePart.CreateFromData(D3DPT_LINELIST, DU_SPHERE_PART_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_sphere_part_vertices.data(),
                                    DU_SPHERE_PART_NUMVERTEX, du_sphere_part_lines.data(), DU_SPHERE_PART_NUMLINES * 2);
    m_WireCylinder.CreateFromData(D3DPT_LINELIST, DU_CYLINDER_NUMLINES, D3DFVF_XYZ | D3DFVF_DIFFUSE, du_cylinder_vertices.data(), DU_CYLINDER_NUMVERTEX,
                                  du_cylinder_lines.data(), DU_CYLINDER_NUMLINES * 2);

    for (u32 i = 0; i < LINE_DIVISION; i++)
    {
        float _sa, _ca;
        DirectX::XMScalarSinCos(&_sa, &_ca, M_PI * 2.f * (i / (float)LINE_DIVISION));

        circledef1[i].set(_ca, _sa, 0);
        circledef2[i].set(0, _ca, _sa);
        circledef3[i].set(_sa, 0, _ca);
    }

    // initialize identity box
    constexpr Fbox bb{-0.505f, -0.505f, -0.505f, 0.505f, 0.505f, 0.505f};
    for (u32 i = 0; i < 8; i++)
    {
        Fvector S;
        Fvector p;
        bb.getpoint(i, p);
        S.set((float)SIGN(p.x), (float)SIGN(p.y), (float)SIGN(p.z));
        boxvert[i * 6 + 0].set(p);
        boxvert[i * 6 + 1].set(p.x - S.x * 0.25f, p.y, p.z);
        boxvert[i * 6 + 2].set(p);
        boxvert[i * 6 + 3].set(p.x, p.y - S.y * 0.25f, p.z);
        boxvert[i * 6 + 4].set(p);
        boxvert[i * 6 + 5].set(p.x, p.y, p.z - S.z * 0.25f);
    }

    // create render stream
    vs_L.create(FVF::F_L, SGeometry::default_vb(), SGeometry::default_ib());
    XR_ASSERT(vs_L.stride() == sizeof(FVF::L));

    vs_TL.create(FVF::F_TL, SGeometry::default_vb(), SGeometry::default_ib());
    XR_ASSERT(vs_TL.stride() == sizeof(FVF::TL));

    vs_LIT.create(FVF::F_LIT, SGeometry::default_vb(), SGeometry::default_ib());
    XR_ASSERT(vs_LIT.stride() == sizeof(FVF::LIT));

    m_Font = xr_new<CGameFont>("stat_font");
}

void CDrawUtilities::OnDeviceDestroy()
{
    xr_delete(m_Font);

    vs_L.destroy();
    vs_TL.destroy();
    vs_LIT.destroy();

    m_SolidBox.Destroy();
    m_SolidCone.Destroy();
    m_SolidSphere.Destroy();
    m_SolidSpherePart.Destroy();
    m_SolidCylinder.Destroy();
    m_WireBox.Destroy();
    m_WireCone.Destroy();
    m_WireSphere.Destroy();
    m_WireSpherePart.Destroy();
    m_WireCylinder.Destroy();

    Device.seqRender.Remove(this);
}

//----------------

void CDrawUtilities::DrawSpotLight(const Fvector& p, const Fvector& d, float range, float phi, u32 clr)
{
    constexpr float da = PI_MUL_2 / LINE_DIVISION;

    f32 a, b;
    DirectX::XMScalarSinCos(&a, &b, PI_DIV_2 - phi / 2.0f);
    a *= range;
    b *= range;

    f32 H, P;
    d.getHP(H, P);

    Fmatrix T;
    T.setHPB(H, P, 0);
    T.translate_over(p);
    Fvector p1;

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(LINE_DIVISION * 2 + 2);

    for (auto [i, v] : std::views::enumerate(verts | std::views::chunk(2) | std::views::take(LINE_DIVISION)))
    {
        f32 _sa, _ca;
        DirectX::XMScalarSinCos(&_sa, &_ca, da * gsl::narrow_cast<f32>(i));

        p1.x = b * _ca;
        p1.y = b * _sa;
        p1.z = a;
        T.transform_tiny(p1);

        // fill VB
        v[0].set(p, clr);
        v[1].set(p1, clr);
    }

    p1.mad(p, d, range);

    verts[LINE_DIVISION * 2].set(p, clr);
    verts[LINE_DIVISION * 2 + 1].set(p1, clr);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(LINE_DIVISION * 2 + 2);
    // and Render it as triangle list
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, LINE_DIVISION + 1);
}

void CDrawUtilities::DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, u32 c)
{
    float r = radius * 0.71f;
    Fvector R, N, D;
    D.normalize(d);
    Fmatrix rot;

    N.set(0, 1, 0);
    if (_abs(D.y) > 0.99f)
        N.set(1, 0, 0);
    R.crossproduct(N, D);
    R.normalize();
    N.crossproduct(D, R);
    N.normalize();
    rot.set(R, N, D, p);
    float sz = radius + range;

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(6);

    verts[0].set(0, 0, r, c);
    rot.transform_tiny(verts[0].p);
    verts[1].set(0, 0, sz, c);
    rot.transform_tiny(verts[1].p);
    verts[2].set(-r, 0, r, c);
    rot.transform_tiny(verts[2].p);
    verts[3].set(-r, 0, sz, c);
    rot.transform_tiny(verts[3].p);
    verts[4].set(r, 0, r, c);
    rot.transform_tiny(verts[4].p);
    verts[5].set(r, 0, sz, c);
    rot.transform_tiny(verts[5].p);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(6);
    // and Render it as triangle list
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, 3);

    Fbox b;
    b.min.set(-r, -r, -r);
    b.max.set(r, r, r);

    DrawLineSphere(p, radius, c, true);
}

void CDrawUtilities::DrawPointLight(const Fvector& p, float radius, u32 c)
{
    RImplementation.get_imm_context().cmd_list.set_xform_world(Fidentity);
    DrawCross(p, radius, radius, radius, radius, radius, radius, c, true);
}

void CDrawUtilities::DrawEntity(u32 clr, ref_shader s)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // fill VB
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(5);

    verts[0].set(0.0f, 0.0f, 0.0f, clr);
    verts[1].set(0.0f, 1.0f, 0.0f, clr);
    verts[2].set(0.0f, 1.0f, 0.5f, clr);
    verts[3].set(0.0f, 0.5f, 0.5f, clr);
    verts[4].set(0.0f, 0.5f, 0.0f, clr);

    auto vBase = cmd_list.Vertex.Unlock<FVF::L>(5);
    // render flagshtok
    DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
    cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);

    if (s)
        DU_DRAW_SH(cmd_list, s);

    // fill VB
    const auto verts_lit = cmd_list.Vertex.Lock<FVF::LIT>(6);

    verts_lit[0].set(0.0f, 1.0f, 0.0f, clr, 0.0f, 0.0f);
    verts_lit[1].set(0.0f, 1.0f, 0.5f, clr, 1.0f, 0.0f);
    verts_lit[2].set(0.0f, 0.5f, 0.5f, clr, 1.0f, 1.0f);
    verts_lit[3].set(0.0f, 0.5f, 0.0f, clr, 0.0f, 1.0f);
    verts_lit[4].set(0.0f, 0.5f, 0.5f, clr, 1.0f, 1.0f);
    verts_lit[5].set(0.0f, 1.0f, 0.5f, clr, 1.0f, 0.0f);

    vBase = cmd_list.Vertex.Unlock<FVF::LIT>(6);
    // and Render it as line list
    cmd_list.dbg_DP(D3DPT_TRIANGLEFAN, vs_LIT, vBase, 4);
}

void CDrawUtilities::DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, u32 clr, BOOL bDrawEntity)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // fill VB
    auto verts = cmd_list.Vertex.Lock<FVF::L>(2);

    verts[0].set(p, clr);
    verts[1].set(p.x, p.y + height, p.z, clr);

    auto vBase = cmd_list.Vertex.Unlock<FVF::L>(2);
    // and Render it as triangle list
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, 1);

    f32 rx, rz;
    DirectX::XMScalarSinCos(&rx, &rz, heading);

    // fill VB
    verts = cmd_list.Vertex.Lock<FVF::L>(6);

    if (bDrawEntity)
    {
        sz *= 0.8f;
        verts[0].set(p.x, p.y + height, p.z, clr);
        verts[1].set(p.x + rx * sz, p.y + height, p.z + rz * sz, clr);

        sz *= 0.5f;
        verts[2].set(p.x, p.y + height * (1.f - sz_fl * 0.5f), p.z, clr);
        verts[3].set(p.x + rx * sz * 0.6f, p.y + height * (1.0f - sz_fl * 0.5f), p.z + rz * sz * 0.75f, clr);
        verts[4].set(p.x, p.y + height * (1.0f - sz_fl), p.z, clr);
        verts[5].set(p.x + rx * sz, p.y + height * (1.0f - sz_fl), p.z + rz * sz, clr);

        vBase = cmd_list.Vertex.Unlock<FVF::L>(6);
        // and Render it as line list
        cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, 3);
    }
    else
    {
        verts[0].set(p.x, p.y + height * (1.0f - sz_fl), p.z, clr);
        verts[1].set(p.x, p.y + height, p.z, clr);
        verts[2].set(p.x + rx * sz, (verts[0].p.y + verts[1].p.y) / 2.0f, p.z + rz * sz, clr);

        verts[3].set(verts[0]);
        verts[4].set(verts[2]);
        verts[5].set(verts[1]);

        vBase = cmd_list.Vertex.Unlock<FVF::L>(6);
        // and Render it as triangle list
        cmd_list.dbg_DP(D3DPT_TRIANGLELIST, vs_L, vBase, 2);
    }
}

//------------------------------------------------------------------------------

void CDrawUtilities::DrawRomboid(const Fvector& p, float r, u32 c)
{
    static constexpr WORD IL[24] = {0, 2, 2, 5, 0, 5, 3, 5, 3, 0, 4, 3, 4, 0, 4, 2, 1, 2, 1, 5, 1, 3, 1, 4};
    static constexpr WORD IT[24] = {2, 4, 0, 4, 3, 0, 3, 5, 0, 5, 2, 0, 4, 2, 1, 2, 5, 1, 5, 3, 1, 3, 4, 1};

    const u32 c1 = Fcolor(c).mul_rgb(0.75).get();

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    auto verts = cmd_list.Vertex.Lock<FVF::L>(6);

    verts[0].set(p.x, p.y + r, p.z, c1);
    verts[1].set(p.x, p.y - r, p.z, c1);
    verts[2].set(p.x, p.y, p.z - r, c1);
    verts[3].set(p.x, p.y, p.z + r, c1);
    verts[4].set(p.x - r, p.y, p.z, c1);
    verts[5].set(p.x + r, p.y, p.z, c1);

    auto vBase = cmd_list.Vertex.Unlock<FVF::L>(6);

    auto indices = cmd_list.Index.Lock(std::size(IT));
    std::ranges::copy(IT, indices.begin());
    auto iBase = cmd_list.Index.Unlock(std::size(IT));

    // and Render it as triangle list
    cmd_list.dbg_DIP(D3DPT_TRIANGLELIST, vs_L, vBase, 0, 6, iBase, std::size(IT) / 3);

    // draw lines
    verts = cmd_list.Vertex.Lock<FVF::L>(6);

    verts[0].set(p.x, p.y + r, p.z, c);
    verts[1].set(p.x, p.y - r, p.z, c);
    verts[2].set(p.x, p.y, p.z - r, c);
    verts[3].set(p.x, p.y, p.z + r, c);
    verts[4].set(p.x - r, p.y, p.z, c);
    verts[5].set(p.x + r, p.y, p.z, c);

    vBase = cmd_list.Vertex.Unlock<FVF::L>(6);

    indices = cmd_list.Index.Lock(std::size(IL));
    std::ranges::copy(IL, indices.begin());
    iBase = cmd_list.Index.Unlock(std::size(IL));

    cmd_list.dbg_DIP(D3DPT_LINELIST, vs_L, vBase, 0, 6, iBase, std::size(IL) / 2);
}

//------------------------------------------------------------------------------

void CDrawUtilities::DrawSound(const Fvector& p, float r, u32 c) { DrawCross(p, r, r, r, r, r, r, c, true); }

//------------------------------------------------------------------------------

void CDrawUtilities::DrawIdentCone(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bWire)
    {
        DU_DRAW_SH_C(cmd_list, RImplementation.m_WireShader, clr_w);
        m_WireCone.Render();
    }

    if (bSolid)
    {
        DU_DRAW_SH_C(cmd_list, color_get_A(clr_s) >= 254 ? RImplementation.m_WireShader : RImplementation.m_SelectionShader, clr_s);
        m_SolidCone.Render();
    }

    cmd_list.dbg_SetRS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentSphere(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bWire)
    {
        DU_DRAW_SH_C(cmd_list, RImplementation.m_WireShader, clr_w);
        m_WireSphere.Render();
    }

    if (bSolid)
    {
        DU_DRAW_SH_C(cmd_list, color_get_A(clr_s) >= 254 ? RImplementation.m_WireShader : RImplementation.m_SelectionShader, clr_s);
        m_SolidSphere.Render();
    }

    cmd_list.dbg_SetRS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentSpherePart(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bWire)
    {
        DU_DRAW_SH_C(cmd_list, RImplementation.m_WireShader, clr_w);
        m_WireSpherePart.Render();
    }

    if (bSolid)
    {
        DU_DRAW_SH_C(cmd_list, color_get_A(clr_s) >= 254 ? RImplementation.m_WireShader : RImplementation.m_SelectionShader, clr_s);
        m_SolidSpherePart.Render();
    }

    cmd_list.dbg_SetRS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentCylinder(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bWire)
    {
        DU_DRAW_SH_C(cmd_list, RImplementation.m_WireShader, clr_w);
        m_WireCylinder.Render();
    }

    if (bSolid)
    {
        DU_DRAW_SH_C(cmd_list, color_get_A(clr_s) >= 254 ? RImplementation.m_WireShader : RImplementation.m_SelectionShader, clr_s);
        m_SolidCylinder.Render();
    }

    cmd_list.dbg_SetRS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawIdentBox(BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bWire)
    {
        DU_DRAW_SH_C(cmd_list, RImplementation.m_WireShader, clr_w);
        m_WireBox.Render();
    }

    if (bSolid)
    {
        DU_DRAW_SH_C(cmd_list, color_get_A(clr_s) >= 254 ? RImplementation.m_WireShader : RImplementation.m_SelectionShader, clr_s);
        m_SolidBox.Render();
    }

    cmd_list.dbg_SetRS(D3DRS_TEXTUREFACTOR, 0xffffffff);
}

void CDrawUtilities::DrawLineSphere(const Fvector& p, float radius, u32 c, BOOL bCross)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    // fill VB
    // seg 0
    auto verts = cmd_list.Vertex.Lock<FVF::L>(LINE_DIVISION + 1);

    for (auto [v, line] : std::views::zip(verts.first<LINE_DIVISION>(), circledef1))
    {
        v.p.mad(p, line, radius);
        v.color = c;
    }

    verts[LINE_DIVISION].set(verts[0]);

    auto vBase = cmd_list.Vertex.Unlock<FVF::L>(LINE_DIVISION + 1);
    cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, LINE_DIVISION);

    // seg 1
    verts = cmd_list.Vertex.Lock<FVF::L>(LINE_DIVISION + 1);

    for (auto [v, line] : std::views::zip(verts.first<LINE_DIVISION>(), circledef2))
    {
        v.p.mad(p, line, radius);
        v.color = c;
    }

    verts[LINE_DIVISION].set(verts[0]);

    vBase = cmd_list.Vertex.Unlock<FVF::L>(LINE_DIVISION + 1);
    cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, LINE_DIVISION);

    // seg 2
    verts = cmd_list.Vertex.Lock<FVF::L>(LINE_DIVISION + 1);

    for (auto [v, line] : std::views::zip(verts.first<LINE_DIVISION>(), circledef3))
    {
        v.p.mad(p, line, radius);
        v.color = c;
    }

    verts[LINE_DIVISION].set(verts[0]);

    vBase = cmd_list.Vertex.Unlock<FVF::L>(LINE_DIVISION + 1);
    cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, LINE_DIVISION);

    if (bCross)
        DrawCross(p, radius, radius, radius, radius, radius, radius, c);
}

//----------------------------------------------------

namespace
{
IC float _x2real(float x) { return (x + 1) * Device.dwWidth * 0.5f; }
IC float _y2real(float y) { return (y + 1) * Device.dwHeight * 0.5f; }
} // namespace

void CDrawUtilities::dbgDrawPlacement(const Fvector& p, int sz, u32 clr, LPCSTR caption, u32 clr_font)
{
    XR_ASSERT(Device.b_is_Ready);

    float w = p.x * Device.mFullTransform._14 + p.y * Device.mFullTransform._24 + p.z * Device.mFullTransform._34 + Device.mFullTransform._44;
    if (w < 0)
        return; // culling

    Fvector c;
    Device.mFullTransform.transform(c, p);
    c.x = (float)iFloor(_x2real(c.x));
    c.y = (float)iFloor(_y2real(-c.y));
    const auto s = gsl::narrow_cast<f32>(sz);

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(5);

    verts[0].set(c.x - s, c.y - s, 0, 1, clr);
    verts[1].set(c.x + s, c.y - s, 0, 1, clr);
    verts[2].set(c.x + s, c.y + s, 0, 1, clr);
    verts[3].set(c.x - s, c.y + s, 0, 1, clr);
    verts[4].set(c.x - s, c.y - s, 0, 1, clr);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::TL>(5);
    // Render it as line strip
    cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_TL, vBase, 4);

    if (caption)
    {
        m_Font->SetColor(clr_font);
        m_Font->Out(c.x, c.y + s, "{}", caption);
    }
}

void CDrawUtilities::dbgDrawVert(const Fvector& p0, u32 clr, LPCSTR caption)
{
    dbgDrawPlacement(p0, 1, clr, caption);
    DrawCross(p0, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
}

void CDrawUtilities::dbgDrawEdge(const Fvector& p0, const Fvector& p1, u32 clr, LPCSTR caption)
{
    dbgDrawPlacement(p0, 1, clr, caption);

    DrawCross(p0, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
    DrawCross(p1, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);

    DrawLine(p0, p1, clr);
}

void CDrawUtilities::dbgDrawFace(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 clr, LPCSTR caption)
{
    dbgDrawPlacement(p0, 1, clr, caption);

    DrawCross(p0, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
    DrawCross(p1, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);
    DrawCross(p2, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, clr, false);

    DrawLine(p0, p1, clr);
    DrawLine(p1, p2, clr);
    DrawLine(p2, p0, clr);
}

//----------------------------------------------------

void CDrawUtilities::DrawLine(const Fvector& p0, const Fvector& p1, u32 c)
{
    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(2);

    verts[0].set(p0, c);
    verts[1].set(p1, c);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(2);
    // and Render it as triangle list
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, 1);
}

//----------------------------------------------------

void CDrawUtilities::DrawSelectionBox(const Fvector& C, const Fvector& S, u32* c)
{
    u32 cc = (c) ? *c : boxcolor;

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(boxvertcount);

    for (auto [v, bv] : std::views::zip(verts, boxvert))
    {
        v.p.mul(bv, S);
        v.p.add(C);
        v.color = cc;
    }

    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(boxvertcount);

    // and Render it as triangle list
    cmd_list.dbg_SetRS(D3DRS_FILLMODE, D3DFILL_SOLID);
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, boxvertcount / 2);
    cmd_list.dbg_SetRS(D3DRS_FILLMODE, FILL_MODE);
}

void CDrawUtilities::DrawBox(const Fvector& offs, const Fvector& Size, BOOL bSolid, BOOL bWire, u32 clr_s, u32 clr_w)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bWire)
    {
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(identboxwirecount);

        for (auto [v, wire] : std::views::zip(verts, identboxwire))
        {
            v.p.mul(wire, Size);
            v.p.mul(2);
            v.p.add(offs);
            v.color = clr_w;
        }

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(identboxwirecount);
        cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, identboxwirecount / 2);
    }

    if (bSolid)
    {
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(DU_BOX_NUMVERTEX2);

        for (auto [v, bv] : std::views::zip(verts, du_box_vertices2))
        {
            v.p.mul(bv, Size);
            v.p.mul(2);
            v.p.add(offs);
            v.color = clr_s;
        }

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(DU_BOX_NUMVERTEX2);
        cmd_list.dbg_DP(D3DPT_TRIANGLELIST, vs_L, vBase, DU_BOX_NUMFACES);
    }
}

//----------------------------------------------------

void CDrawUtilities::DrawOBB(const Fmatrix& parent, const Fobb& box, u32 clr_s, u32 clr_w)
{
    Fmatrix R, S, X;
    box.xform_get(R);
    S.scale(box.m_halfsize.x * 2.f, box.m_halfsize.y * 2.f, box.m_halfsize.z * 2.f);
    X.mul_43(R, S);
    R.mul_43(parent, X);

    RImplementation.get_imm_context().cmd_list.set_xform_world(R);
    DrawIdentBox(true, true, clr_s, clr_w);
}

//----------------------------------------------------

void CDrawUtilities::DrawAABB(const Fmatrix& parent, const Fvector& center, const Fvector& size, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
    Fmatrix R, S;
    S.scale(size.x * 2.f, size.y * 2.f, size.z * 2.f);
    S.translate_over(center);
    R.mul_43(parent, S);

    RImplementation.get_imm_context().cmd_list.set_xform_world(R);
    DrawIdentBox(bSolid, bWire, clr_s, clr_w);
}

void CDrawUtilities::DrawAABB(const Fvector& p0, const Fvector& p1, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
    Fmatrix R;
    Fvector C;
    C.set((p1.x + p0.x) * 0.5f, (p1.y + p0.y) * 0.5f, (p1.z + p0.z) * 0.5f);
    R.scale(_abs(p1.x - p0.x), _abs(p1.y - p0.y), _abs(p1.z - p0.z));
    R.translate_over(C);

    RImplementation.get_imm_context().cmd_list.set_xform_world(R);
    DrawIdentBox(bSolid, bWire, clr_s, clr_w);
}

void CDrawUtilities::DrawSphere(const Fmatrix& parent, const Fvector& center, float radius, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
    Fmatrix B;
    B.scale(radius, radius, radius);
    B.translate_over(center);
    B.mulA_43(parent);

    RImplementation.get_imm_context().cmd_list.set_xform_world(B);
    DrawIdentSphere(bSolid, bWire, clr_s, clr_w);
}

//----------------------------------------------------

void CDrawUtilities::DrawFace(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bSolid)
    {
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(3);

        verts[0].set(p0, clr_s);
        verts[1].set(p1, clr_s);
        verts[2].set(p2, clr_s);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(3);
        cmd_list.dbg_DP(D3DPT_TRIANGLELIST, vs_L, vBase, 1);
    }

    if (bWire)
    {
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(4);

        verts[0].set(p0, clr_w);
        verts[1].set(p1, clr_w);
        verts[2].set(p2, clr_w);
        verts[3].set(p0, clr_w);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(4);
        cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, 3);
    }
}

//----------------------------------------------------

void CDrawUtilities::DrawCylinder(const Fmatrix& parent, const Fvector& center, const Fvector& dir, float height, float radius, u32 clr_s, u32 clr_w,
                                  BOOL bSolid, BOOL bWire)
{
    Fmatrix mScale;
    mScale.scale(2.f * radius, 2.f * radius, height);

    // build final rotation / translation
    Fvector L_dir, L_up, L_right;
    L_dir.set(dir);
    L_dir.normalize();
    L_up.set(0, 1, 0);
    if (_abs(L_up.dotproduct(L_dir)) > .99f)
        L_up.set(0, 0, 1);
    L_right.crossproduct(L_up, L_dir);
    L_right.normalize();
    L_up.crossproduct(L_dir, L_right);
    L_up.normalize();

    Fmatrix mR;
    mR.i = L_right;
    mR._14 = 0;
    mR.j = L_up;
    mR._24 = 0;
    mR.k = L_dir;
    mR._34 = 0;
    mR.c = center;
    mR._44 = 1;

    // final xform
    Fmatrix xf;
    xf.mul(mR, mScale);
    xf.mulA_43(parent);

    RImplementation.get_imm_context().cmd_list.set_xform_world(xf);
    DrawIdentCylinder(bSolid, bWire, clr_s, clr_w);
}

//----------------------------------------------------

void CDrawUtilities::DrawCone(const Fmatrix& parent, const Fvector& apex, const Fvector& dir, float height, float radius, u32 clr_s, u32 clr_w, BOOL bSolid,
                              BOOL bWire)
{
    Fmatrix mScale;
    mScale.scale(2.f * radius, 2.f * radius, height);

    // build final rotation / translation
    Fvector L_dir, L_up, L_right;
    L_dir.set(dir);
    L_dir.normalize();
    L_up.set(0, 1, 0);
    if (_abs(L_up.dotproduct(L_dir)) > .99f)
        L_up.set(0, 0, 1);
    L_right.crossproduct(L_up, L_dir);
    L_right.normalize();
    L_up.crossproduct(L_dir, L_right);
    L_up.normalize();

    Fmatrix mR;
    mR.i = L_right;
    mR._14 = 0;
    mR.j = L_up;
    mR._24 = 0;
    mR.k = L_dir;
    mR._34 = 0;
    mR.c = apex;
    mR._44 = 1;

    // final xform
    Fmatrix xf;
    xf.mul(mR, mScale);
    xf.mulA_43(parent);

    RImplementation.get_imm_context().cmd_list.set_xform_world(xf);
    DrawIdentCone(bSolid, bWire, clr_s, clr_w);
}

//----------------------------------------------------

void CDrawUtilities::DrawPlane(const Fvector& p, const Fvector& n, const Fvector2& scale, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)
{
    if (n.square_magnitude() < EPS_S)
        return;
    // build final rotation / translation
    Fvector L_dir, L_up = n, L_right;
    L_dir.set(0, 0, 1);
    if (_abs(L_up.dotproduct(L_dir)) > .99f)
        L_dir.set(1, 0, 0);
    L_right.crossproduct(L_up, L_dir);
    L_right.normalize();
    L_dir.crossproduct(L_right, L_up);
    L_dir.normalize();

    Fmatrix mR;
    mR.i = L_right;
    mR._14 = 0;
    mR.j = L_up;
    mR._24 = 0;
    mR.k = L_dir;
    mR._34 = 0;
    mR.c = p;
    mR._44 = 1;

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bSolid)
    {
        DU_DRAW_SH(cmd_list, RImplementation.m_SelectionShader);
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(5);

        verts[0].set(-scale.x, 0, -scale.y, clr_s);
        mR.transform_tiny(verts[0].p);
        verts[1].set(-scale.x, 0, +scale.y, clr_s);
        mR.transform_tiny(verts[1].p);
        verts[2].set(+scale.x, 0, +scale.y, clr_s);
        mR.transform_tiny(verts[2].p);
        verts[3].set(+scale.x, 0, -scale.y, clr_s);
        mR.transform_tiny(verts[3].p);
        verts[4].set(verts[0]);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(5);

        if (!bCull)
            cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_NONE);

        cmd_list.dbg_DP(D3DPT_TRIANGLEFAN, vs_L, vBase, 2);

        if (!bCull)
            cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    if (bWire)
    {
        DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(5);

        verts[0].set(-scale.x, 0, -scale.y, clr_w);
        mR.transform_tiny(verts[0].p);
        verts[1].set(+scale.x, 0, -scale.y, clr_w);
        mR.transform_tiny(verts[1].p);
        verts[2].set(+scale.x, 0, +scale.y, clr_w);
        mR.transform_tiny(verts[2].p);
        verts[3].set(-scale.x, 0, +scale.y, clr_w);
        mR.transform_tiny(verts[3].p);
        verts[4].set(verts[0]);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(5);
        cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);
    }
}
//----------------------------------------------------

void CDrawUtilities::DrawPlane(const Fvector& center, const Fvector2& scale, const Fvector& rotate, u32 clr_s, u32 clr_w, BOOL bCull, BOOL bSolid, BOOL bWire)
{
    Fmatrix M;
    M.setHPB(rotate.y, rotate.x, rotate.z);
    M.translate_over(center);

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bSolid)
    {
        DU_DRAW_SH(cmd_list, RImplementation.m_SelectionShader);
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(5);

        verts[0].set(-scale.x, 0, -scale.y, clr_s);
        M.transform_tiny(verts[0].p);
        verts[1].set(-scale.x, 0, +scale.y, clr_s);
        M.transform_tiny(verts[1].p);
        verts[2].set(+scale.x, 0, +scale.y, clr_s);
        M.transform_tiny(verts[2].p);
        verts[3].set(+scale.x, 0, -scale.y, clr_s);
        M.transform_tiny(verts[3].p);
        verts[4].set(verts[0]);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(5);

        if (!bCull)
            cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_NONE);

        cmd_list.dbg_DP(D3DPT_TRIANGLEFAN, vs_L, vBase, 2);

        if (!bCull)
            cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    if (bWire)
    {
        DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(5);

        verts[0].set(-scale.x, 0, -scale.y, clr_w);
        M.transform_tiny(verts[0].p);
        verts[1].set(+scale.x, 0, -scale.y, clr_w);
        M.transform_tiny(verts[1].p);
        verts[2].set(+scale.x, 0, +scale.y, clr_w);
        M.transform_tiny(verts[2].p);
        verts[3].set(-scale.x, 0, +scale.y, clr_w);
        M.transform_tiny(verts[3].p);
        verts[4].set(verts[0]);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(5);
        cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);
    }
}

//----------------------------------------------------

void CDrawUtilities::DrawRectangle(const Fvector& o, const Fvector& u, const Fvector& v, u32 clr_s, u32 clr_w, BOOL bSolid, BOOL bWire)
{
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    if (bSolid)
    {
        DU_DRAW_SH(cmd_list, RImplementation.m_SelectionShader);
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(6);

        verts[0].set(o.x, o.y, o.z, clr_s);
        verts[1].set(o.x + u.x + v.x, o.y + u.y + v.y, o.z + u.z + v.z, clr_s);
        verts[2].set(o.x + v.x, o.y + v.y, o.z + v.z, clr_s);
        verts[3].set(o.x, o.y, o.z, clr_s);
        verts[4].set(o.x + u.x, o.y + u.y, o.z + u.z, clr_s);
        verts[5].set(o.x + u.x + v.x, o.y + u.y + v.y, o.z + u.z + v.z, clr_s);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(6);
        cmd_list.dbg_DP(D3DPT_TRIANGLELIST, vs_L, vBase, 2);
    }

    if (bWire)
    {
        DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
        const auto verts = cmd_list.Vertex.Lock<FVF::L>(5);

        verts[0].set(o.x, o.y, o.z, clr_w);
        verts[1].set(o.x + u.x, o.y + u.y, o.z + u.z, clr_w);
        verts[2].set(o.x + u.x + v.x, o.y + u.y + v.y, o.z + u.z + v.z, clr_w);
        verts[3].set(o.x + v.x, o.y + v.y, o.z + v.z, clr_w);
        verts[4].set(o.x, o.y, o.z, clr_w);

        const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(5);
        cmd_list.dbg_DP(D3DPT_LINESTRIP, vs_L, vBase, 4);
    }
}

//----------------------------------------------------

void CDrawUtilities::DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, u32 clr, BOOL bRot45)
{
    // actual rendering
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(bRot45 ? 12 : 6);

    verts[0].set(p.x + szx2, p.y, p.z, clr);
    verts[1].set(p.x - szx1, p.y, p.z, clr);
    verts[2].set(p.x, p.y + szy2, p.z, clr);
    verts[3].set(p.x, p.y - szy1, p.z, clr);
    verts[4].set(p.x, p.y, p.z + szz2, clr);
    verts[5].set(p.x, p.y, p.z - szz1, clr);

    if (bRot45)
    {
        Fmatrix M;
        M.setHPB(PI_DIV_4, PI_DIV_4, PI_DIV_4);

        for (auto [v, prev] : std::views::zip(verts | std::views::drop(6), verts | std::views::take(6)))
        {
            v.p.sub(prev.p, p);
            M.transform_dir(v.p);
            v.p.add(p);
            v.color = clr;
        }
    }

    // unlock VB and Render it as triangle list
    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(bRot45 ? 12 : 6);
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, bRot45 ? 6 : 3);
}

void CDrawUtilities::DrawPivot(const Fvector& pos, float sz)
{
    DU_DRAW_SH(RImplementation.get_imm_context().cmd_list, RImplementation.m_WireShader);
    DrawCross(pos, sz, sz, sz, sz, sz, sz, 0xFF7FFF7F);
}

void CDrawUtilities::DrawAxis(const Fmatrix& T)
{
    // colors
    constexpr u32 c[6] = {0x00222222, 0x00FF0000, 0x00222222, 0x0000FF00, 0x00222222, 0x000000FF};

    // position
    Fvector p[6];
    p[0].mad(T.c, T.k, 0.25f);
    p[1].set(p[0]);
    p[1].x += .015f;
    p[2].set(p[0]);
    p[3].set(p[0]);
    p[3].y += .015f;
    p[4].set(p[0]);
    p[5].set(p[0]);
    p[5].z += .015f;

    // transform to screen
    float dx = -float(Device.dwWidth) / 2.2f;
    float dy = float(Device.dwHeight) / 2.25f;

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(6);

    for (auto [v, pos, col] : std::views::zip(verts, p, c))
    {
        v.transform(pos, Device.mFullTransform);
        v.set((float)iFloor(_x2real(v.px) + dx), (float)iFloor(_y2real(v.py) + dy), 0, 1, col);
        pos.set(v.px, v.py, 0.0f);
    }

    // unlock VB and Render it as triangle list
    const auto vBase = cmd_list.Vertex.Unlock<FVF::TL>(6);

    cmd_list.dbg_SetRS(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_TL, vBase, 3);
    cmd_list.dbg_SetRS(D3DRS_SHADEMODE, SHADE_MODE);

    m_Font->SetColor(0xFF909090);
    m_Font->Out(p[1].x, p[1].y, "x");
    m_Font->Out(p[3].x, p[3].y, "y");
    m_Font->Out(p[5].x, p[5].y, "z");

    m_Font->SetColor(0xFF000000);
    m_Font->Out(p[1].x - 1, p[1].y - 1, "x");
    m_Font->Out(p[3].x - 1, p[3].y - 1, "y");
    m_Font->Out(p[5].x - 1, p[5].y - 1, "z");
}

void CDrawUtilities::DrawObjectAxis(const Fmatrix& T, float sz, BOOL sel)
{
    XR_ASSERT(Device.b_is_Ready);

    float w = T.c.x * Device.mFullTransform._14 + T.c.y * Device.mFullTransform._24 + T.c.z * Device.mFullTransform._34 + Device.mFullTransform._44;
    if (w < 0)
        return; // culling

    float s = w * sz;
    Fvector c, r, n, d;
    Device.mFullTransform.transform(c, T.c);
    r.mul(T.i, s);
    r.add(T.c);
    Device.mFullTransform.transform(r);
    n.mul(T.j, s);
    n.add(T.c);
    Device.mFullTransform.transform(n);
    d.mul(T.k, s);
    d.add(T.c);
    Device.mFullTransform.transform(d);
    c.x = (float)iFloor(_x2real(c.x));
    c.y = (float)iFloor(_y2real(-c.y));
    r.x = (float)iFloor(_x2real(r.x));
    r.y = (float)iFloor(_y2real(-r.y));
    n.x = (float)iFloor(_x2real(n.x));
    n.y = (float)iFloor(_y2real(-n.y));
    d.x = (float)iFloor(_x2real(d.x));
    d.y = (float)iFloor(_y2real(-d.y));

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(6);

    verts[0].set(c.x, c.y, 0, 1, 0xFF222222, 0, 0);
    verts[1].set(d.x, d.y, 0, 1, sel ? 0xFF0000FF : 0xFF000080, 0, 0);
    verts[2].set(c.x, c.y, 0, 1, 0xFF222222, 0, 0);
    verts[3].set(r.x, r.y, 0, 1, sel ? 0xFFFF0000 : 0xFF800000, 0, 0);
    verts[4].set(c.x, c.y, 0, 1, 0xFF222222, 0, 0);
    verts[5].set(n.x, n.y, 0, 1, sel ? 0xFF00FF00 : 0xFF008000, 0, 0);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::TL>(6);

    // Render it as line list
    cmd_list.dbg_SetRS(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_TL, vBase, 3);
    cmd_list.dbg_SetRS(D3DRS_SHADEMODE, SHADE_MODE);

    m_Font->SetColor(sel ? 0xFF000000 : 0xFF909090);
    m_Font->Out(r.x, r.y, "x");
    m_Font->Out(n.x, n.y, "y");
    m_Font->Out(d.x, d.y, "z");

    m_Font->SetColor(sel ? 0xFFFFFFFF : 0xFF000000);
    m_Font->Out(r.x - 1, r.y - 1, "x");
    m_Font->Out(n.x - 1, n.y - 1, "y");
    m_Font->Out(d.x - 1, d.y - 1, "z");
}

void CDrawUtilities::DrawGrid()
{
    XR_ASSERT(Device.b_is_Ready);

    if (m_GridPoints.empty())
        return;

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(m_GridPoints.size());
    std::ranges::copy(m_GridPoints, verts.begin());
    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(m_GridPoints.size());

    // Render it as triangle list
    Fmatrix ddd;
    ddd.identity();
    cmd_list.set_xform_world(ddd);

    DU_DRAW_SH(cmd_list, RImplementation.m_WireShader);
    cmd_list.dbg_DP(D3DPT_LINELIST, vs_L, vBase, m_GridPoints.size() / 2);
}

void CDrawUtilities::DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd)
{
    XR_ASSERT(Device.b_is_Ready);

    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(4);

    verts[0].set(m_SelStart.x * SCREEN_QUALITY, m_SelStart.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
    verts[1].set(m_SelStart.x * SCREEN_QUALITY, m_SelEnd.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
    verts[2].set(m_SelEnd.x * SCREEN_QUALITY, m_SelEnd.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);
    verts[3].set(m_SelEnd.x * SCREEN_QUALITY, m_SelStart.y * SCREEN_QUALITY, m_SelectionRect, 0.0f, 0.0f);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::TL>(4);

    // Render it as triangle list
    cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_NONE);
    DU_DRAW_SH(cmd_list, RImplementation.m_SelectionShader);
    cmd_list.dbg_DP(D3DPT_TRIANGLEFAN, vs_TL, vBase, 2);
    cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDrawUtilities::DrawPrimitiveL(D3DPRIMITIVETYPE pt, u32 pc, Fvector* vertices, int vc, u32 color, BOOL bCull, BOOL bCycle)
{
    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::L>(bCycle ? vc + 1 : vc);

    for (auto [v, sv] : std::views::zip(verts, std::span{vertices, gsl::narrow_cast<std::size_t>(vc)}))
        v.set(sv, color);

    if (bCycle)
        verts[vc].set(verts[0]);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::L>(bCycle ? vc + 1 : vc);

    if (!bCull)
        cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_NONE);

    cmd_list.dbg_DP(pt, vs_L, vBase, pc);

    if (!bCull)
        cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDrawUtilities::DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, int vc, BOOL bCull, BOOL bCycle)
{
    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::TL>(bCycle ? vc + 1 : vc);

    std::ranges::copy(std::span{vertices, gsl::narrow_cast<std::size_t>(vc)}, verts.begin());

    if (bCycle)
        verts[vc].set(verts[0]);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::TL>(bCycle ? vc + 1 : vc);

    if (!bCull)
        cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_NONE);

    cmd_list.dbg_DP(pt, vs_TL, vBase, pc);

    if (!bCull)
        cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDrawUtilities::DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, int vc, BOOL bCull, BOOL bCycle)
{
    // fill VB
    auto& cmd_list = RImplementation.get_imm_context().cmd_list;
    const auto verts = cmd_list.Vertex.Lock<FVF::LIT>(bCycle ? vc + 1 : vc);

    std::ranges::copy(std::span{vertices, gsl::narrow_cast<std::size_t>(vc)}, verts.begin());

    if (bCycle)
        verts[vc].set(verts[0]);

    const auto vBase = cmd_list.Vertex.Unlock<FVF::LIT>(bCycle ? vc + 1 : vc);

    if (!bCull)
        cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_NONE);

    cmd_list.dbg_DP(pt, vs_LIT, vBase, pc);

    if (!bCull)
        cmd_list.dbg_SetRS(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CDrawUtilities::DrawLink(const Fvector& p0, const Fvector& p1, float sz, u32 clr)
{
    DrawLine(p1, p0, clr);
    Fvector pp[2], D, R, N{0.0f, 1.0f, 0.0f};
    D.sub(p1, p0);
    D.normalize();
    R.crossproduct(N, D);
    R.mul(0.5f);
    D.mul(2.0f);
    N.mul(0.5f);
    // LR
    pp[0].add(R, D);
    pp[0].mul(sz * -0.5f);
    pp[0].add(p1);
    R.invert();
    pp[1].add(R, D);
    pp[1].mul(sz * -0.5f);
    pp[1].add(p1);
    DrawLine(p1, pp[0], clr);
    DrawLine(p1, pp[1], clr);
    // UB
    pp[0].add(N, D);
    pp[0].mul(sz * -0.5f);
    pp[0].add(p1);
    N.invert();
    pp[1].add(N, D);
    pp[1].mul(sz * -0.5f);
    pp[1].add(p1);
    DrawLine(p1, pp[0], clr);
    DrawLine(p1, pp[1], clr);
}

void CDrawUtilities::DrawJoint(const Fvector& p, float radius, u32 clr) { DrawLineSphere(p, radius, clr, false); }

tmc::task<void> CDrawUtilities::OnRender()
{
    m_Font->OnRender();
    co_return;
}

void CDrawUtilities::OutText(const Fvector& pos, LPCSTR text, u32 color, u32 shadow_color)
{
    Fvector p;
    float w = pos.x * Device.mFullTransform._14 + pos.y * Device.mFullTransform._24 + pos.z * Device.mFullTransform._34 + Device.mFullTransform._44;
    if (w >= 0)
    {
        Device.mFullTransform.transform(p, pos);
        p.x = (float)iFloor(_x2real(p.x));
        p.y = (float)iFloor(_y2real(-p.y));

        m_Font->SetColor(shadow_color);
        m_Font->Out(p.x, p.y, "{}", text);
        m_Font->SetColor(color);
        m_Font->Out(p.x - 1, p.y - 1, "{}", text);
    }
}
