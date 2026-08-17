#include "stdafx.h"

#include "dxLensFlareRender.h"

#include "../../xr_3da/xr_efflensflare.h"
#include "../../xr_3da/IGame_Persistent.h"

void dxFlareRender::Copy(IFlareRender& _in)
{
    auto& in{*smart_cast<const dxFlareRender*>(&_in)};

    hShader = in.hShader;
}

void dxFlareRender::CreateShader(LPCSTR sh_name, LPCSTR tex_name)
{
    if (tex_name && tex_name[0])
        hShader.create(sh_name, tex_name);
}

void dxFlareRender::DestroyShader() { hShader.destroy(); }

void dxLensFlareRender::Copy(ILensFlareRender& _in)
{
    auto& in{*smart_cast<const dxLensFlareRender*>(&_in)};

    hGeom = in.hGeom;
}

void dxLensFlareRender::Render(CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient)
{
    XR_TRACY_ZONE_SCOPED();

    auto& cmd_list = RImplementation.get_imm_context().cmd_list;

    const auto fDistance = g_pGamePersistent->Environment().CurrentEnv->far_plane * 0.75f;
    Fcolor dwLight;
    dwLight.set(owner.LightColor);

    xr::inlined_vector<ref_shader, 16> _2render;

    // Source + n flares + gradient
    const auto verts = cmd_list.Vertex.Lock<FVF::LIT>((owner.m_Current->m_Flares.size() + 2) * 4);
    std::size_t written{0};

    if (bSun && owner.m_Current->m_Flags.is(CLensFlareDescriptor::flSource))
    {
        Fvector3 vecSx;
        vecSx.mul(owner.vecX, owner.m_Current->m_Source.fRadius * fDistance);
        Fvector3 vecSy;
        vecSy.mul(owner.vecY, owner.m_Current->m_Source.fRadius * fDistance);

        Fcolor color;
        if (owner.m_Current->m_Source.ignore_color)
            color.set(1.0f, 1.0f, 1.0f, 1.0f);
        else
            color.set(dwLight);

        color.a *= owner.m_StateBlend;
        u32 c = color.get();

        const auto v = verts.subspan(written, 4);
        v[0].set(owner.vecLight.x + vecSx.x - vecSy.x, owner.vecLight.y + vecSx.y - vecSy.y, owner.vecLight.z + vecSx.z - vecSy.z, c, 0, 0);
        v[1].set(owner.vecLight.x + vecSx.x + vecSy.x, owner.vecLight.y + vecSx.y + vecSy.y, owner.vecLight.z + vecSx.z + vecSy.z, c, 0, 1);
        v[2].set(owner.vecLight.x - vecSx.x - vecSy.x, owner.vecLight.y - vecSx.y - vecSy.y, owner.vecLight.z - vecSx.z - vecSy.z, c, 1, 0);
        v[3].set(owner.vecLight.x - vecSx.x + vecSy.x, owner.vecLight.y - vecSx.y + vecSy.y, owner.vecLight.z - vecSx.z + vecSy.z, c, 1, 1);
        written += 4;

        _2render.emplace_back(smart_cast<const dxFlareRender*>(&*owner.m_Current->m_Source.m_pRender)->hShader);
    }

    if (owner.fBlend < EPS_L)
        goto render;

    if (bFlares && owner.m_Current->m_Flags.is(CLensFlareDescriptor::flFlare))
    {
        Fvector3 vecDx;
        vecDx.normalize(owner.vecAxis);
        Fvector3 vecDy;
        vecDy.crossproduct(vecDx, owner.vecDir);

        const auto cnt = owner.m_Current->m_Flares.size() * 4;

        for (auto [v, F] : std::views::zip(verts.subspan(written, cnt) | std::views::chunk(4), owner.m_Current->m_Flares))
        {
            Fvector3 vec;
            vec.mul(owner.vecAxis, F.fPosition);
            vec.add(owner.vecCenter);

            Fvector3 vecSx;
            vecSx.mul(vecDx, F.fRadius * fDistance);
            Fvector3 vecSy;
            vecSy.mul(vecDy, F.fRadius * fDistance);

            Fcolor color;
            color.set(dwLight);
            color.mul_rgba(F.fOpacity * owner.fBlend * owner.m_StateBlend);
            u32 c = color.get();

            v[0].set(vec.x + vecSx.x - vecSy.x, vec.y + vecSx.y - vecSy.y, vec.z + vecSx.z - vecSy.z, c, 0, 0);
            v[1].set(vec.x + vecSx.x + vecSy.x, vec.y + vecSx.y + vecSy.y, vec.z + vecSx.z + vecSy.z, c, 0, 1);
            v[2].set(vec.x - vecSx.x - vecSy.x, vec.y - vecSx.y - vecSy.y, vec.z - vecSx.z - vecSy.z, c, 1, 0);
            v[3].set(vec.x - vecSx.x + vecSy.x, vec.y - vecSx.y + vecSy.y, vec.z - vecSx.z + vecSy.z, c, 1, 1);

            _2render.emplace_back(smart_cast<const dxFlareRender*>(&*F.m_pRender)->hShader);
        }

        written += cnt;
    }

    // gradient
    if (bGradient && owner.m_Current->m_Flags.is(CLensFlareDescriptor::flGradient) && owner.fGradientValue >= EPS_L)
    {
        Fvector3 vecSx;
        vecSx.mul(owner.vecX, owner.m_Current->m_Gradient.fRadius * owner.fGradientValue * fDistance);
        Fvector3 vecSy;
        vecSy.mul(owner.vecY, owner.m_Current->m_Gradient.fRadius * owner.fGradientValue * fDistance);

        Fcolor color;
        color.set(dwLight);
        color.mul_rgba(owner.fGradientValue * owner.m_StateBlend);
        u32 c = color.get();

        const auto v = verts.subspan(written, 4);
        v[0].set(owner.vecLight.x + vecSx.x - vecSy.x, owner.vecLight.y + vecSx.y - vecSy.y, owner.vecLight.z + vecSx.z - vecSy.z, c, 0, 0);
        v[1].set(owner.vecLight.x + vecSx.x + vecSy.x, owner.vecLight.y + vecSx.y + vecSy.y, owner.vecLight.z + vecSx.z + vecSy.z, c, 0, 1);
        v[2].set(owner.vecLight.x - vecSx.x - vecSy.x, owner.vecLight.y - vecSx.y - vecSy.y, owner.vecLight.z - vecSx.z - vecSy.z, c, 1, 0);
        v[3].set(owner.vecLight.x - vecSx.x + vecSy.x, owner.vecLight.y - vecSx.y + vecSy.y, owner.vecLight.z - vecSx.z + vecSy.z, c, 1, 1);
        written += 4;

        _2render.emplace_back(smart_cast<const dxFlareRender*>(&*owner.m_Current->m_Gradient.m_pRender)->hShader);
    }

render:
    const auto VS_Offset = cmd_list.Vertex.Unlock<FVF::LIT>(XR_ASSERT_VAL(written == _2render.size() * 4));

    if (written == 0)
        return;

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Geometry(hGeom);

    for (auto [i, shader] : std::views::enumerate(_2render))
    {
        cmd_list.set_Shader(XR_ASSERT_VAL(shader));
        cmd_list.Render(D3DPT_TRIANGLELIST, VS_Offset + i * 4, 0, 4, 0, 2);
    }
}

void dxLensFlareRender::OnDeviceCreate()
{
    hGeom.create(FVF::F_LIT, SGeometry::default_vb(), RImplementation.QuadIB);
    XR_ASSERT(hGeom.stride() == sizeof(FVF::LIT));
}

void dxLensFlareRender::OnDeviceDestroy() { hGeom.destroy(); }
