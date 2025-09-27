#include "stdafx.h"

#include "fhierrarhyvisual.h"
#include "SkeletonCustom.h"
#include "../../xr_3da/CustomHUD.h"
#include "../../xr_3da/fmesh.h"
#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/xr_object.h"

#include "flod.h"
#include "particlegroup.h"
#include "FTreeVisual.h"

using namespace R_dsgraph;

namespace
{
// Open-coded since multimaps don't have ::try_emplace(), but we want to use piecewise_construct
template <class M, class K, class... Args>
constexpr ICF auto try_emplace(M& m, K&& k, Args&&... args)
{
    return m.emplace(std::piecewise_construct, std::forward_as_tuple(std::forward<K>(k)), std::forward_as_tuple(std::forward<Args>(args)...));
}
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene graph actual insertion and sorting ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
float r_ssaDISCARD;
float r_ssaLOD_A, r_ssaLOD_B;
float r_ssaGLOD_start, r_ssaGLOD_end;

static ICF float CalcSSA(float& distSQ, Fvector& C, dxRender_Visual* V)
{
    float R = V->vis.sphere.R + 0;
    distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;
    return R / distSQ;
}

static ICF float CalcSSA(float& distSQ, Fvector& C, float R)
{
    distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;
    return R / distSQ;
}

static ICF float CalcHudSSA(float& distSQ, Fvector& C, dxRender_Visual* V)
{
    float R = V->vis.sphere.R + 0;
    distSQ = Fvector().set(0.f, 0.f, 0.f).distance_to_sqr(C) + EPS;
    return R / distSQ;
}

void R_dsgraph_structure::insert_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, Fvector& Center)
{
    if (pVisual->vis.marker[context_id] == marker)
        return;
    pVisual->vis.marker[context_id] = marker;

    float distSQ, SSA;
    bool hud = root && root->renderable_HUD();
    if (hud)
        SSA = CalcHudSSA(distSQ, Center, pVisual);
    else
        SSA = CalcSSA(distSQ, Center, pVisual);
    if (SSA <= r_ssaDISCARD)
        return;

    // Distortive geometry should be marked and R2 special-cases it
    // a) Allow to optimize RT order
    // b) Should be rendered to special distort buffer in another pass
    VERIFY(pVisual->shader._get());
    ShaderElement* sh_d = pVisual->shader->E[4]._get();
    if (sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority / 2])
    {
        auto& map = hud ? mapHUDDistort : mapDistort;
        try_emplace(map, distSQ, SSA, root, pVisual, xform, sh_d);
    }

    // Select shader
    ShaderElement* sh = RImplementation.rimp_select_sh_dynamic(root, pVisual, distSQ, phase);
    if (!sh)
        return;
    if (!pmask[sh->flags.iPriority / 2])
        return;

    // HUD rendering
    if (hud)
    {
        if (sh->flags.bStrictB2F)
        {
            if (sh->flags.bEmissive)
                try_emplace(mapHUDEmissive, distSQ, SSA, root, pVisual, xform, sh_d);

            try_emplace(mapHUDSorted, distSQ, SSA, root, pVisual, xform, sh);
            return;
        }
        else
        {
            try_emplace(mapHUD, distSQ, SSA, root, pVisual, xform, sh);

            if (RImplementation.o.ssfx_core && !sh->passes[0]->ps->hud_disabled)
                try_emplace(HUDMask, distSQ, SSA, root, pVisual, xform, sh);

            if (sh->flags.bEmissive)
                try_emplace(mapHUDEmissive, distSQ, SSA, root, pVisual, xform, sh_d);

            return;
        }
    }

    // strict-sorting selection
    if (sh->flags.bStrictB2F)
    {
        try_emplace(mapSorted, distSQ, SSA, root, pVisual, xform, sh);
        return;
    }

    // Emissive geometry should be marked and R2 special-cases it
    // a) Allow to skeep already lit pixels
    // b) Allow to make them 100% lit and really bright
    // c) Should not cast shadows
    // d) Should be rendered to accumulation buffer in the second pass
    if (sh->flags.bEmissive)
        try_emplace(mapEmissive, distSQ, SSA, root, pVisual, xform, sh_d);

    if (sh->flags.bWmark && pmask_wmark)
    {
        try_emplace(mapWmark, distSQ, SSA, root, pVisual, xform, sh);
        return;
    }

    for (u32 iPass = 0; iPass < sh->passes.size(); ++iPass)
    {
        SPass* pass = sh->passes[iPass]._get();
        mapMatrix_T& map = mapMatrixPasses[sh->flags.iPriority / 2][iPass];
        mapMatrixItems& matrixItems = map[pass];

        // Create common node
        try_emplace(matrixItems, SSA, root, pVisual, xform);

        // Need to sort for HZB efficient use
        if (SSA > matrixItems.ssa)
        {
            matrixItems.ssa = SSA;
        }
    }
}

