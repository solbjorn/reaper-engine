#include "stdafx.h"
#include "../xrRender/resourcemanager.h"
#include "blender_light_occq.h"
#include "blender_light_mask.h"
#include "blender_light_direct.h"
#include "blender_light_point.h"
#include "blender_light_spot.h"
#include "blender_light_reflected.h"
#include "blender_combine.h"
#include "blender_bloom_build.h"
#include "blender_luminance.h"
#include "blender_blur.h"
#include "blender_dof.h"
#include "dx11MinMaxSMBlender.h"
#include "../xrRenderDX10/msaa/dx10MSAABlender.h"
#include "../xrRenderDX10/DX10 Rain/dx10RainBlender.h"
#include "../xrRender/dxRenderDeviceRender.h"

void CRenderTarget::u_setrt(const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth = _1->dwWidth;
        dwHeight = _1->dwHeight;
    }
    else
    {
        D3D_DEPTH_STENCIL_VIEW_DESC desc{};
        zb->GetDesc(&desc);

        if (!RImplementation.o.dx10_msaa)
            VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);

        ID3DResource* pRes;

        zb->GetResource(&pRes);

        ID3DTexture2D* pTex = (ID3DTexture2D*)pRes;

        D3D_TEXTURE2D_DESC TexDesc{};

        pTex->GetDesc(&TexDesc);

        dwWidth = TexDesc.Width;
        dwHeight = TexDesc.Height;
        _RELEASE(pRes);
    }

    if (_1)
        RCache.set_RT(_1->pRT, 0);
    else
        RCache.set_RT(NULL, 0);
    if (_2)
        RCache.set_RT(_2->pRT, 1);
    else
        RCache.set_RT(NULL, 1);
    if (_3)
        RCache.set_RT(_3->pRT, 2);
    else
        RCache.set_RT(NULL, 2);

    RCache.set_ZB(zb);
}

void CRenderTarget::u_setrt(const ref_rt& _1, const ref_rt& _2, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth = _1->dwWidth;
        dwHeight = _1->dwHeight;
    }
    else
    {
        D3D_DEPTH_STENCIL_VIEW_DESC desc{};
        zb->GetDesc(&desc);
        if (!RImplementation.o.dx10_msaa)
            VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);

        ID3DResource* pRes;

        zb->GetResource(&pRes);

        ID3DTexture2D* pTex = (ID3DTexture2D*)pRes;

        D3D_TEXTURE2D_DESC TexDesc{};

        pTex->GetDesc(&TexDesc);

        dwWidth = TexDesc.Width;
        dwHeight = TexDesc.Height;
        _RELEASE(pRes);
    }

    if (_1)
        RCache.set_RT(_1->pRT, 0);
    else
        RCache.set_RT(NULL, 0);
    if (_2)
        RCache.set_RT(_2->pRT, 1);
    else
        RCache.set_RT(NULL, 1);

    RCache.set_ZB(zb);
}

void CRenderTarget::u_setrt(u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, ID3DDepthStencilView* zb)
{
    // VERIFY									(_1);
    dwWidth = W;
    dwHeight = H;
    // VERIFY									(_1);
    RCache.set_RT(_1, 0);
    RCache.set_RT(_2, 1);
    RCache.set_RT(_3, 2);
    RCache.set_ZB(zb);
    //	RImplementation.rmNormal				();
}

void CRenderTarget::u_stencil_optimize(eStencilOptimizeMode eSOM)
{
    //	TODO: DX10: remove half pixel offset?
    VERIFY(RImplementation.o.nvstencil);
    // RCache.set_ColorWriteEnable	(FALSE);
    u32 Offset;
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    u32 C = color_rgba(255, 255, 255, 255);
    float eps = 0;
    float _dw = 0.5f;
    float _dh = 0.5f;
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(-_dw, _h - _dh, eps, 1.f, C, 0, 0);
    pv++;
    pv->set(-_dw, -_dh, eps, 1.f, C, 0, 0);
    pv++;
    pv->set(_w - _dw, _h - _dh, eps, 1.f, C, 0, 0);
    pv++;
    pv->set(_w - _dw, -_dh, eps, 1.f, C, 0, 0);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);
    RCache.set_Element(s_occq->E[1]);

    switch (eSOM)
    {
    case SO_Light: StateManager.SetStencilRef(dwLightMarkerID); break;
    case SO_Combine: StateManager.SetStencilRef(0x01); break;
    default: VERIFY(!"CRenderTarget::u_stencil_optimize. switch no default!");
    }

    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

