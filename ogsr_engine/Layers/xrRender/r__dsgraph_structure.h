#pragma once

#include "../../xr_3da/render.h"
#include "../../xrcdb/ispatial.h"
#include "r__dsgraph_types.h"
#include "r__sector.h"

// feedback for receiving visuals
class R_feedback
{
public:
    virtual void rfeedback_static(dxRender_Visual* V) = 0;
};

struct R_dsgraph_structure
{
    R_feedback* val_feedback{}; // feedback for geometry being rendered
    u32 val_feedback_breakp{}; // breakpoint

    ctx_id_t context_id{R__INVALID_CTX_ID};
    u32 phase{};
    u32 marker{};
    bool pmask[2];
    bool pmask_wmark;
    bool use_hom{};

    // Dynamic scene graph
    R_dsgraph::mapNormalPasses_T mapNormalPasses[2]; // 2==(priority/2)
    R_dsgraph::mapMatrixPasses_T mapMatrixPasses[2];
    R_dsgraph::mapSorted_T mapSorted;
    R_dsgraph::mapHUD_T mapHUD;
    R_dsgraph::mapLOD_T mapLOD;
    R_dsgraph::mapSorted_T mapDistort;
    R_dsgraph::mapHUD_T mapHUDSorted;
    R_dsgraph::mapLandscape_T mapLandscape;
    R_dsgraph::HUDMask_T HUDMask;
    R_dsgraph::mapWater_T mapWater;

    R_dsgraph::mapSorted_T mapWmark; // sorted
    R_dsgraph::mapSorted_T mapEmissive;
    R_dsgraph::mapSorted_T mapHUDEmissive;

    xr_vector<CSector*> Sectors;
    xr_vector<CPortal*> Portals;
    CPortalTraverser PortalTraverser;
    xrXRC Sectors_xrc;

    // Runtime structures
    xr_vector<R_dsgraph::mapNormal_T::value_type*> nrmPasses;
    xr_vector<R_dsgraph::mapMatrix_T::value_type*> matPasses;
    xr_vector<R_dsgraph::_LodItem> lstLODs;
    xr_vector<int> lstLODgroups;
    xr_vector<ISpatial*> lstRenderables;
    xr_vector<ISpatial*> lstSpatial;
    xr_vector<dxRender_Visual*> lstVisuals;

    u32 counter_S{};
    u32 counter_D{};

    CBackend cmd_list{};

    void set_Feedback(R_feedback* V, u32 id)
    {
        val_feedback_breakp = id;
        val_feedback = V;
    }
    void get_Counters(u32& s, u32& d)
    {
        s = counter_S;
        d = counter_D;
    }
    void clear_Counters() { counter_S = counter_D = 0; }

    R_dsgraph_structure() { r_pmask(true, true); };

    void reset()
    {
        context_id = R__INVALID_CTX_ID;
        val_feedback = nullptr;

        nrmPasses.clear();
        matPasses.clear();

        lstLODs.clear();
        lstLODgroups.clear();
        lstRenderables.clear();
        lstSpatial.clear();
        lstVisuals.clear();

        for (int i = 0; i < SHADER_PASSES_MAX; ++i)
        {
            mapNormalPasses[0][i].destroy();
            mapNormalPasses[1][i].destroy();
            mapMatrixPasses[0][i].destroy();
            mapMatrixPasses[1][i].destroy();
        }
        mapSorted.destroy();
        mapHUD.destroy();
        mapLOD.destroy();
        mapDistort.destroy();
        mapHUDSorted.destroy();
        mapLandscape.destroy();
        HUDMask.destroy();
        mapWater.destroy();

        mapWmark.destroy();
        mapEmissive.destroy();
        mapHUDEmissive.destroy();

        cmd_list.Invalidate();
    }

    void r_pmask(bool _1, bool _2, bool _wm = false)
    {
        pmask[0] = _1;
        pmask[1] = _2;
        pmask_wmark = _wm;
    }

    void load(const xr_vector<CSector::level_sector_data_t>& sectors, const xr_vector<CPortal::level_portal_data_t>& portals);
    void unload();

    ICF CSector* get_sector(sector_id_t sector_id) const
    {
        VERIFY(sector_id < Sectors.size());
        return Sectors[sector_id];
    }
    sector_id_t detect_sector(const Fvector& P);
    sector_id_t detect_sector(const Fvector& P, Fvector& D);

    void add_static(dxRender_Visual* pVisual, const CFrustum& view, u32 planes);
    void add_leafs_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, bool ignore = false); // if detected node's full visibility
    void add_leafs_static(dxRender_Visual* pVisual); // if detected node's full visibility

    void insert_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, Fvector& Center);
    void insert_static(dxRender_Visual* pVisual);

    void build_subspace(sector_id_t o_sector_id, CFrustum& _frustum);
    void build_subspace(sector_id_t o_sector_id, CFrustum& _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic);

    // render primitives
    void render_graph(u32 _priority);
    void render_hud(bool NoPS = false);
    void render_hud_ui();
    void render_lods(bool _setup_zb, bool _clear);
    void render_sorted();
    void render_emissive(bool clear = true, bool renderHUD = false);
    void render_wmarks();
    void render_distort();
    void render_landscape(u32 pass, bool _clear);
    void render_water_ssr();
    void render_water();
};
