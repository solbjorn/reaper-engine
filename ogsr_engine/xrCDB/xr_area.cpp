#include "stdafx.h"

#include "xr_area.h"

#include "../xr_3da/Feel_Sound.h"
#include "../xr_3da/xr_object.h"
#include "../xr_3da/igame_level.h"
#include "../xr_3da/xrLevel.h"

namespace xxh
{
#include <xxhash.h>
}

using namespace collide;

void IGame_Level::SoundEvent_Register(ref_sound_data_ptr S, float range, float time_to_stop)
{
    if (!g_bLoaded)
        return;

    if (!S)
        return;

    if (S->g_object && S->g_object->getDestroy())
    {
        S->g_object = nullptr;
        return;
    }

    if (!S->feedback)
        return;

    clamp(range, 0.1f, 500.0f);

    const CSound_params* p = S->feedback->get_params();
    Fvector snd_position = p->position;
    if (S->feedback->is_2D())
        snd_position.add(Sound->listener_position());

    VERIFY(p && _valid(range));
    range = _min(range, p->max_ai_distance);
    VERIFY(_valid(snd_position));
    VERIFY(_valid(p->max_ai_distance));
    VERIFY(_valid(p->volume));

    // Query objects
    const Fvector bb_size{range, range, range};
    g_SpatialSpace->q_box(snd_ER, 0, STYPE_REACTTOSOUND, snd_position, bb_size);

    // Iterate
    for (auto isp : snd_ER)
    {
        Feel::Sound* L = isp->dcast_FeelSound();
        if (L == nullptr)
            continue;

        CObject* CO = isp->dcast_CObject();
        VERIFY(CO);
        if (CO->getDestroy())
            continue;

        // Energy and signal
        VERIFY(_valid(isp->spatial.sphere.P));
        const f32 dist = snd_position.distance_to(isp->spatial.sphere.P);
        if (dist > p->max_ai_distance)
            continue;

        VERIFY(_valid(dist));
        VERIFY(!fis_zero(p->max_ai_distance), S->handle->file_name());

        f32 Power = (1.f - dist / p->max_ai_distance) * p->volume;
        VERIFY(_valid(Power));
        if (Power > EPS_S)
        {
            const f32 occ = Sound->get_occlusion_to(isp->spatial.sphere.P, snd_position);
            VERIFY(_valid(occ));

            Power *= occ;
            if (Power > EPS_S)
                snd_Events.emplace_back(L, S, Power, time_to_stop);
        }
    }

    snd_ER.clear();
}

void IGame_Level::SoundEvent_Dispatch()
{
    while (!snd_Events.empty())
    {
        _esound_delegate& D = snd_Events.back();
        VERIFY(D.dest && D.source);

        if (D.source->feedback)
        {
            D.dest->feel_sound_new(D.source->g_object, D.source->g_type, D.source->g_userdata,
                                   D.source->feedback->is_2D() ? Device.vCameraPosition : D.source->feedback->get_params()->position, D.power, D.time_to_stop);
        }

        snd_Events.pop_back();
    }
}

// Lain: added
void IGame_Level::SoundEvent_OnDestDestroy(Feel::Sound* obj)
{
    snd_Events.erase(std::remove_if(snd_Events.begin(), snd_Events.end(), [obj](const _esound_delegate& d) { return d.dest == obj; }), snd_Events.end());
}

namespace
{
void _sound_event(ref_sound_data_ptr S, float range, float time_to_stop)
{
    if (g_pGameLevel && S && S->feedback)
        g_pGameLevel->SoundEvent_Register(S, range, time_to_stop);
}
} // namespace

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------

CObjectSpace::CObjectSpace()
#ifdef PROFILE_CRITICAL_SECTIONS
    : Lock(MUTEX_PROFILE_ID(CObjectSpace::Lock))
#endif // PROFILE_CRITICAL_SECTIONS
{
#ifdef DEBUG
    sh_debug.create("debug\\wireframe", "$null");
#endif

    m_BoundingVolume.invalidate();
}

//----------------------------------------------------------------------

CObjectSpace::~CObjectSpace()
{
    Sound->set_geometry_occ(nullptr);
    Sound->set_geometry_som(nullptr);
    Sound->set_geometry_env(nullptr);
    Sound->set_handler(nullptr);

#ifdef DEBUG
    sh_debug.destroy();
#endif
}