void R_dsgraph_structure::insert_static(dxRender_Visual* pVisual)
{
    if (pVisual->vis.marker[context_id] == marker)
        return;
    pVisual->vis.marker[context_id] = marker;

    float distSQ;
    float SSA = CalcSSA(distSQ, pVisual->vis.sphere.P, pVisual);
    if (SSA <= r_ssaDISCARD)
        return;

    // Distortive geometry should be marked and R2 special-cases it
    // a) Allow to optimize RT order
    // b) Should be rendered to special distort buffer in another pass
    VERIFY(pVisual->shader._get());
    ShaderElement* sh_d = pVisual->shader->E[4]._get();
    if (sh_d && sh_d->flags.bDistort && pmask[sh_d->flags.iPriority / 2])
        try_emplace(mapDistort, distSQ, SSA, pVisual, sh_d);

    // Select shader
    ShaderElement* sh = RImplementation.rimp_select_sh_static(pVisual, distSQ, phase);
    if (!sh)
        return;
    if (!pmask[sh->flags.iPriority / 2])
        return;

    // Water rendering
    if (sh->flags.isWater && RImplementation.o.ssfx_water)
    {
        try_emplace(mapWater, distSQ, SSA, pVisual, sh);
        return;
    }

    // strict-sorting selection
    if (sh->flags.bStrictB2F)
    {
        try_emplace(mapSorted, distSQ, SSA, pVisual, sh);
        return;
    }

    // Emissive geometry should be marked and R2 special-cases it
    // a) Allow to skeep already lit pixels
    // b) Allow to make them 100% lit and really bright
    // c) Should not cast shadows
    // d) Should be rendered to accumulation buffer in the second pass
    if (sh->flags.bEmissive)
        try_emplace(mapEmissive, distSQ, SSA, pVisual, sh_d);

    if (sh->flags.bWmark && pmask_wmark)
    {
        try_emplace(mapWmark, distSQ, SSA, pVisual, sh);
        return;
    }

    if (val_feedback && counter_S == val_feedback_breakp)
        val_feedback->rfeedback_static(pVisual);

    counter_S++;

    if (sh->flags.bLandscape && phase == CRender::PHASE_NORMAL)
    {
        try_emplace(mapLandscape, distSQ, SSA, pVisual, sh);
        return;
    }

    for (u32 iPass = 0; iPass < sh->passes.size(); ++iPass)
    {
        SPass* pass = sh->passes[iPass]._get();
        mapNormal_T& map = mapNormalPasses[sh->flags.iPriority / 2][iPass];
        mapNormalItems& normalItems = map[pass];

        try_emplace(normalItems, SSA, pVisual);

        // Need to sort for HZB efficient use
        if (SSA > normalItems.ssa)
        {
            normalItems.ssa = SSA;
        }
    }
}

// Static geometry optimization
#define O_S_L1_S_LOW 10.f // geometry 3d volume size
#define O_S_L1_D_LOW 150.f // distance, after which it is not rendered
#define O_S_L2_S_LOW 100.f
#define O_S_L2_D_LOW 200.f
#define O_S_L3_S_LOW 500.f
#define O_S_L3_D_LOW 250.f
#define O_S_L4_S_LOW 2500.f
#define O_S_L4_D_LOW 350.f
#define O_S_L5_S_LOW 7000.f
#define O_S_L5_D_LOW 400.f

#define O_S_L1_S_MED 25.f
#define O_S_L1_D_MED 50.f
#define O_S_L2_S_MED 200.f
#define O_S_L2_D_MED 150.f
#define O_S_L3_S_MED 1000.f
#define O_S_L3_D_MED 200.f
#define O_S_L4_S_MED 2500.f
#define O_S_L4_D_MED 300.f
#define O_S_L5_S_MED 7000.f
#define O_S_L5_D_MED 400.f

