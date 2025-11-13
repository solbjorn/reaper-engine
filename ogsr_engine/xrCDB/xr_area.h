#ifndef __XR_AREA_H__
#define __XR_AREA_H__

#include "../xr_3da/xr_collide_form.h"
#include "xr_collide_defs.h"

// refs
class ISpatial;
class ICollisionForm;
class CObject;

//-----------------------------------------------------------------------------------------------------------
// Space Area
//-----------------------------------------------------------------------------------------------------------
class CObjectSpace
{
private:
    // Debug
    CDB::MODEL Static;
    Fbox m_BoundingVolume;

#ifdef DEBUG
public:
    ref_shader sh_debug;
    clQueryCollision q_debug; // MT: dangerous
    xr_vector<std::pair<Fsphere, u32>> dbg_S; // MT: dangerous
#endif

private:
    [[nodiscard]] BOOL _RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::ray_cache* cache, CObject* ignore_object);
    [[nodiscard]] BOOL _RayPick(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object);
    [[nodiscard]] BOOL _RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb,
                                 CObject* ignore_object);
    [[nodiscard]] BOOL _RayQuery2(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb,
                                  CObject* ignore_object);

public:
    CObjectSpace();
    ~CObjectSpace();

    void Load();

    // Occluded/No
    [[nodiscard]] BOOL RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::ray_cache* cache, CObject* ignore_object);

    // Game raypick (nearest) - returns object and addititional params
    [[nodiscard]] BOOL RayPick(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object);

    // General collision query
    [[nodiscard]] BOOL RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb,
                                CObject* ignore_object);
    [[nodiscard]] BOOL RayQuery(collide::rq_results& dest, ICollisionForm* target, const collide::ray_defs& rq);
    [[nodiscard]] bool BoxQuery(Fvector const& box_center, Fvector const& box_z_axis, Fvector const& box_y_axis, Fvector const& box_sizes, xr_vector<Fvector>* out_tris);

    [[nodiscard]] gsl::index GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, float range);
    [[nodiscard]] gsl::index GetNearest(xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object);
    [[nodiscard]] gsl::index GetNearest(xr_vector<ISpatial*>& q_spatial, xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object);

    [[nodiscard]] CDB::TRI* GetStaticTris() { return Static.get_tris(); }
    [[nodiscard]] const CDB::TRI* GetStaticTris() const { return Static.get_tris(); }

    [[nodiscard]] Fvector* GetStaticVerts() { return Static.get_verts(); }
    [[nodiscard]] const Fvector* GetStaticVerts() const { return Static.get_verts(); }

    [[nodiscard]] CDB::MODEL* GetStaticModel() { return &Static; }
    [[nodiscard]] const CDB::MODEL* GetStaticModel() const { return &Static; }

    [[nodiscard]] const Fbox& GetBoundingVolume() const { return m_BoundingVolume; }

    // Debugging
#ifdef DEBUG
    void dbgRender();
    [[nodiscard]] ref_shader dbgGetShader() { return sh_debug; }
#endif
};

#endif //__XR_AREA_H__
