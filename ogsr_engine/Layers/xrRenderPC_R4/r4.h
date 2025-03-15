#pragma once

#include "../xrRender/dxRenderDeviceRender.h"
#include "../xrRender/r__dsgraph_structure.h"
#include "../xrRender/r__occlusion.h"

#include "../xrRender/PSLibrary.h"

#include "r2_types.h"
#include "r4_rendertarget.h"

#include "../xrRender/hom.h"
#include "../xrRender/detailmanager.h"
#include "../xrRender/modelpool.h"
#include "../xrRender/wallmarksengine.h"

#include "smap_allocator.h"
#include "../xrRender/light_db.h"
#include "../xrRender/light_render_direct.h"
#include "../xrRender/LightTrack.h"
#include "../xrRender/r_sun_cascades.h"

#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/fmesh.h"

class dxRender_Visual;

// definition
class CRender : public IRender_interface, public dxRenderDeviceRender
{
public:
    enum
    {
        PHASE_NORMAL = 0, // E[0]
        PHASE_SMAP = 1, // E[1]
    };

public:
    R_dsgraph_structure dsgraph;

    struct _options
    {
        static constexpr bool ssfx_branches = true;
        static constexpr bool ssfx_blood = true;
        static constexpr bool ssfx_rain = true;
        static constexpr bool ssfx_hud_raindrops = true;
        static constexpr bool ssfx_ssr = true;
        static constexpr bool ssfx_terrain = true;
        static constexpr bool ssfx_volumetric = true;
        static constexpr bool ssfx_water = true;
        static constexpr bool ssfx_ao = true;
        static constexpr bool ssfx_il = true;
        static constexpr bool ssfx_core = true;
        static constexpr bool ssfx_bloom = true;
        static constexpr bool ssfx_sss = true;

        u32 HW_smap_FORMAT : 32;
        u32 smapsize : 16;

        u32 distortion : 1;
        u32 disasm : 1;

        u32 dx10_msaa : 1; //	DX10.0 path
        u32 dx10_msaa_samples : 4;

        u32 dx11_enable_tessellation : 1;
    } o;
    struct _stats
    {
        u32 l_total, l_visible;
        u32 l_shadowed, l_unshadowed;
        s32 s_used, s_merged, s_finalclip;
        u32 o_queries, o_culled;
        u32 ic_total, ic_culled;
    } stats;

    bool is_sun();

    // Sector detection and visibility
    sector_id_t last_sector_id{INVALID_SECTOR_ID};
    Fvector vLastCameraPos;
    u32 uLastLTRACK;
    CDB::MODEL* rmPortals;
    CHOM HOM;
    R_occlusion HWOCC;

    // Global vertex-buffer container
    xr_vector<FSlideWindowItem> SWIs;
    xr_vector<ref_shader> Shaders;
    typedef svector<D3DVERTEXELEMENT9, MAXD3DDECLLENGTH + 1> VertexDeclarator;
    xr_vector<VertexDeclarator> nDC, xDC;
    xr_vector<ID3DVertexBuffer*> nVB, xVB;
    xr_vector<ID3DIndexBuffer*> nIB, xIB;
    xr_vector<dxRender_Visual*> Visuals;
    CPSLibrary PSLibrary;

    CDetailManager* Details;
    CModelPool* Models;
    CWallmarksEngine* Wallmarks;

    CRenderTarget* Target; // Render-target

    CLight_DB Lights;
    CLight_Compute_XFORM_and_VIS LR;
    xr_vector<light*> Lights_LastFrame;
    SMAP_Allocator LP_smap_pool;
    light_Package LP_normal;

    bool m_bFirstFrameAfterReset; // Determines weather the frame is the first after resetting device.

    xr_vector<sun::cascade> m_sun_cascades;

    bool need_to_render_sunshafts{false};
    bool last_cascade_chain_mode{false};

private:
    // Loading / Unloading
    void LoadBuffers(CStreamReader* fs, BOOL _alternative);
    void LoadVisuals(IReader* fs);
    void LoadLights(IReader* fs);
    void LoadSectors(IReader* fs);
    void LoadSWIs(CStreamReader* fs);
    void Load3DFluid();

public:
    void render_forward();
    void render_lights(light_Package& LP);
    void render_menu();
    void render_rain();

    void init_cacades();
    void render_sun_cascades();
    void render_sun_cascade(u32 cascade_ind);

private:
    void calculate_sun_async();

    xr_task_group* tg{};
    void calculate_sun(sun::cascade& cascade);

    sector_id_t largest_sector_id{INVALID_SECTOR_ID};

public:
    ShaderElement* rimp_select_sh_static(dxRender_Visual* pVisual, float cdist_sq, u32 phase);
    ShaderElement* rimp_select_sh_dynamic(IRenderable* root, dxRender_Visual* pVisual, float cdist_sq, u32 phase);