#define O_S_L1_S_HII 50.f
#define O_S_L1_D_HII 50.f
#define O_S_L2_S_HII 400.f
#define O_S_L2_D_HII 150.f
#define O_S_L3_S_HII 1500.f
#define O_S_L3_D_HII 200.f
#define O_S_L4_S_HII 5000.f
#define O_S_L4_D_HII 300.f
#define O_S_L5_S_HII 20000.f
#define O_S_L5_D_HII 350.f

#define O_S_L1_S_ULT 50.f
#define O_S_L1_D_ULT 35.f
#define O_S_L2_S_ULT 500.f
#define O_S_L2_D_ULT 125.f
#define O_S_L3_S_ULT 1750.f
#define O_S_L3_D_ULT 175.f
#define O_S_L4_S_ULT 5250.f
#define O_S_L4_D_ULT 250.f
#define O_S_L5_S_ULT 25000.f
#define O_S_L5_D_ULT 300.f

// Dyn geometry optimization

#define O_D_L1_S_LOW 1.f // geometry 3d volume size
#define O_D_L1_D_LOW 80.f // distance, after which it is not rendered
#define O_D_L2_S_LOW 3.f
#define O_D_L2_D_LOW 150.f
#define O_D_L3_S_LOW 4000.f
#define O_D_L3_D_LOW 250.f

#define O_D_L1_S_MED 1.f
#define O_D_L1_D_MED 40.f
#define O_D_L2_S_MED 4.f
#define O_D_L2_D_MED 100.f
#define O_D_L3_S_MED 4000.f
#define O_D_L3_D_MED 200.f

#define O_D_L1_S_HII 1.4f
#define O_D_L1_D_HII 30.f
#define O_D_L2_S_HII 4.f
#define O_D_L2_D_HII 80.f
#define O_D_L3_S_HII 4000.f
#define O_D_L3_D_HII 150.f

#define O_D_L1_S_ULT 2.0f
#define O_D_L1_D_ULT 30.f
#define O_D_L2_S_ULT 8.f
#define O_D_L2_D_ULT 50.f
#define O_D_L3_S_ULT 4000.f
#define O_D_L3_D_ULT 110.f

static constexpr Fvector4 o_optimize_static_l1_dist = {O_S_L1_D_LOW, O_S_L1_D_MED, O_S_L1_D_HII, O_S_L1_D_ULT};
static constexpr Fvector4 o_optimize_static_l1_size = {O_S_L1_S_LOW, O_S_L1_S_MED, O_S_L1_S_HII, O_S_L1_S_ULT};
static constexpr Fvector4 o_optimize_static_l2_dist = {O_S_L2_D_LOW, O_S_L2_D_MED, O_S_L2_D_HII, O_S_L2_D_ULT};
static constexpr Fvector4 o_optimize_static_l2_size = {O_S_L2_S_LOW, O_S_L2_S_MED, O_S_L2_S_HII, O_S_L2_S_ULT};
static constexpr Fvector4 o_optimize_static_l3_dist = {O_S_L3_D_LOW, O_S_L3_D_MED, O_S_L3_D_HII, O_S_L3_D_ULT};
static constexpr Fvector4 o_optimize_static_l3_size = {O_S_L3_S_LOW, O_S_L3_S_MED, O_S_L3_S_HII, O_S_L3_S_ULT};
static constexpr Fvector4 o_optimize_static_l4_dist = {O_S_L4_D_LOW, O_S_L4_D_MED, O_S_L4_D_HII, O_S_L4_D_ULT};
static constexpr Fvector4 o_optimize_static_l4_size = {O_S_L4_S_LOW, O_S_L4_S_MED, O_S_L4_S_HII, O_S_L4_S_ULT};
static constexpr Fvector4 o_optimize_static_l5_dist = {O_S_L5_D_LOW, O_S_L5_D_MED, O_S_L5_D_HII, O_S_L5_D_ULT};
static constexpr Fvector4 o_optimize_static_l5_size = {O_S_L5_S_LOW, O_S_L5_S_MED, O_S_L5_S_HII, O_S_L5_S_ULT};

