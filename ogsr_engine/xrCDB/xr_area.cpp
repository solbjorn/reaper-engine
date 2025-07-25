#include "stdafx.h"

#include "xr_area.h"
#include "../xr_3da/Feel_Sound.h"
#include "../xr_3da/xr_object.h"
#include "../xr_3da/igame_level.h"
#include "../xr_3da/xrLevel.h"

#include <xxhash.h>

using namespace collide;

extern BOOL g_bLoaded;

void IGame_Level::SoundEvent_Register(ref_sound_data_ptr S, float range, float time_to_stop)
{
    if (!g_bLoaded)
        return;
    if (!S)
        return;
    if (S->g_object && S->g_object->getDestroy())
    {
        S->g_object = 0;
        return;
    }
    if (0 == S->feedback)
        return;

    clamp(range, 0.1f, 500.f);

    const CSound_params* p = S->feedback->get_params();
    Fvector snd_position = p->position;
    if (S->feedback->is_2D())
    {
        snd_position.add(Sound->listener_position());
    }

    VERIFY(p && _valid(range));
    range = _min(range, p->max_ai_distance);
    VERIFY(_valid(snd_position));
    VERIFY(_valid(p->max_ai_distance));
    VERIFY(_valid(p->volume));

    // Query objects
    Fvector bb_size = {range, range, range};
    g_SpatialSpace->q_box(snd_ER, 0, STYPE_REACTTOSOUND, snd_position, bb_size);

    // Iterate
    xr_vector<ISpatial*>::iterator it = snd_ER.begin();
    xr_vector<ISpatial*>::iterator end = snd_ER.end();
    for (; it != end; it++)
    {
        Feel::Sound* L = (*it)->dcast_FeelSound();
        if (0 == L)
            continue;
        CObject* CO = (*it)->dcast_CObject();
        VERIFY(CO);
        if (CO->getDestroy())
            continue;

        // Energy and signal
        VERIFY(_valid((*it)->spatial.sphere.P));
        float dist = snd_position.distance_to((*it)->spatial.sphere.P);
        if (dist > p->max_ai_distance)
            continue;
        VERIFY(_valid(dist));
        VERIFY2(!fis_zero(p->max_ai_distance), S->handle->file_name());
        float Power = (1.f - dist / p->max_ai_distance) * p->volume;
        VERIFY(_valid(Power));
        if (Power > EPS_S)
        {
            float occ = Sound->get_occlusion_to((*it)->spatial.sphere.P, snd_position);
            VERIFY(_valid(occ));
            Power *= occ;
            if (Power > EPS_S)
            {
                _esound_delegate D = {.dest = L, .source = S, .power = Power, .time_to_stop = time_to_stop};
                snd_Events.push_back(D);
            }
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

void __stdcall _sound_event(ref_sound_data_ptr S, float range, float time_to_stop)
{
    if (g_pGameLevel && S && S->feedback)
        g_pGameLevel->SoundEvent_Register(S, range, time_to_stop);
}

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
int CObjectSpace::GetNearest(xr_vector<ISpatial*>& q_spatial, xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object)
{
    q_spatial.clear();
    // Query objects
    q_nearest.clear();
    Fsphere Q;
    Q.set(point, range);
    Fvector B;
    B.set(range, range, range);
    g_SpatialSpace->q_box(q_spatial, 0, STYPE_COLLIDEABLE, point, B);

    // Iterate
    xr_vector<ISpatial*>::iterator it = q_spatial.begin();
    xr_vector<ISpatial*>::iterator end = q_spatial.end();
    for (; it != end; it++)
    {
        CObject* O = (*it)->dcast_CObject();
        if (0 == O)
            continue;
        if (O == ignore_object)
            continue;
        Fsphere mS = {O->spatial.sphere.P, O->spatial.sphere.R};
        if (Q.intersect(mS))
            q_nearest.push_back(O);
    }

    return q_nearest.size();
}

//----------------------------------------------------------------------
int CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object)
{
    xr_vector<ISpatial*> r_spatial;
    return (GetNearest(r_spatial, q_nearest, point, range, ignore_object));
}

//----------------------------------------------------------------------
int CObjectSpace::GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, float range)
{
    CObject* O = obj->Owner();
    return GetNearest(q_nearest, O->spatial.sphere.P, range + O->spatial.sphere.R, O);
}

//----------------------------------------------------------------------
static void __stdcall build_callback(Fvector* V, size_t Vcnt, CDB::TRI* T, size_t Tcnt, void* params) { g_pGameLevel->Load_GameSpecific_CFORM(T, Tcnt); }
static void __stdcall serialize_callback(IWriter& writer) { g_pGameLevel->Load_GameSpecific_CFORM_Serialize(writer); }
static bool __stdcall deserialize_callback(IReader& reader) { return g_pGameLevel->Load_GameSpecific_CFORM_Deserialize(reader); }

void CObjectSpace::Load()
{
    IReader* F = FS.r_open("$level$", "level.cform");
    R_ASSERT(F);

    hdrCFORM H;
    F->r(&H, sizeof(hdrCFORM));
    Fvector* verts = (Fvector*)F->pointer();
    CDB::TRI* tris = (CDB::TRI*)(verts + H.vertcount);
    R_ASSERT(CFORM_CURRENT_VERSION == H.version);

    F->seek(0);
    XXH64_hash_t xxh = XXH3_64bits(F->pointer(), F->length());

    string_path cache;
    xr_strcpy(cache, "levels_cache\\");
    xr_strcat(cache, g_pGameLevel->name().c_str());
    xr_strcat(cache, ".cform");
    FS.update_path(cache, "$app_data_root$", cache);

    if (!FS.exist(cache) || !Static.deserialize(cache, xxh, deserialize_callback))
    {
        Static.build(verts, H.vertcount, tris, H.facecount, build_callback);
        Static.serialize(cache, xxh, serialize_callback);
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
    for (u32 i = 0; i < q_debug.boxes.size(); i++)
    {
        Fobb& obb = q_debug.boxes[i];
        Fmatrix X, S, R;
        obb.xform_get(X);
        RCache.dbg_DrawOBB(X, obb.m_halfsize, D3DCOLOR_XRGB(255, 0, 0));
        S.scale(obb.m_halfsize);
        R.mul(X, S);
        RCache.dbg_DrawEllipse(R, D3DCOLOR_XRGB(0, 0, 255));
    }
    q_debug.boxes.clear();

    for (i = 0; i < dbg_S.size(); i++)
    {
        std::pair<Fsphere, u32>& P = dbg_S[i];
        Fsphere& S = P.first;
        Fmatrix M;
        M.scale(S.R, S.R, S.R);
        M.translate_over(S.P);
        RCache.dbg_DrawEllipse(M, P.second);
    }
    dbg_S.clear();
}
#endif