    D3DVERTEXELEMENT9* getVB_Format(int id, BOOL _alt = FALSE);
    ID3DVertexBuffer* getVB(int id, BOOL _alt = FALSE);
    ID3DIndexBuffer* getIB(int id, BOOL _alt = FALSE);
    FSlideWindowItem* getSWI(int id);
    IRenderVisual* model_CreatePE(LPCSTR name);

    // HW-occlusion culling
    IC u32 occq_begin(u32& ID) { return HWOCC.occq_begin(ID); }
    IC void occq_end(u32& ID) { HWOCC.occq_end(ID); }
    IC R_occlusion::occq_result occq_get(u32& ID) { return HWOCC.occq_get(ID); }
    IC void occq_free(u32 ID) { HWOCC.occq_free(ID); }

    // Loading / Unloading
    virtual void create();
    virtual void destroy();
    virtual void reset_begin();
    virtual void reset_end();

    virtual void level_Load(IReader*);
    virtual void level_Unload();

    ID3DBaseTexture* texture_load(LPCSTR fname, u32& msize);
    virtual HRESULT shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result);

    // Information
    virtual void Statistics(CGameFont* F);
    virtual LPCSTR getShaderPath() { return ""; }
    virtual ref_shader getShader(int id);
    virtual IRenderVisual* getVisual(int id);
    IRender_Target* getTarget() override { return Target; }

    // Main
    void add_Visual(u32 context_id, IRenderable* root, IRenderVisual* V, Fmatrix& m) override;

    // wallmarks
    virtual void add_StaticWallmark(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    virtual void add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    virtual void add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    virtual void clear_static_wallmarks();
    virtual void add_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm);
    virtual void add_SkeletonWallmark(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size);
    virtual void add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector& start, const Fvector& dir, float size);

    //
    virtual IBlenderXr* blender_create(CLASS_ID cls);
    virtual void blender_destroy(IBlenderXr*&);

    //
    virtual IRender_ObjectSpecific* ros_create(IRenderable* parent);
    virtual void ros_destroy(IRender_ObjectSpecific*&);

    // Lighting
    virtual IRender_Light* light_create();
    virtual IRender_Glow* glow_create();

    // Models
    virtual IRenderVisual* model_CreateParticles(LPCSTR name);
    virtual IRender_DetailModel* model_CreateDM(IReader* F);
    virtual IRenderVisual* model_Create(LPCSTR name, IReader* data = 0);
    virtual IRenderVisual* model_CreateChild(LPCSTR name, IReader* data);
    virtual IRenderVisual* model_Duplicate(IRenderVisual* V);
    virtual void model_Delete(IRenderVisual*& V, BOOL bDiscard);
    virtual void model_Delete(IRender_DetailModel*& F);
    virtual void model_Logging(BOOL bEnable) { Models->Logging(bEnable); }
    virtual void models_Prefetch();
    virtual void models_Clear(BOOL b_complete);
    virtual void models_savePrefetch();
    virtual void models_begin_prefetch1(bool val);

    // Occlusion culling
    virtual BOOL occ_visible(vis_data& V);
    virtual BOOL occ_visible(Fbox& B);
    virtual BOOL occ_visible(sPoly& P);

    // Main
    void OnCameraUpdated() override;
    virtual void Calculate();
    virtual void Render();
    virtual void Screenshot(ScreenshotMode mode = SM_NORMAL, LPCSTR name = 0);
    virtual void OnFrame();
    virtual void BeforeWorldRender(); //--#SM+#-- +SecondVP+ Вызывается перед началом рендера мира и пост-эффектов
    virtual void AfterWorldRender(const bool save_bb_before_ui); //--#SM+#-- +SecondVP+ Вызывается после рендера мира и перед UI
    void AfterUIRender() override; // После рендеринга UI. Вызывать только если нам нужно отрендерить кадр для пда.

    // Render mode
    virtual void rmNear();
    virtual void rmFar();
    virtual void rmNormal();

    u32 active_phase() const override { return dsgraph.phase; }

    // Constructor/destructor/loader
    CRender();
    virtual ~CRender();

    void addShaderOption(const char* name, const char* value);
    void clearAllShaderOptions() { m_ShaderOptions.clear(); }

private:
    xr_vector<D3D_SHADER_MACRO> m_ShaderOptions;

protected:
    virtual void ScreenshotImpl(ScreenshotMode mode, LPCSTR name, CMemoryWriter* memory_writer);
};

void fix_texture_name(const char* fn);

extern CRender RImplementation;
