#include "stdafx.h"

#include "../../xr_3da/render.h"
#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../../xr_3da/CustomHUD.h"

#include "FBasicVisual.h"

using namespace R_dsgraph;

extern float r_ssaGLOD_start, r_ssaGLOD_end;

static ICF float calcLOD(float ssa /*fDistSq*/, float /*R*/) { return _sqrt(clampr((ssa - r_ssaGLOD_end) / (r_ssaGLOD_start - r_ssaGLOD_end), 0.f, 1.f)); }

// ALPHA
void __fastcall R_dsgraph_structure::sorted_L1(float key, _MatrixItemS& val)
{
    dxRender_Visual* V = val.pVisual;
    VERIFY(V && V->shader._get());

    cmd_list.set_Element(val.se);
    cmd_list.set_xform_world(val.Matrix);
    cmd_list.apply_lmaterial(val.pObject);

    const float LOD = calcLOD(key, V->vis.sphere.R);
    cmd_list.LOD.set_LOD(LOD);
    V->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
}

void __fastcall R_dsgraph_structure::water_node_ssr(float key, _MatrixItemS& val)
{
    dxRender_Visual* V = val.pVisual;
    VERIFY(V);

    cmd_list.set_Shader(RImplementation.Target->s_ssfx_water_ssr);
    cmd_list.set_xform_world(val.Matrix);
    cmd_list.apply_lmaterial(val.pObject);

    cmd_list.set_c("cam_pos", RImplementation.Target->Position_previous.x, RImplementation.Target->Position_previous.y, RImplementation.Target->Position_previous.z, 0.0f);

    // Previous matrix data
    cmd_list.set_c("m_current", RImplementation.Target->Matrix_current);
    cmd_list.set_c("m_previous", RImplementation.Target->Matrix_previous);

    V->Render(cmd_list, calcLOD(key, V->vis.sphere.R), phase == CRender::PHASE_SMAP);
}

void __fastcall R_dsgraph_structure::water_node(float key, _MatrixItemS& val)
{
    dxRender_Visual* V = val.pVisual;
    VERIFY(V);

    if (RImplementation.o.ssfx_water)
        cmd_list.set_Shader(RImplementation.Target->s_ssfx_water);

    cmd_list.set_xform_world(val.Matrix);
    cmd_list.apply_lmaterial(val.pObject);

    // Wind settings
    float WindDir = g_pGamePersistent->Environment().CurrentEnv->wind_direction;
    float WindVel = g_pGamePersistent->Environment().CurrentEnv->wind_velocity;
    cmd_list.set_c("wind_setup", WindDir, WindVel, 0.f, 0.f);

    V->Render(cmd_list, calcLOD(key, V->vis.sphere.R), phase == CRender::PHASE_SMAP);
}

