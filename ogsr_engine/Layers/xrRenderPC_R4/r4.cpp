#include "stdafx.h"
#include "r4.h"
#include "../xrRender/fbasicvisual.h"
#include "../../xr_3da/xr_object.h"
#include "../../xr_3da/CustomHUD.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../xrRender/SkeletonCustom.h"
#include "../xrRender/LightTrack.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include "../xrRender/dxWallMarkArray.h"
#include "../xrRender/dxUIShader.h"

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"
#include "../xrRender/ShaderResourceTraits.h"

#include <xxhash.h>

CRender RImplementation;

//////////////////////////////////////////////////////////////////////////
class CGlow : public IRender_Glow
{
public:
    bool bActive;

public:
    CGlow() : bActive(false) {}
    virtual void set_active(bool b) { bActive = b; }
    virtual bool get_active() { return bActive; }
    virtual void set_position(const Fvector& P) {}
    virtual void set_direction(const Fvector& D) {}
    virtual void set_radius(float R) {}
    virtual void set_texture(LPCSTR name) {}
    virtual void set_color(const Fcolor& C) {}
    virtual void set_color(float r, float g, float b) {}
};

bool CRender::is_sun()
{
    Fcolor sun_color = ((light*)Lights.sun_adapted._get())->color;
    return (ps_r2_ls_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r, sun_color.g, sun_color.b) > EPS));
}

float r_dtex_range = 50.f;
//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_dynamic(dxRender_Visual* pVisual, float cdist_sq)
{
    int id = SE_R2_SHADOW;
    if (CRender::PHASE_NORMAL == RImplementation.phase)
    {
        id = (RImplementation.val_bHUD || ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range)) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
    }
    return pVisual->shader->E[id]._get();
}

//////////////////////////////////////////////////////////////////////////
ShaderElement* CRender::rimp_select_sh_static(dxRender_Visual* pVisual, float cdist_sq)
{
    int id = SE_R2_SHADOW;
    if (CRender::PHASE_NORMAL == RImplementation.phase)
    {
        if (pVisual->shader->E[0]->flags.isLandscape)
        {
            float sec_dist = _sqrt(cdist_sq) - pVisual->vis.sphere.R;
            id = (sec_dist < ps_ssfx_terrain_quality.x * 10) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;

            // Very low shader variation
            if (sec_dist > 240)
                id = 3;
        }
        else
        {
            id = ((_sqrt(cdist_sq) - pVisual->vis.sphere.R) < r_dtex_range) ? SE_R2_NORMAL_HQ : SE_R2_NORMAL_LQ;
        }
    }
    return pVisual->shader->E[id]._get();
}
static class cl_parallax : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        float h = ps_r2_df_parallax_h;
        RCache.set_c(C, h, -h / 2.f, 1.f / r_dtex_range, 1.f / r_dtex_range);
    }
} binder_parallax;

static class cl_LOD : public R_constant_setup
{
    virtual void setup(R_constant* C) { RCache.LOD.set_LOD(C); }
} binder_LOD;

static class cl_pos_decompress_params : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        const float VertTan = -1.0f * tanf(deg2rad(Device.fFOV / 2.0f));
        const float HorzTan = -VertTan / Device.fASPECT;

        RCache.set_c(C, HorzTan, VertTan, (2.0f * HorzTan) / (float)Device.dwWidth, (2.0f * VertTan) / (float)Device.dwHeight);
    }
} binder_pos_decompress_params;

static class cl_water_intensity : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        float fValue = E.m_fWaterIntensity;
        RCache.set_c(C, fValue, fValue, fValue, 0.f);
    }
} binder_water_intensity;

static class cl_sun_shafts_intensity : public R_constant_setup
{
    virtual void setup(R_constant* C)
    {
        const CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        const float fValue = E.m_fSunShaftsIntensity;
        RCache.set_c(C, fValue, fValue, fValue, 0.f);
    }
} binder_sun_shafts_intensity;

static class cl_alpha_ref : public R_constant_setup
{
    virtual void setup(R_constant* C) { StateManager.BindAlphaRef(C); }
} binder_alpha_ref;