//----------------------------------------------------------------------

gsl::index CObjectSpace::GetNearest(xr_vector<ISpatial*>& q_spatial, xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object)
{
    q_spatial.clear();
    // Query objects
    q_nearest.clear();

    const Fsphere Q{point, range};
    const Fvector B{range, range, range};
    g_SpatialSpace->q_box(q_spatial, 0, STYPE_COLLIDEABLE, point, B);

    // Iterate
    for (auto isp : q_spatial)
    {
        CObject* O = isp->dcast_CObject();
        if (O == nullptr)
            continue;
        if (O == ignore_object)
            continue;

        const Fsphere mS{O->spatial.sphere.P, O->spatial.sphere.R};
        if (Q.intersect(mS))
            q_nearest.push_back(O);
    }

    return std::ssize(q_nearest);
}

//----------------------------------------------------------------------

gsl::index CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object)
{
    xr_vector<ISpatial*> r_spatial;
    return GetNearest(r_spatial, q_nearest, point, range, ignore_object);
}

//----------------------------------------------------------------------

gsl::index CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, float range)
{
    CObject* O = obj->Owner();
    return GetNearest(q_nearest, O->spatial.sphere.P, range + O->spatial.sphere.R, O);
}

//----------------------------------------------------------------------

namespace
{
void build_callback(std::span<Fvector>, std::span<CDB::TRI> T, void*) { g_pGameLevel->Load_GameSpecific_CFORM(T); }
void serialize_callback(IWriter& writer) { g_pGameLevel->Load_GameSpecific_CFORM_Serialize(writer); }
bool deserialize_callback(IReader& reader) { return g_pGameLevel->Load_GameSpecific_CFORM_Deserialize(reader); }
} // namespace

void CObjectSpace::Load()
{
    IReader* F = FS.r_open("$level$", "level.cform");
    R_ASSERT(F);

    hdrCFORM H;
    F->r(&H, sizeof(hdrCFORM));
    const Fvector* verts = static_cast<const Fvector*>(F->pointer());
    const CDB::TRI* tris = reinterpret_cast<const CDB::TRI*>(verts + H.vertcount);
    R_ASSERT(CFORM_CURRENT_VERSION == H.version);

    F->seek(0);
    xxh::XXH64_hash_t xxh = xxh::XXH3_64bits(F->pointer(), gsl::narrow_cast<size_t>(F->length()));

    string_path cache;
    xr_strcpy(cache, "levels_cache\\");
    xr_strcat(cache, g_pGameLevel->name().c_str());
    xr_strcat(cache, ".cform");
    std::ignore = FS.update_path(cache, "$app_data_root$", cache);

    if (!FS.exist(cache) || !Static.deserialize(cache, xxh, deserialize_callback))
    {
        Static.build(std::span{verts, H.vertcount}, std::span{tris, H.facecount}, build_callback);
        std::ignore = Static.serialize(cache, xxh, serialize_callback);
    }

    m_BoundingVolume.set(H.aabb);
    g_SpatialSpace->initialize(H.aabb);
    g_SpatialSpacePhysic->initialize(H.aabb);
    Sound->set_geometry_occ(&Static);
    Sound->set_handler(_sound_event);

    FS.r_close(F);
}

//----------------------------------------------------------------------

#ifdef DEBUG
void CObjectSpace::dbgRender()
{
    R_ASSERT(bDebug);

    RCache.set_Shader(sh_debug);

    for (auto& obb : q_debug.boxes)
    {
        Fmatrix X, S, R;
        obb.xform_get(X);
        RCache.dbg_DrawOBB(X, obb.m_halfsize, D3DCOLOR_XRGB(255, 0, 0));
        S.scale(obb.m_halfsize);
        R.mul(X, S);
        RCache.dbg_DrawEllipse(R, D3DCOLOR_XRGB(0, 0, 255));
    }

    q_debug.boxes.clear();

    for (auto& P : dbg_S)
    {
        Fsphere& S = P.first;
        Fmatrix M;
        M.scale(S.R, S.R, S.R);
        M.translate_over(S.P);
        RCache.dbg_DrawEllipse(M, P.second);
    }

    dbg_S.clear();
}
#endif
