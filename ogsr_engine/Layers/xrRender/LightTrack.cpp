// LightTrack.cpp: implementation of the CROS_impl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "../../include/xrRender/RenderVisual.h"
#include "../../xr_3da/xr_object.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CROS_impl::CROS_impl()
{
    approximate.set(0, 0, 0);
    dwFrame = u32(-1);
    shadow_recv_frame = u32(-1);
    shadow_recv_slot = -1;

    result_count = 0;
    result_iterator = 0;
    result_frame = u32(-1);
    result_sun = 0;
    value.hemi = 0.5f;
    smooth.hemi = 0.5f;
    value.sun = 0.2f;
    smooth.sun = 0.2f;

    memset(value.hemi_cube, 0, sizeof(value.hemi_cube));
    memset(smooth.hemi_cube, 0, sizeof(smooth.hemi_cube));

    last_position.set(0.0f, 0.0f, 0.0f);
    ticks_to_update = 0;
    sky_rays_uptodate = 0;

    MODE = IRender_ObjectSpecific::TRACE_ALL;
}

void CROS_impl::add(light* source)
{
    // Search
    for (xr_vector<Item>::iterator I = track.begin(); I != track.end(); I++)
        if (source == I->source)
        {
            I->frame_touched = Device.dwFrame;
            return;
        }

    // Register _new_
    auto& L = track.emplace_back();
    L.frame_touched = Device.dwFrame;
    L.source = source;
    L.cache.verts[0].set(0, 0, 0);
    L.cache.verts[1].set(0, 0, 0);
    L.cache.verts[2].set(0, 0, 0);
    L.test = 0.f;
    L.energy = 0.f;
}

//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable : 4305)

static constexpr float hdir[lt_hemisamples][3] = {
    {-0.26287, 0.52573, 0.80902},  {0.27639, 0.44721, 0.85065},   {-0.95106, 0.00000, 0.30902},
    {-0.95106, 0.00000, -0.30902}, {0.58779, 0.00000, -0.80902},  {0.58779, 0.00000, 0.80902},

    {-0.00000, 0.00000, 1.00000},  {0.52573, 0.85065, 0.00000},   {-0.26287, 0.52573, -0.80902},
    {-0.42533, 0.85065, 0.30902},  {0.95106, 0.00000, 0.30902},   {0.95106, 0.00000, -0.30902},

    {0.00000, 1.00000, 0.00000},   {-0.58779, 0.00000, 0.80902},  {-0.72361, 0.44721, 0.52573},
    {-0.72361, 0.44721, -0.52573}, {-0.58779, 0.00000, -0.80902}, {0.16246, 0.85065, -0.50000},

    {0.89443, 0.44721, 0.00000},   {-0.85065, 0.52573, -0.00000}, {0.16246, 0.85065, 0.50000},
    {0.68819, 0.52573, -0.50000},  {0.27639, 0.44721, -0.85065},  {0.00000, 0.00000, -1.00000},

    {-0.42533, 0.85065, -0.30902}, {0.68819, 0.52573, 0.50000},
};
#pragma warning(pop)

inline void CROS_impl::accum_hemi(float* hemi_cube, Fvector3& dir, float scale)
{
    if (dir.x > 0)
        hemi_cube[CUBE_FACE_POS_X] += dir.x * scale;
    else
        hemi_cube[CUBE_FACE_NEG_X] -= dir.x * scale; //	dir.x <= 0

    if (dir.y > 0)
        hemi_cube[CUBE_FACE_POS_Y] += dir.y * scale;
    else
        hemi_cube[CUBE_FACE_NEG_Y] -= dir.y * scale; //	dir.y <= 0

    if (dir.z > 0)
        hemi_cube[CUBE_FACE_POS_Z] += dir.z * scale;
    else
        hemi_cube[CUBE_FACE_NEG_Z] -= dir.z * scale; //	dir.z <= 0
}