//////////////////////////////////////////////////////////////////////////
// Just two static storage
void CRender::create()
{
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 0x12345678);

    m_skinning = -1;
    m_MSAASample = -1;

    // Supported OS versions: Windows 10 or newer
    // https://www.codeproject.com/Articles/5336372/Windows-Version-Detection
    auto sharedUserData = (BYTE*)0x7ffe0000;
    R_ASSERT(*(ULONG*)(sharedUserData + 0x26c) >= 10);

    // hardware
    constexpr const char* hwerr = "Hardware doesn't meet minimum feature-level";
    R_ASSERT(HW.Caps.raster.dwMRT_count >= 3, hwerr);
    R_ASSERT(HW.Caps.raster.b_MRT_mixdepth, hwerr);
    R_ASSERT(HW.Caps.raster.dwInstructions >= 256, hwerr);
    R_ASSERT(HW.Caps.raster_major >= 3, hwerr);
    R_ASSERT(HW.Caps.geometry_major >= 1, hwerr);
    R_ASSERT(HW.Caps.geometry.bVTF, hwerr);
    R_ASSERT(HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0, hwerr);

    // For ATI it's much faster on DX10 to use D32F format
    if (HW.Caps.id_vendor == 0x1002)
        o.HW_smap_FORMAT = D3DFMT_D32F_LOCKABLE;
    else
        o.HW_smap_FORMAT = D3DFMT_D24X8;

    // options (smap-pool-size)
    o.smapsize = r2_SmapSize;

    // options
    o.distortion = true;
    o.disasm = (strstr(Core.Params, "-disasm")) ? TRUE : FALSE;

    //	MSAA option dependencies

    o.dx10_msaa = !!ps_r3_msaa;
    o.dx10_msaa_samples = (1 << ps_r3_msaa);

    o.dx11_enable_tessellation = ps_r2_ls_flags_ext.test(R2FLAGEXT_ENABLE_TESSELLATION);

    // constants
    CResourceManager* RM = dxRenderDeviceRender::Instance().Resources;
    RM->RegisterConstantSetup("parallax", &binder_parallax);
    RM->RegisterConstantSetup("water_intensity", &binder_water_intensity);
    RM->RegisterConstantSetup("sun_shafts_intensity", &binder_sun_shafts_intensity);
    RM->RegisterConstantSetup("m_AlphaRef", &binder_alpha_ref);
    RM->RegisterConstantSetup("pos_decompression_params", &binder_pos_decompress_params);
    RM->RegisterConstantSetup("triLOD", &binder_LOD);

    Target = xr_new<CRenderTarget>(); // Main target

    Models = xr_new<CModelPool>();
    PSLibrary.OnCreate();

    rmNormal();
    marker = 0;

    ::PortalTraverser.initialize();

    FluidManager.Initialize(70, 70, 70);
    FluidManager.SetScreenSize(Device.dwWidth, Device.dwHeight);
}

void CRender::destroy()
{
    FluidManager.Destroy();

    ::PortalTraverser.destroy();
    /*
    for (u32 i=0; i<HW.Caps.iGPUNum; ++i)
        _RELEASE				(q_sync_point[i]);
    */

    HWOCC.occq_destroy();
    xr_delete(Models);
    xr_delete(Target);
    PSLibrary.OnDestroy();
    Device.seqFrame.Remove(this);
    r_dsgraph_destroy();
}

extern u32 reset_frame;
void CRender::reset_begin()
{
    // Update incremental shadowmap-visibility solver
    // BUG-ID: 10646
    {
        u32 it = 0;
        for (it = 0; it < Lights_LastFrame.size(); it++)
        {
            if (0 == Lights_LastFrame[it])
                continue;
            // try
            //{
            Lights_LastFrame[it]->svis.resetoccq();
            /*}
            catch (...)
            {
                Msg("! Failed to flush-OCCq on light [%d] %X", it, *(u32*)(&Lights_LastFrame[it]));
            }*/
        }
        Lights_LastFrame.clear();
    }

    reset_frame = Device.dwFrame;

    if (b_loaded /*&& ((dm_current_size != dm_size) || (ps_r__Detail_density != ps_current_detail_density))*/)
    {
        Details->Unload();
        xr_delete(Details);
    }
    //-AVO

    xr_delete(Target);
    HWOCC.occq_destroy();
    /*
    for (u32 i=0; i<HW.Caps.iGPUNum; ++i)
        _RELEASE				(q_sync_point[i]);
    */
}

void CRender::reset_end()
{
    Target = xr_new<CRenderTarget>();

    if (b_loaded /*&& ((dm_current_size != dm_size) || (ps_r__Detail_density != ps_current_detail_density))*/)
    {
        Details = xr_new<CDetailManager>();
        Details->Load();
    }
    //-AVO

    FluidManager.SetScreenSize(Device.dwWidth, Device.dwHeight);

    // Set this flag true to skip the first render frame,
    // that some data is not ready in the first frame (for example device camera position)
    m_bFirstFrameAfterReset = true;
}

