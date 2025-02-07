#include "stdafx.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

struct v_build
{
    Fvector4 p;
    Fvector2 uv0;
    Fvector2 uv1;
    Fvector2 uv2;
    Fvector2 uv3;
};
static_assert(sizeof(v_build) == 48);

struct v_filter
{
    Fvector4 p;
    Fvector4 uv0;
    Fvector4 uv1;
    Fvector4 uv2;
    Fvector4 uv3;
    Fvector4 uv4;
    Fvector4 uv5;
    Fvector4 uv6;
    Fvector4 uv7;
};
static_assert(sizeof(v_filter) == 144);

// Gauss filtering coeffs
// Samples:			0-central, -1, -2,..., -7, 1, 2,... 7
//
void CalcGauss_k7(Fvector4& w0, // weight
                  Fvector4& w1, // weight
                  float r = 3.3f, // gaussian radius
                  float s_out = 1.f // resulting magnitude
)
{
    float W[8];

    // calculate
    float mag = 0;
    for (int i = -7; i <= 0; i++)
        W[-i] = expf(-float(i * i) / (2 * r * r)); // weight
    for (int i = 0; i < 8; i++)
        mag += i ? 2 * W[i] : W[i]; // symmetrical weight
    for (int i = 0; i < 8; i++)
        W[i] = s_out * W[i] / mag;

    // W[0]=0, W[7]=-7
    w0.set(W[1], W[2], W[3], W[4]); // -1, -2, -3, -4
    w1.set(W[5], W[6], W[7], W[0]); // -5, -6, -7, 0
}
void CalcGauss_wave(Fvector4& w0, // weight
                    Fvector4& w1, // weight
                    float r_base = 3.3f, // gaussian radius
                    float r_detail = 1.0f, // gaussian radius
                    float s_out = 1.f // resulting magnitude
)
{
    Fvector4 t0, t1;
    CalcGauss_k7(w0, w1, r_base, s_out);
    CalcGauss_k7(t0, t1, r_detail, s_out);
    w0.add(t0);
    w1.add(t1);
}

