#include "stdafx.h"

void CRenderTarget::draw_rain(CBackend& cmd_list, const light& RainSetup)
{
    XR_TRACY_ZONE_SCOPED();

    float fRainFactor = g_pGamePersistent->Environment().CurrentEnv->rain_density;

    // Common calc for quad-rendering
    constexpr u32 C = color_rgba(255, 255, 255, 255);
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    Fvector2 p0, p1;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);
    float d_Z = EPS_S;
    constexpr float d_W = 1.f;

    // Common constants (light-related)
    Fvector L_dir;
    Device.mView.transform_dir(L_dir, RainSetup.direction);
    L_dir.normalize();

    Fvector W_dirX;
    Device.mView.transform_dir(W_dirX, Fvector().set(1.0f, 0.0f, 0.0f));
    W_dirX.normalize();

    Fvector W_dirZ;
    Device.mView.transform_dir(W_dirZ, Fvector().set(0.0f, 0.0f, 1.0f));
    W_dirZ.normalize();

    // recalculate d_Z, to perform depth-clipping
    const float fRainFar = ps_ssfx_gloss_method == 0 ? ps_r3_dyn_wet_surf_far : 250.f;

    Fvector center_pt;
    center_pt.mad(Device.vCameraPosition, Device.vCameraDirection, fRainFar);
    Device.mFullTransform.transform(center_pt);
    d_Z = center_pt.z;

    // Perform lighting
    {
        // texture adjustment matrix
        constexpr float fRange{1.0f};
        constexpr float fBias{-0.0001f};
        const f32 smapsize = gsl::narrow_cast<f32>(s32{RImplementation.o.smapsize});
        const f32 fTexelOffs = 0.5f / smapsize;
        float view_dimX = float(RainSetup.X.D.maxX - RainSetup.X.D.minX) / smapsize;
        float view_dimY = float(RainSetup.X.D.maxX - RainSetup.X.D.minX) / smapsize;
        float view_sx = float(RainSetup.X.D.minX) / smapsize;
        float view_sy = float(RainSetup.X.D.minY) / smapsize;
        const Fmatrix m_TexelAdjust{view_dimX / 2.f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    -view_dimY / 2.f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    fRange,
                                    0.0f,
                                    view_dimX / 2.f + view_sx + fTexelOffs,
                                    view_dimY / 2.f + view_sy + fTexelOffs,
                                    fBias,
                                    1.0f};

        // compute xforms

        // shadow xform
        Fmatrix m_shadow;
        {
            Fmatrix xf_project;
            xf_project.mul(m_TexelAdjust, RainSetup.X.D.combine[0]);
            m_shadow.mul(xf_project, Device.mInvView);
        }

        // clouds xform
        Fmatrix m_clouds_shadow;
        {
            constexpr float w_shift = 0;
            Fmatrix m_xform;
            Fvector normal;
            normal.setHP(1, 0);
            m_xform.identity();
            Fvector localnormal;
            m_xform.transform_dir(localnormal, normal);
            localnormal.normalize();
            m_clouds_shadow.mul(m_xform, Device.mInvView);
            m_xform.scale(1.f, 1.f, 1.f);
            m_clouds_shadow.mulA_44(m_xform);
            m_xform.translate(localnormal.mul(w_shift));
            m_clouds_shadow.mulA_44(m_xform);
        }

        // Make jitter texture
        Fvector2 j0, j1;
        float scale_X = float(Device.dwWidth) / float(TEX_jitter);
        float offset = (.5f / float(TEX_jitter));
        j0.set(offset, offset);
        j1.set(scale_X, scale_X).add(offset);

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<FVF::TL2uv>(4);

        verts[0].set(-1, -1, d_Z, d_W, C, 0, 1, 0, scale_X);
        verts[1].set(-1, 1, d_Z, d_W, C, 0, 0, 0, 0);
        verts[2].set(1, -1, d_Z, d_W, C, 1, 1, scale_X, scale_X);
        verts[3].set(1, 1, d_Z, d_W, C, 1, 0, scale_X, 0);

        const auto Offset = cmd_list.Vertex.Unlock<FVF::TL2uv>(4);

        cmd_list.set_Geometry(g_combine_2UV);

        //	Use for intermediate results
        //	Patch normal
        u_setrt(cmd_list, rt_Accumulator, {}, {}, rt_MSAADepth);

        cmd_list.set_Element(s_rain->E[1]);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0.f);
        cmd_list.set_c("WorldX", W_dirX.x, W_dirX.y, W_dirX.z, 0.f);
        cmd_list.set_c("WorldZ", W_dirZ.x, W_dirZ.y, W_dirZ.z, 0.f);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);
        cmd_list.set_c("RainDensity", fRainFactor, 0.f, 0.f, 0.f);
        cmd_list.set_c("RainFallof", ps_r3_dyn_wet_surf_near, ps_r3_dyn_wet_surf_far, 0.f, 0.f);

        if (!RImplementation.o.dx10_msaa)
        {
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }
        else
        {
            // per pixel execution
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

            // per sample
            cmd_list.set_Element(s_rain_msaa->E[0]);
            cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0.f);
            cmd_list.set_c("WorldX", W_dirX.x, W_dirX.y, W_dirX.z, 0.f);
            cmd_list.set_c("WorldZ", W_dirZ.x, W_dirZ.y, W_dirZ.z, 0.f);
            cmd_list.set_c("m_shadow", m_shadow);
            cmd_list.set_c("m_sunmask", m_clouds_shadow);
            cmd_list.set_c("RainDensity", fRainFactor, 0.f, 0.f, 0.f);
            cmd_list.set_c("RainFallof", ps_r3_dyn_wet_surf_near, ps_r3_dyn_wet_surf_far, 0.f, 0.f);
            cmd_list.set_CullMode(CULL_NONE);
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }

        //	Apply normal
        cmd_list.set_Element(s_rain->E[2]);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0.f);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);

        u_setrt(cmd_list, rt_Position, {}, {}, rt_MSAADepth);

        if (!RImplementation.o.dx10_msaa)
        {
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }
        else
        {
            // per pixel execution
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

            // per sample
            cmd_list.set_Element(s_rain_msaa->E[1]);
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0);
            cmd_list.set_CullMode(CULL_NONE);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }

        //	Apply gloss
        cmd_list.set_Element(s_rain->E[3]);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0.f);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);

        u_setrt(cmd_list, rt_Color, {}, {}, rt_MSAADepth);

        if (!RImplementation.o.dx10_msaa)
        {
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }
        else
        {
            // per pixel execution
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

            // per sample
            cmd_list.set_Element(s_rain_msaa->E[2]);
            cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x81, 0x81, 0);
            cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }
    }
}