void CRender::OnFrame()
{
    Models->DeleteQueue();

    bool b_main_menu_is_active = (g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive());

    if (!b_main_menu_is_active && g_pGameLevel)
    {
        if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))
        {
            if (Details)
                Details->StartAsync();

            if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM))
            {
                // MT-HOM (@front)
                Device.add_to_seq_parallel(fastdelegate::MakeDelegate(&HOM, &CHOM::MT_RENDER));
            }
        }

        if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_RAIN))
        {
            g_pGamePersistent->Environment().StartCalculateAsync();
        }

        if (Details)
            g_pGamePersistent->GrassBendersUpdateAnimations();

        calculate_sun_async();
    }
}

// Перед началом рендера мира --#SM+#-- +SecondVP+
void CRender::BeforeWorldRender() {}

// После рендера мира и пост-эффектов --#SM+#-- +SecondVP+
void CRender::AfterWorldRender(const bool save_bb_before_ui)
{
    if (save_bb_before_ui || Device.m_SecondViewport.IsSVPFrame())
    {
        // Делает копию бэкбуфера (текущего экрана) в рендер-таргет второго вьюпорта (для использования в 3д прицеле либо в рендер-таргет вьюпорта, из которого мы вернем заберем
        // кадр после рендера ui. Именно этот кадр будет позже выведен на экран.)
        ID3DTexture2D* pBuffer{};
        HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer));
        HW.pContext->CopyResource(save_bb_before_ui ? Target->rt_BeforeUi->pSurface : Target->rt_secondVP->pSurface, pBuffer);
        pBuffer->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)
    }
}

void CRender::AfterUIRender()
{
    // Делает копию бэкбуфера (текущего экрана) в рендер-таргет второго вьюпорта (для использования в пда)
    ID3DTexture2D* pBuffer{};
    HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer));
    HW.pContext->CopyResource(Target->rt_secondVP->pSurface, pBuffer);
    pBuffer->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)

    // Возвращаем на экран кадр, который сохранили до рендера ui для пда
    ID3DTexture2D* pBuffer2{};
    HW.m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBuffer2));
    HW.pContext->CopyResource(pBuffer2, Target->rt_BeforeUi->pSurface);
    pBuffer2->Release(); // Корректно очищаем ссылку на бэкбуфер (иначе игра зависнет в опциях)
}

// Implementation
IRender_ObjectSpecific* CRender::ros_create(IRenderable* parent) { return xr_new<CROS_impl>(); }
void CRender::ros_destroy(IRender_ObjectSpecific*& p) { xr_delete(p); }
IRenderVisual* CRender::model_Create(LPCSTR name, IReader* data) { return Models->Create(name, data); }
IRenderVisual* CRender::model_CreateChild(LPCSTR name, IReader* data) { return Models->CreateChild(name, data); }
IRenderVisual* CRender::model_Duplicate(IRenderVisual* V) { return Models->Instance_Duplicate((dxRender_Visual*)V); }

void CRender::model_Delete(IRenderVisual*& V, BOOL bDiscard)
{
    dxRender_Visual* pVisual = (dxRender_Visual*)V;
    Models->Delete(pVisual, bDiscard);
    V = 0;
}

IRender_DetailModel* CRender::model_CreateDM(IReader* F)
{
    CDetail* D = xr_new<CDetail>();
    D->Load(F);
    return D;
}

void CRender::model_Delete(IRender_DetailModel*& F)
{
    if (F)
    {
        CDetail* D = (CDetail*)F;
        D->Unload();
        xr_delete(D);
        F = NULL;
    }
}

IRenderVisual* CRender::model_CreatePE(LPCSTR name)
{
    PS::CPEDef* SE = PSLibrary.FindPED(name);
    R_ASSERT3(SE, "Particle effect doesn't exist", name);
    return Models->CreatePE(SE);
}

IRenderVisual* CRender::model_CreateParticles(LPCSTR name)
{
    PS::CPEDef* SE = PSLibrary.FindPED(name);
    if (SE)
        return Models->CreatePE(SE);
    else
    {
        PS::CPGDef* SG = PSLibrary.FindPGD(name);
        R_ASSERT3(SG, "Particle effect or group doesn't exist", name);
        return Models->CreatePG(SG);
    }
}

void CRender::models_Prefetch() { Models->Prefetch(); }
void CRender::models_Clear(BOOL b_complete) { Models->ClearPool(b_complete); }
void CRender::models_savePrefetch() { Models->save_vis_prefetch(); }
void CRender::models_begin_prefetch1(bool val) { Models->begin_prefetch1(val); }