void CRenderTarget::phase_bloom()
{
    PIX_EVENT(phase_bloom);
    u32 Offset;

    // Targets
    u_setrt(rt_Bloom_1, NULL, NULL, NULL); // No need for ZBuffer at all
    // RImplementation.rmNormal();

    // Clear	- don't clear - it's stupid here :)
    // Stencil	- disable
    // Misc		- draw everything (no culling)
    // CHK_DX		(HW.pDevice->SetRenderState	( D3DRS_ZENABLE,		FALSE				));
    RCache.set_Z(FALSE);

    // Transfer into Bloom1
    {
        float _w = float(Device.dwWidth);
        float _h = float(Device.dwHeight);
        float _2w = _w / 2;
        float tw = BLOOM_size_X;
        float _2h = _h / 2;
        float th = BLOOM_size_Y;
        float _aspect_w = _2w / tw;
        float _aspect_h = _2h / th;
        Fvector2 one = {1.f / _w, 1.f / _h};
        one.x *= _aspect_w;
        one.y *= _aspect_h;
        Fvector2 half = {.5f / _w, .5f / _h};
        Fvector2 a_0 = {half.x + 0, half.y + 0};
        Fvector2 a_1 = {half.x + one.x, half.y + 0};
        Fvector2 a_2 = {half.x + 0, half.y + one.y};
        Fvector2 a_3 = {half.x + one.x, half.y + one.y};
        Fvector2 b_0 = {1 + a_0.x, 1 + a_0.y};
        Fvector2 b_1 = {1 + a_1.x, 1 + a_1.y};
        Fvector2 b_2 = {1 + a_2.x, 1 + a_2.y};
        Fvector2 b_3 = {1 + a_3.x, 1 + a_3.y};

        // Fill vertex buffer
        v_build* pv = (v_build*)RCache.Vertex.Lock(4, g_bloom_build->vb_stride, Offset);
        pv->p.set(EPS, float(th + EPS), EPS, 1.f);
        pv->uv0.set(a_0.x, b_0.y);
        pv->uv1.set(a_1.x, b_1.y);
        pv->uv2.set(a_2.x, b_2.y);
        pv->uv3.set(a_3.x, b_3.y);
        pv++;
        pv->p.set(EPS, EPS, EPS, 1.f);
        pv->uv0.set(a_0.x, a_0.y);
        pv->uv1.set(a_1.x, a_1.y);
        pv->uv2.set(a_2.x, a_2.y);
        pv->uv3.set(a_3.x, a_3.y);
        pv++;
        pv->p.set(float(tw + EPS), float(th + EPS), EPS, 1.f);
        pv->uv0.set(b_0.x, b_0.y);
        pv->uv1.set(b_1.x, b_1.y);
        pv->uv2.set(b_2.x, b_2.y);
        pv->uv3.set(b_3.x, b_3.y);
        pv++;
        pv->p.set(float(tw + EPS), EPS, EPS, 1.f);
        pv->uv0.set(b_0.x, a_0.y);
        pv->uv1.set(b_1.x, a_1.y);
        pv->uv2.set(b_2.x, a_2.y);
        pv->uv3.set(b_3.x, a_3.y);
        pv++;
        RCache.Vertex.Unlock(4, g_bloom_build->vb_stride);

        // Perform combine (all scalers must account for 4 samples + final diffuse multiply);
        float s = ps_r2_ls_bloom_threshold; // scale
        f_bloom_factor = .9f * f_bloom_factor + .1f * ps_r2_ls_bloom_speed * Device.fTimeDelta; // speed
        if (!RImplementation.o.dx10_msaa)
            RCache.set_Element(s_bloom->E[0]);
        else
            RCache.set_Element(s_bloom_msaa->E[0]);
        RCache.set_c("b_params", s, s, s, f_bloom_factor);
        RCache.set_Geometry(g_bloom_build);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Capture luminance values
    phase_luminance();

    if (RImplementation.o.ssfx_bloom)
        return;

    // SLOW FILTER
    // Transfer into Bloom2, horizontal filter
    {
        float _w = BLOOM_size_X;
        float _h = BLOOM_size_Y;
        Fvector2 two = {2.f / _w, 2.f / _h};
        Fvector2 one = {1.f / _w, 1.f / _h};
        Fvector2 half = {.5f / _w, .5f / _h};
        Fvector4 a_0 = {half.x, half.y, half.y, half.x}; // center
        Fvector4 a_1 = {a_0.x - one.x - half.x, half.y, half.y, a_0.w + one.x + half.x}; // -1,+1i
        Fvector4 a_2 = {a_1.x - two.x, half.y, half.y, a_1.w + two.x}; // -2,+2i
        Fvector4 a_3 = {a_2.x - two.x, half.y, half.y, a_2.w + two.x}; // -3,+3i
        Fvector4 a_4 = {a_3.x - two.x, half.y, half.y, a_3.w + two.x}; // -4,+4i
        Fvector4 a_5 = {a_4.x - two.x, half.y, half.y, a_4.w + two.x}; // -5,+5i
        Fvector4 a_6 = {a_5.x - two.x, half.y, half.y, a_5.w + two.x}; // -6,+6i
        Fvector4 a_7 = {a_6.x - two.x, half.y, half.y, a_6.w + two.x}; // -7,+7i

        // Fill vertex buffer
        v_filter* pv = (v_filter*)RCache.Vertex.Lock(4, g_bloom_filter->vb_stride, Offset);

        // 0 - LB
        pv->p.set(EPS, float(_h + EPS), EPS, 1.f);
        pv->uv0.set(a_0.x, 1 + a_0.y, 0, 0);
        pv->uv1.set(a_1.x, 1 + a_1.y, 1 + a_1.z, a_1.w);
        pv->uv2.set(a_2.x, 1 + a_2.y, 1 + a_2.z, a_2.w);
        pv->uv3.set(a_3.x, 1 + a_3.y, 1 + a_3.z, a_3.w);
        pv->uv4.set(a_4.x, 1 + a_4.y, 1 + a_4.z, a_4.w);
        pv->uv5.set(a_5.x, 1 + a_5.y, 1 + a_5.z, a_5.w);
        pv->uv6.set(a_6.x, 1 + a_6.y, 1 + a_6.z, a_6.w);
        pv->uv7.set(a_7.x, 1 + a_7.y, 1 + a_7.z, a_7.w);
        pv++;

        // 1 - LT
        pv->p.set(EPS, EPS, EPS, 1.f);
        pv->uv0.set(a_0.x, a_0.y, 0, 0);
        pv->uv1.set(a_1.x, a_1.y, a_1.z, a_1.w);
        pv->uv2.set(a_2.x, a_2.y, a_2.z, a_2.w);
        pv->uv3.set(a_3.x, a_3.y, a_3.z, a_3.w);
        pv->uv4.set(a_4.x, a_4.y, a_4.z, a_4.w);
        pv->uv5.set(a_5.x, a_5.y, a_5.z, a_5.w);
        pv->uv6.set(a_6.x, a_6.y, a_6.z, a_6.w);
        pv->uv7.set(a_7.x, a_7.y, a_7.z, a_7.w);
        pv++;

        // 2 - RB
        pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f);
        pv->uv0.set(1 + a_0.x, 1 + a_0.y, 0, 0);
        pv->uv1.set(1 + a_1.x, 1 + a_1.y, 1 + a_1.z, 1 + a_1.w);
        pv->uv2.set(1 + a_2.x, 1 + a_2.y, 1 + a_2.z, 1 + a_2.w);
        pv->uv3.set(1 + a_3.x, 1 + a_3.y, 1 + a_3.z, 1 + a_3.w);
        pv->uv4.set(1 + a_4.x, 1 + a_4.y, 1 + a_4.z, 1 + a_4.w);
        pv->uv5.set(1 + a_5.x, 1 + a_5.y, 1 + a_5.z, 1 + a_5.w);
        pv->uv6.set(1 + a_6.x, 1 + a_6.y, 1 + a_6.z, 1 + a_6.w);
        pv->uv7.set(1 + a_7.x, 1 + a_7.y, 1 + a_7.z, 1 + a_7.w);
        pv++;

        // 3 - RT
        pv->p.set(float(_w + EPS), EPS, EPS, 1.f);
        pv->uv0.set(1 + a_0.x, a_0.y, 0, 0);
        pv->uv1.set(1 + a_1.x, a_1.y, a_1.z, 1 + a_1.w);
        pv->uv2.set(1 + a_2.x, a_2.y, a_2.z, 1 + a_2.w);
        pv->uv3.set(1 + a_3.x, a_3.y, a_3.z, 1 + a_3.w);
        pv->uv4.set(1 + a_4.x, a_4.y, a_4.z, 1 + a_4.w);
        pv->uv5.set(1 + a_5.x, a_5.y, a_5.z, 1 + a_5.w);
        pv->uv6.set(1 + a_6.x, a_6.y, a_6.z, 1 + a_6.w);
        pv->uv7.set(1 + a_7.x, a_7.y, a_7.z, 1 + a_7.w);
        pv++;
        RCache.Vertex.Unlock(4, g_bloom_filter->vb_stride);

        // Perform filtering
        Fvector4 w0, w1;
        float kernel = ps_r2_ls_bloom_kernel_g;
        CalcGauss_wave(w0, w1, kernel, kernel / 3.f, ps_r2_ls_bloom_kernel_scale);
        u_setrt(rt_Bloom_2, NULL, NULL, NULL); // No need for ZBuffer at all
        RCache.set_Element(s_bloom->E[1]);
        RCache.set_ca("weight", 0, w0);
        RCache.set_ca("weight", 1, w1);
        RCache.set_Geometry(g_bloom_filter);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Transfer into Bloom1, vertical filter
    {
        float _w = BLOOM_size_X;
        float _h = BLOOM_size_Y;
        Fvector2 two = {2.f / _w, 2.f / _h};
        Fvector2 one = {1.f / _w, 1.f / _h};
        Fvector2 half = {.5f / _w, .5f / _h};
        Fvector4 a_0 = {half.x, half.y, half.y, half.x}; // center
        Fvector4 a_1 = {half.x, a_0.y - one.y - half.y, half.y + one.y + a_0.z, half.x}; // -1,+1i
        Fvector4 a_2 = {half.x, a_1.y - two.y, two.y + a_1.z, half.x}; // -2,+2i
        Fvector4 a_3 = {half.x, a_2.y - two.y, two.y + a_2.z, half.x}; // -3,+3i
        Fvector4 a_4 = {half.x, a_3.y - two.y, two.y + a_3.z, half.x}; // -4,+4i
        Fvector4 a_5 = {half.x, a_4.y - two.y, two.y + a_4.z, half.x}; // -5,+5i
        Fvector4 a_6 = {half.x, a_5.y - two.y, two.y + a_5.z, half.x}; // -6,+6i
        Fvector4 a_7 = {half.x, a_6.y - two.y, two.y + a_6.z, half.x}; // -7,+7i

        // Fill vertex buffer
        v_filter* pv = (v_filter*)RCache.Vertex.Lock(4, g_bloom_filter->vb_stride, Offset);

        // 0 - LB
        pv->p.set(EPS, float(_h + EPS), EPS, 1.f);
        pv->uv0.set(a_0.x, 1 + a_0.y, 0, 0);
        pv->uv1.set(a_1.x, 1 + a_1.y, 1 + a_1.z, a_1.w);
        pv->uv2.set(a_2.x, 1 + a_2.y, 1 + a_2.z, a_2.w);
        pv->uv3.set(a_3.x, 1 + a_3.y, 1 + a_3.z, a_3.w);
        pv->uv4.set(a_4.x, 1 + a_4.y, 1 + a_4.z, a_4.w);
        pv->uv5.set(a_5.x, 1 + a_5.y, 1 + a_5.z, a_5.w);
        pv->uv6.set(a_6.x, 1 + a_6.y, 1 + a_6.z, a_6.w);
        pv->uv7.set(a_7.x, 1 + a_7.y, 1 + a_7.z, a_7.w);
        pv++;

        // 1 - LT
        pv->p.set(EPS, EPS, EPS, 1.f);
        pv->uv0.set(a_0.x, a_0.y, 0, 0);
        pv->uv1.set(a_1.x, a_1.y, a_1.z, a_1.w);
        pv->uv2.set(a_2.x, a_2.y, a_2.z, a_2.w);
        pv->uv3.set(a_3.x, a_3.y, a_3.z, a_3.w);
        pv->uv4.set(a_4.x, a_4.y, a_4.z, a_4.w);
        pv->uv5.set(a_5.x, a_5.y, a_5.z, a_5.w);
        pv->uv6.set(a_6.x, a_6.y, a_6.z, a_6.w);
        pv->uv7.set(a_7.x, a_7.y, a_7.z, a_7.w);
        pv++;

        // 2 - RB
        pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f);
        pv->uv0.set(1 + a_0.x, 1 + a_0.y, 0, 0);
        pv->uv1.set(1 + a_1.x, 1 + a_1.y, 1 + a_1.z, 1 + a_1.w);
        pv->uv2.set(1 + a_2.x, 1 + a_2.y, 1 + a_2.z, 1 + a_2.w);
        pv->uv3.set(1 + a_3.x, 1 + a_3.y, 1 + a_3.z, 1 + a_3.w);
        pv->uv4.set(1 + a_4.x, 1 + a_4.y, 1 + a_4.z, 1 + a_4.w);
        pv->uv5.set(1 + a_5.x, 1 + a_5.y, 1 + a_5.z, 1 + a_5.w);
        pv->uv6.set(1 + a_6.x, 1 + a_6.y, 1 + a_6.z, 1 + a_6.w);
        pv->uv7.set(1 + a_7.x, 1 + a_7.y, 1 + a_7.z, 1 + a_7.w);
        pv++;

        // 3 - RT
        pv->p.set(float(_w + EPS), EPS, EPS, 1.f);
        pv->uv0.set(1 + a_0.x, a_0.y, 0, 0);
        pv->uv1.set(1 + a_1.x, a_1.y, a_1.z, 1 + a_1.w);
        pv->uv2.set(1 + a_2.x, a_2.y, a_2.z, 1 + a_2.w);
        pv->uv3.set(1 + a_3.x, a_3.y, a_3.z, 1 + a_3.w);
        pv->uv4.set(1 + a_4.x, a_4.y, a_4.z, 1 + a_4.w);
        pv->uv5.set(1 + a_5.x, a_5.y, a_5.z, 1 + a_5.w);
        pv->uv6.set(1 + a_6.x, a_6.y, a_6.z, 1 + a_6.w);
        pv->uv7.set(1 + a_7.x, a_7.y, a_7.z, 1 + a_7.w);
        pv++;
        RCache.Vertex.Unlock(4, g_bloom_filter->vb_stride);

        // Perform filtering
        Fvector4 w0, w1;
        float kernel = ps_r2_ls_bloom_kernel_g * float(Device.dwHeight) / float(Device.dwWidth);
        CalcGauss_wave(w0, w1, kernel, kernel / 3.f, ps_r2_ls_bloom_kernel_scale);
        u_setrt(rt_Bloom_1, NULL, NULL, NULL); // No need for ZBuffer at all
        RCache.set_Element(s_bloom->E[2]);
        RCache.set_ca("weight", 0, w0);
        RCache.set_ca("weight", 1, w1);
        RCache.set_Geometry(g_bloom_filter);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // we are left here with bloom-target setup as primary one
    bool _menu_pp = g_pGamePersistent ? g_pGamePersistent->OnRenderPPUI_query() : false;
    if (_menu_pp)
    {
        // CHK_DX				(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	0,	1.0f, 0L));
        constexpr FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
    };

    // re-enable z-buffer
    // CHK_DX		(HW.pDevice->SetRenderState	( D3DRS_ZENABLE,	TRUE				));
    RCache.set_Z(TRUE);
}

void CRenderTarget::phase_ssfx_bloom()
{
    // Constants
    u32 Offset = 0;
    constexpr u32 C = color_rgba(0, 0, 0, 0);

    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    // BLOOM BUILD ////////////////////////////////////////////////////
    // Half resolution is the max size for everything
    set_viewport_size(HW.pContext, w / 2.0f, h / 2.0f);

    u_setrt(rt_ssfx_bloom1, 0, 0, NULL);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
    pv++;
    pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
    pv++;
    pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
    pv++;
    pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_bloom->E[0]);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // BLOOM LENS /////////////////////////////////////////////////////
    if (ps_r2_mask_control.x > 0)
    {
        set_viewport_size(HW.pContext, w / 4.0f, h / 4.0f);

        u_setrt(rt_ssfx_bloom_tmp4, 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
        pv++;
        pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
        pv++;
        pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
        pv++;
        pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_bloom_lens->E[0]);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

        // Lens 2 Phase blur
        ref_rt* rt_LensBlur[2] = {&rt_ssfx_bloom_tmp4_2, &rt_ssfx_bloom_lens};

        for (int lensblur = 0; lensblur < 2; lensblur++)
        {
            u_setrt(*rt_LensBlur[lensblur], 0, 0, NULL);
            RCache.set_CullMode(CULL_NONE);
            RCache.set_Stencil(FALSE);

            // Fill vertex buffer
            pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
            pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
            pv++;
            pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
            pv++;
            pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
            pv++;
            pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
            pv++;
            RCache.Vertex.Unlock(4, g_combine->vb_stride);

            // Draw COLOR
            RCache.set_Element(s_ssfx_bloom_lens->E[1 + lensblur]);
            RCache.set_c("blur_setup", w / 4, h / 4, 0.f, 2.0f + (3.0f * lensblur));
            RCache.set_Geometry(g_combine);
            RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
        }
    }

    int SampleScale = 0;

    ref_rt* rt_Down[6] = {&rt_ssfx_bloom_tmp2, &rt_ssfx_bloom_tmp4, &rt_ssfx_bloom_tmp8, &rt_ssfx_bloom_tmp16, &rt_ssfx_bloom_tmp32, &rt_ssfx_bloom_tmp64};

    // BLOOM DOWNSAMPLE ///////////////////////////////////////////////
    for (int downsample = 0; downsample < 6; downsample++)
    {
        SampleScale = 1 << (downsample + 1);

        set_viewport_size(HW.pContext, w / SampleScale, h / SampleScale);

        u_setrt(*rt_Down[downsample], 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
        pv++;
        pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
        pv++;
        pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
        pv++;
        pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_bloom_downsample->E[downsample]);
        RCache.set_c("blur_setup", w / SampleScale, h / SampleScale, 0.f, ps_ssfx_bloom_2.x);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // End with `rt_ssfx_bloom_tmp2`
    ref_rt* rt_Up[5] = {&rt_ssfx_bloom_tmp32_2, &rt_ssfx_bloom_tmp16_2, &rt_ssfx_bloom_tmp8_2, &rt_ssfx_bloom_tmp4_2, &rt_ssfx_bloom_tmp2};

    // BLOOM UPSAMPLE /////////////////////////////////////////////////
    for (int upsample = 0; upsample < 5; upsample++)
    {
        SampleScale = 1 << (5 - upsample);

        set_viewport_size(HW.pContext, w / SampleScale, h / SampleScale);

        u_setrt(*rt_Up[upsample], 0, 0, NULL);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_Stencil(FALSE);

        // Fill vertex buffer
        pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
        pv++;
        pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
        pv++;
        pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
        pv++;
        pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
        pv++;
        RCache.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw COLOR
        RCache.set_Element(s_ssfx_bloom_upsample->E[upsample]);
        RCache.set_c("blur_setup", w / SampleScale, h / SampleScale, 0.f, ps_ssfx_bloom_2.x);
        RCache.set_Geometry(g_combine);
        RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // The Upsample ends with `Half Res`

    // BLOOM COMBINE ///////////////////////////////////////////////
    u_setrt(rt_ssfx_bloom1, 0, 0, NULL);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, h, EPS_S, 1.0f, C, 0.0f, 1.0f);
    pv++;
    pv->set(0, 0, EPS_S, 1.0f, C, 0.0f, 0.0f);
    pv++;
    pv->set(w, h, EPS_S, 1.0f, C, 1.0f, 1.0f);
    pv++;
    pv->set(w, 0, EPS_S, 1.0f, C, 1.0f, 0.0f);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    RCache.set_Element(s_ssfx_bloom->E[1]);
    RCache.set_c("mask_control", ps_r2_mask_control);
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

    // Restore Viewport
    set_viewport_size(HW.pContext, Device.dwWidth, Device.dwHeight);
}
