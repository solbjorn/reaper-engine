#pragma once

#include "../xrCore/xrPool.h"

#include "xr_collide_defs.h"

static constexpr Fvector c_spatial_offset[8] = {{-1, -1, -1}, {1, -1, -1}, {-1, 1, -1}, {1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {-1, 1, 1}, {1, 1, 1}};

using sector_id_t = size_t;
constexpr sector_id_t INVALID_SECTOR_ID = sector_id_t(-1);

/*
Requirements:
0. Generic
    * O(1) insertion
        - radius completely determines	"level"
        - position completely determines "node"
    * O(1) removal
    *
1. Rendering
    * Should live inside spatial DB
    * Should have at least "bounding-sphere" or "bounding-box"
    * Should have pointer to "sector" it lives in
    * Approximate traversal order relative to point ("camera")
2. Spatial queries
    * Should live inside spatial DB
    * Should have at least "bounding-sphere" or "bounding-box"
*/

constexpr float c_spatial_min = 8.f;
//////////////////////////////////////////////////////////////////////////
enum
{
    STYPE_RENDERABLE = (1 << 0),
    STYPE_LIGHTSOURCE = (1 << 1),
    STYPE_COLLIDEABLE = (1 << 2),
    STYPE_VISIBLEFORAI = (1 << 3),
    STYPE_REACTTOSOUND = (1 << 4),
    STYPE_PHYSIC = (1 << 5),
    STYPE_OBSTACLE = (1 << 6),
    STYPE_SHAPE = (1 << 7),
    STYPE_LIGHTSOURCEHEMI = (1 << 8),

    STYPEFLAG_INVALIDSECTOR = (1 << 16)
};
//////////////////////////////////////////////////////////////////////////
// Comment:
//		ordinal objects			- renderable?, collideable?, visibleforAI?
//		physical-decorations	- renderable, collideable
//		lights					- lightsource
//		particles(temp-objects)	- renderable
//		glow					- renderable
//		sound					- ???
//////////////////////////////////////////////////////////////////////////

class ISpatial_NODE;
class ISpatial_DB;

namespace Feel
{
class Sound;
}

class IRenderable;
class IRender_Light;

class ISpatial
{
public:
    struct _spatial
    {
        Fsphere sphere{};
        u32 type{};
        Fvector node_center{}; // Cached node center for TBV optimization
        float node_radius{}; // Cached node bounds for TBV optimization
        ISpatial_NODE* node_ptr{}; // Cached parent node for "empty-members" optimization
        sector_id_t sector_id{INVALID_SECTOR_ID};
        ISpatial_DB* space{}; // allow different spaces
    } spatial;

private:
    void spatial_updatesector_internal(sector_id_t sector_id);

public:
    bool spatial_inside();
    virtual void spatial_register();
    void spatial_unregister();
    virtual void spatial_move();
    virtual Fvector spatial_sector_point() { return spatial.sphere.P; }

    void spatial_updatesector(sector_id_t sector_id)
    {
        if (!(spatial.type & STYPEFLAG_INVALIDSECTOR))
            return;

        spatial_updatesector_internal(sector_id);
    }

    virtual CObject* dcast_CObject() { return 0; }
    virtual Feel::Sound* dcast_FeelSound() { return 0; }
    virtual IRenderable* dcast_Renderable() { return 0; }
    virtual IRender_Light* dcast_Light() { return 0; }

    ISpatial(ISpatial_DB* space) { spatial.space = space; }
    virtual ~ISpatial();
};

//////////////////////////////////////////////////////////////////////////
class ISpatial_NODE
{
public:
    ISpatial_NODE* parent; // parent node for "empty-members" optimization
    ISpatial_NODE* children[8]; // children nodes
    xr_vector<ISpatial*> items; // own items

    void _init(ISpatial_NODE* _parent);
    void _remove(ISpatial* S);
    void _insert(ISpatial* S);
    bool _empty()
    {
        return items.empty() &&
            0 ==
            (uintptr_t(children[0]) | uintptr_t(children[1]) | uintptr_t(children[2]) | uintptr_t(children[3]) | uintptr_t(children[4]) | uintptr_t(children[5]) |
             uintptr_t(children[6]) | uintptr_t(children[7]));
    }
};

//////////////////////////////////////////////////////////////////////////
class ISpatial_DB
{
private:
    xrCriticalSection cs;

    poolSS<ISpatial_NODE, 128> allocator;
    xr_vector<ISpatial_NODE*> allocator_pool;
    ISpatial* rt_insert_object{};

public:
    ISpatial_NODE* m_root{};
    Fvector m_center{};
    float m_bounds{};
    xr_vector<ISpatial*>* q_result{};
    u32 stat_nodes{};
    u32 stat_objects{};
    CStatTimer stat_insert;
    CStatTimer stat_remove;

private:
    IC u32 _octant(u32 x, u32 y, u32 z) { return z * 4 + y * 2 + x; }
    IC u32 _octant(Fvector& base, Fvector& rel)
    {
        u32 o = 0;
        if (rel.x > base.x)
            o += 1;
        if (rel.y > base.y)
            o += 2;
        if (rel.z > base.z)
            o += 4;
        return o;
    }

    ISpatial_NODE* _node_create();
    void _node_destroy(ISpatial_NODE*& P);

    void _insert(ISpatial_NODE* N, Fvector& n_center, float n_radius);
    void _remove(ISpatial_NODE* N, ISpatial_NODE* N_sub);

public:
    ISpatial_DB() {}
    ~ISpatial_DB();

    // managing
    void initialize(const Fbox& BB);
    void insert(ISpatial* S);
    void remove(ISpatial* S);
    void update(u32 nodes = 8);
    BOOL verify();

    enum
    {
        O_ONLYFIRST = (1 << 0),
        O_ONLYNEAREST = (1 << 1),
        O_ORDERED = (1 << 2),
        O_force_u32 = u32(-1)
    };

    // query
    void q_ray(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_and, const Fvector& _start, const Fvector& _dir, float _range);
    void q_box(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_or, const Fvector& _center, const Fvector& _size);
    void q_sphere(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_or, const Fvector& _center, const float _radius);
    void q_frustum(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_or, const CFrustum& _frustum);
};

extern ISpatial_DB* g_SpatialSpace;
extern ISpatial_DB* g_SpatialSpacePhysic;