ref_shader CRender::getShader(int id)
{
    VERIFY(id < int(Shaders.size()));
    return Shaders[id];
}

IRender_Portal* CRender::getPortal(int id)
{
    VERIFY(id < int(Portals.size()));
    return Portals[id];
}

IRender_Sector* CRender::getSector(int id)
{
    VERIFY(id < int(Sectors.size()));
    return Sectors[id];
}

IRender_Sector* CRender::getSectorActive() { return pLastSector; }

IRenderVisual* CRender::getVisual(int id)
{
    VERIFY(id < int(Visuals.size()));
    return Visuals[id];
}

D3DVERTEXELEMENT9* CRender::getVB_Format(int id, BOOL _alt)
{
    if (_alt)
    {
        VERIFY(id < int(xDC.size()));
        return xDC[id].begin();
    }
    else
    {
        VERIFY(id < int(nDC.size()));
        return nDC[id].begin();
    }
}

ID3DVertexBuffer* CRender::getVB(int id, BOOL _alt)
{
    if (_alt)
    {
        VERIFY(id < int(xVB.size()));
        return xVB[id];
    }
    else
    {
        VERIFY(id < int(nVB.size()));
        return nVB[id];
    }
}

ID3DIndexBuffer* CRender::getIB(int id, BOOL _alt)
{
    if (_alt)
    {
        VERIFY(id < int(xIB.size()));
        return xIB[id];
    }
    else
    {
        VERIFY(id < int(nIB.size()));
        return nIB[id];
    }
}

FSlideWindowItem* CRender::getSWI(int id)
{
    VERIFY(id < int(SWIs.size()));
    return &SWIs[id];
}

IRender_Target* CRender::getTarget() { return Target; }

IRender_Light* CRender::light_create() { return Lights.Create(); }
IRender_Glow* CRender::glow_create() { return xr_new<CGlow>(); }

void CRender::flush() { r_dsgraph_render_graph(0); }

BOOL CRender::occ_visible(vis_data& P) { return HOM.visible(P); }
BOOL CRender::occ_visible(sPoly& P) { return HOM.visible(P); }
BOOL CRender::occ_visible(Fbox& P) { return HOM.visible(P); }

void CRender::add_Visual(IRenderVisual* V) { add_leafs_Dynamic((dxRender_Visual*)V, V->_ignore_optimization); }
void CRender::add_Geometry(IRenderVisual* V) { add_Static((dxRender_Visual*)V, View->getMask()); }
void CRender::add_StaticWallmark(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* verts)
{
    if (T->suppress_wm)
        return;
    VERIFY2(_valid(P) && _valid(s) && T && verts && (s > EPS_L), "Invalid static wallmark params");
    Wallmarks->AddStaticWallmark(T, verts, P, &*S, s);
}

void CRender::add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
    dxWallMarkArray* pWMA = (dxWallMarkArray*)pArray;
    ref_shader* pShader = pWMA->dxGenerateWallmark();
    if (pShader)
        add_StaticWallmark(*pShader, P, s, T, V);
}

void CRender::add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V)
{
    dxUIShader* pShader = (dxUIShader*)&*S;
    add_StaticWallmark(pShader->hShader, P, s, T, V);
}

void CRender::clear_static_wallmarks() { Wallmarks->clear(); }

void CRender::add_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm) { Wallmarks->AddSkeletonWallmark(wm); }
void CRender::add_SkeletonWallmark(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
    Wallmarks->AddSkeletonWallmark(xf, obj, sh, start, dir, size);
}
void CRender::add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector& start, const Fvector& dir, float size)
{
    dxWallMarkArray* pWMA = (dxWallMarkArray*)pArray;
    ref_shader* pShader = pWMA->dxGenerateWallmark();
    if (pShader)
        add_SkeletonWallmark(xf, (CKinematics*)obj, *pShader, start, dir, size);
}

void CRender::add_Occluder(Fbox2& bb_screenspace) { HOM.occlude(bb_screenspace); }
void CRender::set_Object(IRenderable* O) { val_pObject = O; }

