#ifndef r_backendH
#define r_backendH

// #define RBackend_PGO

#ifdef RBackend_PGO
#define PGO(a) a
#else
#define PGO(a)
#endif

#include "LightTrack.h"
#include "r_DStreams.h"
#include "r_constants_cache.h"
#include "r_backend_xform.h"
#include "r_backend_hemi.h"
#include "r_backend_tree.h"

#include "../xrRenderDX10/StateManager/dx10ShaderResourceStateCache.h"
#include "../xrRenderDX10/StateManager/dx10StateManager.h"

#include "../xrRenderPC_R4/r_backend_lod.h"
#include "../xrRenderPC_R4/SMAP_Allocator.h"

#include "fvf.h"

constexpr inline u32 CULL_CCW{D3DCULL_CCW};
constexpr inline u32 CULL_CW{D3DCULL_CW};
constexpr inline u32 CULL_NONE{D3DCULL_NONE};

///		detailed statistic
struct R_statistics_element
{
    u32 verts, dips;
    ICF void add(u32 _verts)
    {
        verts += _verts;
        dips++;
    }
};

struct R_statistics
{
    R_statistics_element s_static;
    R_statistics_element s_flora;
    R_statistics_element s_flora_lods;
    R_statistics_element s_details;
    R_statistics_element s_ui;
    R_statistics_element s_dynamic;
    R_statistics_element s_dynamic_sw;
    R_statistics_element s_dynamic_inst;
    R_statistics_element s_dynamic_1B;
    R_statistics_element s_dynamic_2B;
    R_statistics_element s_dynamic_3B;
    R_statistics_element s_dynamic_4B;
};

class CBackend
{
public:
    enum
    {
        MaxCBuffers = 14
    };

    R_xforms xforms;
    R_hemi hemi;
    R_tree tree;
    R_LOD LOD;

    ref_cbuffer m_aVertexConstants[MaxCBuffers];
    ref_cbuffer m_aPixelConstants[MaxCBuffers];

    ref_cbuffer m_aGeometryConstants[MaxCBuffers];
    ref_cbuffer m_aComputeConstants[MaxCBuffers];

    ref_cbuffer m_aHullConstants[MaxCBuffers];
    ref_cbuffer m_aDomainConstants[MaxCBuffers];

    D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
    ID3DInputLayout* m_pInputLayout;

private:
    // Render-targets
    ID3DRenderTargetView* pRT[4];
    ID3DDepthStencilView* pZB;

    // Vertices/Indices/etc
    SDeclaration* decl;
    ID3DVertexBuffer* vb;
    ID3DIndexBuffer* ib;
    u32 vb_stride;

public:
    // Pixel/Vertex constants
    alignas(16) R_constants constants;

private:
    R_constant_table* ctable;

    // Shaders/State
    ID3DState* state;
    ID3DPixelShader* ps;
    ID3DVertexShader* vs;
    ID3DGeometryShader* gs;
    ID3D11HullShader* hs;
    ID3D11DomainShader* ds;
    ID3D11ComputeShader* cs;

#ifdef DEBUG
    LPCSTR ps_name;
    LPCSTR vs_name;
    LPCSTR gs_name;
    LPCSTR hs_name;
    LPCSTR ds_name;
    LPCSTR cs_name;
#endif
    u32 stencil_enable;
    u32 stencil_func;
    u32 stencil_ref;
    u32 stencil_mask;
    u32 stencil_writemask;
    u32 stencil_fail;
    u32 stencil_pass;
    u32 stencil_zfail;
    u32 colorwrite_mask;
    u32 fill_mode;
    u32 cull_mode;
    u32 z_enable;
    u32 z_func;
    u32 alpha_ref;

    // Lists
    STextureList* T;

