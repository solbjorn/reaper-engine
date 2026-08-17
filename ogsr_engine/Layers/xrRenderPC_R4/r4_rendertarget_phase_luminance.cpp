#include "stdafx.h"

struct v_build final
{
    Fvector4 p;
    Fvector2 uv0;
    Fvector2 uv1;
    Fvector2 uv2;
    Fvector2 uv3;
};
static_assert(sizeof(v_build) == CRenderTarget::bloom_build_stride);

struct v_filter final
{
    Fvector4 p;
    Fvector4 uv[8];
};
static_assert(sizeof(v_filter) == CRenderTarget::bloom_filter_stride);

void CRenderTarget::phase_luminance(CBackend& cmd_list)
{
    XR_TRACY_ZONE_SCOPED();

    constexpr float eps = 0;

    // Targets
    cmd_list.set_Stencil(FALSE);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_ColorWriteEnable();
    cmd_list.set_Z(FALSE);

    // 000: Perform LUM-SAT, pass 0, 256x256 => 64x64
    u_setrt(cmd_list, rt_LUM_64, {}, {}, nullptr);

    {
        constexpr float ts = 64;
        float _w = float(BLOOM_size_X);
        float _h = float(BLOOM_size_Y);
        Fvector2 one{2.f / _w, 2.f / _h}; // two, infact
        Fvector2 half{1.f / _w, 1.f / _h}; // one, infact
        Fvector2 a_0{half.x + 0, half.y + 0};
        Fvector2 a_1{half.x + one.x, half.y + 0};
        Fvector2 a_2{half.x + 0, half.y + one.y};
        Fvector2 a_3{half.x + one.x, half.y + one.y};
        Fvector2 b_0{1 + a_0.x, 1 + a_0.y};
        Fvector2 b_1{1 + a_1.x, 1 + a_1.y};
        Fvector2 b_2{1 + a_2.x, 1 + a_2.y};
        Fvector2 b_3{1 + a_3.x, 1 + a_3.y};

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<v_build>(4);

        verts[0].p.set(eps, float(ts + eps), eps, 1.f);
        verts[0].uv0.set(a_0.x, b_0.y);
        verts[0].uv1.set(a_1.x, b_1.y);
        verts[0].uv2.set(a_2.x, b_2.y);
        verts[0].uv3.set(a_3.x, b_3.y);

        verts[1].p.set(eps, eps, eps, 1.f);
        verts[1].uv0.set(a_0.x, a_0.y);
        verts[1].uv1.set(a_1.x, a_1.y);
        verts[1].uv2.set(a_2.x, a_2.y);
        verts[1].uv3.set(a_3.x, a_3.y);

        verts[2].p.set(float(ts + eps), float(ts + eps), eps, 1.f);
        verts[2].uv0.set(b_0.x, b_0.y);
        verts[2].uv1.set(b_1.x, b_1.y);
        verts[2].uv2.set(b_2.x, b_2.y);
        verts[2].uv3.set(b_3.x, b_3.y);

        verts[3].p.set(float(ts + eps), eps, eps, 1.f);
        verts[3].uv0.set(b_0.x, a_0.y);
        verts[3].uv1.set(b_1.x, a_1.y);
        verts[3].uv2.set(b_2.x, a_2.y);
        verts[3].uv3.set(b_3.x, a_3.y);

        const auto Offset = cmd_list.Vertex.Unlock<v_build>(4);

        cmd_list.set_Element(s_luminance->E[0]);
        cmd_list.set_Geometry(g_bloom_build);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // 111: Perform LUM-SAT, pass 1, 64x64 => 8x8
    u_setrt(cmd_list, rt_LUM_8, {}, {}, nullptr);

    {
        // Build filter-kernel
        constexpr float _ts = 8;
        constexpr float _src = float(64);
        Fvector2 a[16], b[16];
        for (int k = 0; k < 16; k++)
        {
            int _x = (k * 2 + 1) % 8; // 1,3,5,7
            int _y = ((k / 4) * 2 + 1); // 1,1,1,1 ~ 3,3,3,3 ~...etc...
            a[k].set(_x, _y).div(_src);
            b[k].set(a[k]).add(1);
        }

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<v_filter>(4);

        verts[0].p.set(eps, float(_ts + eps), eps, 1.f);
        // xy/yx	- left+down
        for (int t = 0; t < 8; t++)
            verts[0].uv[t].set(a[t].x, b[t].y, b[t + 8].y, a[t + 8].x);

        verts[1].p.set(eps, eps, eps, 1.f);
        // xy/yx	- left+up
        for (int t = 0; t < 8; t++)
            verts[1].uv[t].set(a[t].x, a[t].y, a[t + 8].y, a[t + 8].x);

        verts[2].p.set(float(_ts + eps), float(_ts + eps), eps, 1.f);
        // xy/yx	- right+down
        for (int t = 0; t < 8; t++)
            verts[2].uv[t].set(b[t].x, b[t].y, b[t + 8].y, b[t + 8].x);

        verts[3].p.set(float(_ts + eps), eps, eps, 1.f);
        // xy/yx	- right+up
        for (int t = 0; t < 8; t++)
            verts[3].uv[t].set(b[t].x, a[t].y, a[t + 8].y, b[t + 8].x);

        const auto Offset = cmd_list.Vertex.Unlock<v_filter>(4);

        cmd_list.set_Element(s_luminance->E[1]);
        cmd_list.set_Geometry(g_bloom_filter);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // 222: Perform LUM-SAT, pass 2, 8x8 => 1x1
    u32 gpu_id = Device.dwFrame % HW.Caps.iGPUNum;
    u_setrt(cmd_list, rt_LUM_pool[gpu_id * 2 + 1], {}, {}, nullptr);

    {
        // Build filter-kernel
        constexpr float _ts = 1;
        constexpr float _src = float(8);
        Fvector2 a[16], b[16];
        for (int k = 0; k < 16; k++)
        {
            int _x = (k * 2 + 1) % 8; // 1,3,5,7
            int _y = ((k / 4) * 2 + 1); // 1,1,1,1 ~ 3,3,3,3 ~...etc...
            a[k].set(_x, _y).div(_src);
            b[k].set(a[k]).add(1);
        }

        // Fill vertex buffer
        const auto verts = cmd_list.Vertex.Lock<v_filter>(4);

        verts[0].p.set(eps, float(_ts + eps), eps, 1.f);
        // xy/yx	- left+down
        for (int t = 0; t < 8; t++)
            verts[0].uv[t].set(a[t].x, b[t].y, b[t + 8].y, a[t + 8].x);

        verts[1].p.set(eps, eps, eps, 1.f);
        // xy/yx	- left+up
        for (int t = 0; t < 8; t++)
            verts[1].uv[t].set(a[t].x, a[t].y, a[t + 8].y, a[t + 8].x);

        verts[2].p.set(float(_ts + eps), float(_ts + eps), eps, 1.f);
        // xy/yx	- right+down
        for (int t = 0; t < 8; t++)
            verts[2].uv[t].set(b[t].x, b[t].y, b[t + 8].y, b[t + 8].x);

        verts[3].p.set(float(_ts + eps), eps, eps, 1.f);
        // xy/yx	- right+up
        for (int t = 0; t < 8; t++)
            verts[3].uv[t].set(b[t].x, a[t].y, a[t + 8].y, b[t + 8].x);

        const auto Offset = cmd_list.Vertex.Unlock<v_filter>(4);

        f_luminance_adapt = .9f * f_luminance_adapt + .1f * Device.fTimeDelta * ps_r2_tonemap_adaptation;
        float amount = ps_r2_ls_flags.test(R2FLAG_TONEMAP) ? ps_r2_tonemap_amount : 0;
        constexpr Fvector3 _none{1.f, 0.f, 1.f};
        Fvector3 _full, _result;
        _full.set(ps_r2_tonemap_middlegray, 1.f, ps_r2_tonemap_low_lum);
        _result.lerp(_none, _full, amount);

        cmd_list.set_Element(s_luminance->E[2]);
        cmd_list.set_Geometry(g_bloom_filter);
        cmd_list.set_c("MiddleGray", _result.x, _result.y, _result.z, f_luminance_adapt);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Cleanup states
    cmd_list.set_Z(TRUE);
}
