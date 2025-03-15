#include "stdafx.h"

#include "../../xr_3da/render.h"
#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../../xr_3da/CustomHUD.h"

#include "FBasicVisual.h"

extern float psHUD_FOV;

using namespace R_dsgraph;

extern float r_ssaHZBvsTEX;
extern float r_ssaGLOD_start, r_ssaGLOD_end;

ICF float calcLOD(float ssa /*fDistSq*/, float /*R*/) { return _sqrt(clampr((ssa - r_ssaGLOD_end) / (r_ssaGLOD_start - r_ssaGLOD_end), 0.f, 1.f)); }

template <class T>
bool cmp_ssa(const T& lhs, const T& rhs)
{
    return lhs.ssa > rhs.ssa;
}

// Sorting by SSA and changes minimizations
template <typename T>
bool cmp_pass(const T& left, const T& right)
{
    if (left->key->equal(*right->key))
        return false;
    return left->val.ssa >= right->val.ssa;
}

// ALPHA
static void __fastcall sorted_L1(mapSorted_Node* N, void* arg)
{
    R_dsgraph_structure& dsgraph = *(R_dsgraph_structure*)arg;

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());

    RCache.set_Element(N->val.se);
    RCache.set_xform_world(N->val.Matrix);
    RCache.apply_lmaterial(N->val.pObject);

    const float LOD = calcLOD(N->key, V->vis.sphere.R);
    RCache.LOD.set_LOD(LOD);
    V->Render(LOD, dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall water_node_ssr(mapSorted_Node* N, void* arg)
{
    R_dsgraph_structure& dsgraph = *(R_dsgraph_structure*)arg;

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V);

    RCache.set_Shader(RImplementation.Target->s_ssfx_water_ssr);

    RCache.set_xform_world(N->val.Matrix);
    RCache.apply_lmaterial(N->val.pObject);

    RCache.set_c("cam_pos", RImplementation.Target->Position_previous.x, RImplementation.Target->Position_previous.y, RImplementation.Target->Position_previous.z, 0.0f);

    // Previous matrix data
    RCache.set_c("m_current", RImplementation.Target->Matrix_current);
    RCache.set_c("m_previous", RImplementation.Target->Matrix_previous);

    V->Render(calcLOD(N->key, V->vis.sphere.R), dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall water_node(mapSorted_Node* N, void* arg)
{
    R_dsgraph_structure& dsgraph = *(R_dsgraph_structure*)arg;

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V);

    if (RImplementation.o.ssfx_water)
        RCache.set_Shader(RImplementation.Target->s_ssfx_water);

    RCache.set_xform_world(N->val.Matrix);
    RCache.apply_lmaterial(N->val.pObject);

    // Wind settings
    float WindDir = g_pGamePersistent->Environment().CurrentEnv->wind_direction;
    float WindVel = g_pGamePersistent->Environment().CurrentEnv->wind_velocity;
    RCache.set_c("wind_setup", WindDir, WindVel, 0.f, 0.f);

    V->Render(calcLOD(N->key, V->vis.sphere.R), dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall hud_node(mapSorted_Node* N, void* arg)
{
    R_dsgraph_structure& dsgraph = *(R_dsgraph_structure*)arg;

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());
    RCache.set_xform_world(N->val.Matrix);

    if (N->val.se->passes[0]->ps->hud_disabled)
        return;

    int skinning = N->val.se->passes[0]->vs->skinning;
    RCache.set_Shader(RImplementation.Target->s_ssfx_hud[skinning]);

    RImplementation.Target->Matrix_HUD_previous.set(N->val.PrevMatrix);
    N->val.PrevMatrix.set(RCache.xforms.m_wvp);

    RImplementation.Target->RVelocity = true;

    const float LOD = calcLOD(N->key, V->vis.sphere.R);
    RCache.LOD.set_LOD(LOD);
    V->Render(LOD, dsgraph.phase == CRender::PHASE_SMAP);

    RImplementation.Target->RVelocity = false;
}

void R_dsgraph_structure::render_graph(u32 _priority)
{
    PIX_EVENT(r_dsgraph_render_graph);
    Device.Statistic->RenderDUMP.Begin();

    RCache.set_xform_world(Fidentity);

    // **************************************************** NORMAL
    // Perform sorting based on ScreenSpaceArea
    // Sorting by SSA and changes minimizations
    // Render several passes
    {
        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapNormalPasses[_priority][iPass];

            map.getANY_P(nrmPasses);
            std::ranges::sort(nrmPasses, cmp_pass<mapNormal_T::value_type*>);
            for (const auto& it : nrmPasses)
            {
                RCache.set_Pass(it->key);
                RCache.apply_lmaterial();

                mapNormalItems& items = it->val;
                items.ssa = 0;

                std::ranges::sort(items, cmp_ssa<_NormalItem>);
                for (const auto& item : items)
                {
                    const float LOD = calcLOD(item.ssa, item.pVisual->vis.sphere.R);
                    RCache.LOD.set_LOD(LOD);
                    item.pVisual->Render(LOD, phase == CRender::PHASE_SMAP);
                }
                items.clear();
            }
            nrmPasses.clear();
            map.clear();
        }
    }

    // **************************************************** MATRIX
    // Perform sorting based on ScreenSpaceArea
    // Sorting by SSA and changes minimizations
    // Render several passes
    {
        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapMatrixPasses[_priority][iPass];

            map.getANY_P(matPasses);
            std::ranges::sort(matPasses, cmp_pass<mapMatrix_T::value_type*>);
            for (const auto& it : matPasses)
            {
                RCache.set_Pass(it->key);

                mapMatrixItems& items = it->val;
                items.ssa = 0;

                std::ranges::sort(items, cmp_ssa<_MatrixItem>);
                for (auto& item : items)
                {
                    RCache.set_xform_world(item.Matrix);
                    RCache.apply_lmaterial(item.pObject);

                    const float LOD = calcLOD(item.ssa, item.pVisual->vis.sphere.R);
                    RCache.LOD.set_LOD(LOD);
                    // --#SM+#-- Обновляем шейдерные данные модели [update shader values for this model]
                    // RCache.hemi.c_update(item.pVisual);

                    item.pVisual->Render(LOD, phase == CRender::PHASE_SMAP);
                }
                items.clear();
            }
            matPasses.clear();
            map.clear();
        }
    }

    Device.Statistic->RenderDUMP.End();
}

