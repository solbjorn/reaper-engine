// occRasterizer.cpp: implementation of the occRasterizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "occRasterizer.h"

#if DEBUG
#include "dxRenderDeviceRender.h"
#include "xrRender_console.h"
#endif

occRasterizer Raster;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static constexpr ptrdiff_t __declspec(align(32)) zeros[32 / sizeof(occTri*)]{};
static constexpr float __declspec(align(32)) ones[32 / sizeof(float)] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};

static ICF void MemFill(void* dst, const void* src, size_t size)
{
    u8* cdst = reinterpret_cast<u8*>(dst);
    const u8* csrc = reinterpret_cast<const u8*>(src);

    do
    {
        _mm256_store_si256((__m256i*)cdst, _mm256_load_si256((const __m256i*)csrc));
        cdst += 32;
        size -= 32;
    } while (size >= 32);
}

occRasterizer::occRasterizer()
#ifdef DEBUG
    : dbg_HOM_draw_initialized(false)
#endif
{
    static_assert(!offsetof(occRasterizer, bufFrame));
    static_assert(!(sizeof(bufFrame) % 32));
    static_assert(!(sizeof(bufDepth) % 32));

    MemFill(this, zeros, sizeof(*this));
}

void occRasterizer::clear()
{
    MemFill(bufFrame, zeros, sizeof(bufFrame));
    MemFill(bufDepth, ones, sizeof(bufDepth));
}

static inline bool shared(occTri* T1, occTri* T2)
{
    if (T1 == T2)
        return true;
    if (T1->adjacent[0] == T2)
        return true;
    if (T1->adjacent[1] == T2)
        return true;
    if (T1->adjacent[2] == T2)
        return true;
    return false;
}

void occRasterizer::propagade()
{
    // Clip-and-propagade zero level
    occTri** pFrame = get_frame();
    float* pDepth = get_depth();
    for (int y = 0; y < occ_dim_0; y++)
    {
        for (int x = 0; x < occ_dim_0; x++)
        {
            int ox = x + 2, oy = y + 2;

            // Y2-connect
            int pos = oy * occ_dim + ox;
            int pos_up = pos - occ_dim;
            if (pos_up < 0)
                pos_up = pos;
            int pos_down = pos + occ_dim;
            if (pos_down >= occ_dim_0 * occ_dim_0)
                pos_down = pos;
            int pos_down2 = pos_down + occ_dim;
            if (pos_down2 >= occ_dim_0 * occ_dim_0)
                pos_down2 = pos_down;

            occTri* Tu1 = pFrame[pos_up];
            if (Tu1)
            {
                // We has pixel 1scan up
                if (pFrame[pos_down] && shared(Tu1, pFrame[pos_down]))
                {
                    // We has pixel 1scan down
                    float ZR = (pDepth[pos_up] + pDepth[pos_down]) / 2;
                    if (ZR < pDepth[pos])
                    {
                        pFrame[pos] = Tu1;
                        pDepth[pos] = ZR;
                    }
                }
                else if (pFrame[pos_down2] && shared(Tu1, pFrame[pos_down2]))
                {
                    // We has pixel 2scan down
                    float ZR = (pDepth[pos_up] + pDepth[pos_down2]) / 2;
                    if (ZR < pDepth[pos])
                    {
                        pFrame[pos] = Tu1;
                        pDepth[pos] = ZR;
                    }
                }
            }

            //
            float d = pDepth[pos];
            clamp(d, -1.99f, 1.99f);
            bufDepth_0[y][x] = df_2_s32(d);
        }
    }
}

void occRasterizer::on_dbg_render()
{
#ifdef DEBUG
    if (!ps_r2_ls_flags_ext.is(R_FLAGEXT_HOM_DEPTH_DRAW))
    {
        dbg_HOM_draw_initialized = false;
        return;
    }

    for (int i = 0; i < occ_dim_0; ++i)
    {
        for (int j = 0; j < occ_dim_0; ++j)
        {
            if (bDebug)
            {
                Fvector quad, left_top, right_bottom, box_center, box_r;
                quad.set((float)j - occ_dim_0 / 2.f, -((float)i - occ_dim_0 / 2.f), (float)bufDepth_0[i][j] / occQ_s32);
                Device.mProject;

                float z = -Device.mProject._43 / (float)(Device.mProject._33 - quad.z);
                left_top.set(quad.x * z / Device.mProject._11 / (occ_dim_0 / 2.f), quad.y * z / Device.mProject._22 / (occ_dim_0 / 2.f), z);
                right_bottom.set((quad.x + 1) * z / Device.mProject._11 / (occ_dim_0 / 2.f), (quad.y + 1) * z / Device.mProject._22 / (occ_dim_0 / 2.f), z);

                box_center.set((right_bottom.x + left_top.x) / 2, (right_bottom.y + left_top.y) / 2, z);
                box_r = right_bottom;
                box_r.sub(box_center);

                Device.mInvView.transform(box_center);
                Device.mInvView.transform_dir(box_r);

                pixel_box& tmp = dbg_pixel_boxes[i * occ_dim_0 + j];
                tmp.center = box_center;
                tmp.radius = box_r;
                tmp.z = quad.z;
                dbg_HOM_draw_initialized = true;
            }

            if (!dbg_HOM_draw_initialized)
                return;

            pixel_box& tmp = dbg_pixel_boxes[i * occ_dim_0 + j];
            Fmatrix Transform;
            Transform.identity();
            Transform.translate(tmp.center);

            // draw wire
            Device.SetNearer(TRUE);

            RCache.set_Shader(RImplementation.m_SelectionShader);
            RCache.dbg_DrawOBB(Transform, tmp.radius, D3DCOLOR_XRGB(u32(255 * pow(tmp.z, 20.f)), u32(255 * (1 - pow(tmp.z, 20.f))), 0));
            Device.SetNearer(FALSE);
        }
    }
#endif
}

static BOOL test_Level(occD* depth, int dim, float _x0, float _y0, float _x1, float _y1, occD z)
{
    int x0 = iFloor(_x0 * dim + .5f);
    clamp(x0, 0, dim - 1);
    int x1 = iFloor(_x1 * dim + .5f);
    clamp(x1, x0, dim - 1);
    int y0 = iFloor(_y0 * dim + .5f);
    clamp(y0, 0, dim - 1);
    int y1 = iFloor(_y1 * dim + .5f);
    clamp(y1, y0, dim - 1);

    for (int y = y0; y <= y1; y++)
    {
        occD* base = depth + y * dim;
        occD* it = base + x0;
        occD* end = base + x1;
        for (; it <= end; it++)
            if (z < *it)
                return TRUE;
    }
    return FALSE;
}

BOOL occRasterizer::test(float _x0, float _y0, float _x1, float _y1, float _z)
{
    occD z = df_2_s32up(_z) + 1;
    return test_Level(get_depth_level(), occ_dim_0, _x0, _y0, _x1, _y1, z);
}