static constexpr Fvector4 o_optimize_dynamic_l1_dist = {O_D_L1_D_LOW, O_D_L1_D_MED, O_D_L1_D_HII, O_D_L1_D_ULT};
static constexpr Fvector4 o_optimize_dynamic_l1_size = {O_D_L1_S_LOW, O_D_L1_S_MED, O_D_L1_S_HII, O_D_L1_S_ULT};
static constexpr Fvector4 o_optimize_dynamic_l2_dist = {O_D_L2_D_LOW, O_D_L2_D_MED, O_D_L2_D_HII, O_D_L2_D_ULT};
static constexpr Fvector4 o_optimize_dynamic_l2_size = {O_D_L2_S_LOW, O_D_L2_S_MED, O_D_L2_S_HII, O_D_L2_S_ULT};
static constexpr Fvector4 o_optimize_dynamic_l3_dist = {O_D_L3_D_LOW, O_D_L3_D_MED, O_D_L3_D_HII, O_D_L3_D_ULT};
static constexpr Fvector4 o_optimize_dynamic_l3_size = {O_D_L3_S_LOW, O_D_L3_S_MED, O_D_L3_S_HII, O_D_L3_S_ULT};

#define BASE_FOV 67.f

static float GetDistFromCamera(const IRenderable* root, const Fvector& from_position)
{
    // Aproximate, adjusted by fov, distance from camera to position (For right work when looking though binoculars and scopes)
    float distance = !(root && root->renderable_HUD()) ? Device.vCameraPosition.distance_to(from_position) : from_position.magnitude();
    float fov_K = BASE_FOV / Device.fFOV;

    return distance / fov_K;
}

