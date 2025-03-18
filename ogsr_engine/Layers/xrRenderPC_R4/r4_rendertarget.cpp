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
#include "../xrRenderDX10/msaa/dx10MSAABlender.h"
#include "../xrRenderDX10/DX10 Rain/dx10RainBlender.h"
////////////////////////////lvutner
#include "blender_gasmask_drops.h"
#include "blender_gasmask_dudv.h"
#include "blender_blur.h"
#include "blender_dof.h"

#include "../xrRender/dxRenderDeviceRender.h"

void CRenderTarget::set_viewport_size(const CBackend& cmd_list, float w, float h) const
{
    const D3D11_VIEWPORT vp = {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = w,
        .Height = h,
        .MinDepth = 0.f,
        .MaxDepth = 1.f,
    };

    cmd_list.SetViewport(vp);
}

void CRenderTarget::u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth[cmd_list.context_id] = _1->dwWidth;
        dwHeight[cmd_list.context_id] = _1->dwHeight;
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

        dwWidth[cmd_list.context_id] = TexDesc.Width;
        dwHeight[cmd_list.context_id] = TexDesc.Height;
        _RELEASE(pRes);
    }

    if (_1)
        cmd_list.set_RT(_1->pRT, 0);
    else
        cmd_list.set_RT(NULL, 0);
    if (_2)
        cmd_list.set_RT(_2->pRT, 1);
    else
        cmd_list.set_RT(NULL, 1);
    if (_3)
        cmd_list.set_RT(_3->pRT, 2);
    else
        cmd_list.set_RT(NULL, 2);

    cmd_list.set_ZB(zb);
}

void CRenderTarget::u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth[cmd_list.context_id] = _1->dwWidth;
        dwHeight[cmd_list.context_id] = _1->dwHeight;
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

        dwWidth[cmd_list.context_id] = TexDesc.Width;
        dwHeight[cmd_list.context_id] = TexDesc.Height;
        _RELEASE(pRes);
    }

    if (_1)
        cmd_list.set_RT(_1->pRT, 0);
    else
        cmd_list.set_RT(NULL, 0);
    if (_2)
        cmd_list.set_RT(_2->pRT, 1);
    else
        cmd_list.set_RT(NULL, 1);

    cmd_list.set_ZB(zb);
}

void CRenderTarget::u_setrt(CBackend& cmd_list, u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, ID3DDepthStencilView* zb)
{
    dwWidth[cmd_list.context_id] = W;
    dwHeight[cmd_list.context_id] = H;

    cmd_list.set_RT(_1, 0);
    cmd_list.set_RT(_2, 1);
    cmd_list.set_RT(_3, 2);

    cmd_list.set_ZB(zb);
}

// 2D texgen (texture adjustment matrix)
void CRenderTarget::u_compute_texgen_screen(CBackend& cmd_list, Fmatrix& m_Texgen)
{
    static constexpr Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
    m_Texgen.mul(m_TexelAdjust, cmd_list.xforms.m_wvp);
}