    // Lists-expanded
    CTexture* textures_ps[CTexture::mtMaxPixelShaderTextures]; // stages
    CTexture* textures_vs[CTexture::mtMaxVertexShaderTextures]; // 4 vs
    CTexture* textures_gs[CTexture::mtMaxGeometryShaderTextures]; // 4 vs
    CTexture* textures_hs[CTexture::mtMaxHullShaderTextures]; // 4 vs
    CTexture* textures_ds[CTexture::mtMaxDomainShaderTextures]; // 4 vs
    CTexture* textures_cs[CTexture::mtMaxComputeShaderTextures]; // 4 vs

public:
    struct _stats
    {
        u32 polys;
        u32 verts;
        u32 calls;
        u32 vs;
        u32 ps;
#ifdef DEBUG
        u32 decl;
        u32 vb;
        u32 ib;
        u32 states; // Number of times the shader-state changes
        u32 textures; // Number of times the shader-tex changes
        u32 matrices; // Number of times the shader-xform changes
        u32 constants; // Number of times the shader-consts changes
#endif
        u32 xforms;
        u32 target_rt;
        u32 target_zb;

        R_statistics r;
    } stat;

    ctx_id_t context_id{R__IMM_CTX_ID};

    ICF ID3D11DeviceContext1* context() const;

    IC CTexture* get_ActiveTexture(u32 stage)
    {
        if (stage < CTexture::rstVertex)
            return textures_ps[stage];
        if (stage < CTexture::rstGeometry)
            return textures_vs[stage - CTexture::rstVertex];
        if (stage < CTexture::rstHull)
            return textures_gs[stage - CTexture::rstGeometry];
        if (stage < CTexture::rstDomain)
            return textures_hs[stage - CTexture::rstHull];
        if (stage < CTexture::rstCompute)
            return textures_ds[stage - CTexture::rstDomain];
        if (stage < CTexture::rstInvalid)
            return textures_cs[stage - CTexture::rstCompute];

        VERIFY(!"Invalid texture stage");
        return nullptr;
    }

    CROS_impl::lmaterial o;

    void apply_lmaterial(IRenderable* O = nullptr);

    IC void get_ConstantDirect(const char* n, size_t DataSize, void** pVData, void** pGData, void** pPData);

    void Invalidate();

    // API
    IC void set_xform(u32 ID, const Fmatrix& M);
    IC void set_xform_world(const Fmatrix& M);
    IC void set_xform_view(const Fmatrix& M);
    IC void set_xform_project(const Fmatrix& M);
    IC const Fmatrix& get_xform_world();
    IC const Fmatrix& get_xform_view();
    IC const Fmatrix& get_xform_project();

    IC ID3DRenderTargetView* get_RT(u32 ID = 0);
    IC ID3DDepthStencilView* get_ZB();
    IC void set_RT(ID3DRenderTargetView* RT, u32 ID = 0);
    IC void set_ZB(ID3DDepthStencilView* ZB);

    IC void ClearRT(ID3DRenderTargetView* rt, const Fcolor& color);
    IC void ClearZB(ID3DDepthStencilView* zb, float depth);
    IC void ClearZB(ID3DDepthStencilView* zb, float depth, u32 stencil);

    IC void set_ZB(const ref_rt& zb) { set_ZB(zb ? zb->pZRT[context_id] : nullptr); }
    ICF void ClearRT(ref_rt& rt, const Fcolor& color) { ClearRT(rt->pRT, color); }
    ICF void ClearZB(ref_rt& zb, float depth) { ClearZB(zb->pZRT[context_id], depth); }
    ICF void ClearZB(ref_rt& zb, float depth, u32 stencil) { ClearZB(zb->pZRT[context_id], depth, stencil); }

    IC void set_Constants(R_constant_table* C);
    IC void set_Constants(ref_ctable& C) { set_Constants(&*C); }

    void set_Textures(STextureList* T);
    IC void set_Textures(ref_texture_list& T) { set_Textures(&*T); }

    IC void set_Pass(SPass* P);
    void set_Pass(ref_pass& P) { set_Pass(&*P); }

    ICF void set_Element(ShaderElement* S, u32 pass = 0);
    void set_Element(ref_selement& S, u32 pass = 0) { set_Element(&*S, pass); }