//////////////////////////////////////////////////////////////////////////
void CROS_impl::update(IRenderable* O)
{
    // clip & verify
    if (dwFrame == Device.dwFrame)
        return;
    dwFrame = Device.dwFrame;
    if (0 == O)
        return;
    if (0 == O->renderable.visual)
        return;
    VERIFY(smart_cast<CROS_impl*>(O->renderable_ROS()));
    // float	dt			=	Device.fTimeDelta;

    CObject* _object = smart_cast<CObject*>(O);

    if (skip) [[unlikely]]
        return;

    if (isnan(_object->renderable.xform._41) || isnan(_object->renderable.xform._42) || isnan(_object->renderable.xform._43)) [[unlikely]]
    {
        if (!skip)
        {
            Msg("~ NaN position for obj id=%d sect [%s] name [%s]. Skip!", _object->ID(), _object->cNameSect().c_str(), _object->cName().c_str());
            skip = true;
        }

        return;
    }

    // select sample, randomize position inside object
    vis_data& vis = O->renderable.visual->getVisData();
    Fvector position;
    O->renderable.xform.transform_tiny(position, vis.sphere.P);
    position.y += .3f * vis.sphere.R;
    Fvector direction;
    direction.random_dir();

    static_assert(sizeof(value.hemi_cube) == 3 * sizeof(u64));
    *reinterpret_cast<u64*>(&value.hemi_cube[0]) = 0;
    *reinterpret_cast<u64*>(&value.hemi_cube[2]) = 0;
    *reinterpret_cast<u64*>(&value.hemi_cube[4]) = 0;

    bool bFirstTime = (0 == result_count);
    calc_sun_value(position, _object);
    calc_sky_hemi_value(position, _object);

    prepare_lights(position, O);

    // Process ambient lighting and approximate average lighting
    // Process our lights to find average luminescences
    const auto& desc = *g_pGamePersistent->Environment().CurrentEnv;
    Fvector accum = {desc.ambient.x, desc.ambient.y, desc.ambient.z};
    Fvector hemi = {desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z};
    Fvector sun_ = {desc.sun_color.x, desc.sun_color.y, desc.sun_color.z};
    if (MODE & IRender_ObjectSpecific::TRACE_HEMI)
        hemi.mul(smooth.hemi);
    else
        hemi.mul(.2f);
    accum.add(hemi);
    if (MODE & IRender_ObjectSpecific::TRACE_SUN)
        sun_.mul(smooth.sun);
    else
        sun_.mul(.2f);
    accum.add(sun_);
    if (MODE & IRender_ObjectSpecific::TRACE_LIGHTS)
    {
        float hemi_cube_light[NUM_FACES]{};
        Fvector lacc{};

        for (u32 lit = 0; lit < lights.size(); lit++)
        {
            light* L = lights[lit].source;
            float d = L->position.distance_to(position);

            float a = (1 / (L->attenuation0 + L->attenuation1 * d + L->attenuation2 * d * d) - d * L->falloff) * (L->flags.bStatic ? 1.f : 2.f);
            a = (a > 0) ? a : 0.0f;

            Fvector3 dir;
            dir.sub(L->position, position);
            dir.normalize_safe();

            // multiply intensity on attenuation and accumulate result in hemi cube face
            float koef = (lights[lit].color.r + lights[lit].color.g + lights[lit].color.b) / 3.0f * a * ps_r2_dhemi_light_scale;

            accum_hemi(hemi_cube_light, dir, koef);

            lacc.x += lights[lit].color.r * a;
            lacc.y += lights[lit].color.g * a;
            lacc.z += lights[lit].color.b * a;
        }

        const float minHemiValue = 1 / 255.f;

        float hemi_light = (lacc.x + lacc.y + lacc.z) / 3.0f * ps_r2_dhemi_light_scale;

        value.hemi += hemi_light;
        value.hemi = std::max(value.hemi, minHemiValue);

        for (size_t i = 0; i < NUM_FACES; ++i)
        {
            value.hemi_cube[i] += hemi_cube_light[i] * (1 - ps_r2_dhemi_light_flow) + ps_r2_dhemi_light_flow * hemi_cube_light[(i + NUM_FACES / 2) % NUM_FACES];
            value.hemi_cube[i] = std::max(value.hemi_cube[i], minHemiValue);
        }

        //		lacc.x		*= desc.lmap_color.x;
        //		lacc.y		*= desc.lmap_color.y;
        //		lacc.z		*= desc.lmap_color.z;
        //		Msg				("- rgb[%f,%f,%f]",lacc.x,lacc.y,lacc.z);
        accum.add(lacc);
    }
    else
        accum.set(.1f, .1f, .1f);

    // clamp(hemi_value, 0.0f, 1.0f); //Possibly can change hemi value
    if (bFirstTime)
    {
        smooth.hemi = value.hemi;
        CopyMemory(smooth.hemi_cube, value.hemi_cube, sizeof(value.hemi_cube));
    }

    update_smooth();
    approximate = accum;
}

