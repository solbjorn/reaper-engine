// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DetailManager.h"

#include "../../xrCDB/cl_intersect.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

#include "xr_task.h"

#include <xmmintrin.h>

//--------------------------------------------------- Decompression

namespace
{
constexpr int magic4x4[4][4] = {{0, 14, 3, 13}, {11, 5, 8, 6}, {12, 2, 15, 1}, {7, 9, 4, 10}};

void bwdithermap(int levels, int magic[16][16])
{
    /* Get size of each step */
    float N = 255.0f / (levels - 1);

    /*
     * Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
     * and doesn't give us full intensity range (only 17 sublevels).
     *
     * magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
     * N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
     * pixels incremented to the next level (this is reserved for the
     * pixel value with mod N == 0 at the next level).
     */

    const float magicfact = (N - 1) / 16;
    for (u32 i = 0; i < 4; i++)
        for (u32 j = 0; j < 4; j++)
            for (u32 k = 0; k < 4; k++)
                for (u32 l = 0; l < 4; l++)
                    magic[4 * k + i][4 * l + j] = (int)(0.5 + magic4x4[i][j] * magicfact + (magic4x4[k][l] / 16.) * magicfact);
}
} // namespace

//--------------------------------------------------- Decompression

void CDetailManager::SSwingValue::lerp(const SSwingValue& A, const SSwingValue& B, float f)
{
    float fi = 1.f - f;
    amp1 = fi * A.amp1 + f * B.amp1;
    amp2 = fi * A.amp2 + f * B.amp2;
    rot1 = fi * A.rot1 + f * B.rot1;
    rot2 = fi * A.rot2 + f * B.rot2;
    speed = fi * A.speed + f * B.speed;
}
//---------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager()
{
    tg = &xr_task_group_get();

    // KD: variable detail radius
    dm_size = dm_current_size;
    dm_cache_line = dm_current_cache_line;
    dm_cache1_line = dm_current_cache1_line;
    dm_cache_size = dm_current_cache_size;
    dm_fade = dm_current_fade;

    cache_level1 = (CacheSlot1**)xr_malloc(dm_cache1_line * sizeof(CacheSlot1*));
    for (u32 i = 0; i < dm_cache1_line; ++i)
    {
        cache_level1[i] = (CacheSlot1*)xr_malloc(dm_cache1_line * sizeof(CacheSlot1));
        for (u32 j = 0; j < dm_cache1_line; ++j)
            new (&(cache_level1[i][j])) CacheSlot1();
    }

    cache = (Slot***)xr_malloc(dm_cache_line * sizeof(Slot**));
    for (u32 i = 0; i < dm_cache_line; ++i)
        cache[i] = (Slot**)xr_malloc(dm_cache_line * sizeof(Slot*));

    cache_pool = (Slot*)xr_malloc(dm_cache_size * sizeof(Slot));
    for (u32 i = 0; i < dm_cache_size; ++i)
        new (&(cache_pool[i])) Slot();
}

CDetailManager::~CDetailManager()
{
    tg->cancel_put();

    if (dtFS)
    {
        FS.r_close(dtFS);
        dtFS = nullptr;
    }

    for (u32 i = 0; i < dm_cache_size; ++i)
        cache_pool[i].~Slot();
    xr_mfree(cache_pool);

    for (u32 i = 0; i < dm_cache_line; ++i)
        xr_mfree(cache[i]);
    xr_mfree(cache);

    for (u32 i = 0; i < dm_cache1_line; ++i)
    {
        for (u32 j = 0; j < dm_cache1_line; ++j)
            cache_level1[i][j].~CacheSlot1();
        xr_mfree(cache_level1[i]);
    }
    xr_mfree(cache_level1);
}

