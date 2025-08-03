#pragma once

#include "../../xr_3da/render.h"
#include "../../xrcdb/ispatial.h"
#include "r__dsgraph_types.h"
#include "r__sector.h"

// feedback for receiving visuals
class XR_NOVTABLE R_feedback
{
public:
    virtual ~R_feedback() = 0;

    virtual void rfeedback_static(dxRender_Visual* V) = 0;
};

inline R_feedback::~R_feedback() = default;

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

    R_dsgraph::map_sorted mapSorted;
    R_dsgraph::map_item mapHUD;
    R_dsgraph::map_lod mapLOD;
    R_dsgraph::map_sorted mapDistort;
    R_dsgraph::map_sorted mapHUDDistort;
    R_dsgraph::map_sorted mapHUDSorted;
    R_dsgraph::map_item mapLandscape;
    R_dsgraph::map_item HUDMask;
    R_dsgraph::map_item mapWater;

    R_dsgraph::map_item mapWmark; // sorted
    R_dsgraph::map_item mapEmissive;
    R_dsgraph::map_item mapHUDEmissive;

    xr_vector<CSector*> Sectors;
    xr_vector<CPortal*> Portals;
    CPortalTraverser PortalTraverser;
    xrXRC Sectors_xrc;

    // Runtime structures
    xr_multimap<float, R_dsgraph::mapNormal_T::value_type*, std::greater<float>> nrmPasses;
    xr_multimap<float, R_dsgraph::mapMatrix_T::value_type*, std::greater<float>> matPasses;
    xr_vector<u32> lstLODgroups;
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

        lstLODgroups.clear();
        lstRenderables.clear();
        lstSpatial.clear();
        lstVisuals.clear();

        for (int i = 0; i < SHADER_PASSES_MAX; ++i)
        {
            mapNormalPasses[0][i].clear();
            mapNormalPasses[1][i].clear();
            mapMatrixPasses[0][i].clear();
            mapMatrixPasses[1][i].clear();
        }

        mapSorted.clear();
        mapHUD.clear();
        mapLOD.clear();
        mapDistort.clear();
        mapHUDDistort.clear();
        mapHUDSorted.clear();
        mapLandscape.clear();
        HUDMask.clear();
        mapWater.clear();

        mapWmark.clear();
        mapEmissive.clear();
        mapHUDEmissive.clear();

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

private:
    // render primitives
    void __fastcall sorted_L1(float key, R_dsgraph::_MatrixItemS& val);
    void __fastcall water_node_ssr(float key, R_dsgraph::_MatrixItemS& val);
    void __fastcall water_node(float key, R_dsgraph::_MatrixItemS& val);
    void __fastcall hud_node(float key, R_dsgraph::_MatrixItemS& val);
    void __fastcall pLandscape_0(float key, R_dsgraph::_MatrixItemS& val);
    void __fastcall pLandscape_1(float key, R_dsgraph::_MatrixItemS& val);

public:
    void render_graph(u32 _priority);
    void render_hud(bool NoPS = false);
    void render_hud_ui();
    void render_lods();
    void render_sorted();
    void render_emissive(bool clear = true, bool renderHUD = false);
    void render_wmarks();
    void render_distort();
    void render_landscape(u32 pass, bool _clear);
    void render_water_ssr();
    void render_water();
};