//	Update ticks settings
constexpr s32 s_iUTFirstTimeMin = 1;
constexpr s32 s_iUTFirstTimeMax = 1;
constexpr s32 s_iUTPosChangedMin = 3;
constexpr s32 s_iUTPosChangedMax = 6;
constexpr s32 s_iUTIdleMin = 1000;
constexpr s32 s_iUTIdleMax = 2000;

void CROS_impl::smart_update(IRenderable* O)
{
    if (!O)
        return;
    if (0 == O->renderable.visual)
        return;

    --ticks_to_update;

    //	Acquire current position
    Fvector position;
    VERIFY(smart_cast<CROS_impl*>(O->renderable_ROS()));
    vis_data& vis = O->renderable.visual->getVisData();
    O->renderable.xform.transform_tiny(position, vis.sphere.P);

    if (ticks_to_update <= 0)
    {
        update(O);
        last_position = position;

        if (result_count < lt_hemisamples)
            ticks_to_update = ::Random.randI(s_iUTFirstTimeMin, s_iUTFirstTimeMax + 1);
        else if (sky_rays_uptodate < lt_hemisamples)
            ticks_to_update = ::Random.randI(s_iUTPosChangedMin, s_iUTPosChangedMax + 1);
        else
            ticks_to_update = ::Random.randI(s_iUTIdleMin, s_iUTIdleMax + 1);
    }
    else
    {
        if (!last_position.similar(position, 0.15f))
        {
            sky_rays_uptodate = 0;
            update(O);
            last_position = position;

            if (result_count < lt_hemisamples)
                ticks_to_update = ::Random.randI(s_iUTFirstTimeMin, s_iUTFirstTimeMax + 1);
            else
                ticks_to_update = ::Random.randI(s_iUTPosChangedMin, s_iUTPosChangedMax + 1);
        }
    }
}

extern float ps_r2_lt_smooth;

// hemi & sun: update and smooth
void CROS_impl::update_smooth(IRenderable* O)
{
    if (dwFrameSmooth == Device.dwFrame)
        return;

    dwFrameSmooth = Device.dwFrame;

    smart_update(O);

    float l_f = Device.fTimeDelta * ps_r2_lt_smooth;
    clamp(l_f, 0.f, 1.f);
    float l_i = 1.f - l_f;
    smooth.hemi = value.hemi * l_f + smooth.hemi * l_i;
    smooth.sun = value.sun * l_f + smooth.sun * l_i;
    for (size_t i = 0; i < NUM_FACES; ++i)
        smooth.hemi_cube[i] = value.hemi_cube[i] * l_f + smooth.hemi_cube[i] * l_i;
}

void CROS_impl::calc_sun_value(Fvector& position, CObject* _object)
{
    light* sun = (light*)RImplementation.Lights.sun._get();

    if (MODE & IRender_ObjectSpecific::TRACE_SUN)
    {
        if (--result_sun < 0)
        {
            result_sun += ::Random.randI(lt_hemisamples / 4, lt_hemisamples / 2);
            Fvector direction;
            direction.set(sun->direction).invert().normalize();
            value.sun = !(g_pGameLevel->ObjectSpace.RayTest(position, direction, 500.f, collide::rqtBoth, &cache_sun, _object)) ? 1.f : 0.f;
        }
    }
}