void CRender::rmNear()
{
    IRender_Target* T = getTarget();
    const D3D_VIEWPORT VP = {0, 0, (float)T->get_width(), (float)T->get_height(), 0, 0.02f};
    HW.pContext->RSSetViewports(1, &VP);
    // CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void CRender::rmFar()
{
    IRender_Target* T = getTarget();
    const D3D_VIEWPORT VP = {0, 0, (float)T->get_width(), (float)T->get_height(), 0.99999f, 1.f};
    HW.pContext->RSSetViewports(1, &VP);
    // CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void CRender::rmNormal()
{
    IRender_Target* T = getTarget();
    const D3D_VIEWPORT VP = {0, 0, (float)T->get_width(), (float)T->get_height(), 0, 1.f};
    HW.pContext->RSSetViewports(1, &VP);
    // CHK_DX				(HW.pDevice->SetViewport(&VP));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender() : m_bFirstFrameAfterReset(false) { init_cacades(); }

CRender::~CRender() {}

#include "../../xr_3da/GameFont.h"
void CRender::Statistics(CGameFont* _F)
{
    CGameFont& F = *_F;
    F.OutNext(" **** LT:%2d,LV:%2d **** ", stats.l_total, stats.l_visible);
    stats.l_visible = 0;
    F.OutNext("    S(%2d)   | (%2d)NS   ", stats.l_shadowed, stats.l_unshadowed);
    F.OutNext("smap use[%2d], merge[%2d], finalclip[%2d]", stats.s_used, stats.s_merged - stats.s_used, stats.s_finalclip);
    stats.s_used = 0;
    stats.s_merged = 0;
    stats.s_finalclip = 0;
    F.OutSkip();
    F.OutNext(" **** Occ-Q(%03.1f) **** ", 100.f * f32(stats.o_culled) / f32(stats.o_queries ? stats.o_queries : 1));
    F.OutNext(" total  : %2d", stats.o_queries);
    stats.o_queries = 0;
    F.OutNext(" culled : %2d", stats.o_culled);
    stats.o_culled = 0;
    F.OutSkip();
    u32 ict = stats.ic_total + stats.ic_culled;
    F.OutNext(" **** iCULL(%03.1f) **** ", 100.f * f32(stats.ic_culled) / f32(ict ? ict : 1));
    F.OutNext(" visible: %2d", stats.ic_total);
    stats.ic_total = 0;
    F.OutNext(" culled : %2d", stats.ic_culled);
    stats.ic_culled = 0;
#ifdef DEBUG
    HOM.stats();
#endif
}

void CRender::addShaderOption(const char* name, const char* value)
{
    D3D_SHADER_MACRO macro = {name, value};
    m_ShaderOptions.push_back(macro);
}

template <typename T>
static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, T*& result, bool const disasm, const char* dbg_name)
{
    result->sh = ShaderTypeTraits<T>::CreateHWShader(buffer, buffer_size);

    if (result->sh)
    {
        result->sh->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
    }

    ID3DShaderReflection* pReflection = 0;

    HRESULT const _hr = D3DReflect(buffer, buffer_size, IID_PPV_ARGS(&pReflection));
    if (SUCCEEDED(_hr) && pReflection)
    {
        // Parse constant table data
        result->constants.parse(pReflection, ShaderTypeTraits<T>::GetShaderDest());

        _RELEASE(pReflection);
    }
    else
    {
        Msg("! D3DReflectShader %s hr == 0x%08x", file_name, _hr);
    }

    return _hr;
}

static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, void*& result, bool const disasm)
{
    string128 dbg_name{}, dbg_ext{};
    _splitpath(file_name, nullptr, nullptr, dbg_name, dbg_ext);
    strcat_s(dbg_name, dbg_ext);

    HRESULT _result = E_FAIL;
    if (pTarget[0] == 'p')
    {
        SPS* sps_result = (SPS*)result;
        _result = HW.pDevice->CreatePixelShader(buffer, buffer_size, 0, &sps_result->ps);
        if (!SUCCEEDED(_result))
        {
            Msg("! PS: [%s]", file_name);
            Msg("! CreatePixelShader hr == 0x%08x", _result);
            return E_FAIL;
        }

        if (sps_result->ps)
        {
            sps_result->ps->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
        }

        ID3DShaderReflection* pReflection = 0;

        _result = D3DReflect(buffer, buffer_size, IID_PPV_ARGS(&pReflection));

        //	Parse constant, texture, sampler binding
        //	Store input signature blob
        if (SUCCEEDED(_result) && pReflection)
        {
            //	Let constant table parse it's data
            sps_result->constants.parse(pReflection, RC_dest_pixel);

            _RELEASE(pReflection);
        }
        else
        {
            Msg("! PS: [%s]", file_name);
            Msg("! D3DReflectShader hr == 0x%08x", _result);
        }
    }
    else if (pTarget[0] == 'v')
    {
        SVS* svs_result = (SVS*)result;

        _result = HW.pDevice->CreateVertexShader(buffer, buffer_size, 0, &svs_result->vs);

        if (!SUCCEEDED(_result))
        {
            Msg("! VS: [%s]", file_name);
            Msg("! CreateVertexShader hr == 0x%08x", _result);
            return E_FAIL;
        }

        if (svs_result->vs)
        {
            svs_result->vs->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
        }

        ID3DShaderReflection* pReflection = 0;

        _result = D3DReflect(buffer, buffer_size, IID_PPV_ARGS(&pReflection));

        //	Parse constant, texture, sampler binding
        //	Store input signature blob
        if (SUCCEEDED(_result) && pReflection)
        {
            //	TODO: DX10: share the same input signatures

            //	Store input signature (need only for VS)
            // CHK_DX( D3DxxGetInputSignatureBlob(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &_vs->signature) );
            ID3DBlob* pSignatureBlob;
            CHK_DX(D3DGetInputSignatureBlob(buffer, buffer_size, &pSignatureBlob));
            VERIFY(pSignatureBlob);

            svs_result->signature = dxRenderDeviceRender::Instance().Resources->_CreateInputSignature(pSignatureBlob);

            _RELEASE(pSignatureBlob);

            //	Let constant table parse it's data
            svs_result->constants.parse(pReflection, RC_dest_vertex);

            _RELEASE(pReflection);
        }
        else
        {
            Msg("! VS: [%s]", file_name);
            Msg("! D3DXFindShaderComment hr == 0x%08x", _result);
        }
    }
    else if (pTarget[0] == 'g')
    {
        SGS* sgs_result = (SGS*)result;
        _result = HW.pDevice->CreateGeometryShader(buffer, buffer_size, 0, &sgs_result->gs);
        if (!SUCCEEDED(_result))
        {
            Msg("! GS: [%s]", file_name);
            Msg("! CreateGeometryShaderhr == 0x%08x", _result);
            return E_FAIL;
        }

        if (sgs_result->gs)
        {
            sgs_result->gs->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(dbg_name), dbg_name);
        }

        ID3DShaderReflection* pReflection = 0;

        _result = D3DReflect(buffer, buffer_size, IID_PPV_ARGS(&pReflection));

        //	Parse constant, texture, sampler binding
        //	Store input signature blob
        if (SUCCEEDED(_result) && pReflection)
        {
            //	Let constant table parse it's data
            sgs_result->constants.parse(pReflection, RC_dest_geometry);

            _RELEASE(pReflection);
        }
        else
        {
            Msg("! PS: [%s]", file_name);
            Msg("! D3DReflectShader hr == 0x%08x", _result);
        }
    }
    else if (pTarget[0] == 'c')
    {
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SCS*&)result, disasm, dbg_name);
    }
    else if (pTarget[0] == 'h')
    {
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SHS*&)result, disasm, dbg_name);
    }
    else if (pTarget[0] == 'd')
    {
        _result = create_shader(pTarget, buffer, buffer_size, file_name, (SDS*&)result, disasm, dbg_name);
    }
    else
    {
        NODEFAULT;
    }

    if (disasm)
    {
        ID3DBlob* disasm = 0;
        D3DDisassemble(buffer, buffer_size, FALSE, 0, &disasm);
        string_path dname;
        xr_strconcat(dname, file_name, "\\disasm.txt");
        IWriter* W = FS.w_open(dname);
        W->w(disasm->GetBufferPointer(), (u32)disasm->GetBufferSize());
        FS.w_close(W);
        _RELEASE(disasm);
    }

    return _result;
}