// 2D texgen (texture adjustment matrix)
void CRenderTarget::u_compute_texgen_screen(Fmatrix& m_Texgen)
{
    // float	_w						= float(Device.dwWidth);
    // float	_h						= float(Device.dwHeight);
    // float	o_w						= (.5f / _w);
    // float	o_h						= (.5f / _h);
    Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                             //	Removing half pixel offset
                             // 0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
                             0.5f, 0.5f, 0.0f, 1.0f};
    m_Texgen.mul(m_TexelAdjust, RCache.xforms.m_wvp);
}

// 2D texgen for jitter (texture adjustment matrix)
void CRenderTarget::u_compute_texgen_jitter(Fmatrix& m_Texgen_J)
{
    // place into	0..1 space
    Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
    m_Texgen_J.mul(m_TexelAdjust, RCache.xforms.m_wvp);

    // rescale - tile it
    float scale_X = float(Device.dwWidth) / float(TEX_jitter);
    float scale_Y = float(Device.dwHeight) / float(TEX_jitter);
    // float	offset			= (.5f / float(TEX_jitter));
    m_TexelAdjust.scale(scale_X, scale_Y, 1.f);
    // m_TexelAdjust.translate_over(offset,	offset,	0	);
    m_Texgen_J.mulA_44(m_TexelAdjust);
}

static void generate_jitter(u32* dest, u32 elem_count)
{
    constexpr int cmax = 8;
    svector<Ivector2, cmax> samples;
    while (samples.size() < elem_count * 2)
    {
        Ivector2 test;
        test.set(::Random.randI(0, 256), ::Random.randI(0, 256));
        BOOL valid = TRUE;
        for (u32 t = 0; t < samples.size(); t++)
        {
            int dist = _abs(test.x - samples[t].x) + _abs(test.y - samples[t].y);
            if (dist < 32)
            {
                valid = FALSE;
                break;
            }
        }
        if (valid)
            samples.push_back(test);
    }
    for (u32 it = 0; it < elem_count; it++, dest++)
        *dest = color_rgba(samples[2 * it].x, samples[2 * it].y, samples[2 * it + 1].y, samples[2 * it + 1].x);
}

