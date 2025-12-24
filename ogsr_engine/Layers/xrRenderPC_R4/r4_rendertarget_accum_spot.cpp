#include "stdafx.h"

#include "../xrRender/du_cone.h"

void CRenderTarget::accum_spot(light* L)
{
    XR_TRACY_ZONE_SCOPED();

    phase_accumulator(RCache);
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
        RCache.set_xform_world(L->m_xform);
        RCache.set_xform_view(Device.mView);
        RCache.set_xform_project(Device.mProject);
        enable_scissor(L);

        // *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
        // *** thus can cope without stencil clear with 127 lights
        // *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
        //	Done in blender!
        // RCache.set_ColorWriteEnable		(FALSE);
        RCache.set_Element(s_accum_mask->E[SE_MASK_SPOT]); // masker

        // backfaces: if (stencil>=1 && zfail)			stencil = light_id
        RCache.set_CullMode(CULL_CW);
        if (!RImplementation.o.dx10_msaa)
            RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        else
            RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        draw_volume(L);

        // frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
        RCache.set_CullMode(CULL_CCW);
        if (!RImplementation.o.dx10_msaa)
            RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        else
            RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0x7f, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        draw_volume(L);
    }

    // *****************************	Minimize overdraw	*************************************
    // Select shader (front or back-faces), *** back, if intersect near plane
    RCache.set_ColorWriteEnable();
    RCache.set_CullMode(CULL_CW); // back

    // 2D texgens
    Fmatrix m_Texgen;
    u_compute_texgen_screen(RCache, m_Texgen);
    Fmatrix m_Texgen_J;
    u_compute_texgen_jitter(RCache, m_Texgen_J);

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
        RCache.set_Element(shader->E[_id]);

        RCache.set_CullMode(CULL_CW); // back

        // Constants
        float att_R = L->range * .95f;
        float att_factor = 1.f / (att_R * att_R);
        RCache.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
        RCache.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        RCache.set_c("m_texgen", m_Texgen);
        RCache.set_c("m_texgen_J", m_Texgen_J);
        RCache.set_c("m_shadow", m_Shadow);
        RCache.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        RCache.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);

        if (!Device.m_SecondViewport.IsSVPFrame())
            RCache.set_c("sss_id", L->sss_id);
        else
            RCache.set_c("sss_id", -1);

        if (!RImplementation.o.dx10_msaa)
        {
            RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, 0x00);
            draw_volume(L);
        }
        else
        {
            // per pixel
            RCache.set_Element(shader->E[_id]);
            RCache.set_Stencil(TRUE, D3DCMP_EQUAL, dwLightMarkerID, 0xff, 0x00);
            RCache.set_CullMode(D3DCULL_CW);
            draw_volume(L);
            // per sample
            RCache.set_Element(shader_msaa->E[_id]);
            RCache.set_Stencil(TRUE, D3DCMP_EQUAL, dwLightMarkerID | 0x80, 0xff, 0x00);
            RCache.set_CullMode(D3DCULL_CW);
            draw_volume(L);
            RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, 0x00);
        }
    }

    RCache.set_Scissor(nullptr);
    increment_light_marker(RCache);
}

void CRenderTarget::accum_volumetric(light* L)
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
        phase_vol_accumulator(RCache);
    }
    else
    {
        if (!m_bHasActiveVolumetric_spot)
        {
            m_bHasActiveVolumetric_spot = true;
            RCache.ClearRT(rt_ssfx_volumetric, {});
        }

        u_setrt(RCache, rt_ssfx_volumetric, {}, {}, nullptr);

        RCache.set_Stencil(FALSE);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_ColorWriteEnable();
    }

    // *** assume accumulator setted up ***
    // *****************************	Mask by stencil		*************************************
    {
        // setup xform
        L->xform_calc();
        RCache.set_xform_world(L->m_xform);
        RCache.set_xform_view(Device.mView);
        RCache.set_xform_project(Device.mProject);
        enable_scissor(L);
    }

    RCache.set_ColorWriteEnable();
    RCache.set_CullMode(CULL_NONE); // back

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
        RCache.set_Element(s_accum_volume->E[0]);

        // Constants
        float att_R = L->m_volumetric_distance * L->range * 0.95f;
        float att_factor = 1.f / (att_R * att_R);
        RCache.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
        RCache.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        RCache.set_c("m_shadow", m_Shadow);
        RCache.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        RCache.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);
        RCache.set_c("vMinBounds", aabb.x1, aabb.y1, aabb.z1, 0.f);
        //	Increase camera-space aabb z size to compensate decrease of slices number
        RCache.set_c("vMaxBounds", aabb.x2, aabb.y2, aabb.z1 + (aabb.z2 - aabb.z1) / fQuality, 0.0f);

        //	Set up user clip planes
        static constexpr const char* strFrustumClipPlane = "FrustumClipPlane";
        //	TODO: DX10: Check if it's equivalent to the previouse code.
        // RCache.set_ClipPlanes (TRUE,ClipFrustum.planes,ClipFrustum.p_count);

        //	Transform frustum to clip space
        Fmatrix PlaneTransform;
        PlaneTransform.transpose(Device.mInvFullTransform);
        // HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x3F);

        for (gsl::index i = 0; i < 6; ++i)
        {
            Fvector4& ClipPlane = *(Fvector4*)&ClipFrustum.planes[i].n.x;
            Fvector4 TransformedPlane;
            PlaneTransform.transform(TransformedPlane, ClipPlane);
            TransformedPlane.mul(-1.0f);
            RCache.set_ca(strFrustumClipPlane, i, TransformedPlane);
            // HW.pDevice->SetClipPlane( i, &TransformedPlane.x);
        }

        RCache.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

        RCache.set_Geometry(g_accum_volumetric);
        //	Igor: no need to do it per sub-sample. Plain AA will go just fine.
        RCache.Render(D3DPT_TRIANGLELIST, 0, 0, iNumSlices * 4, 0, iNumSlices * 2);

        RCache.set_ColorWriteEnable();
        RCache.set_ClipPlanes(FALSE, static_cast<Fmatrix*>(nullptr), 0);
    }

    RCache.set_Scissor(nullptr);
}