    ICF void set_Shader(Shader* S, u32 pass = 0);
    void set_Shader(ref_shader& S, u32 pass = 0) { set_Shader(&*S, pass); }

    ICF void set_States(ID3DState* _state);
    ICF void set_States(ref_state& _state) { set_States(_state->state); }

    ICF void set_Format(SDeclaration* _decl);

    ICF void set_PS(ID3DPixelShader* _ps, LPCSTR _n = nullptr);
    ICF void set_PS(ref_ps& _ps) { set_PS(_ps->ps, _ps->cName.c_str()); }

    ICF void set_GS(ID3DGeometryShader* _gs, LPCSTR _n = nullptr);
    ICF void set_GS(ref_gs& _gs) { set_GS(_gs->gs, _gs->cName.c_str()); }

    ICF void set_HS(ID3D11HullShader* _hs, LPCSTR _n = nullptr);
    ICF void set_HS(ref_hs& _hs) { set_HS(_hs->sh, _hs->cName.c_str()); }

    ICF void set_DS(ID3D11DomainShader* _ds, LPCSTR _n = nullptr);
    ICF void set_DS(ref_ds& _ds) { set_DS(_ds->sh, _ds->cName.c_str()); }

    ICF void set_CS(ID3D11ComputeShader* _cs, LPCSTR _n = nullptr);
    ICF void set_CS(ref_cs& _cs) { set_CS(_cs->sh, _cs->cName.c_str()); }

    ICF bool is_TessEnabled();

    ICF void set_VS(ref_vs& _vs);
    ICF void set_VS(SVS* _vs);

protected: //	In DX10 we need input shader signature which is stored in ref_vs
    ICF void set_VS(ID3DVertexShader* _vs, LPCSTR _n = nullptr);

public:
    ICF void set_Vertices(ID3DVertexBuffer* _vb, u32 _vb_stride);
    ICF void set_Indices(ID3DIndexBuffer* _ib);
    ICF void set_Geometry(SGeometry* _geom);
    ICF void set_Geometry(ref_geom& _geom) { set_Geometry(&*_geom); }
    IC void set_Stencil(u32 _enable, u32 _func = D3DCMP_ALWAYS, u32 _ref = 0x00, u32 _mask = 0x00, u32 _writemask = 0x00, u32 _fail = D3DSTENCILOP_KEEP,
                        u32 _pass = D3DSTENCILOP_KEEP, u32 _zfail = D3DSTENCILOP_KEEP);
    IC void set_Z(u32 _enable);
    IC void set_ZFunc(u32 _func);
    IC void set_AlphaRef(u32 _value);
    IC void set_ColorWriteEnable(u32 _mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    IC void set_CullMode(u32 _mode);
    IC u32 get_CullMode() { return cull_mode; }
    IC void set_FillMode(u32 _mode);
    void set_ClipPlanes(u32 _enable, Fplane* _planes = nullptr, u32 count = 0);
    void set_ClipPlanes(u32 _enable, Fmatrix* _xform = nullptr, u32 fmask = 0xff);
    IC void set_Scissor(Irect* rect = nullptr);
    IC void SetViewport(const D3D_VIEWPORT& viewport) const;
    IC void set_viewport_size(float w, float h) const;

    // constants
    ICF ref_constant get_c(LPCSTR n)
    {
        if (ctable)
            return ctable->get(n);

        return ref_constant{};
    }

    ICF ref_constant get_c(const shared_str& n)
    {
        if (ctable)
            return ctable->get(n);

        return ref_constant{};
    }

    // constants - direct (fast)
    template <typename... Args>
    ICF void set_c(R_constant* C, Args&&... args)
    {
        if (!C)
            return;
        constants.set(C, std::forward<Args>(args)...);
    }

    template <typename... Args>
    ICF void set_ca(R_constant* C, Args&&... args)
    {
        if (!C)
            return;
        constants.seta(C, std::forward<Args>(args)...);
    }

    // constants - raw string (slow)
    template <typename... Args>
    ICF void set_c(const char* name, Args&&... args)
    {
        if (!ctable)
            return;
        set_c(ctable->get(name)._get(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    ICF void set_ca(const char* name, Args&&... args)
    {
        if (!ctable)
            return;
        set_ca(ctable->get(name)._get(), std::forward<Args>(args)...);
    }

    // constants - shared_str (average)
    template <typename... Args>
    ICF void set_c(const shared_str& name, Args&&... args)
    {
        if (!ctable)
            return;
        set_c(ctable->get(name)._get(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    ICF void set_ca(const shared_str& name, Args&&... args)
    {
        if (!ctable)
            return;
        set_ca(ctable->get(name)._get(), std::forward<Args>(args)...);
    }

    ICF void Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);
    ICF void Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC);

    ICF void Compute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ);

    ICF void submit()
    {
        XR_TRACY_ZONE_SCOPED();

        VERIFY(context_id != R__IMM_CTX_ID);
        ID3D11CommandList* pCommandList{};

        CHK_DX(HW.get_context(context_id)->FinishCommandList(false, &pCommandList));
        HW.get_context(R__IMM_CTX_ID)->ExecuteCommandList(pCommandList, false);

        _RELEASE(pCommandList);
    }

    // Device create / destroy / frame signaling
    void OnFrameBegin();
    void OnFrameEnd();
    void OnDeviceCreate();
    void OnDeviceDestroy();
    void SetupStates();

    // Debug render
    void dbg_DP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 vBase, u32 pc);
    void dbg_DIP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);

    void dbg_SetRS(D3DRENDERSTATETYPE, u32) { VERIFY(!"Not implemented"); }
    void dbg_SetSS(u32, D3DSAMPLERSTATETYPE, u32) { VERIFY(!"Not implemented"); }

    void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, const u16* pIdx, int pcnt);
    void dbg_Draw_Near(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, const u16* pIdx, int pcnt);
    void dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);