//////////////////////////////////////////////////////////////////////////
// HUD render
void R_dsgraph_structure::render_hud(bool NoPS)
{
    PIX_EVENT(r_dsgraph_render_hud);

    if ((!NoPS && mapHUD.empty()) || (NoPS && HUDMask.empty()))
        return;

    // Change projection
    Fmatrix Pold = Device.mProject;
    Fmatrix FTold = Device.mFullTransform;
    Fmatrix FVold = Device.mView;
    Device.mView.build_camera_dir(Fvector().set(0.f, 0.f, 0.f), Device.vCameraDirection, Device.vCameraTop);
    Device.mProject.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), Device.fASPECT, HUD_VIEWPORT_NEAR,
                                     g_pGamePersistent->Environment().CurrentEnv->far_plane);

    Device.mFullTransform.mul(Device.mProject, Device.mView);
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);

    // Rendering
    RImplementation.rmNear();
    if (!NoPS)
    {
        mapHUD.traverseLR(sorted_L1, this);
        mapHUD.clear();
    }
    else
    {
        HUDMask.traverseLR(hud_node, this);
        HUDMask.clear();
    }
    RImplementation.rmNormal();

    // Restore projection
    Device.mProject = Pold;
    Device.mFullTransform = FTold;
    Device.mView = FVold;
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);
}