static bool IsValuableToRender(const IRenderable* root, dxRender_Visual* pVisual, bool isStatic, bool sm, const Fmatrix* transform_matrix)
{
    if ((isStatic && opt_static >= 1) || (!isStatic && opt_dynamic >= 1))
    {
        float sphere_volume = pVisual->getVisData().sphere.volume();
        Fvector dpos;
        Fvector& fpos = dpos;
        float adjusted_distane;

        if (isStatic)
            fpos = pVisual->vis.sphere.P;
        else
            // dynamic geometry position needs to be transformed by transform matrix, to get world coordinates, dont forget ;)
            transform_matrix->transform_tiny(dpos, pVisual->vis.sphere.P);

        adjusted_distane = GetDistFromCamera(root, fpos);

        if (sm && !!psDeviceFlags2.test(rsOptShadowGeom)) // Highest cut off for shadow map
        {
            if (sphere_volume < 50000.f && adjusted_distane > ps_ssfx_shadow_cascades.z)
                // don't need geometry behind the farest sun shadow cascade
                return false;

            if ((sphere_volume < o_optimize_static_l1_size.z) && (adjusted_distane > o_optimize_static_l1_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l2_size.z) && (adjusted_distane > o_optimize_static_l2_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l3_size.z) && (adjusted_distane > o_optimize_static_l3_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l4_size.z) && (adjusted_distane > o_optimize_static_l4_dist.z))
                return false;
            else if ((sphere_volume < o_optimize_static_l5_size.z) && (adjusted_distane > o_optimize_static_l5_dist.z))
                return false;

            return true;
        }

        if (isStatic)
        {
            if (pVisual->Type == MT_LOD || pVisual->Type == MT_TREE_PM || pVisual->Type == MT_TREE_ST)
                return true;

            if (opt_static == 2)
            {
                if ((sphere_volume < o_optimize_static_l1_size.y) && (adjusted_distane > o_optimize_static_l1_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.y) && (adjusted_distane > o_optimize_static_l2_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.y) && (adjusted_distane > o_optimize_static_l3_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.y) && (adjusted_distane > o_optimize_static_l4_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.y) && (adjusted_distane > o_optimize_static_l5_dist.y))
                    return false;
            }
            else if (opt_static == 3)
            {
                if ((sphere_volume < o_optimize_static_l1_size.z) && (adjusted_distane > o_optimize_static_l1_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.z) && (adjusted_distane > o_optimize_static_l2_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.z) && (adjusted_distane > o_optimize_static_l3_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.z) && (adjusted_distane > o_optimize_static_l4_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.z) && (adjusted_distane > o_optimize_static_l5_dist.z))
                    return false;
            }
            else if (opt_static == 4)
            {
                if ((sphere_volume < o_optimize_static_l1_size.w) && (adjusted_distane > o_optimize_static_l1_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.w) && (adjusted_distane > o_optimize_static_l2_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.w) && (adjusted_distane > o_optimize_static_l3_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.w) && (adjusted_distane > o_optimize_static_l4_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.w) && (adjusted_distane > o_optimize_static_l5_dist.w))
                    return false;
            }
            else
            {
                if ((sphere_volume < o_optimize_static_l1_size.x) && (adjusted_distane > o_optimize_static_l1_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l2_size.x) && (adjusted_distane > o_optimize_static_l2_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l3_size.x) && (adjusted_distane > o_optimize_static_l3_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l4_size.x) && (adjusted_distane > o_optimize_static_l4_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_static_l5_size.x) && (adjusted_distane > o_optimize_static_l5_dist.x))
                    return false;
            }
        }
        else
        {
            if (opt_dynamic == 2)
            {
                if ((sphere_volume < o_optimize_dynamic_l1_size.y) && (adjusted_distane > o_optimize_dynamic_l1_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l2_size.y) && (adjusted_distane > o_optimize_dynamic_l2_dist.y))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l3_size.y) && (adjusted_distane > o_optimize_dynamic_l3_dist.y))
                    return false;
            }
            else if (opt_dynamic == 3)
            {
                if ((sphere_volume < o_optimize_dynamic_l1_size.z) && (adjusted_distane > o_optimize_dynamic_l1_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l2_size.z) && (adjusted_distane > o_optimize_dynamic_l2_dist.z))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l3_size.z) && (adjusted_distane > o_optimize_dynamic_l3_dist.z))
                    return false;
            }
            else if (opt_dynamic == 4)
            {
                if ((sphere_volume < o_optimize_dynamic_l1_size.w) && (adjusted_distane > o_optimize_dynamic_l1_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l2_size.w) && (adjusted_distane > o_optimize_dynamic_l2_dist.w))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l3_size.w) && (adjusted_distane > o_optimize_dynamic_l3_dist.w))
                    return false;
            }
            else
            {
                if ((sphere_volume < o_optimize_dynamic_l1_size.x) && (adjusted_distane > o_optimize_dynamic_l1_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l2_size.x) && (adjusted_distane > o_optimize_dynamic_l2_dist.x))
                    return false;
                else if ((sphere_volume < o_optimize_dynamic_l3_size.x) && (adjusted_distane > o_optimize_dynamic_l3_dist.x))
                    return false;
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void R_dsgraph_structure::add_leafs_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, bool ignore)
{
    if (!pVisual)
        return;

    if (!ignore && !IsValuableToRender(root, pVisual, false, phase == CRender::PHASE_SMAP, &xform))
        return;

    // Visual is 100% visible - simply add it
    switch (pVisual->Type)
    {
    case MT_PARTICLE_GROUP: {
        // Add all children, doesn't perform any tests
        PS::CParticleGroup* pG{smart_cast<PS::CParticleGroup*>(pVisual)};

        for (auto& it : pG->items)
        {
            PS::CParticleGroup::SItem& I = it;
            if (I._effect)
                add_leafs_dynamic(root, I._effect, xform, ignore);
            for (auto& pit : I._children_related)
                add_leafs_dynamic(root, pit, xform, ignore);
            for (auto& pit : I._children_free)
                add_leafs_dynamic(root, pit, xform, ignore);
        }
    }
        return;
    case MT_HIERRARHY: {
        for (dxRender_Visual* Vis : smart_cast<FHierrarhyVisual*>(pVisual)->children)
            if (Vis->getRZFlag())
                add_leafs_dynamic(root, Vis, xform, ignore);
    }
        return;
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        auto pV = smart_cast<CKinematics*>(pVisual);
        BOOL _use_lod{};

        if (pV->m_lod)
        {
            Fvector Tpos;
            float D;
            xform.transform_tiny(Tpos, pV->vis.sphere.P);
            float ssa = CalcSSA(D, Tpos, pV->vis.sphere.R / 2.f); // assume dynamics never consume full sphere
            if (ssa < r_ssaLOD_A)
                _use_lod = TRUE;
        }

        if (_use_lod)
        {
            add_leafs_dynamic(root, pV->m_lod, xform, ignore);
        }
        else
        {
            pV->CalculateBones(TRUE);
            if (phase == CRender::PHASE_NORMAL)
                pV->CalculateWallmarks(root && root->renderable_HUD());

            for (dxRender_Visual* Vis : pV->children)
                if (Vis->getRZFlag())
                    add_leafs_dynamic(root, Vis, xform, ignore);
        }
    }
        return;
    default: {
        // General type of visual
        // Calculate distance to it's center
        Fvector Tpos;
        xform.transform_tiny(Tpos, pVisual->vis.sphere.P);
        insert_dynamic(root, pVisual, xform, Tpos);
    }
        return;
    }
}