    void dbg_DrawAABB(Fvector& T, float sx, float sy, float sz, u32 C)
    {
        Fvector half_dim;
        half_dim.set(sx, sy, sz);
        Fmatrix TM;
        TM.translate(T);
        dbg_DrawOBB(TM, half_dim, C);
    }

    void dbg_DrawOBB(Fmatrix& T, Fvector& half_dim, u32 C);
    void dbg_DrawTRI(const Fmatrix& T, const Fvector* p, u32 C) { dbg_DrawTRI(T, p[0], p[1], p[2], C); }
    void dbg_DrawTRI(const Fmatrix& T, const Fvector& p1, const Fvector& p2, const Fvector& p3, u32 C);
    void dbg_DrawLINE(Fmatrix& T, Fvector& p1, Fvector& p2, u32 C);
    void dbg_DrawEllipse(Fmatrix& T, u32 C);

    CBackend() { Invalidate(); }

private:
    // Debug Draw
    void InitializeDebugDraw();
    void DestroyDebugDraw();
    ref_geom vs_L, vs_TL;

    //	DirectX 10 internal functionality
    void ApplyVertexLayout();
    void ApplyPrimitieTopology(D3D_PRIMITIVE_TOPOLOGY Topology);
    bool CBuffersNeedUpdate(ref_cbuffer buf1[MaxCBuffers], ref_cbuffer buf2[MaxCBuffers], u32& uiMin, u32& uiMax);

    void apply_object(IRenderable& O);

    ID3DBlob* m_pInputSignature{};
    ID3DUserDefinedAnnotation* pAnnotation{};
    bool m_bChangedRTorZB;

public:
    void ApplyRTandZB();

    bool RVelocity{};

    dx10StateManager StateManager;
    dx10ShaderResourceStateCache SRVSManager;

    SMAP_Allocator LP_smap_pool;
};

#define RCache (RImplementation.get_imm_context().cmd_list)

#endif