//--------------------------------------------------------------------------------------------------------------
class includer final : public ID3DInclude
{
    IReader* R{};

public:
    HRESULT __stdcall Open(D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
    {
        string_path pname;
        strconcat(sizeof(pname), pname, ::Render->getShaderPath(), pFileName);
        R = FS.r_open("$game_shaders$", pname);
        if (!R)
        {
            // possibly in shared directory or somewhere else - open directly
            R = FS.r_open("$game_shaders$", pFileName);
            if (!R)
                return E_FAIL;
        }

        R->skip_bom(pFileName);

        *ppData = R->pointer();
        *pBytes = R->elapsed();
        return D3D_OK;
    }

    HRESULT __stdcall Close(LPCVOID) override
    {
        if (R)
            FS.r_close(R);
        return D3D_OK;
    }
};

HRESULT CRender::shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result)
{
    xr_vector<D3D_SHADER_MACRO> defines{m_ShaderOptions};
    defines.reserve(55);

    char c_smapsize[10]{};
    char c_sun_shafts[10]{};
    char c_sun_quality[10]{};
    char c_ssao[10]{};
    char c_smaa_quality[10]{};
    char samples[10]{};

    // SSS preprocessor stuff
    char c_ssfx_sss_dir_quality[10]{};
    char c_ssfx_sss_omni_quality[10]{};
    char c_ssfx_terrain_pom_refine[10]{};
    char c_ssfx_pom_refine[10]{};
    char c_ssfx_il[10]{};
    char c_ssfx_ao[10]{};
    char c_ssfx_water[10]{};
    char c_ssfx_water_parallax[10]{};
    char c_ssr_quality[10]{};
    char c_rain_quality[10]{};
    char c_inter_grass[10]{};

    sprintf_s(c_smapsize, "%d", o.smapsize);
    defines.emplace_back("SMAP_size", c_smapsize);

    if (ps_r2_ls_flags.test(R2FLAG_MBLUR))
        defines.emplace_back("USE_MBLUR", "1");

    if (o.dx10_msaa)
        defines.emplace_back("ISAMPLE", "0");

    // skinning
    if (m_skinning < 0)
        defines.emplace_back("SKIN_NONE", "1");
    else if (0 == m_skinning)
        defines.emplace_back("SKIN_0", "1");
    else if (1 == m_skinning)
        defines.emplace_back("SKIN_1", "1");
    else if (2 == m_skinning)
        defines.emplace_back("SKIN_2", "1");
    else if (3 == m_skinning)
        defines.emplace_back("SKIN_3", "1");
    else if (4 == m_skinning)
        defines.emplace_back("SKIN_4", "1");

    if (ps_r2_ls_flags.test(R2FLAG_DOF))
        defines.emplace_back("USE_DOF", "1");

    if (ps_r_sun_shafts)
    {
        sprintf_s(c_sun_shafts, "%d", ps_r_sun_shafts);
        defines.emplace_back("SUN_SHAFTS_QUALITY", c_sun_shafts);
    }

    if (ps_r_ao_mode == AO_MODE_GTAO)
        defines.emplace_back("USE_GTAO", "1");

    if (ps_r_ssao)
    {
        sprintf_s(c_ssao, "%d", ps_r_ssao);
        defines.emplace_back("SSAO_QUALITY", c_ssao);
    }

    if (ps_r_sun_quality)
    {
        sprintf_s(c_sun_quality, "%d", ps_r_sun_quality);
        defines.emplace_back("SUN_QUALITY", c_sun_quality);
    }

    if (ps_r2_ls_flags.test(R2FLAG_STEEP_PARALLAX))
        defines.emplace_back("ALLOW_STEEPPARALLAX", "1");

    if (ps_smaa_quality)
    {
        sprintf_s(c_smaa_quality, "%d", ps_smaa_quality);
        defines.emplace_back("SMAA_QUALITY", c_smaa_quality);
    }

    if (o.dx10_msaa)
    {
        defines.emplace_back("USE_MSAA", "1");
        defines.emplace_back("MSAA_OPTIMIZATION", "1");

        sprintf_s(samples, "%d", o.dx10_msaa_samples);
        defines.emplace_back("MSAA_SAMPLES", samples);
    }

    if (ps_ssfx_rain_1.w > 0.f)
    {
        sprintf_s(c_rain_quality, "%.0f", ps_ssfx_rain_1.w);
        defines.emplace_back("SSFX_RAIN_QUALITY", c_rain_quality);
    }

    if (ps_ssfx_grass_interactive.y > 0)
    {
        sprintf_s(c_inter_grass, "%d", u8(ps_ssfx_grass_interactive.y));
        defines.emplace_back("SSFX_INT_GRASS", c_inter_grass);
    }

    sprintf_s(c_ssr_quality, "%d", u8(std::min(std::max(ps_ssfx_ssr_quality, 0), 5)));
    defines.emplace_back("SSFX_SSR_QUALITY", c_ssr_quality);

    sprintf_s(c_ssfx_water, "%d", u8(std::min(std::max(ps_ssfx_water_quality.x, 0.0f), 4.0f)));
    defines.emplace_back("SSFX_WATER_QUALITY", c_ssfx_water);

    sprintf_s(c_ssfx_water_parallax, "%d", u8(std::min(std::max(ps_ssfx_water_quality.y, 0.0f), 3.0f)));
    defines.emplace_back("SSFX_WATER_PARALLAX", c_ssfx_water_parallax);

    sprintf_s(c_ssfx_il, "%d", u8(std::min(std::max(ps_ssfx_il_quality, 0), 64)));
    defines.emplace_back("SSFX_IL_QUALITY", c_ssfx_il);

    sprintf_s(c_ssfx_ao, "%d", u8(std::min(std::max(ps_ssfx_ao_quality, 2), 8)));
    defines.emplace_back("SSFX_AO_QUALITY", c_ssfx_ao);

    sprintf_s(c_ssfx_pom_refine, "%d", u8(std::min(std::max(ps_ssfx_pom_refine, 0), 1)));
    defines.emplace_back("SSFX_POM_REFINE", c_ssfx_pom_refine);

    sprintf_s(c_ssfx_terrain_pom_refine, "%d", u8(std::min(std::max(ps_ssfx_terrain_pom_refine, 0), 1)));
    defines.emplace_back("SSFX_TERRA_POM_REFINE", c_ssfx_terrain_pom_refine);

    sprintf_s(c_ssfx_sss_dir_quality, "%d", u8(std::min(std::max((int)ps_ssfx_sss_quality.x, 1), 24)));
    defines.emplace_back("SSFX_SSS_DIR_QUALITY", c_ssfx_sss_dir_quality);

    sprintf_s(c_ssfx_sss_omni_quality, "%d", u8(std::min(std::max((int)ps_ssfx_sss_quality.y, 1), 12)));
    defines.emplace_back("SSFX_SSS_OMNI_QUALITY", c_ssfx_sss_omni_quality);

    defines.emplace_back("SSFX_MODEXE", "1");

    // finish
    defines.emplace_back(nullptr, nullptr);

    char extension[3]{};
    strncpy_s(extension, pTarget, 2);

    string_path file_name{};

    string_path file{};
    xr_strcpy(file, "shaders_cache\\");
    xr_strcat(file, name);
    xr_strcat(file, ".");
    xr_strcat(file, extension);
    FS.update_path(file_name, "$app_data_root$", file);

    includer Includer;
    LPD3DBLOB pShaderBuf{};
    LPD3DBLOB pErrorBuf{};
    HRESULT _result;
    XXH64_hash_t xxh = XXH64_hash_t(-1);

    _result = D3DPreprocess(pSrcData, SrcDataLen, "", defines.data(), &Includer, &pShaderBuf, &pErrorBuf);
    if (SUCCEEDED(_result))
        xxh = XXH3_64bits(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize());
    if (pShaderBuf)
        pShaderBuf->Release();
    if (pErrorBuf)
        pErrorBuf->Release();

    snprintf(file, sizeof(file), "%s\\%s-%s-%x", file_name, pFunctionName, pTarget, Flags);
    if (FS.exist(file))
    {
        IReader* fp = FS.r_open(file);
        fp->skip_bom(file);

        if (fp->elapsed() > 2 * sizeof(xxh))
        {
            XXH64_hash_t xxh_read = fp->r_u64();
            if (SUCCEEDED(_result) && xxh_read != xxh)
                _result = E_FAIL;
            xxh_read = fp->r_u64();
            if (SUCCEEDED(_result) && xxh_read != XXH3_64bits(fp->pointer(), fp->elapsed()))
                _result = E_FAIL;
            if (SUCCEEDED(_result))
                _result = create_shader(pTarget, (DWORD*)fp->pointer(), fp->elapsed(), file_name, result, o.disasm);
        }
        else
            _result = E_FAIL;

        FS.r_close(fp);
    }
    else
        _result = E_FAIL;

    pShaderBuf = NULL;
    pErrorBuf = NULL;

    if (FAILED(_result))
    {
        // Msg("--Compiling shader [%s] %s] [%s]", name, pTarget, pFunctionName);

        _result = D3DCompile(pSrcData, SrcDataLen, "", defines.data(), &Includer, pFunctionName, pTarget, Flags, 0, &pShaderBuf, &pErrorBuf);
        if (SUCCEEDED(_result))
        {
            IWriter* fp = FS.w_open(file);
            fp->w_u64(xxh);

            xxh = XXH3_64bits(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize());
            fp->w_u64(xxh);

            fp->w(pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize());
            FS.w_close(fp);

            _result = create_shader(pTarget, (DWORD*)pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize(), file_name, result, o.disasm);
        }
    }

    if (FAILED(_result))
    {
        Msg("! %s", file_name);
        if (pErrorBuf)
            Log("! error: " + std::string{reinterpret_cast<const char*>(pErrorBuf->GetBufferPointer())});
        else
            Msg("Can't compile shader hr=0x%08x", _result);
    }

    if (pErrorBuf)
        pErrorBuf->Release();

    return _result;
}