void CDetailManager::Load()
{
    // Open file stream
    if (!FS.exist("$level$", "level.details"))
    {
        dtFS = nullptr;
        return;
    }

    string_path fn;
    std::ignore = FS.update_path(fn, "$level$", "level.details");
    dtFS = FS.r_open(fn);

    // Header
    std::ignore = dtFS->r_chunk_safe(0, &dtH, sizeof(dtH));
    R_ASSERT(dtH.version == DETAIL_VERSION);

    u32 m_count = dtH.object_count;
    objects.reserve(m_count);

    // Models
    IReader* m_fs = dtFS->open_chunk(1);
    for (u32 m_id = 0; m_id < m_count; m_id++)
    {
        CDetail* dt = xr_new<CDetail>();
        IReader* S = m_fs->open_chunk(m_id);
        dt->Load(S);
        objects.push_back(dt);
        S->close();
    }
    m_fs->close();

    // Get pointer to database (slots)
    IReader* m_slots = dtFS->open_chunk(2);
    dtSlots = static_cast<const DetailSlot*>(m_slots->pointer());
    m_slots->close();

    // Initialize 'vis' and 'cache'
    for (u32 i = 0; i < 3; ++i)
        m_visibles[i].resize(objects.size());
    cache_Initialize();

    // Make dither matrix
    bwdithermap(2, dither);

    // Hardware specific optimizations
    hw_Load();

    // swing desc
    // normal
    swing_desc[0].amp1 = pSettings->r_float("details", "swing_normal_amp1");
    swing_desc[0].amp2 = pSettings->r_float("details", "swing_normal_amp2");
    swing_desc[0].rot1 = pSettings->r_float("details", "swing_normal_rot1");
    swing_desc[0].rot2 = pSettings->r_float("details", "swing_normal_rot2");
    swing_desc[0].speed = pSettings->r_float("details", "swing_normal_speed");
    // fast
    swing_desc[1].amp1 = pSettings->r_float("details", "swing_fast_amp1");
    swing_desc[1].amp2 = pSettings->r_float("details", "swing_fast_amp2");
    swing_desc[1].rot1 = pSettings->r_float("details", "swing_fast_rot1");
    swing_desc[1].rot2 = pSettings->r_float("details", "swing_fast_rot2");
    swing_desc[1].speed = pSettings->r_float("details", "swing_fast_speed");
}

void CDetailManager::Unload()
{
    hw_Unload();

    for (CDetail* detailObject : objects)
    {
        detailObject->Unload();
        xr_delete(detailObject);
    }

    objects.clear();
    m_visibles[0].clear();
    m_visibles[1].clear();
    m_visibles[2].clear();
    FS.r_close(dtFS);
    dtFS = nullptr;
}

void CDetailManager::UpdateVisibleM(const Fvector& EYE)
{
    // Фикс мерцания и прочих глюков травы при активном двойном рендеринге. Для теней от травы SSS16 фикс не нужен, наоборот вредит.
    // if (Device.m_SecondViewport.IsSVPFrame())
    //     return;

    XR_TRACY_ZONE_SCOPED();

    // Clean up
    for (auto& vec : m_visibles)
    {
        for (auto& vis : vec)
            vis.clear();
    }

    CFrustum View;
    View.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

    float fade_limit = dm_fade;
    fade_limit = fade_limit * fade_limit;
    float fade_start = 1.f;
    fade_start = fade_start * fade_start;
    float fade_range = fade_limit - fade_start;
    float r_ssaCHEAP = 16 * r_ssaDISCARD;

    // Initialize 'vis' and 'cache'
    // Collect objects for rendering
    Device.Statistic->RenderDUMP_DT_VIS.Begin();
    for (u32 _mz = 0; _mz < dm_cache1_line; _mz++)
    {
        for (u32 _mx = 0; _mx < dm_cache1_line; _mx++)
        {
            CacheSlot1& MS = cache_level1[_mz][_mx];
            if (MS.empty)
            {
                continue;
            }
            u32 mask = 0xff;
            u32 res = View.testSphere(MS.vis.sphere.P, MS.vis.sphere.R, mask);
            if (fcvNone == res)
            {
                continue; // invisible-view frustum
            }
            // test slots

            u32 dwCC = dm_cache1_count * dm_cache1_count;

            for (u32 _i = 0; _i < dwCC; _i++)
            {
                Slot* PS = *MS.slots[_i];
                Slot& S = *PS;

                //				if ( ( _i + 1 ) < dwCC );
                //					_mm_prefetch( (char *) *MS.slots[ _i + 1 ]  , _MM_HINT_T1 );

                // if slot empty - continue
                if (S.empty)
                {
                    continue;
                }

                // if upper test = fcvPartial - test inner slots
                if (fcvPartial == res)
                {
                    u32 _mask = mask;
                    u32 _res = View.testSphere(S.vis.sphere.P, S.vis.sphere.R, _mask);
                    if (fcvNone == _res)
                    {
                        continue; // invisible-view frustum
                    }
                }

                if (!RImplementation.HOM.visible(S.vis))
                {
                    continue; // invisible-occlusion
                }

                // Add to visibility structures
                if (Device.dwFrame > S.frame)
                {
                    // Calc fade factor	(per slot)
                    float dist_sq = EYE.distance_to_sqr(S.vis.sphere.P);
                    if (dist_sq > fade_limit)
                    {
                        S.hidden = true;
                        continue;
                    }
                    float alpha = (dist_sq < fade_start) ? 0.f : (dist_sq - fade_start) / fade_range;
                    float alpha_i = 1.f - alpha;
                    float dist_sq_rcp = 1.f / dist_sq;

                    S.frame = Device.dwFrame + Random.randI(15, 30);
                    for (int sp_id = 0; sp_id < dm_obj_in_slot; sp_id++)
                    {
                        SlotPart& sp = S.G[sp_id];
                        if (sp.id == DetailSlot::ID_Empty)
                            continue;

                        sp.r_items[0].clear();
                        sp.r_items[1].clear();
                        sp.r_items[2].clear();

                        float R = objects[sp.id]->bv_sphere.R;
                        float Rq_drcp = R * R * dist_sq_rcp; // reordered expression for 'ssa' calc

                        for (auto& el : sp.items)
                        {
                            if (el == nullptr)
                                continue;

                            SlotItem& Item = *el;
                            float scale = ps_no_scale_on_fade ? (Item.scale_calculated = Item.scale) : (Item.scale_calculated = Item.scale * alpha_i);
                            float ssa = ps_no_scale_on_fade ? scale : scale * scale * Rq_drcp;
                            if (ssa < r_ssaDISCARD)
                            {
                                Item.alpha_target = 0;
                                continue;
                            }
                            u32 vis_id = 0;
                            if (ssa > r_ssaCHEAP)
                                vis_id = Item.vis_ID;

                            sp.r_items[vis_id].push_back(el);

                            if (S.hidden)
                            {
                                Item.alpha = 0;
                                S.hidden = false;
                            }
                            Item.alpha_target = 1;
                            Item.distance = dist_sq;
                            Item.position = S.vis.sphere.P;
                        }
                    }
                }
                for (int sp_id = 0; sp_id < dm_obj_in_slot; sp_id++)
                {
                    SlotPart& sp = S.G[sp_id];
                    if (sp.id == DetailSlot::ID_Empty)
                        continue;
                    if (!sp.r_items[0].empty())
                    {
                        m_visibles[0][sp.id].push_back(&sp.r_items[0]);
                    }
                    if (!sp.r_items[1].empty())
                    {
                        m_visibles[1][sp.id].push_back(&sp.r_items[1]);
                    }
                    if (!sp.r_items[2].empty())
                    {
                        m_visibles[2][sp.id].push_back(&sp.r_items[2]);
                    }
                }
            }
        }
    }
    Device.Statistic->RenderDUMP_DT_VIS.End();
}

