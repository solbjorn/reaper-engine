#pragma once

#include "bitmap.h"

#include "..\..\Include\xrRender\RenderDeviceRender.h"
#include "r__dsgraph_structure.h"
#include "xr_effgamma.h"

class CResourceManager;

class dxRenderDeviceRender : public IRenderDeviceRender, public pureFrame
{
public:
    dxRenderDeviceRender();

    virtual void Copy(IRenderDeviceRender& _in);

    //	Gamma correction functions
    virtual void setGamma(float fGamma);
    virtual void setBrightness(float fGamma);
    virtual void setContrast(float fGamma);
    virtual void updateGamma();

    //	Destroy
    virtual void OnDeviceDestroy(BOOL bKeepTextures);
    virtual void DestroyHW();
    virtual void Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2);
    //	Init
    virtual void SetupStates();
    virtual void OnDeviceCreate(LPCSTR shName);
    virtual void Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) override;
    virtual void SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF);
    //	Overdraw
    virtual void overdrawBegin();
    virtual void overdrawEnd();

    //	Resources control
    virtual void DeferredLoad(BOOL E);
    virtual void ResourcesDeferredUpload();
    virtual void ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
    virtual void ResourcesDumpMemoryUsage();

    //	Device state
    virtual DeviceState GetDeviceState();
    virtual BOOL GetForceGPU_REF();
    virtual u32 GetCacheStatPolys();
    virtual void Begin() override;
    virtual void Clear() override;
    virtual void End() override;
    virtual void ClearTarget() override;
    virtual void SetCacheXform(Fmatrix& mView, Fmatrix& mProject);
    virtual void OnAssetsChanged();

    IResourceManager* GetResourceManager() const override;

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

        contexts_used.clear(id);
    }

    ICF R_dsgraph_structure& get_imm_context() { return contexts_pool[R__IMM_CTX_ID]; }

    void cleanup_contexts();

protected:
    xr_bitmap<R__NUM_CONTEXTS> contexts_used;
    R_dsgraph_structure contexts_pool[R__NUM_CONTEXTS];

private:
    CGammaControl m_Gamma;

protected:
    bool b_loaded{};
};
