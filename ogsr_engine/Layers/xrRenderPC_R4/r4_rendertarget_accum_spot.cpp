#include "stdafx.h"

#include "../xrRender/du_cone.h"

void CRenderTarget::accum_spot(CBackend& cmd_list, light* L)
{
    XR_TRACY_ZONE_SCOPED();

    phase_accumulator(cmd_list);
    RImplementation.stats.l_visible++;

    // *** assume accumulator already setup ***
    // *****************************	Mask by stencil		*************************************
    ref_shader shader;
    ref_shader shader_msaa;
    if (IRender_Light::OMNIPART == L->flags.type)
    {
        shader = L->s_point;
        shader_msaa = L->s_point_msaa;
        if (!shader)
        {
            shader = s_accum_point;
            shader_msaa = s_accum_point_msaa;
        }
    }
    else
    {
        shader = L->s_spot;
        shader_msaa = L->s_spot_msaa;
        if (!shader)
        {
            shader = s_accum_spot;
            shader_msaa = s_accum_spot_msaa;
        }
    }

    {
        // setup xform
        L->xform_calc();
        cmd_list.set_xform_world(L->m_xform);
        cmd_list.set_xform_view(Device.mView);
        cmd_list.set_xform_project(Device.mProject);
        enable_scissor(L);

        // *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
        // *** thus can cope without stencil clear with 127 lights
        // *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
        //	Done in blender!
        // cmd_list.set_ColorWriteEnable		(FALSE);
        cmd_list.set_Element(s_accum_mask->E[SE_MASK_SPOT]); // masker

        // backfaces: if (stencil>=1 && zfail)			stencil = light_id
        cmd_list.set_CullMode(CULL_CW);

        if (!RImplementation.o.dx10_msaa)
            cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        else
            cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);

        draw_volume(cmd_list, L);

        // frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
        cmd_list.set_CullMode(CULL_CCW);

        if (!RImplementation.o.dx10_msaa)
            cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        else
            cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0x7f, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);

        draw_volume(cmd_list, L);
    }

    // *****************************	Minimize overdraw	*************************************
    // Select shader (front or back-faces), *** back, if intersect near plane
    cmd_list.set_ColorWriteEnable();
    cmd_list.set_CullMode(CULL_CW); // back

    // 2D texgens
    Fmatrix m_Texgen;
    u_compute_texgen_screen(cmd_list, m_Texgen);
    Fmatrix m_Texgen_J;
    u_compute_texgen_jitter(cmd_list, m_Texgen_J);

    // Shadow xform (+texture adjustment matrix)
    Fmatrix m_Shadow, m_Lmap;
    {
        float smapsize = float(RImplementation.o.smapsize);
        float fTexelOffs = (.5f / smapsize);
        float view_dim = float(L->X.S.size - 2) / smapsize;
        float view_sx = float(L->X.S.posX + 1) / smapsize;
        float view_sy = float(L->X.S.posY + 1) / smapsize;
        float fRange = float(1.f) * ps_r2_ls_depth_scale;
        float fBias = ps_r2_ls_depth_bias;
        const Fmatrix m_TexelAdjust{view_dim / 2.f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    -view_dim / 2.f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    fRange,
                                    0.0f,
                                    view_dim / 2.f + view_sx + fTexelOffs,
                                    view_dim / 2.f + view_sy + fTexelOffs,
                                    fBias,
                                    1.0f};

        // compute xforms
        Fmatrix xf_view = L->X.S.view;
        Fmatrix xf_project;
        xf_project.mul(m_TexelAdjust, L->X.S.project);
        m_Shadow.mul(xf_view, Device.mInvView);
        m_Shadow.mulA_44(xf_project);

        // lmap
        view_dim = 1.f;
        view_sx = 0.f;
        view_sy = 0.f;
        const Fmatrix m_TexelAdjust2{view_dim / 2.f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     -view_dim / 2.f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     fRange,
                                     0.0f,
                                     view_dim / 2.f + view_sx + fTexelOffs,
                                     view_dim / 2.f + view_sy + fTexelOffs,
                                     fBias,
                                     1.0f};

        // compute xforms
        xf_project.mul(m_TexelAdjust2, L->X.S.project);
        m_Lmap.mul(xf_view, Device.mInvView);
        m_Lmap.mulA_44(xf_project);
    }

    // Common constants
    Fvector L_clr, L_pos;
    float L_spec;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_clr.mul(L->get_LOD());
    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_tiny(L_pos, L->position);

    // Draw volume with projective texgen
    {
        // Select shader
        u32 _id = 0;
        if (L->flags.bShadow)
        {
            bool bFullSize = (L->X.S.size == RImplementation.o.smapsize);
            if (bFullSize)
                _id = SE_L_FULLSIZE;
            else
                _id = SE_L_NORMAL;
        }
        else
        {
            _id = SE_L_UNSHADOWED;
            m_Shadow = m_Lmap;
        }

        cmd_list.set_Element(shader->E[_id]);
        cmd_list.set_CullMode(CULL_CW); // back

        // Constants
        float att_R = L->range * .95f;
        float att_factor = 1.f / (att_R * att_R);
        cmd_list.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        cmd_list.set_c("m_texgen", m_Texgen);
        cmd_list.set_c("m_texgen_J", m_Texgen_J);
        cmd_list.set_c("m_shadow", m_Shadow);
        cmd_list.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        cmd_list.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);

        if (!Device.m_SecondViewport.IsSVPFrame())
            cmd_list.set_c("sss_id", L->sss_id);
        else
            cmd_list.set_c("sss_id", -1);

        if (!RImplementation.o.dx10_msaa)
        {
            cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, 0x00);
            draw_volume(cmd_list, L);
        }
        else
        {
            // per pixel
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, dwLightMarkerID, 0xff, 0x00);
            cmd_list.set_CullMode(D3DCULL_CW);

            draw_volume(cmd_list, L);

            // per sample
            cmd_list.set_Element(shader_msaa->E[_id]);
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, dwLightMarkerID | 0x80, 0xff, 0x00);
            cmd_list.set_CullMode(D3DCULL_CW);

            draw_volume(cmd_list, L);

            cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, 0x00);
        }
    }

    cmd_list.set_Scissor(nullptr);
    increment_light_marker(cmd_list);
}