void CDetailManager::Render(CBackend& cmd_list, float fade_distance, const Fvector* light_position)
{
    if (!RImplementation.Details || !dtFS || !psDeviceFlags.is(rsDetails))
        return;

    XR_TRACY_ZONE_SCOPED();

    tg->wait();

    Device.Statistic->RenderDUMP_DT_Render.Begin();

    const float factor = g_pGamePersistent->Environment().wind_strength_factor;

    swing_current.lerp(swing_desc[0], swing_desc[1], factor);

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_xform_world(Fidentity);

    hw_Render(cmd_list, fade_distance, light_position);

    cmd_list.set_CullMode(CULL_CCW);
    Device.Statistic->RenderDUMP_DT_Render.End();
}

u32 reset_frame = 0;

void CDetailManager::run_async()
{
    // костыли чтоб в 3д прицелах не мерцала трава. Фикс не совсем идеальный, иногда всё равно проскакивает, но не критично.
    if (!(Device.m_SecondViewport.IsSVPFrame() || !Device.m_SecondViewport.IsSVPActive() || (((Device.dwFrame - 1) % g_3dscopes_fps_factor) != 0)))
        return;

    // Заметка: сначала рендерится фрейм для 3д прицела, а уже следующий фрейм для всего мира вне прицела
    tg->run([this] {
        if (reset_frame == Device.dwFrame)
            return;
        if (!RImplementation.Details)
            return; // possibly deleted
        if (!dtFS)
            return;
        if (!psDeviceFlags.is(rsDetails))
            return;

        XR_TRACY_ZONE_SCOPED();

        if (need_init) [[unlikely]]
        {
            cache_Initialize();
            need_init = false;
        }

        Fvector EYE = Device.vCameraPosition;

        const int s_x = iFloor(EYE.x / dm_slot_size + .5f);
        const int s_z = iFloor(EYE.z / dm_slot_size + .5f);

        Device.Statistic->RenderDUMP_DT_Cache.Begin();
        cache_Update(s_x, s_z, EYE);
        Device.Statistic->RenderDUMP_DT_Cache.End();

        UpdateVisibleM(EYE);
    });
}

void CDetailManager::details_clear()
{
    if (ps_ssfx_grass_shadows.x <= 0)
        return;

    for (auto& list : m_visibles)
    {
        for (auto& vis : list)
            vis.clear();
    }
}