void R_dsgraph_structure::add_leafs_static(dxRender_Visual* pVisual)
{
    if (use_hom && !RImplementation.HOM.visible(pVisual->vis))
        return;

    if (!pVisual->_ignore_optimization && !IsValuableToRender(nullptr, pVisual, true, phase == CRender::PHASE_SMAP, nullptr))
        return;

    // Visual is 100% visible - simply add it
    switch (pVisual->Type)
    {
    case MT_PARTICLE_GROUP: return;
    case MT_HIERRARHY: {
        for (dxRender_Visual* Vis : smart_cast<FHierrarhyVisual*>(pVisual)->children)
            if (Vis->getRZFlag())
                add_leafs_static(Vis);
    }
        return;
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        auto pV = smart_cast<CKinematics*>(pVisual);
        pV->CalculateBones(TRUE);

        for (dxRender_Visual* Vis : pV->children)
            if (Vis->getRZFlag())
                add_leafs_static(Vis);
    }
        return;
    case MT_LOD: {
        auto pV = smart_cast<FLOD*>(pVisual);
        float D;
        float ssa = CalcSSA(D, pV->vis.sphere.P, pV);
        ssa *= pV->lod_factor;
        if (ssa < r_ssaLOD_A)
        {
            if (ssa < r_ssaDISCARD)
                return;

            try_emplace(mapLOD, D, ssa, pVisual);
        }

        if (ssa > r_ssaLOD_B || phase == CRender::PHASE_SMAP)
        {
            // Add all children, doesn't perform any tests
            for (dxRender_Visual* Vis : pV->children)
                add_leafs_static(Vis);
        }
    }
        return;
    case MT_TREE_PM:
    case MT_TREE_ST: {
        // General type of visual
        insert_static(pVisual);
    }
        return;
    default: {
        // General type of visual
        insert_static(pVisual);
    }
        return;
    }
}

void R_dsgraph_structure::add_static(dxRender_Visual* pVisual, const CFrustum& view, u32 planes)
{
    if (!pVisual->_ignore_optimization && !IsValuableToRender(nullptr, pVisual, true, phase == CRender::PHASE_SMAP, nullptr))
        return;

    vis_data& vis = pVisual->vis;

    // Check frustum visibility and calculate distance to visual's center
    const EFC_Visible VIS = view.testSAABB(vis.sphere.P, vis.sphere.R, vis.box.data(), planes);
    if (VIS == fcvNone)
        return;

    if (use_hom && !RImplementation.HOM.visible(vis))
        return;

    // If we get here visual is visible or partially visible
    switch (pVisual->Type)
    {
    case MT_PARTICLE_GROUP: return;
    case MT_HIERRARHY: {
        if (fcvPartial == VIS)
        {
            for (dxRender_Visual* Vis : smart_cast<FHierrarhyVisual*>(pVisual)->children)
                if (Vis->getRZFlag())
                    add_static(Vis, view, planes);
        }
        else
        {
            for (dxRender_Visual* Vis : smart_cast<FHierrarhyVisual*>(pVisual)->children)
                if (Vis->getRZFlag())
                    add_leafs_static(Vis);
        }
    }
    break;
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        auto pV = smart_cast<CKinematics*>(pVisual);
        pV->CalculateBones(TRUE);

        if (fcvPartial == VIS)
        {
            for (dxRender_Visual* Vis : pV->children)
                if (Vis->getRZFlag())
                    add_static(Vis, view, planes);
        }
        else
        {
            for (dxRender_Visual* Vis : pV->children)
                if (Vis->getRZFlag())
                    add_leafs_static(Vis);
        }
    }
    break;
    case MT_LOD: {
        auto pV = smart_cast<FLOD*>(pVisual);
        float D;
        float ssa = CalcSSA(D, pV->vis.sphere.P, pV);
        ssa *= pV->lod_factor;
        if (ssa < r_ssaLOD_A)
        {
            if (ssa < r_ssaDISCARD)
                return;

            try_emplace(mapLOD, D, ssa, pVisual);
        }

        if (ssa > r_ssaLOD_B || phase == CRender::PHASE_SMAP)
        {
            for (dxRender_Visual* Vis : pV->children)
                add_leafs_static(Vis);
        }
    }
    break;
    case MT_TREE_ST:
    case MT_TREE_PM: {
        // General type of visual
        insert_static(pVisual);
    }
        return;
    default: {
        // General type of visual
        insert_static(pVisual);
    }
    break;
    }
}