void __fastcall R_dsgraph_structure::hud_node(float key, _MatrixItemS& val)
{
    dxRender_Visual* V = val.pVisual;
    VERIFY(V && V->shader._get());
    cmd_list.set_xform_world(val.Matrix);

    if (val.se->passes[0]->ps->hud_disabled)
        return;

    int skinning = val.se->passes[0]->vs->skinning;
    cmd_list.set_Shader(RImplementation.Target->s_ssfx_hud[skinning]);

    RImplementation.Target->Matrix_HUD_previous.set(val.PrevMatrix);
    val.PrevMatrix.set(cmd_list.xforms.m_wvp);

    cmd_list.RVelocity = true;

    const float LOD = calcLOD(key, V->vis.sphere.R);
    cmd_list.LOD.set_LOD(LOD);
    V->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);

    cmd_list.RVelocity = false;
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

            for (auto& pm : map)
                nrmPasses.emplace(pm.second.ssa, &pm);

            for (const auto& it : nrmPasses)
            {
                cmd_list.set_Pass(it.second->first);
                cmd_list.apply_lmaterial();

                mapNormalItems& items = it.second->second;
                items.ssa = 0;

                for (const auto& item : items)
                {
                    const float LOD = calcLOD(item.first, item.second.pVisual->vis.sphere.R);
                    cmd_list.LOD.set_LOD(LOD);
                    item.second.pVisual->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
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

            for (auto& pm : map)
                matPasses.emplace(pm.second.ssa, &pm);

            for (const auto& it : matPasses)
            {
                cmd_list.set_Pass(it.second->first);

                mapMatrixItems& items = it.second->second;
                items.ssa = 0;

                for (const auto& item : items)
                {
                    cmd_list.set_xform_world(item.second.Matrix);
                    cmd_list.apply_lmaterial(item.second.pObject);

                    const float LOD = calcLOD(item.first, item.second.pVisual->vis.sphere.R);
                    cmd_list.LOD.set_LOD(LOD);
                    // --#SM+#-- Обновляем шейдерные данные модели [update shader values for this model]
                    // cmd_list.hemi.c_update(item.second.pVisual);

                    item.second.pVisual->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
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
    PIX_EVENT_CTX(cmd_list, r_dsgraph_render_hud);

    if ((!NoPS && mapHUD.empty()) || (NoPS && HUDMask.empty()))
        return;

    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);
    if (!NoPS)
    {
        for (auto& elem : mapHUD)
            sorted_L1(elem.first, elem.second);

        mapHUD.clear();
    }
    else
    {
        for (auto& elem : HUDMask)
            hud_node(elem.first, elem.second);

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
    for (auto& elem : mapSorted)
        sorted_L1(elem.first, elem.second);

    mapSorted.clear();

    if (mapHUDSorted.empty())
        return;

    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);

    for (auto& elem : mapHUDSorted)
        sorted_L1(elem.first, elem.second);

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
    for (auto& elem : mapEmissive)
        sorted_L1(elem.first, elem.second);

    if (clear)
        mapEmissive.clear();

    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);

    // Sorted (back to front)
    for (auto& elem : mapHUDEmissive)
        sorted_L1(elem.first, elem.second);

    if (clear)
        mapHUDEmissive.clear();

    if (renderHUD)
    {
        for (auto& elem : mapHUDSorted)
            sorted_L1(elem.first, elem.second);
    }

    RImplementation.rmNormal(cmd_list);

    // Restore projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

void R_dsgraph_structure::render_water_ssr()
{
    for (auto& elem : mapWater)
        water_node_ssr(elem.first, elem.second);
}

void R_dsgraph_structure::render_water()
{
    for (auto& elem : mapWater)
        water_node(elem.first, elem.second);

    mapWater.clear();
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_wmarks()
{
    // Sorted (back to front)
    for (auto& elem : mapWmark)
        sorted_L1(elem.first, elem.second);

    mapWmark.clear();
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::render_distort()
{
    // Sorted (back to front)
    for (auto& elem : mapDistort)
        sorted_L1(elem.first, elem.second);

    mapDistort.clear();

    if (mapHUDDistort.empty())
        return;

    // Change projection
    cmd_list.set_xform_view(Device.mViewHud);
    cmd_list.set_xform_project(Device.mProjectHud);

    // Rendering
    RImplementation.rmNear(cmd_list);

    for (auto& elem : mapHUDDistort)
        sorted_L1(elem.first, elem.second);

    mapHUDDistort.clear();
    RImplementation.rmNormal(cmd_list);

    // Restore projection
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

void __fastcall R_dsgraph_structure::pLandscape_0(float key, _MatrixItemS& val)
{
    dxRender_Visual* V = val.pVisual;
    VERIFY(V && V->shader._get());

    cmd_list.set_Element(val.se, 0);

    float LOD = calcLOD(val.ssa, V->vis.sphere.R);
    cmd_list.LOD.set_LOD(LOD);
    V->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
}

void __fastcall R_dsgraph_structure::pLandscape_1(float key, _MatrixItemS& val)
{
    dxRender_Visual* V = val.pVisual;
    VERIFY(V && V->shader._get());

    cmd_list.set_Element(val.se, 1);
    cmd_list.apply_lmaterial();

    float LOD = calcLOD(val.ssa, V->vis.sphere.R);
    cmd_list.LOD.set_LOD(LOD);
    V->Render(cmd_list, LOD, phase == CRender::PHASE_SMAP);
}

void R_dsgraph_structure::render_landscape(u32 pass, bool _clear)
{
    cmd_list.set_xform_world(Fidentity);

    if (pass == 0)
    {
        for (auto& elem : mapLandscape)
            pLandscape_0(elem.first, elem.second);
    }
    else
    {
        for (auto& elem : mapLandscape)
            pLandscape_1(elem.first, elem.second);
    }

    if (_clear)
        mapLandscape.clear();
}
