#pragma once

#include "..\..\Include\xrRender\RenderDeviceRender.h"
#include "r__dsgraph_structure.h"
#include "xr_effgamma.h"

class CResourceManager;

class dxRenderDeviceRender : public IRenderDeviceRender, public pureFrame
{
    RTTI_DECLARE_TYPEINFO(dxRenderDeviceRender, IRenderDeviceRender, pureFrame);

public:
    dxRenderDeviceRender();
    ~dxRenderDeviceRender() override;

    void Copy(IRenderDeviceRender& _in) override;

    //	Gamma correction functions
    void setGamma(f32 fGamma) override;
    void setBrightness(f32 fGamma) override;
    void setContrast(f32 fGamma) override;
    void updateGamma() override;

    //	Destroy
    void OnDeviceDestroy(BOOL bKeepTextures) override;
    tmc::task<void> DestroyHW() override;
    tmc::task<void> Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, f32& fWidth_2, f32& fHeight_2) override;

    //	Init
    void SetupStates() override;
    tmc::task<void> OnDeviceCreate() override;
    tmc::task<void> Create(HWND hWnd, u32& dwWidth, u32& dwHeight, f32& fWidth_2, f32& fHeight_2) override;
    void SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF) override;

    //	Overdraw
    void overdrawBegin() override;
    void overdrawEnd() override;

    //	Resources control
    void DeferredLoad(BOOL E) override;
    tmc::task<void> ResourcesDeferredUpload() override;
    void ResourcesGetMemoryUsage(xr::render_memory_usage& usage) const override;
    void ResourcesDumpMemoryUsage() const override;

    //	Device state
    tmc::task<DeviceState> GetDeviceState() override;
    [[nodiscard]] BOOL GetForceGPU_REF() override;
    [[nodiscard]] u32 GetCacheStatPolys() override;
    void Begin() override;
    void Clear() override;
    tmc::task<void> End() override;
    void ClearTarget() override;
    void SetCacheXform(Fmatrix& mView, Fmatrix& mProject) override;

    [[nodiscard]] IResourceManager* GetResourceManager() const override;

    void editor_new() override;
    void editor_end() override;
    void editor_render() override;

    void CreateQuadIB();

    CResourceManager* Resources{};

    ref_shader m_WireShader;
    ref_shader m_SelectionShader;
    ref_shader m_PortalFadeShader;
    ref_geom m_PortalFadeGeom;

    // Dynamic geometry streams
    _VertexStream Vertex;
    _IndexStream Index;

    ID3DIndexBuffer* QuadIB;
    ID3DIndexBuffer* old_QuadIB;

    ctx_id_t alloc_context();

    ICF R_dsgraph_structure& get_context(ctx_id_t id)
    {
        VERIFY(id < R__NUM_CONTEXTS);

        VERIFY(contexts_used.test(id));
        VERIFY(contexts_pool[id].context_id == id);

        return contexts_pool[id];
    }

    ICF void release_context(ctx_id_t id)
    {
        VERIFY(id != R__IMM_CTX_ID); // never release immediate context
        VERIFY(id < R__NUM_PARALLEL_CONTEXTS);

        VERIFY(contexts_used.test(id));
        VERIFY(contexts_pool[id].context_id != R__INVALID_CTX_ID);

        contexts_used.reset(id);
    }

    ICF R_dsgraph_structure& get_imm_context() { return contexts_pool[R__IMM_CTX_ID]; }

    void cleanup_contexts();

protected:
    xr::bitset<R__NUM_CONTEXTS> contexts_used;
    R_dsgraph_structure contexts_pool[R__NUM_CONTEXTS];

private:
    CGammaControl m_Gamma;

protected:
    bool b_loaded{};
};