// 2D texgen for jitter (texture adjustment matrix)
void CRenderTarget::u_compute_texgen_jitter(CBackend& cmd_list, Fmatrix& m_Texgen_J)
{
    // place into	0..1 space
    Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
    m_Texgen_J.mul(m_TexelAdjust, cmd_list.xforms.m_wvp);

    // rescale - tile it
    float scale_X = float(Device.dwWidth) / float(TEX_jitter);
    float scale_Y = float(Device.dwHeight) / float(TEX_jitter);
    m_TexelAdjust.scale(scale_X, scale_Y, 1.f);
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
    const u32 SampleCount = RImplementation.o.dx10_msaa ? RImplementation.o.dx10_msaa_samples : 1;

#ifdef DEBUG
    Msg("MSAA samples = %d", SampleCount);
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
    param_color_add.set(0.0f, 0.0f, 0.0f);

    dwAccumulatorClearMark = 0;
    RImplementation.Resources->Evict();

    // Blenders
    b_occq = xr_new<CBlender_light_occq>();
    b_accum_mask = xr_new<CBlender_accum_direct_mask>();
    b_accum_direct = xr_new<CBlender_accum_direct>();
    b_accum_point = xr_new<CBlender_accum_point>();
    b_accum_spot = xr_new<CBlender_accum_spot>();
    b_accum_reflected = xr_new<CBlender_accum_reflected>();
    b_bloom = xr_new<CBlender_bloom_build>();
    if (RImplementation.o.dx10_msaa)
    {
        b_bloom_msaa = xr_new<CBlender_bloom_build_msaa>();
        b_postprocess_msaa = xr_new<CBlender_postprocess_msaa>();
    }
    else
    {
        b_bloom_msaa = nullptr;
        b_postprocess_msaa = nullptr;
    }
    b_luminance = xr_new<CBlender_luminance>();
    b_combine = xr_new<CBlender_combine>();
    b_blur = xr_new<CBlender_blur>();
    b_dof = xr_new<CBlender_dof>();
    b_gasmask_drops = xr_new<CBlender_gasmask_drops>();
    b_gasmask_dudv = xr_new<CBlender_gasmask_dudv>();

    // Screen Space Shaders Stuff
    b_ssfx_rain = xr_new<CBlender_ssfx_rain>();
    b_ssfx_water_blur = xr_new<CBlender_ssfx_water_blur>();
    b_ssfx_bloom = xr_new<CBlender_ssfx_bloom_build>();
    b_ssfx_bloom_lens = xr_new<CBlender_ssfx_bloom_lens>();
    b_ssfx_bloom_downsample = xr_new<CBlender_ssfx_bloom_downsample>();
    b_ssfx_bloom_upsample = xr_new<CBlender_ssfx_bloom_upsample>();
    b_ssfx_sss_ext = xr_new<CBlender_ssfx_sss_ext>(); // SSS
    b_ssfx_sss = xr_new<CBlender_ssfx_sss>(); // SSS
    b_ssfx_ssr = xr_new<CBlender_ssfx_ssr>(); // SSR
    b_ssfx_volumetric_blur = xr_new<CBlender_ssfx_volumetric_blur>(); // Volumetric Blur
    b_ssfx_ao = xr_new<CBlender_ssfx_ao>(); // AO

    if (RImplementation.o.dx10_msaa)
    {
        for (int i = 0; i < 1; ++i)
        {
            static constexpr LPCSTR SampleDefs[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
            b_combine_msaa[i] = xr_new<CBlender_combine_msaa>();
            b_accum_mask_msaa[i] = xr_new<CBlender_accum_direct_mask_msaa>();
            b_accum_direct_msaa[i] = xr_new<CBlender_accum_direct_msaa>();
            b_accum_direct_volumetric_msaa[i] = xr_new<CBlender_accum_direct_volumetric_msaa>();
            b_accum_spot_msaa[i] = xr_new<CBlender_accum_spot_msaa>();
            b_accum_volumetric_msaa[i] = xr_new<CBlender_accum_volumetric_msaa>();
            b_accum_point_msaa[i] = xr_new<CBlender_accum_point_msaa>();
            b_accum_reflected_msaa[i] = xr_new<CBlender_accum_reflected_msaa>();
            static_cast<CBlender_accum_direct_mask_msaa*>(b_accum_mask_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
            static_cast<CBlender_accum_direct_volumetric_msaa*>(b_accum_direct_volumetric_msaa[i])->SetDefine("ISAMPLE", SampleDefs[i]);
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

        rt_Base.resize(HW.BackBufferCount);
        for (u32 i = 0; i < HW.BackBufferCount; i++)
        {
            string32 temp;
            xr_sprintf(temp, "%s%u", r2_RT_base, i);
            rt_Base[i].create(temp, w, h, HW.Caps.fTarget, 1, {CRT::CreateBase});
        }
        rt_Base_Depth.create(r2_RT_base_depth, w, h, HW.Caps.fDepth, 1, {CRT::CreateBase});

        rt_Position.create(r2_RT_P, w, h, D3DFMT_A16B16G16R16F, SampleCount);

        if (RImplementation.o.dx10_msaa)
            rt_MSAADepth.create(r2_RT_MSAAdepth, w, h, D3DFMT_D24S8, SampleCount);

        // select albedo & accum
        rt_Color.create(r2_RT_albedo, w, h, D3DFMT_A8R8G8B8, SampleCount);
        rt_Accumulator.create(r2_RT_accum, w, h, D3DFMT_A16B16G16R16F, SampleCount);

        // generic(LDR) RTs
        rt_Generic_0.create(r2_RT_generic0, w, h, D3DFMT_A8R8G8B8, 1);
        rt_Generic_1.create(r2_RT_generic1, w, h, D3DFMT_A8R8G8B8, 1);
        rt_Generic.create(r2_RT_generic, w, h, D3DFMT_A8R8G8B8, 1);

        rt_Generic_temp.create("$user$generic_temp", w, h, D3DFMT_A8R8G8B8, RImplementation.o.dx10_msaa ? SampleCount : 1);
        rt_dof.create(r2_RT_dof, w, h, D3DFMT_A8R8G8B8);

        rt_secondVP.create(r2_RT_secondVP, w, h, D3DFMT_A8R8G8B8, 1); //--#SM+#-- +SecondVP+
        rt_BeforeUi.create("$user$viewport_temp_before_ui", w, h, D3DFMT_A8R8G8B8, 1);

        // RT Blur
        rt_blur_h_2.create(r2_RT_blur_h_2, u32(w / 2), u32(h / 2), D3DFMT_A8R8G8B8);
        rt_blur_2.create(r2_RT_blur_2, u32(w / 2), u32(h / 2), D3DFMT_A8R8G8B8);
        rt_blur_2_zb.create(r2_RT_blur_2_zb, u32(w / 2), u32(h / 2), D3DFMT_D24S8);

        rt_blur_h_4.create(r2_RT_blur_h_4, u32(w / 4), u32(h / 4), D3DFMT_A8R8G8B8);
        rt_blur_4.create(r2_RT_blur_4, u32(w / 4), u32(h / 4), D3DFMT_A8R8G8B8);
        rt_blur_4_zb.create(r2_RT_blur_4_zb, u32(w / 4), u32(h / 4), D3DFMT_D24S8);

        rt_blur_h_8.create(r2_RT_blur_h_8, u32(w / 8), u32(h / 8), D3DFMT_A8R8G8B8);
        rt_blur_8.create(r2_RT_blur_8, u32(w / 8), u32(h / 8), D3DFMT_A8R8G8B8);
        rt_blur_8_zb.create(r2_RT_blur_8_zb, u32(w / 8), u32(h / 8), D3DFMT_D24S8);

        // Screen Space Shaders Stuff
        rt_ssfx.create(r2_RT_ssfx, w, h, D3DFMT_A8R8G8B8); // Temp RT
        rt_ssfx_temp.create(r2_RT_ssfx_temp, w, h, D3DFMT_A8R8G8B8); // Temp RT
        rt_ssfx_temp2.create(r2_RT_ssfx_temp2, w, h, D3DFMT_A8R8G8B8); // Temp RT
        rt_ssfx_temp3.create(r2_RT_ssfx_temp3, w, h, D3DFMT_A8R8G8B8); // Temp RT

        rt_ssfx_accum.create(r2_RT_ssfx_accum, w, h, D3DFMT_A16B16G16R16F, SampleCount); // Volumetric Acc
        rt_ssfx_ssr.create(r2_RT_ssfx_ssr, w, h, D3DFMT_A8R8G8B8); // SSR Acc
        rt_ssfx_water.create(r2_RT_ssfx_water, w, h, D3DFMT_A8R8G8B8); // Water Acc
        rt_ssfx_ao.create(r2_RT_ssfx_ao, w, h, D3DFMT_A8R8G8B8); // AO Acc
        rt_ssfx_il.create(r2_RT_ssfx_il, w, h, D3DFMT_A8R8G8B8); // IL Acc

        if (RImplementation.o.ssfx_sss)
        {
            rt_ssfx_sss.create(r2_RT_ssfx_sss, w, h, D3DFMT_A8R8G8B8); // SSS Acc
            rt_ssfx_sss_ext.create(r2_RT_ssfx_sss_ext, w, h, D3DFMT_A8R8G8B8); // SSS EXT Acc
            rt_ssfx_sss_ext2.create(r2_RT_ssfx_sss_ext2, w, h, D3DFMT_A8R8G8B8); // SSS EXT Acc
            rt_ssfx_sss_tmp.create(r2_RT_ssfx_sss_tmp, w, h, D3DFMT_A8R8G8B8); // SSS EXT Acc
        }

        if (RImplementation.o.ssfx_bloom)
        {
            rt_ssfx_bloom1.create(r2_RT_ssfx_bloom1, w / 2.0f, h / 2.0f, D3DFMT_A16B16G16R16F); // Bloom
            rt_ssfx_bloom_emissive.create(r2_RT_ssfx_bloom_emissive, w, h, D3DFMT_A8R8G8B8, SampleCount); // Emissive
            rt_ssfx_bloom_lens.create(r2_RT_ssfx_bloom_lens, w / 4.0f, h / 4.0f, D3DFMT_A8R8G8B8); // Lens

            rt_ssfx_bloom_tmp2.create(r2_RT_ssfx_bloom_tmp2, w / 2.0f, h / 2.0f, D3DFMT_A16B16G16R16F); // Bloom / 2
            rt_ssfx_bloom_tmp4.create(r2_RT_ssfx_bloom_tmp4, w / 4.0f, h / 4.0f, D3DFMT_A16B16G16R16F); // Bloom / 4
            rt_ssfx_bloom_tmp8.create(r2_RT_ssfx_bloom_tmp8, w / 8.0f, h / 8.0f, D3DFMT_A16B16G16R16F); // Bloom / 8
            rt_ssfx_bloom_tmp16.create(r2_RT_ssfx_bloom_tmp16, w / 16.0f, h / 16.0f, D3DFMT_A16B16G16R16F); // Bloom / 16
            rt_ssfx_bloom_tmp32.create(r2_RT_ssfx_bloom_tmp32, w / 32.0f, h / 32.0f, D3DFMT_A16B16G16R16F); // Bloom / 32
            rt_ssfx_bloom_tmp64.create(r2_RT_ssfx_bloom_tmp64, w / 64.0f, h / 64.0f, D3DFMT_A16B16G16R16F); // Bloom / 64

            rt_ssfx_bloom_tmp32_2.create(r2_RT_ssfx_bloom_tmp32_2, w / 32.0f, h / 32.0f, D3DFMT_A16B16G16R16F); // Bloom / 32
            rt_ssfx_bloom_tmp16_2.create(r2_RT_ssfx_bloom_tmp16_2, w / 16.0f, h / 16.0f, D3DFMT_A16B16G16R16F); // Bloom / 16
            rt_ssfx_bloom_tmp8_2.create(r2_RT_ssfx_bloom_tmp8_2, w / 8.0f, h / 8.0f, D3DFMT_A16B16G16R16F); // Bloom / 8
            rt_ssfx_bloom_tmp4_2.create(r2_RT_ssfx_bloom_tmp4_2, w / 4.0f, h / 4.0f, D3DFMT_A16B16G16R16F); // Bloom / 4
        }

        rt_ssfx_volumetric.create(r2_RT_ssfx_volumetric, w / 8.0f, h / 8.0f, D3DFMT_A16B16G16R16F); // Volumetric
        rt_ssfx_volumetric_tmp.create(r2_RT_ssfx_volumetric_tmp, w / 8.0f, h / 8.0f, D3DFMT_A16B16G16R16F); // Volumetric
        rt_ssfx_rain.create(r2_RT_ssfx_rain, w / 8.0f, h / 8.0f, D3DFMT_A8R8G8B8); // Rain refraction buffer
        rt_ssfx_water_waves.create(r2_RT_ssfx_water_waves, 512, 512, D3DFMT_A8R8G8B8); // Water Waves

        rt_ssfx_prevPos.create(r2_RT_ssfx_prevPos, w, h, D3DFMT_A16B16G16R16F, SampleCount);

        rt_ssfx_hud.create(r2_RT_ssfx_hud, w, h, D3DFMT_A16B16G16R16F); // HUD mask & Velocity buffer

        if (RImplementation.o.dx10_msaa)
        {
            rt_Generic_0_r.create(r2_RT_generic0_r, w, h, D3DFMT_A8R8G8B8, SampleCount);
            rt_Generic_1_r.create(r2_RT_generic1_r, w, h, D3DFMT_A8R8G8B8, SampleCount);
        }
        //	Igor: for volumetric lights
        // rt_Generic_2.create			(r2_RT_generic2,w,h,D3DFMT_A8R8G8B8		);
        //	temp: for higher quality blends
        rt_Generic_2.create(r2_RT_generic2, w, h, D3DFMT_A16B16G16R16F, SampleCount);
    }

    s_blur.create(b_blur, "r2\\blur");
    s_dof.create(b_dof, "r2\\dof");
    s_gasmask_drops.create(b_gasmask_drops, "r2\\gasmask_drops");
    s_gasmask_dudv.create(b_gasmask_dudv, "r2\\gasmask_dudv");

    // OCCLUSION
    s_occq.create(b_occq, "r2\\occq");

    // Screen Space Shaders Stuff
    s_ssfx_rain.create(b_ssfx_rain, "ssfx_rain"); // SSS Rain
    s_ssfx_bloom.create(b_ssfx_bloom, "ssfx_bloom"); // SSS Bloom
    s_ssfx_bloom_lens.create(b_ssfx_bloom_lens, "ssfx_bloom_flares"); // SSS Bloom Lens flare
    s_ssfx_bloom_downsample.create(b_ssfx_bloom_downsample, "ssfx_bloom_downsample"); // SSS Bloom
    s_ssfx_bloom_upsample.create(b_ssfx_bloom_upsample, "ssfx_bloom_upsample"); // SSS Bloom
    s_ssfx_sss_ext.create(b_ssfx_sss_ext, "ssfx_sss_ext"); // SSS Extended
    s_ssfx_sss.create(b_ssfx_sss, "ssfx_sss"); // SSS
    s_ssfx_ssr.create(b_ssfx_ssr, "ssfx_ssr"); // SSR
    s_ssfx_volumetric_blur.create(b_ssfx_volumetric_blur, "ssfx_volumetric_blur"); // Volumetric Blur

    s_ssfx_water_ssr.create("ssfx_water_ssr"); // Water SSR
    s_ssfx_water.create("ssfx_water"); // Water
    s_ssfx_water_blur.create(b_ssfx_water_blur, "ssfx_water_blur"); // Water

    s_ssfx_ao.create(b_ssfx_ao, "ssfx_ao"); // SSR

    string32 cskin_buffer;
    for (int skin_num = 0; skin_num < 5; skin_num++)
    {
        sprintf(cskin_buffer, "ssfx_hud_skin%i", skin_num);
        s_ssfx_hud[skin_num].create(cskin_buffer);
    }

    // DIRECT (spot)
    D3DFORMAT depth_format = (D3DFORMAT)RImplementation.o.HW_smap_FORMAT;

    u32 size = RImplementation.o.smapsize;
    rt_smap_depth.create(r2_RT_smap_depth, size, size, depth_format);

    s_accum_mask.create(b_accum_mask, "accum_mask");
    s_accum_direct.create(b_accum_direct, "accum_direct");

    if (RImplementation.o.dx10_msaa)
    {
        for (int i = 0; i < 1; ++i)
        {
            s_accum_direct_msaa[i].create(b_accum_direct_msaa[i], "accum_direct");
            s_accum_mask_msaa[i].create(b_accum_mask_msaa[i], "accum_direct");
        }
    }

    s_accum_direct_volumetric.create("accum_volumetric_sun_nomsaa");

    if (RImplementation.o.dx10_msaa)
    {
        static constexpr LPCSTR snames[] = {"accum_volumetric_sun_msaa0", "accum_volumetric_sun_msaa1", "accum_volumetric_sun_msaa2", "accum_volumetric_sun_msaa3",
                                            "accum_volumetric_sun_msaa4", "accum_volumetric_sun_msaa5", "accum_volumetric_sun_msaa6", "accum_volumetric_sun_msaa7"};

        for (int i = 0; i < 1; ++i)
        {
            // s_accum_direct_volumetric_msaa[i].create		(b_accum_direct_volumetric_sun_msaa[i],			"accum_direct");
            s_accum_direct_volumetric_msaa[i].create(snames[i]);
        }
    }

    //	RAIN
    //	TODO: DX10: Create resources only when DX10 rain is enabled.
    //	Or make DX10 rain switch dynamic?
    {
        CBlender_rain TempBlender;
        s_rain.create(&TempBlender, "null");

        if (RImplementation.o.dx10_msaa)
        {
            static constexpr LPCSTR SampleDefs[] = {"0", "1", "2", "3", "4", "5", "6", "7"};
            CBlender_rain_msaa TempBlender[8];

            for (int i = 0; i < 1; ++i)
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
            for (int i = 0; i < 1; ++i)
            {
                s_accum_reflected_msaa[i].create(b_accum_reflected_msaa[i], "null");
            }
        }
    }

    // BLOOM
    {
        constexpr D3DFORMAT fmt = D3DFMT_A8R8G8B8; //;		// D3DFMT_X8R8G8B8
        u32 w = BLOOM_size_X, h = BLOOM_size_Y;
        constexpr u32 fvf_build = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
        constexpr u32 fvf_filter = (u32)D3DFVF_XYZRHW | D3DFVF_TEX8 | D3DFVF_TEXCOORDSIZE4(0) | D3DFVF_TEXCOORDSIZE4(1) | D3DFVF_TEXCOORDSIZE4(2) | D3DFVF_TEXCOORDSIZE4(3) |
            D3DFVF_TEXCOORDSIZE4(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6) | D3DFVF_TEXCOORDSIZE4(7);
        rt_Bloom_1.create(r2_RT_bloom1, w, h, fmt);
        rt_Bloom_2.create(r2_RT_bloom2, w, h, fmt);
        g_bloom_build.create(fvf_build, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_bloom_filter.create(fvf_filter, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
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

            Fcolor ColorRGBA = {127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f, 127.0f / 255.0f};
            RCache.ClearRT(rt_LUM_pool[it], ColorRGBA);
        }

        u_setrt(RCache, Device.dwWidth, Device.dwHeight, get_base_rt(), NULL, NULL, get_base_zb());
    }

    // COMBINE
    {
        static constexpr D3DVERTEXELEMENT9 dwDecl[] = {{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // pos+uv
                                                       D3DDECL_END()};
        s_combine.create(b_combine, "r2\\combine");
        s_combine_volumetric.create("combine_volumetric");
        g_combine_VP.create(dwDecl, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_combine.create(FVF::F_TL, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_combine_2UV.create(FVF::F_TL2uv, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_combine_cuboid.create(dwDecl, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());

        constexpr u32 fvf_aa_blur = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
        g_aa_blur.create(fvf_aa_blur, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

        constexpr u32 fvf_aa_AA = D3DFVF_XYZRHW | D3DFVF_TEX7 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3) |
            D3DFVF_TEXCOORDSIZE2(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6);
        g_aa_AA.create(fvf_aa_AA, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

        t_envmap_0.create(r2_T_envs0);
        t_envmap_1.create(r2_T_envs1);
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
                t_noise[it] = RImplementation.Resources->_CreateTexture(name);
                t_noise[it]->surface_set(t_noise_surf[it]);
            }
        }
    }

    // PP
    s_postprocess.create("postprocess");
    g_postprocess.create(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX3, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

    // Menu
    s_menu.create("distort");
    g_menu.create(FVF::F_TL, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

    //
    dwWidth[RCache.context_id] = Device.dwWidth;
    dwHeight[RCache.context_id] = Device.dwHeight;
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

    ////////////lvutner
    xr_delete(b_blur);
    xr_delete(b_dof);
    xr_delete(b_gasmask_drops);
    xr_delete(b_gasmask_dudv);

    // [ SSS Stuff ]
    xr_delete(b_ssfx_rain); // SSS Rain
    xr_delete(b_ssfx_water_blur); // SSS Water Blur
    xr_delete(b_ssfx_bloom); // SSS Bloom
    xr_delete(b_ssfx_bloom_lens); // SSS Bloom Lens
    xr_delete(b_ssfx_bloom_downsample); // SSS Bloom Blur
    xr_delete(b_ssfx_bloom_upsample); // SSS Bloom Blur
    xr_delete(b_ssfx_sss_ext); // SSS Phase Ext
    xr_delete(b_ssfx_sss); // SSS Phase
    xr_delete(b_ssfx_ssr); // SSR Phase
    xr_delete(b_ssfx_volumetric_blur); // Volumetric Phase
    xr_delete(b_ssfx_ao); // AO Phase

    if (RImplementation.o.dx10_msaa)
    {
        for (int i = 0; i < 1; ++i)
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

        xr_delete(b_postprocess_msaa);
        xr_delete(b_bloom_msaa);
    }
    xr_delete(b_accum_mask);
    xr_delete(b_occq);
}

void CRenderTarget::reset_light_marker(CBackend& cmd_list, bool bResetStencil)
{
    dwLightMarkerID = 5;

    if (!bResetStencil)
        return;

    u32 Offset;
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    constexpr u32 C = color_rgba(255, 255, 255, 255);
    constexpr float eps = 0;
    constexpr float _dw = 0.5f;
    constexpr float _dh = 0.5f;
    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(-_dw, _h - _dh, eps, 1.f, C, 0, 0);
    pv++;
    pv->set(-_dw, -_dh, eps, 1.f, C, 0, 0);
    pv++;
    pv->set(_w - _dw, _h - _dh, eps, 1.f, C, 0, 0);
    pv++;
    pv->set(_w - _dw, -_dh, eps, 1.f, C, 0, 0);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);
    cmd_list.set_Element(s_occq->E[2]);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

void CRenderTarget::increment_light_marker(CBackend& cmd_list)
{
    dwLightMarkerID += 2;

    const u32 iMaxMarkerValue = RImplementation.o.dx10_msaa ? 127 : 255;

    if (dwLightMarkerID > iMaxMarkerValue)
        reset_light_marker(cmd_list, true);
}

bool CRenderTarget::need_to_render_sunshafts()
{
    if (!ps_r_sunshafts_mode)
        return false;

    {
        CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        if (E.m_fSunShaftsIntensity < EPS)
            return false;
    }

    return true;
}