void CROS_impl::calc_sky_hemi_value(Fvector& position, CObject* _object)
{
    // hemi-tracing
    if (MODE & IRender_ObjectSpecific::TRACE_HEMI)
    {
        sky_rays_uptodate += ps_r2_dhemi_count;
        sky_rays_uptodate = _min(sky_rays_uptodate, lt_hemisamples);

        for (u32 it = 0; it < (u32)ps_r2_dhemi_count; it++)
        { // five samples per one frame
            u32 sample = 0;
            if (result_count < lt_hemisamples)
            {
                sample = result_count;
                result_count++;
            }
            else
            {
                sample = (result_iterator % lt_hemisamples);
                result_iterator++;
            }

            // take sample
            Fvector direction;
            direction.set(hdir[sample][0], hdir[sample][1], hdir[sample][2]).normalize();
            //.			result[sample]	=	!g_pGameLevel->ObjectSpace.RayTest(position,direction,50.f,collide::rqtBoth,&cache[sample],_object);
            result[sample] = !g_pGameLevel->ObjectSpace.RayTest(position, direction, 50.f, collide::rqtStatic, &cache[sample], _object);
            //	Msg				("%d:-- %s",sample,result[sample]?"true":"false");
        }
    }
    // hemi & sun: update and smooth
    //	float	l_f				=	dt*lt_smooth;
    //	float	l_i				=	1.f-l_f;
    int _pass = 0;
    for (int it = 0; it < result_count; it++)
        if (result[it])
            _pass++;
    value.hemi = float(_pass) / float(result_count ? result_count : 1);
    value.hemi *= ps_r2_dhemi_sky_scale;

    for (int it = 0; it < result_count; it++)
    {
        if (result[it])
            accum_hemi(value.hemi_cube, Fvector3().set(hdir[it][0], hdir[it][1], hdir[it][2]), ps_r2_dhemi_sky_scale);
    }
}

void CROS_impl::prepare_lights(Fvector& position, IRenderable* O)
{
    CObject* _object = smart_cast<CObject*>(O);
    float dt = Device.fTimeDelta;

    vis_data& vis = O->renderable.visual->getVisData();
    float radius;
    radius = vis.sphere.R;
    // light-tracing
    BOOL bTraceLights = MODE & IRender_ObjectSpecific::TRACE_LIGHTS;
    if ((!O->renderable_ShadowGenerate()) && (!O->renderable_ShadowReceive()))
        bTraceLights = FALSE;
    if (bTraceLights)
    {
        // Select nearest lights
        const Fvector bb_size = {radius, radius, radius};

        static xr_vector<ISpatial*> lstSpatial;
        g_SpatialSpace->q_box(lstSpatial, 0, STYPE_LIGHTSOURCEHEMI, position, bb_size);

        for (auto* spatial : lstSpatial)
        {
            light* source = (light*)spatial->dcast_Light();
            VERIFY(source); // sanity check
            float R = radius + source->range;
            if (position.distance_to(source->position) < R && source->flags.bStatic)
                add(source);
        }

        // Trace visibility
        lights.clear();

        for (s32 id = 0; id < s32(track.size()); id++)
        {
            // remove untouched lights
            xr_vector<CROS_impl::Item>::iterator I = track.begin() + id;
            if (I->frame_touched != Device.dwFrame)
            {
                track.erase(I);
                id--;
                continue;
            }

            // Trace visibility
            Fvector P, D;
            float amount = 0;
            light* xrL = I->source;
            Fvector& LP = xrL->position;

            P = position;

            // point/spot
            float f = D.sub(P, LP).magnitude();
            if (g_pGameLevel->ObjectSpace.RayTest(LP, D.div(f), f, collide::rqtStatic, &I->cache, _object))
                amount -= lt_dec;
            else
                amount += lt_inc;
            I->test += amount * dt;
            clamp(I->test, -.5f, 1.f);
            I->energy = .9f * I->energy + .1f * I->test;

            //
            float E = I->energy * xrL->color.intensity();
            if (E > EPS)
            {
                // Select light
                lights.push_back(CROS_impl::Light());
                CROS_impl::Light& L = lights.back();
                L.source = xrL;
                L.color.mul_rgb(xrL->color, I->energy / 2);
                L.energy = I->energy / 2;
                if (!xrL->flags.bStatic)
                {
                    L.color.mul_rgb(.5f);
                    L.energy *= .5f;
                }
            }
        }

        // Sort lights by importance - important for R1-shadows
        std::ranges::sort(lights, [](const Light& L1, const Light& L2) { return L1.energy > L2.energy; });
    }
}