void CRenderTarget::accum_volumetric(CBackend& cmd_list, light* L)
{
    // [ SSS ] Fade through distance volumetric lights.
    if (ps_ssfx_volumetric.x > 0)
    {
        float Falloff = ps_ssfx_volumetric.y - std::min(std::max((L->vis.distance - 20) * 0.01f, 0.0f), 1.0f) * ps_ssfx_volumetric.y;
        L->m_volumetric_intensity = Falloff;

        if (!L->flags.bVolumetricManual)
            L->flags.bVolumetric = Falloff <= 0 ? false : true;
    }

    if (!L->flags.bVolumetric)
        return;

    XR_TRACY_ZONE_SCOPED();

    if (!RImplementation.o.ssfx_volumetric)
    {
        phase_vol_accumulator(cmd_list);
    }
    else
    {
        if (!m_bHasActiveVolumetric_spot)
        {
            m_bHasActiveVolumetric_spot = true;
            cmd_list.ClearRT(rt_ssfx_volumetric, {});
        }

        u_setrt(cmd_list, rt_ssfx_volumetric, {}, {}, nullptr);

        cmd_list.set_Stencil(FALSE);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_ColorWriteEnable();
    }

    // *** assume accumulator setted up ***
    // *****************************	Mask by stencil		*************************************
    {
        // setup xform
        L->xform_calc();
        cmd_list.set_xform_world(L->m_xform);
        cmd_list.set_xform_view(Device.mView);
        cmd_list.set_xform_project(Device.mProject);
        enable_scissor(L);
    }

    cmd_list.set_ColorWriteEnable();
    cmd_list.set_CullMode(CULL_NONE); // back

    // Shadow xform (+texture adjustment matrix)
    Fmatrix m_Shadow, m_Lmap;
    Fmatrix mFrustumSrc;
    CFrustum ClipFrustum;
    {
        float smapsize = float(RImplementation.o.smapsize);
        float fTexelOffs = (.5f / smapsize);
        float view_dim = float(L->X.S.size - 2) / smapsize;
        float view_sx = float(L->X.S.posX + 1) / smapsize;
        float view_sy = float(L->X.S.posY + 1) / smapsize;
        float fRange = float(1.f) * ps_r2_ls_depth_scale;
        float fBias = ps_r2_ls_depth_bias;
        const Fmatrix m_TexelAdjust{view_dim / 2.f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    -view_dim / 2.f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    fRange,
                                    0.0f,
                                    view_dim / 2.f + view_sx + fTexelOffs,
                                    view_dim / 2.f + view_sy + fTexelOffs,
                                    fBias,
                                    1.0f};

        // compute xforms
        Fmatrix xf_view = L->X.S.view;
        Fmatrix xf_project;
        xf_project.mul(m_TexelAdjust, L->X.S.project);
        m_Shadow.mul(xf_view, Device.mInvView);
        m_Shadow.mulA_44(xf_project);

        // lmap
        view_dim = 1.f;
        view_sx = 0.f;
        view_sy = 0.f;
        const Fmatrix m_TexelAdjust2{view_dim / 2.f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     -view_dim / 2.f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     fRange,
                                     0.0f,
                                     view_dim / 2.f + view_sx + fTexelOffs,
                                     view_dim / 2.f + view_sy + fTexelOffs,
                                     fBias,
                                     1.0f};

        // compute xforms
        xf_project.mul(m_TexelAdjust2, L->X.S.project);
        m_Lmap.mul(xf_view, Device.mInvView);
        m_Lmap.mulA_44(xf_project);

        // Compute light frustum in world space
        mFrustumSrc.mul(L->X.S.project, xf_view);
        ClipFrustum.CreateFromMatrix(mFrustumSrc, FRUSTUM_P_ALL);
        //	Adjust frustum far plane
        //	4 - far, 5 - near
        ClipFrustum.planes[4].d -= (ClipFrustum.planes[4].d + ClipFrustum.planes[5].d) * (1 - L->m_volumetric_distance);
    }

    //	Calculate camera space AABB
    //	Adjust AABB according to the adjusted distance for the light volume
    Fbox aabb;

    float scaledRadius = L->spatial.sphere.R * L->m_volumetric_distance;
    Fvector rr = Fvector().set(scaledRadius, scaledRadius, scaledRadius);
    Fvector pt = L->spatial.sphere.P;
    pt.sub(L->position);
    pt.mul(L->m_volumetric_distance);
    pt.add(L->position);

    Device.mView.transform(pt);
    aabb.setb(pt, rr);

    // Common vars
    float fQuality{};
    int iNumSlices{};

    // Color and intensity vars
    Fvector L_clr, L_pos;
    float L_spec;
    float IntensityMod = 1.0f;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_clr.mul(L->m_volumetric_distance);

    if (ps_ssfx_volumetric.x <= 0)
    {
        // Vanilla Method
        fQuality = L->m_volumetric_quality;
        iNumSlices = (int)((float)VOLUMETRIC_SLICES * fQuality);
        // min 10 surfaces
        iNumSlices = std::max(10, iNumSlices);

        // Set Intensity
        fQuality = (float)iNumSlices / (float)VOLUMETRIC_SLICES;
        L_clr.mul(L->m_volumetric_intensity);
        L_clr.mul(1 / fQuality);
        L_clr.mul(L->get_LOD());
    }
    else
    {
        // SSS Method
        fQuality = ps_ssfx_volumetric.z;
        iNumSlices = (int)(24.f * fQuality);

        // Intensity mod to OMNIPART && HUD
        if (L->flags.type == IRender_Light::OMNIPART || L->flags.bHudMode)
            IntensityMod = 0.2f;

        // Set Intensity
        L_clr.mul(L->m_volumetric_intensity * IntensityMod);
        L_clr.mul(1.0f / fQuality);
        L_clr.mul(L->get_LOD());
        fQuality = (float)iNumSlices / 120.f; // Max setting ( 24 * 5 )
    }

    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_tiny(L_pos, L->position);

    // Draw volume with projective texgen
    {
        cmd_list.set_Element(s_accum_volume->E[0]);

        // Constants
        float att_R = L->m_volumetric_distance * L->range * 0.95f;
        float att_factor = 1.f / (att_R * att_R);
        cmd_list.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        cmd_list.set_c("m_shadow", m_Shadow);
        cmd_list.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        cmd_list.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);
        cmd_list.set_c("vMinBounds", aabb.x1, aabb.y1, aabb.z1, 0.f);
        //	Increase camera-space aabb z size to compensate decrease of slices number
        cmd_list.set_c("vMaxBounds", aabb.x2, aabb.y2, aabb.z1 + (aabb.z2 - aabb.z1) / fQuality, 0.0f);

        //	Set up user clip planes
        static constexpr const char* strFrustumClipPlane = "FrustumClipPlane";

        //	Transform frustum to clip space
        Fmatrix PlaneTransform;
        PlaneTransform.transpose(Device.mInvFullTransform);

        for (gsl::index i = 0; i < 6; ++i)
        {
            Fvector4& ClipPlane = *(Fvector4*)&ClipFrustum.planes[i].n.x;
            Fvector4 TransformedPlane;
            PlaneTransform.transform(TransformedPlane, ClipPlane);
            TransformedPlane.mul(-1.0f);
            cmd_list.set_ca(strFrustumClipPlane, i, TransformedPlane);
        }

        cmd_list.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

        cmd_list.set_Geometry(g_accum_volumetric);
        //	Igor: no need to do it per sub-sample. Plain AA will go just fine.
        cmd_list.Render(D3DPT_TRIANGLELIST, 0, 0, iNumSlices * 4, 0, iNumSlices * 2);

        cmd_list.set_ColorWriteEnable();
    }

    cmd_list.set_Scissor(nullptr);
}