void R_dsgraph_structure::render_hud_ui()
{
    // Change projection
    Fmatrix Pold = Device.mProject;
    Fmatrix FTold = Device.mFullTransform;
    Fmatrix Vold = Device.mView;
    Device.mView.build_camera_dir(Fvector().set(0.f, 0.f, 0.f), Device.vCameraDirection, Device.vCameraTop);
    Device.mProject.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), Device.fASPECT, HUD_VIEWPORT_NEAR,
                                     g_pGamePersistent->Environment().CurrentEnv->far_plane);

    Device.mFullTransform.mul(Device.mProject, Device.mView);
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);

    RImplementation.rmNear();
    g_hud->RenderActiveItemUI();
    RImplementation.rmNormal();

    // Restore projection
    Device.mProject = Pold;
    Device.mFullTransform = FTold;
    Device.mView = Vold;
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_sorted()
{
    // Sorted (back to front)
    mapSorted.traverseRL(sorted_L1, this);
    mapSorted.clear();

    if (mapHUDSorted.empty())
        return;

    // Change projection
    Fmatrix Pold = Device.mProject;
    Fmatrix FTold = Device.mFullTransform;
    Fmatrix Vold = Device.mView;
    Device.mView.build_camera_dir(Fvector().set(0.f, 0.f, 0.f), Device.vCameraDirection, Device.vCameraTop);
    Device.mProject.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), Device.fASPECT, HUD_VIEWPORT_NEAR,
                                     g_pGamePersistent->Environment().CurrentEnv->far_plane);

    Device.mFullTransform.mul(Device.mProject, Device.mView);
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);

    // Rendering
    RImplementation.rmNear();
    mapHUDSorted.traverseRL(sorted_L1, this);
    mapHUDSorted.clear();
    RImplementation.rmNormal();

    // Restore projection
    Device.mProject = Pold;
    Device.mFullTransform = FTold;
    Device.mView = Vold;
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_emissive(bool clear, bool renderHUD)
{
    // Sorted (back to front)
    mapEmissive.traverseLR(sorted_L1, this);
    if (clear)
        mapEmissive.clear();

    // Change projection
    Fmatrix Pold = Device.mProject;
    Fmatrix FTold = Device.mFullTransform;
    Fmatrix Vold = Device.mView;
    Device.mView.build_camera_dir(Fvector().set(0.f, 0.f, 0.f), Device.vCameraDirection, Device.vCameraTop);
    Device.mProject.build_projection(deg2rad(psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV), Device.fASPECT, HUD_VIEWPORT_NEAR,
                                     g_pGamePersistent->Environment().CurrentEnv->far_plane);

    Device.mFullTransform.mul(Device.mProject, Device.mView);
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);

    // Rendering
    RImplementation.rmNear();
    // Sorted (back to front)
    mapHUDEmissive.traverseLR(sorted_L1, this);

    if (clear)
        mapHUDEmissive.clear();

    if (renderHUD)
        mapHUDSorted.traverseRL(sorted_L1, this);

    RImplementation.rmNormal();

    // Restore projection
    Device.mProject = Pold;
    Device.mFullTransform = FTold;
    Device.mView = Vold;
    RCache.set_xform_view(Device.mView);
    RCache.set_xform_project(Device.mProject);
}

void R_dsgraph_structure::render_water_ssr() { mapWater.traverseLR(water_node_ssr, this); }

void R_dsgraph_structure::render_water()
{
    mapWater.traverseLR(water_node, this);
    mapWater.clear();
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_wmarks()
{
    // Sorted (back to front)
    mapWmark.traverseLR(sorted_L1, this);
    mapWmark.clear();
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_distort()
{
    // Sorted (back to front)
    mapDistort.traverseRL(sorted_L1, this);
    mapDistort.clear();
}

static void __fastcall pLandscape_0(mapLandscape_Node* N, void* arg)
{
    R_dsgraph_structure& dsgraph = *(R_dsgraph_structure*)arg;

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());
    RCache.set_Element(N->val.se, 0);
    float LOD = calcLOD(N->val.ssa, V->vis.sphere.R);
    RCache.LOD.set_LOD(LOD);
    V->Render(LOD, dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall pLandscape_1(mapLandscape_Node* N, void* arg)
{
    R_dsgraph_structure& dsgraph = *(R_dsgraph_structure*)arg;

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());
    RCache.set_Element(N->val.se, 1);
    RCache.apply_lmaterial();
    float LOD = calcLOD(N->val.ssa, V->vis.sphere.R);
    RCache.LOD.set_LOD(LOD);
    V->Render(LOD, dsgraph.phase == CRender::PHASE_SMAP);
}

void R_dsgraph_structure::render_landscape(u32 pass, bool _clear)
{
    RCache.set_xform_world(Fidentity);

    if (pass == 0)
        mapLandscape.traverseLR(pLandscape_0, this);
    else
        mapLandscape.traverseLR(pLandscape_1, this);

    if (_clear)
        mapLandscape.clear();
}
