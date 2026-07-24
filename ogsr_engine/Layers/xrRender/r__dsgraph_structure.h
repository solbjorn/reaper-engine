#pragma once

#include "../../xr_3da/Render.h"
#include "../../xrCDB/ISpatial.h"

#include "r__dsgraph_types.h"
#include "r__sector.h"

// feedback for receiving visuals
class XR_NOVTABLE R_feedback : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(R_feedback);

public:
    ~R_feedback() override = 0;

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

private:
    R_dsgraph::map_item mapHUD;
    R_dsgraph::map_lod mapLOD;

public:
    R_dsgraph::map_sorted mapDistort;
    R_dsgraph::map_sorted mapHUDDistort;
    R_dsgraph::map_sorted mapHUDSorted;
    R_dsgraph::map_item mapLandscape;
    R_dsgraph::map_item HUDMask;
    R_dsgraph::map_item mapWater;

    R_dsgraph::map_item mapWmark; // sorted
    R_dsgraph::map_item mapEmissive;
    R_dsgraph::map_item mapHUDEmissive;

    xr_vector<std::unique_ptr<CSector>> Sectors;
    xr_vector<std::unique_ptr<CPortal>> Portals;

private:
    CPortalTraverser PortalTraverser;
    xrXRC Sectors_xrc;

    // Runtime structures
    xr_multimap<float, R_dsgraph::mapNormal_T::value_type*, std::greater<float>> nrmPasses;
    xr_multimap<float, R_dsgraph::mapMatrix_T::value_type*, std::greater<float>> matPasses;
    xr_vector<u32> lstLODgroups;
    xr_vector<ISpatial*> lstRenderables;

    u32 counter_S{};
    u32 counter_D{};

public:
    CBackend cmd_list{};

    constexpr R_dsgraph_structure() { r_pmask(true, true); }

    constexpr void r_pmask(bool _1, bool _2, bool _wm = false)
    {
        pmask[0] = _1;
        pmask[1] = _2;
        pmask_wmark = _wm;
    }

    constexpr void set_Feedback(R_feedback* V, u32 id)
    {
        val_feedback = V;
        val_feedback_breakp = id;
    }

    constexpr void get_Counters(u32& s, u32& d) const
    {
        s = counter_S;
        d = counter_D;
    }

    constexpr void clear_Counters()
    {
        counter_S = 0;
        counter_D = 0;
    }

    void reset();
    void load(std::span<const CSector::level_sector_data_t> sectors_data, std::span<const CPortal::level_portal_data_t> portals_data);
    void unload();

    [[nodiscard]] ICF auto get_sector(sector_id_t sector_id) const { return Sectors[sector_id].get(); }

    [[nodiscard]] sector_id_t detect_sector(const Fvector& P);
    [[nodiscard]] sector_id_t detect_sector(const Fvector& P, Fvector& D);

    void add_static(dxRender_Visual* pVisual, const CFrustum& view, u32 planes);
    void add_leafs_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, bool ignore = false); // if detected node's full visibility
    void add_leafs_static(dxRender_Visual* pVisual); // if detected node's full visibility

    void insert_dynamic(IRenderable* root, dxRender_Visual* pVisual, Fmatrix& xform, Fvector& Center);
    void insert_static(dxRender_Visual* pVisual);

    tmc::task<void> build_subspace(sector_id_t o_sector_id, CFrustum& _frustum);
    void build_subspace(sector_id_t o_sector_id, CFrustum& _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic);

private:
    // render primitives
    void sorted_L1(float key, R_dsgraph::_MatrixItemS& val);
    void water_node_ssr(float key, R_dsgraph::_MatrixItemS& val);
    void water_node(float key, R_dsgraph::_MatrixItemS& val);
    void hud_node(float key, R_dsgraph::_MatrixItemS& val);
    void pLandscape_0(R_dsgraph::_MatrixItemS& val);
    void pLandscape_1(R_dsgraph::_MatrixItemS& val);

    void render_sorted();

public:
    void render_graph(u32 _priority);
    void render_hud(bool NoPS = false);
    void render_hud_ui();
    void render_lods();
    void render_emissive(bool clear = true, bool renderHUD = false);
    void render_wmarks();
    void render_distort();
    void render_landscape(u32 pass, bool _clear);
    void render_water_ssr();
    void render_water();
    void render_forward();
};

// r__dsgraph_build.cpp
extern float r_ssaDISCARD;
extern float r_ssaGLOD_end;
extern float r_ssaGLOD_start;
extern float r_ssaLOD_A;
extern float r_ssaLOD_B;
