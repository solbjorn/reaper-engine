#include "stdafx.h"

#include "../../xr_3da/render.h"
#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../../xr_3da/CustomHUD.h"

#include "FBasicVisual.h"

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
    auto& dsgraph = RImplementation.get_context((uintptr_t)arg);

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());

    dsgraph.cmd_list.set_Element(N->val.se);
    dsgraph.cmd_list.set_xform_world(N->val.Matrix);
    dsgraph.cmd_list.apply_lmaterial(N->val.pObject);

    const float LOD = calcLOD(N->key, V->vis.sphere.R);
    dsgraph.cmd_list.LOD.set_LOD(LOD);
    V->Render(dsgraph.cmd_list, LOD, dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall water_node_ssr(mapSorted_Node* N, void* arg)
{
    auto& dsgraph = RImplementation.get_context((uintptr_t)arg);

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V);

    dsgraph.cmd_list.set_Shader(RImplementation.Target->s_ssfx_water_ssr);

    dsgraph.cmd_list.set_xform_world(N->val.Matrix);
    dsgraph.cmd_list.apply_lmaterial(N->val.pObject);

    dsgraph.cmd_list.set_c("cam_pos", RImplementation.Target->Position_previous.x, RImplementation.Target->Position_previous.y, RImplementation.Target->Position_previous.z, 0.0f);

    // Previous matrix data
    dsgraph.cmd_list.set_c("m_current", RImplementation.Target->Matrix_current);
    dsgraph.cmd_list.set_c("m_previous", RImplementation.Target->Matrix_previous);

    V->Render(dsgraph.cmd_list, calcLOD(N->key, V->vis.sphere.R), dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall water_node(mapSorted_Node* N, void* arg)
{
    auto& dsgraph = RImplementation.get_context((uintptr_t)arg);

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V);

    if (RImplementation.o.ssfx_water)
        dsgraph.cmd_list.set_Shader(RImplementation.Target->s_ssfx_water);

    dsgraph.cmd_list.set_xform_world(N->val.Matrix);
    dsgraph.cmd_list.apply_lmaterial(N->val.pObject);

    // Wind settings
    float WindDir = g_pGamePersistent->Environment().CurrentEnv->wind_direction;
    float WindVel = g_pGamePersistent->Environment().CurrentEnv->wind_velocity;
    dsgraph.cmd_list.set_c("wind_setup", WindDir, WindVel, 0.f, 0.f);

    V->Render(dsgraph.cmd_list, calcLOD(N->key, V->vis.sphere.R), dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall hud_node(mapSorted_Node* N, void* arg)
{
    auto& dsgraph = RImplementation.get_context((uintptr_t)arg);

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());
    dsgraph.cmd_list.set_xform_world(N->val.Matrix);

    if (N->val.se->passes[0]->ps->hud_disabled)
        return;

    int skinning = N->val.se->passes[0]->vs->skinning;
    dsgraph.cmd_list.set_Shader(RImplementation.Target->s_ssfx_hud[skinning]);

    RImplementation.Target->Matrix_HUD_previous.set(N->val.PrevMatrix);
    N->val.PrevMatrix.set(dsgraph.cmd_list.xforms.m_wvp);

    dsgraph.cmd_list.RVelocity = true;

    const float LOD = calcLOD(N->key, V->vis.sphere.R);
    dsgraph.cmd_list.LOD.set_LOD(LOD);
    V->Render(dsgraph.cmd_list, LOD, dsgraph.phase == CRender::PHASE_SMAP);

    dsgraph.cmd_list.RVelocity = false;
}

void R_dsgraph_structure::render_graph(u32 _priority)
{
    PIX_EVENT_CTX(cmd_list, dsgraph_render_graph);
    Device.Statistic->RenderDUMP.Begin();

    cmd_list.set_xform_world(Fidentity);

    // **************************************************** NORMAL
    // Perform sorting based on ScreenSpaceArea
    // Sorting by SSA and changes minimizations
    // Render several passes
    {
        PIX_EVENT_CTX(cmd_list, dsgraph_render_static);

        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapNormalPasses[_priority][iPass];

            map.getANY_P(nrmPasses);
            std::ranges::sort(nrmPasses, cmp_pass<mapNormal_T::value_type*>);
            for (const auto& it : nrmPasses)
            {
                cmd_list.set_Pass(it->key);
                cmd_list.apply_lmaterial();

                mapNormalItems& items = it->val;
                items.ssa = 0;

                std::ranges::sort(items, cmp_ssa<_NormalItem>);
                for (const auto& item : items)
                {
                    const float LOD = calcLOD(item.ssa, item.pVisual->vis.sphere.R);
                    cmd_list.LOD.set_LOD(LOD);
                    item.pVisual->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
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
        PIX_EVENT_CTX(cmd_list, dsgraph_render_dynamic);

        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapMatrixPasses[_priority][iPass];

            map.getANY_P(matPasses);
            std::ranges::sort(matPasses, cmp_pass<mapMatrix_T::value_type*>);
            for (const auto& it : matPasses)
            {
                cmd_list.set_Pass(it->key);

                mapMatrixItems& items = it->val;
                items.ssa = 0;

                std::ranges::sort(items, cmp_ssa<_MatrixItem>);
                for (auto& item : items)
                {
                    cmd_list.set_xform_world(item.Matrix);
                    cmd_list.apply_lmaterial(item.pObject);

                    const float LOD = calcLOD(item.ssa, item.pVisual->vis.sphere.R);
                    cmd_list.LOD.set_LOD(LOD);
                    // --#SM+#-- Обновляем шейдерные данные модели [update shader values for this model]
                    // cmd_list.hemi.c_update(item.pVisual);

                    item.pVisual->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
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
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);
    if (!NoPS)
    {
        mapHUD.traverseLR(sorted_L1, (void*)(uintptr_t)context_id);
        mapHUD.clear();
    }
    else
    {
        HUDMask.traverseLR(hud_node, (void*)(uintptr_t)context_id);
        HUDMask.clear();
    }
    RImplementation.rmNormal(cmd_list);

    // Restore projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

void R_dsgraph_structure::render_hud_ui()
{
    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    RImplementation.rmNear(cmd_list);
    g_hud->RenderActiveItemUI();
    RImplementation.rmNormal(cmd_list);

    // Restore projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_sorted()
{
    // Sorted (back to front)
    mapSorted.traverseRL(sorted_L1, (void*)(uintptr_t)context_id);
    mapSorted.clear();

    if (mapHUDSorted.empty())
        return;

    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);
    mapHUDSorted.traverseRL(sorted_L1, (void*)(uintptr_t)context_id);
    mapHUDSorted.clear();
    RImplementation.rmNormal(cmd_list);

    // Restore projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_emissive(bool clear, bool renderHUD)
{
    // Sorted (back to front)
    mapEmissive.traverseLR(sorted_L1, (void*)(uintptr_t)context_id);
    if (clear)
        mapEmissive.clear();

    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);
    // Sorted (back to front)
    mapHUDEmissive.traverseLR(sorted_L1, (void*)(uintptr_t)context_id);

    if (clear)
        mapHUDEmissive.clear();

    if (renderHUD)
        mapHUDSorted.traverseRL(sorted_L1, (void*)(uintptr_t)context_id);

    RImplementation.rmNormal(cmd_list);

    // Restore projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

void R_dsgraph_structure::render_water_ssr() { mapWater.traverseLR(water_node_ssr, (void*)(uintptr_t)context_id); }

void R_dsgraph_structure::render_water()
{
    mapWater.traverseLR(water_node, (void*)(uintptr_t)context_id);
    mapWater.clear();
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_wmarks()
{
    // Sorted (back to front)
    mapWmark.traverseLR(sorted_L1, (void*)(uintptr_t)context_id);
    mapWmark.clear();
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_distort()
{
    // Sorted (back to front)
    mapDistort.traverseRL(sorted_L1, (void*)(uintptr_t)context_id);
    mapDistort.clear();
}

static void __fastcall pLandscape_0(mapLandscape_Node* N, void* arg)
{
    auto& dsgraph = RImplementation.get_context((uintptr_t)arg);

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());
    dsgraph.cmd_list.set_Element(N->val.se, 0);
    float LOD = calcLOD(N->val.ssa, V->vis.sphere.R);
    dsgraph.cmd_list.LOD.set_LOD(LOD);
    V->Render(dsgraph.cmd_list, LOD, dsgraph.phase == CRender::PHASE_SMAP);
}

static void __fastcall pLandscape_1(mapLandscape_Node* N, void* arg)
{
    auto& dsgraph = RImplementation.get_context((uintptr_t)arg);

    VERIFY(N);
    dxRender_Visual* V = N->val.pVisual;
    VERIFY(V && V->shader._get());
    dsgraph.cmd_list.set_Element(N->val.se, 1);
    dsgraph.cmd_list.apply_lmaterial();
    float LOD = calcLOD(N->val.ssa, V->vis.sphere.R);
    dsgraph.cmd_list.LOD.set_LOD(LOD);
    V->Render(dsgraph.cmd_list, LOD, dsgraph.phase == CRender::PHASE_SMAP);
}

void R_dsgraph_structure::render_landscape(u32 pass, bool _clear)
{
    cmd_list.set_xform_world(Fidentity);

    if (pass == 0)
        mapLandscape.traverseLR(pLandscape_0, (void*)(uintptr_t)context_id);
    else
        mapLandscape.traverseLR(pLandscape_1, (void*)(uintptr_t)context_id);

    if (_clear)
        mapLandscape.clear();
}