CRenderTarget::CRenderTarget()
{
    u32 SampleCount = 1;

    if (RImplementation.o.dx10_msaa)
        SampleCount = RImplementation.o.dx10_msaa_samples;

#ifdef DEBUG
    Msg("MSAA samples = %d", SampleCount);
    if (RImplementation.o.dx10_msaa_opt)
        Msg("dx10_MSAA_opt = on");
    if (RImplementation.o.dx10_gbuffer_opt)
        Msg("dx10_gbuffer_opt = on");
#endif // DEBUG
    param_blur = 0.f;
    param_gray = 0.f;
    param_noise = 0.f;
    param_duality_h = 0.f;
    param_duality_v = 0.f;
    param_noise_fps = 25.f;
    param_noise_scale = 1.f;

    im_noise_time = 1.f / 100.0f;
    im_noise_shift_w = 0;
    im_noise_shift_h = 0;

    param_color_base = color_rgba(127, 127, 127, 0);
    param_color_gray = color_rgba(85, 85, 85, 0);
    // param_color_add		= color_rgba(0,0,0,			0);
    param_color_add.set(0.0f, 0.0f, 0.0f);

    dwAccumulatorClearMark = 0;
    dxRenderDeviceRender::Instance().Resources->Evict();

    // Blenders
    b_occq = xr_new<CBlender_light_occq>();
    b_accum_mask = xr_new<CBlender_accum_direct_mask>();
    b_accum_direct = xr_new<CBlender_accum_direct>();
    b_accum_point = xr_new<CBlender_accum_point>();
    b_accum_spot = xr_new<CBlender_accum_spot>();
    b_accum_reflected = xr_new<CBlender_accum_reflected>();
    b_bloom = xr_new<CBlender_bloom_build>();
    b_blur = xr_new<CBlender_blur>();
    b_dof = xr_new<CBlender_dof>();

    if (RImplementation.o.dx10_msaa)
    {
        b_bloom_msaa = xr_new<CBlender_bloom_build_msaa>();
        b_postprocess_msaa = xr_new<CBlender_postprocess_msaa>();
    }
    b_luminance = xr_new<CBlender_luminance>();
    b_combine = xr_new<CBlender_combine>();

    if (RImplementation.o.dx10_msaa)
    {
        int bound = RImplementation.o.dx10_msaa_samples;

        if (RImplementation.o.dx10_msaa_opt)
            bound = 1;

        for (int i = 0; i < bound; ++i)
        {
            static LPCSTR SampleDefs[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
            b_combine_msaa[i] = xr_new<CBlender_combine_msaa>();
            b_accum_mask_msaa[i] = xr_new<CBlender_accum_direct_mask_msaa>();
            b_accum_direct_msaa[i] = xr_new<CBlender_accum_direct_msaa>();
            b_accum_direct_volumetric_msaa[i] = xr_new<CBlender_accum_direct_volumetric_msaa>();
            // b_accum_direct_volumetric_sun_msaa[i]	= xr_new<CBlender_accum_direct_volumetric_sun_msaa>			();
            b_accum_spot_msaa[i] = xr_new<CBlender_accum_spot_msaa>();
            b_accum_volumetric_msaa[i] = xr_new<CBlender_accum_volumetric_msaa>();
            b_accum_point_msaa[i] = xr_new<CBlender_accum_point_msaa>();
            b_accum_reflected_msaa[i] = xr_new<CBlender_accum_reflected_msaa>();
            static_cast<CBlender_accum_direct_mask_msaa*>(b_accum_mask_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_direct_volumetric_msaa*>(b_accum_direct_volumetric_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            // static_cast<CBlender_accum_direct_volumetric_sun_msaa*>(b_accum_direct_volumetric_sun_msaa[i])->SetDefine( "ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_direct_msaa*>(b_accum_direct_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_volumetric_msaa*>(b_accum_volumetric_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_spot_msaa*>(b_accum_spot_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_point_msaa*>(b_accum_point_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_reflected_msaa*>(b_accum_reflected_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_combine_msaa*>(b_combine_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
        }
    }
    //	NORMAL
    {
        u32 w = Device.dwWidth, h = Device.dwHeight;
        rt_Position.create(r2_RT_P, w, h, D3DFMT_A16B16G16R16F, SampleCount);

        if (RImplementation.o.dx10_msaa)
            rt_MSAADepth.create(r2_RT_MSAAdepth, w, h, D3DFMT_D24S8, SampleCount);

        if (!RImplementation.o.dx10_gbuffer_opt)
            rt_Normal.create(r2_RT_N, w, h, D3DFMT_A16B16G16R16F, SampleCount);

        // select albedo & accum
        if (RImplementation.o.mrtmixdepth)
        {
            // NV50
            rt_Color.create(r2_RT_albedo, w, h, D3DFMT_A8R8G8B8, SampleCount);
            rt_Accumulator.create(r2_RT_accum, w, h, D3DFMT_A16B16G16R16F, SampleCount);
        }
        else
        {
            // can't - mix-depth
            if (RImplementation.o.fp16_blend)
            {
                // NV40
                if (!RImplementation.o.dx10_gbuffer_opt)
                {
                    rt_Color.create(r2_RT_albedo, w, h, D3DFMT_A16B16G16R16F, SampleCount); // expand to full
                    rt_Accumulator.create(r2_RT_accum, w, h, D3DFMT_A16B16G16R16F, SampleCount);
                }
                else
                {
                    rt_Color.create(r2_RT_albedo, w, h, D3DFMT_A8R8G8B8, SampleCount); // expand to full
                    rt_Accumulator.create(r2_RT_accum, w, h, D3DFMT_A16B16G16R16F, SampleCount);
                }
            }
            else
            {
                // R4xx, no-fp-blend,-> albedo_wo
                VERIFY(RImplementation.o.albedo_wo);
                rt_Color.create(r2_RT_albedo, w, h, D3DFMT_A8R8G8B8, SampleCount); // normal
                rt_Accumulator.create(r2_RT_accum, w, h, D3DFMT_A16B16G16R16F, SampleCount);
                rt_Accumulator_temp.create(r2_RT_accum_temp, w, h, D3DFMT_A16B16G16R16F, SampleCount);
            }
        }

        // generic(LDR) RTs
        rt_Generic_0.create(r2_RT_generic0, w, h, D3DFMT_A8R8G8B8, 1);
        rt_Generic_1.create(r2_RT_generic1, w, h, D3DFMT_A8R8G8B8, 1);

        if (RImplementation.o.dx10_msaa)
            rt_Generic_0_temp.create(r2_RT_generic0_temp, w, h, D3DFMT_A8R8G8B8, SampleCount);
        else
            rt_Generic_0_temp.create(r2_RT_generic0_temp, w, h, D3DFMT_A8R8G8B8, 1);

        		// RT Blur
        rt_blur_h_2.create(r2_RT_blur_h_2, u32(w / 2), u32(h / 2), D3DFMT_A8R8G8B8);
        rt_blur_2.create(r2_RT_blur_2, u32(w / 2), u32(h / 2), D3DFMT_A8R8G8B8);

        rt_blur_h_4.create(r2_RT_blur_h_4, u32(w / 4), u32(h / 4), D3DFMT_A8R8G8B8);
        rt_blur_4.create(r2_RT_blur_4, u32(w / 4), u32(h / 4), D3DFMT_A8R8G8B8);

        rt_blur_h_8.create(r2_RT_blur_h_8, u32(w / 8), u32(h / 8), D3DFMT_A8R8G8B8);
        rt_blur_8.create(r2_RT_blur_8, u32(w / 8), u32(h / 8), D3DFMT_A8R8G8B8);

        rt_dof.create(r2_RT_dof, w, h, D3DFMT_A8R8G8B8);
        rt_secondVP.create(r2_RT_secondVP, w, h, D3DFMT_A8R8G8B8, 1); //--#SM+#-- +SecondVP+
        rt_BeforeUi.create("$user$viewport_temp_before_ui", w, h, D3DFMT_A8R8G8B8, 1);

        if (RImplementation.o.dx10_msaa)
        {
            rt_Generic_0_r.create(r2_RT_generic0_r, w, h, D3DFMT_A8R8G8B8, SampleCount);
            rt_Generic_1_r.create(r2_RT_generic1_r, w, h, D3DFMT_A8R8G8B8, SampleCount);
            rt_Generic.create(r2_RT_generic, w, h, D3DFMT_A8R8G8B8, 1);
        }
        //	Igor: for volumetric lights
        // rt_Generic_2.create			(r2_RT_generic2,w,h,D3DFMT_A8R8G8B8		);
        //	temp: for higher quality blends
        if (RImplementation.o.advancedpp)
            rt_Generic_2.create(r2_RT_generic2, w, h, D3DFMT_A16B16G16R16F, SampleCount);
    }

    // OCCLUSION
    s_occq.create(b_occq, "r2\\occq");

    s_blur.create(b_blur, "r2\\blur");
    s_dof.create(b_dof, "r2\\dof");

    s_lut.create("ogsr_lut");

    // DIRECT (spot)
    D3DFORMAT depth_format = (D3DFORMAT)RImplementation.o.HW_smap_FORMAT;

    if (RImplementation.o.HW_smap)
    {
        u32 size = RImplementation.o.smapsize;
        rt_smap_depth.create(r2_RT_smap_depth, size, size, depth_format);

        if (RImplementation.o.dx10_minmax_sm)
        {
            rt_smap_depth_minmax.create(r2_RT_smap_depth_minmax, size / 4, size / 4, D3DFMT_R32F);
            CBlender_createminmax TempBlender;
            s_create_minmax_sm.create(&TempBlender, "null");
        }

        s_accum_mask.create(b_accum_mask, "r3\\accum_mask");
        s_accum_direct.create(b_accum_direct, "r3\\accum_direct");

        if (RImplementation.o.dx10_msaa)
        {
            int bound = RImplementation.o.dx10_msaa_samples;

            if (RImplementation.o.dx10_msaa_opt)
                bound = 1;

            for (int i = 0; i < bound; ++i)
            {
                s_accum_direct_msaa[i].create(b_accum_direct_msaa[i], "r3\\accum_direct");
                s_accum_mask_msaa[i].create(b_accum_mask_msaa[i], "r3\\accum_direct");
            }
        }
        if (RImplementation.o.advancedpp)
        {
            s_accum_direct_volumetric.create("accum_volumetric_sun_nomsaa");

            if (RImplementation.o.dx10_minmax_sm)
                s_accum_direct_volumetric_minmax.create("accum_volumetric_sun_nomsaa_minmax");

            if (RImplementation.o.dx10_msaa)
            {
                static LPCSTR snames[] = {"accum_volumetric_sun_msaa0", "accum_volumetric_sun_msaa1", "accum_volumetric_sun_msaa2", "accum_volumetric_sun_msaa3",
                                          "accum_volumetric_sun_msaa4", "accum_volumetric_sun_msaa5", "accum_volumetric_sun_msaa6", "accum_volumetric_sun_msaa7"};
                int bound = RImplementation.o.dx10_msaa_samples;

                if (RImplementation.o.dx10_msaa_opt)
                    bound = 1;

                for (int i = 0; i < bound; ++i)
                {
                    // s_accum_direct_volumetric_msaa[i].create		(b_accum_direct_volumetric_sun_msaa[i],			"r3\\accum_direct");
                    s_accum_direct_volumetric_msaa[i].create(snames[i]);
                }
            }
        }
    }
    else
    {
        //	TODO: DX10: Check if we need old-style SMap
        VERIFY(!"Use HW SMAPs only!");
        // u32	size					=RImplementation.o.smapsize	;
        // rt_smap_surf.create			(r2_RT_smap_surf,			size,size,D3DFMT_R32F);
        // rt_smap_depth				= NULL;
        // R_CHK						(HW.pDevice->CreateDepthStencilSurface	(size,size,D3DFMT_D24X8,D3DMULTISAMPLE_NONE,0,TRUE,&rt_smap_ZB,NULL));
        // s_accum_mask.create			(b_accum_mask,				"r2\\accum_mask");
        // s_accum_direct.create		(b_accum_direct,			"r2\\accum_direct");
        // if (RImplementation.o.advancedpp)
        //	s_accum_direct_volumetric.create("accum_volumetric_sun");
    }

    //	RAIN
    //	TODO: DX10: Create resources only when DX10 rain is enabled.
    //	Or make DX10 rain switch dynamic?
    {
        CBlender_rain TempBlender;
        s_rain.create(&TempBlender, "null");

        if (RImplementation.o.dx10_msaa)
        {
            static LPCSTR SampleDefs[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
            CBlender_rain_msaa TempBlender[8];

            int bound = RImplementation.o.dx10_msaa_samples;

            if (RImplementation.o.dx10_msaa_opt)
                bound = 1;

            for (int i = 0; i < bound; ++i)
            {
                TempBlender[i].SetDefine("ISAMPLE", SampleDefs[i]);
                s_rain_msaa[i].create(&TempBlender[i], "null");
                s_accum_spot_msaa[i].create(b_accum_spot_msaa[i], "r2\\accum_spot_s", "lights\\lights_spot01");
                s_accum_point_msaa[i].create(b_accum_point_msaa[i], "r2\\accum_point_s");
                // s_accum_volume_msaa[i].create(b_accum_direct_volumetric_msaa[i], "lights\\lights_spot01");
                s_accum_volume_msaa[i].create(b_accum_volumetric_msaa[i], "lights\\lights_spot01");
                s_combine_msaa[i].create(b_combine_msaa[i], "r2\\combine");
            }
        }
    }

    if (RImplementation.o.dx10_msaa)
    {
        CBlender_msaa TempBlender;

        s_mark_msaa_edges.create(&TempBlender, "null");
    }

    // POINT
    {
        s_accum_point.create(b_accum_point, "r2\\accum_point_s");
        accum_point_geom_create();
        g_accum_point.create(D3DFVF_XYZ, g_accum_point_vb, g_accum_point_ib);
        accum_omnip_geom_create();
        g_accum_omnipart.create(D3DFVF_XYZ, g_accum_omnip_vb, g_accum_omnip_ib);
    }

    // SPOT
    {
        s_accum_spot.create(b_accum_spot, "r2\\accum_spot_s", "lights\\lights_spot01");
        accum_spot_geom_create();
        g_accum_spot.create(D3DFVF_XYZ, g_accum_spot_vb, g_accum_spot_ib);
    }

    {
        s_accum_volume.create("accum_volumetric", "lights\\lights_spot01");
        accum_volumetric_geom_create();
        g_accum_volumetric.create(D3DFVF_XYZ, g_accum_volumetric_vb, g_accum_volumetric_ib);
    }

    // REFLECTED
    {
        s_accum_reflected.create(b_accum_reflected, "r2\\accum_refl");
        if (RImplementation.o.dx10_msaa)
        {
            int bound = RImplementation.o.dx10_msaa_samples;

            if (RImplementation.o.dx10_msaa_opt)
                bound = 1;

            for (int i = 0; i < bound; ++i)
            {
                s_accum_reflected_msaa[i].create(b_accum_reflected_msaa[i], "null");
            }
        }
    }

    // BLOOM
    {
        D3DFORMAT fmt = D3DFMT_A8R8G8B8; //;		// D3DFMT_X8R8G8B8
        u32 w = BLOOM_size_X, h = BLOOM_size_Y;
        u32 fvf_build = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
        u32 fvf_filter = (u32)D3DFVF_XYZRHW | D3DFVF_TEX8 | D3DFVF_TEXCOORDSIZE4(0) | D3DFVF_TEXCOORDSIZE4(1) | D3DFVF_TEXCOORDSIZE4(2) | D3DFVF_TEXCOORDSIZE4(3) |
            D3DFVF_TEXCOORDSIZE4(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6) | D3DFVF_TEXCOORDSIZE4(7);
        rt_Bloom_1.create(r2_RT_bloom1, w, h, fmt);
        rt_Bloom_2.create(r2_RT_bloom2, w, h, fmt);
        g_bloom_build.create(fvf_build, RCache.Vertex.Buffer(), RCache.QuadIB);
        g_bloom_filter.create(fvf_filter, RCache.Vertex.Buffer(), RCache.QuadIB);
        s_bloom_dbg_1.create("effects\\screen_set", r2_RT_bloom1);
        s_bloom_dbg_2.create("effects\\screen_set", r2_RT_bloom2);
        s_bloom.create(b_bloom, "r2\\bloom");
        if (RImplementation.o.dx10_msaa)
        {
            s_bloom_msaa.create(b_bloom_msaa, "r2\\bloom");
            s_postprocess_msaa.create(b_postprocess_msaa, "r2\\post");
        }
        f_bloom_factor = 0.5f;
    }

    // TONEMAP
    {
        rt_LUM_64.create(r2_RT_luminance_t64, 64, 64, D3DFMT_A16B16G16R16F);
        rt_LUM_8.create(r2_RT_luminance_t8, 8, 8, D3DFMT_A16B16G16R16F);
        s_luminance.create(b_luminance, "r2\\luminance");
        f_luminance_adapt = 0.5f;

        t_LUM_src.create(r2_RT_luminance_src);
        t_LUM_dest.create(r2_RT_luminance_cur);

        // create pool
        for (u32 it = 0; it < HW.Caps.iGPUNum * 2; it++)
        {
            string256 name;
            xr_sprintf(name, "%s_%d", r2_RT_luminance_pool, it);
            rt_LUM_pool[it].create(name, 1, 1, D3DFMT_R32F);
            // u_setrt						(rt_LUM_pool[it],	0,	0,	0			);
            // CHK_DX						(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	0x7f7f7f7f,	1.0f, 0L));
            FLOAT ColorRGBA[4] = {127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f};
            HW.pContext->ClearRenderTargetView(rt_LUM_pool[it]->pRT, ColorRGBA);
        }
        u_setrt(Device.dwWidth, Device.dwHeight, HW.pBaseRT, NULL, NULL, HW.pBaseZB);
    }

    // COMBINE
    {
        static D3DVERTEXELEMENT9 dwDecl[] = {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // pos+uv
                                             D3DDECL_END()};
        s_combine.create(b_combine, "r2\\combine");
        s_combine_volumetric.create("combine_volumetric");
        s_combine_dbg_0.create("effects\\screen_set", r2_RT_smap_surf);
        s_combine_dbg_1.create("effects\\screen_set", r2_RT_luminance_t8);
        s_combine_dbg_Accumulator.create("effects\\screen_set", r2_RT_accum);
        g_combine_VP.create(dwDecl, RCache.Vertex.Buffer(), RCache.QuadIB);
        g_combine.create(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
        g_combine_2UV.create(FVF::F_TL2uv, RCache.Vertex.Buffer(), RCache.QuadIB);
        g_combine_cuboid.create(dwDecl, RCache.Vertex.Buffer(), RCache.Index.Buffer());

        u32 fvf_aa_blur = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
        g_aa_blur.create(fvf_aa_blur, RCache.Vertex.Buffer(), RCache.QuadIB);

        u32 fvf_aa_AA = D3DFVF_XYZRHW | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) |
            D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6);
        g_aa_AA.create(fvf_aa_AA, RCache.Vertex.Buffer(), RCache.QuadIB);

        t_envmap_0.create(r2_T_envs0);
        t_envmap_1.create(r2_T_envs1);

        s_rain_drops.create("ogsr_rain_drops");
    }

    u32 w = Device.dwWidth;
    u32 h = Device.dwHeight;

    // SMAA RTs
    {
        rt_smaa_edgetex.create(r2_RT_smaa_edgetex, w, h, D3DFMT_A8R8G8B8);
        rt_smaa_blendtex.create(r2_RT_smaa_blendtex, w, h, D3DFMT_A8R8G8B8);

        s_pp_antialiasing.create("effects_pp_antialiasing");
    }

    // Mrmnwar SunShaft Screen Space
    {
        rt_SunShaftsMask.create(r2_RT_SunShaftsMask, w, h, D3DFMT_A8R8G8B8);
        rt_SunShaftsMaskSmoothed.create(r2_RT_SunShaftsMaskSmoothed, w, h, D3DFMT_A8R8G8B8);
        rt_SunShaftsPass0.create(r2_RT_SunShaftsPass0, w, h, D3DFMT_A8R8G8B8);
        s_ssss_mrmnwar.create("effects\\ss_sunshafts_mrmnwar");
    }

    // RT - KD Screen space sunshafts
    {
        rt_sunshafts_0.create(r2_RT_sunshafts0, w, h, D3DFMT_A8R8G8B8);
        rt_sunshafts_1.create(r2_RT_sunshafts1, w, h, D3DFMT_A8R8G8B8);
        s_ssss_ogse.create("effects\\ss_sunshafts_ogse");
    }

    // Build textures
    {
        // Build noise table
        {
            constexpr u32 sampleSize = 4;
            u32 tempData[TEX_jitter_count][TEX_jitter * TEX_jitter]{};

            D3D_TEXTURE2D_DESC desc{};
            desc.Width = TEX_jitter;
            desc.Height = TEX_jitter;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
            // desc.Usage = D3D_USAGE_IMMUTABLE;
            desc.Usage = D3D_USAGE_DEFAULT;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            D3D_SUBRESOURCE_DATA subData[TEX_jitter_count]{};

            for (u32 it{}; it < TEX_jitter_count; it++)
            {
                subData[it].pSysMem = tempData[it];
                subData[it].SysMemPitch = desc.Width * sampleSize;
            }

            // Fill it,
            for (u32 y = 0; y < TEX_jitter; y++)
            {
                for (u32 x = 0; x < TEX_jitter; x++)
                {
                    u32 data[TEX_jitter_count];
                    generate_jitter(data, TEX_jitter_count);
                    for (u32 it = 0; it < TEX_jitter_count; it++)
                    {
                        u32* p = (u32*)((u8*)(subData[it].pSysMem) + y * subData[it].SysMemPitch + x * 4);
                        *p = data[it];
                    }
                }
            }

            for (u32 it{}; it < TEX_jitter_count; it++)
            {
                string_path name;
                xr_sprintf(name, "%s%d", r2_jitter, it);
                R_CHK(HW.pDevice->CreateTexture2D(&desc, &subData[it], &t_noise_surf[it]));
                t_noise[it] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(name);
                t_noise[it]->surface_set(t_noise_surf[it]);
            }
        }
    }

    // PP
    s_postprocess.create("postprocess");
    g_postprocess.create(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX3, RCache.Vertex.Buffer(), RCache.QuadIB);

    // Menu
    s_menu.create("distort");
    g_menu.create(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);

    //
    dwWidth = Device.dwWidth;
    dwHeight = Device.dwHeight;
}

CRenderTarget::~CRenderTarget()
{
    // Textures
    t_LUM_src->surface_set(NULL);
    t_LUM_dest->surface_set(NULL);

#ifdef DEBUG
    ID3DBaseTexture* pSurf = 0;

    pSurf = t_envmap_0->surface_get();
    _SHOW_REF("t_envmap_0 - #small", pSurf);

    pSurf = t_envmap_1->surface_get();
    _SHOW_REF("t_envmap_1 - #small", pSurf);
    //_SHOW_REF("t_envmap_0 - #small",t_envmap_0->pSurface);
    //_SHOW_REF("t_envmap_1 - #small",t_envmap_1->pSurface);
#endif // DEBUG
    t_envmap_0->surface_set(NULL);
    t_envmap_1->surface_set(NULL);
    t_envmap_0.destroy();
    t_envmap_1.destroy();

    //	TODO: DX10: Check if we need old style SMAPs
    //	_RELEASE					(rt_smap_ZB);

    // Jitter
    for (u32 it = 0; it < TEX_jitter_count; it++)
    {
        t_noise[it]->surface_set(NULL);
#ifdef DEBUG
        _SHOW_REF("t_noise_surf[it]", t_noise_surf[it]);
#endif // DEBUG
        _RELEASE(t_noise_surf[it]);
    }

    //
    accum_spot_geom_destroy();
    accum_omnip_geom_destroy();
    accum_point_geom_destroy();
    accum_volumetric_geom_destroy();

    // Blenders
    xr_delete(b_combine);
    xr_delete(b_luminance);
    xr_delete(b_bloom);
    xr_delete(b_accum_reflected);
    xr_delete(b_accum_spot);
    xr_delete(b_accum_point);
    xr_delete(b_accum_direct);
    xr_delete(b_blur);
    xr_delete(b_dof);

    if (RImplementation.o.dx10_msaa)
    {
        xr_delete(b_bloom_msaa);
        xr_delete(b_postprocess_msaa);

        int bound = RImplementation.o.dx10_msaa_samples;

        if (RImplementation.o.dx10_msaa_opt)
            bound = 1;

        for (int i = 0; i < bound; ++i)
        {
            xr_delete(b_combine_msaa[i]);
            xr_delete(b_accum_direct_msaa[i]);
            xr_delete(b_accum_mask_msaa[i]);
            xr_delete(b_accum_direct_volumetric_msaa[i]);
            // xr_delete					(b_accum_direct_volumetric_sun_msaa[i]);
            xr_delete(b_accum_spot_msaa[i]);
            xr_delete(b_accum_volumetric_msaa[i]);
            xr_delete(b_accum_point_msaa[i]);
            xr_delete(b_accum_reflected_msaa[i]);
        }
    }
    xr_delete(b_accum_mask);
    xr_delete(b_occq);
}

void CRenderTarget::reset_light_marker(bool bResetStencil)
{
    dwLightMarkerID = 5;
    if (bResetStencil)
    {
        u32 Offset;
        float _w = float(Device.dwWidth);
        float _h = float(Device.dwHeight);
        u32 C = color_rgba(255, 255, 255, 255);
        float eps = 0;
        float _dw = 0.5f;
        float _dh = 0.5f;
        FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(-_dw, _h - _dh, eps, 1.f, C, 0, 0);
        pv++;
        pv->set(-_dw, -_dh, eps, 1.f, C, 0, 0);
        pv++;
        pv->set(_w - _dw, _h - _dh, eps, 1.f, C, 0, 0);
        pv++;
        pv->set(_w - _dw, -_dh, eps, 1.f, C, 0, 0);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);
        RCache.set_Element(s_occq->E[2]);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }
}

void CRenderTarget::increment_light_marker()
{
    dwLightMarkerID += 2;

    // if (dwLightMarkerID>10)
    const u32 iMaxMarkerValue = RImplementation.o.dx10_msaa ? 127 : 255;

    if (dwLightMarkerID > iMaxMarkerValue)
        reset_light_marker(true);
}

bool CRenderTarget::need_to_render_sunshafts()
{
    {
        CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        Fcolor sun_color = ((light*)RImplementation.Lights.sun_adapted._get())->color;
        float fValue = E.m_fSunShaftsIntensity * u_diffuse2s(sun_color.r, sun_color.g, sun_color.b);
        if (fValue < EPS)
            return false;
    }

    return true;
}

bool CRenderTarget::use_minmax_sm_this_frame()
{
    switch (RImplementation.o.dx10_minmax_sm)
    {
    case CRender::MMSM_ON: return true;
    case CRender::MMSM_AUTO: return need_to_render_sunshafts();
    case CRender::MMSM_AUTODETECT: {
        u32 dwScreenArea = HW.m_ChainDesc.BufferDesc.Width * HW.m_ChainDesc.BufferDesc.Height;

        if ((dwScreenArea >= RImplementation.o.dx10_minmax_sm_screenarea_threshold))
            return need_to_render_sunshafts();
        else
            return false;
    }

    default: return false;
    }
}