// sub-space rendering - PHASE_NORMAL
void R_dsgraph_structure::build_subspace(sector_id_t o_sector_id, CFrustum& _frustum)
{
    marker++;

    phase = CRender::PHASE_NORMAL;
    use_hom = true;

    RImplementation.HOM.wait_async();

    // Calculate sector(s) and their objects
    if (o_sector_id == INVALID_SECTOR_ID)
    {
    last:
        if (g_pGameLevel)
            g_hud->Render_Last(context_id); // HUD

        return;
    }

    if (RImplementation.rmPortals)
    {
        // Check if camera is too near to some portal - if so force DualRender
        constexpr float eps = VIEWPORT_NEAR + EPS_L;
        constexpr Fvector box_radius{eps, eps, eps};

        Sectors_xrc.box_query(CDB::OPT_FULL_TEST, RImplementation.rmPortals, Device.vCameraPosition, box_radius);

        for (size_t K = 0; K < Sectors_xrc.r_count(); K++)
        {
            CPortal* pPortal = Portals[RImplementation.rmPortals->get_tris()[Sectors_xrc.r_begin()[K].id].dummy];
            pPortal->bDualRender = TRUE;
        }
    }

    // Traverse sector/portal structure
    PortalTraverser.traverse(Sectors[o_sector_id], _frustum, Device.vCameraPosition, Device.mFullTransform,
                             CPortalTraverser::VQ_HOM + CPortalTraverser::VQ_SSA + CPortalTraverser::VQ_FADE);

    // Determine visibility for static geometry hierarchy
    for (auto* sector : PortalTraverser.r_sectors)
    {
        dxRender_Visual* root = sector->root();
        for (const auto& view : sector->r_frustums)
            add_static(root, view, view.getMask());
    }

    // Traverse object database
    g_SpatialSpace->q_frustum(lstRenderables, ISpatial_DB::O_ORDERED, STYPE_RENDERABLE + STYPE_LIGHTSOURCE, _frustum);

    // Exact sorting order (front-to-back)
    std::ranges::sort(lstRenderables, [](const auto* s1, const auto* s2) {
        const float d1 = s1->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
        const float d2 = s2->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
        return d1 < d2;
    });

    u32 uID_LTRACK = 0xffffffff;
    RImplementation.uLastLTRACK++;
    if (!lstRenderables.empty())
        uID_LTRACK = RImplementation.uLastLTRACK % lstRenderables.size();

    // update light-vis for current entity / actor
    CObject* O = g_pGameLevel->CurrentViewEntity();
    if (O)
    {
        CROS_impl* R = (CROS_impl*)O->ROS();
        if (R)
            R->update(O);
    }

    // Determine visibility for dynamic part of scene
    for (u32 o_it = 0; o_it < lstRenderables.size(); o_it++)
    {
        ISpatial* spatial = lstRenderables[o_it];
        const auto& entity_pos = spatial->spatial_sector_point();
        spatial->spatial_updatesector(detect_sector(entity_pos));

        const auto& [sector_id, type, sphere] = std::tuple(spatial->spatial.sector_id, spatial->spatial.type, spatial->spatial.sphere);
        if (sector_id == INVALID_SECTOR_ID)
            continue; // disassociated from S/P structure
        auto* sector = Sectors[sector_id];

        if (type & STYPE_LIGHTSOURCE)
        {
            // lightsource
            light* L = (light*)spatial->dcast_Light();
            VERIFY(L);
            float lod = L->get_LOD();
            if (lod > EPS_L)
            {
                vis_data& vis = L->get_homdata();
                if (RImplementation.HOM.visible(vis))
                    RImplementation.Lights.add_light(L);
            }
            continue;
        }

        if (PortalTraverser.i_marker != sector->r_marker)
            continue; // inactive (untouched) sector

        for (auto& view : sector->r_frustums)
        {
            if (!view.testSphere_dirty(sphere.P, sphere.R))
                continue;

            if (type & STYPE_RENDERABLE)
            {
                // renderable
                IRenderable* renderable = spatial->dcast_Renderable();
                VERIFY(renderable);

                // Occlusion
                //	casting is faster then using getVis method
                vis_data& v_orig = ((dxRender_Visual*)renderable->renderable.visual)->vis;
                vis_data v_copy = v_orig;

                v_copy.box.xform(renderable->renderable.xform);
                BOOL bVisible = RImplementation.HOM.visible(v_copy);

                xr_memcpy16(&v_orig.accept_frame, &v_copy.accept_frame);
                v_orig.marker = v_copy.marker;

                if (!bVisible)
                    break; // exit loop on frustums

                // update light-vis for selected entity
                if (o_it == uID_LTRACK && renderable->renderable_ROS())
                {
                    // track lighting environment
                    CROS_impl* T = (CROS_impl*)renderable->renderable_ROS();
                    T->update(renderable);
                }

                // Rendering
                renderable->renderable_Render(context_id, renderable);
            }

            break; // exit loop on frustums
        }
    }

    goto last;
}

// sub-space rendering - PHASE_SMAP
void R_dsgraph_structure::build_subspace(sector_id_t o_sector_id, CFrustum& _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic)
{
    VERIFY(sector_id != INVALID_SECTOR_ID);
    marker++; // !!! critical here

    phase = CRender::PHASE_SMAP;
    use_hom = false;

    // Traverse sector/portal structure
    PortalTraverser.traverse(Sectors[o_sector_id], _frustum, _cop, mCombined, 0);

    // Determine visibility for static geometry hierarchy
    for (auto* sector : PortalTraverser.r_sectors)
    {
        dxRender_Visual* root = sector->root();
        for (const auto& view : sector->r_frustums)
            add_static(root, view, view.getMask());
    }

    if (!_dynamic)
        return;

    // Traverse object database
    g_SpatialSpace->q_frustum(lstRenderables, ISpatial_DB::O_ORDERED, STYPE_RENDERABLE, _frustum);

    // Determine visibility for dynamic part of scene
    for (auto* spatial : lstRenderables)
    {
        const auto& [sector_id, sphere] = std::tuple(spatial->spatial.sector_id, spatial->spatial.sphere);
        if (sector_id == INVALID_SECTOR_ID)
            continue; // disassociated from S/P structure

        auto* sector = Sectors[sector_id];
        if (PortalTraverser.i_marker != sector->r_marker)
            continue; // inactive (untouched) sector

        for (auto& view : sector->r_frustums)
        {
            if (!view.testSphere_dirty(sphere.P, sphere.R))
                continue;

            // renderable
            IRenderable* renderable = spatial->dcast_Renderable();
            if (!renderable)
                continue; // unknown, but renderable object (r1_glow???)

            renderable->renderable_Render(context_id, nullptr);
        }
    }

    if (!g_pGameLevel || !ps_r2_ls_flags_ext.test(R2FLAGEXT_ACTOR_SHADOW))
        return;

    CObject* viewEntity = g_pGameLevel->CurrentViewEntity();
    if (!viewEntity)
        return;

    const auto& entity_pos = viewEntity->spatial_sector_point();
    viewEntity->spatial_updatesector(detect_sector(entity_pos));

    const auto& [sector_id, sphere] = std::tuple(viewEntity->spatial.sector_id, viewEntity->spatial.sphere);
    if (sector_id == INVALID_SECTOR_ID)
        return; // disassociated from S/P structure

    auto* sector = Sectors[sector_id];
    if (PortalTraverser.i_marker != sector->r_marker)
        return; // inactive (untouched) sector

    for (const CFrustum& view : sector->r_frustums)
    {
        if (!view.testSphere_dirty(sphere.P, sphere.R))
            continue;

        // renderable
        g_hud->Render_First(context_id);
    }
}

void R_dsgraph_structure::load(const xr_vector<CSector::level_sector_data_t>& sectors_data, const xr_vector<CPortal::level_portal_data_t>& portals_data)
{
    const sector_id_t sectors_count = sectors_data.size();
    const u32 portals_count = portals_data.size();

    Sectors.resize(sectors_count);
    Portals.resize(portals_count);

    for (u32 idx = 0; idx < portals_count; ++idx)
    {
        auto* portal = xr_new<CPortal>();
        Portals[idx] = portal;
    }

    for (sector_id_t idx = 0; idx < sectors_count; ++idx)
    {
        auto* sector = xr_new<CSector>();

        sector->unique_id = idx;
        sector->setup(sectors_data[idx], Portals);
        Sectors[idx] = sector;
    }

    for (u32 idx = 0; idx < portals_count; ++idx)
    {
        auto* portal = static_cast<CPortal*>(Portals[idx]);
        portal->setup(portals_data[idx], Sectors);
    }
}

void R_dsgraph_structure::unload()
{
    for (auto* sector : Sectors)
        xr_delete(sector);
    Sectors.clear();

    for (auto* portal : Portals)
        xr_delete(portal);
    Portals.clear();
}
