#pragma once

class IResourceManager;
enum class DeviceState;

namespace xr
{
struct render_memory_usage
{
    gsl::index c_base;
    gsl::index c_lmaps;
    gsl::index m_base;
    gsl::index m_lmaps;
    gsl::index lua;
};
} // namespace xr

class XR_NOVTABLE IRenderDeviceRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IRenderDeviceRender);

public:
    ~IRenderDeviceRender() override = 0;

    virtual void Copy(IRenderDeviceRender& _in) = 0;

    //	Gamma correction functions
    virtual void setGamma(float fGamma) = 0;
    virtual void setBrightness(float fGamma) = 0;
    virtual void setContrast(float fGamma) = 0;
    virtual void updateGamma() = 0;

    //	Destroy
    virtual void OnDeviceDestroy(BOOL bKeepTextures) = 0;
    virtual void DestroyHW() = 0;
    virtual void Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) = 0;
    //	Init
    virtual void SetupStates() = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) = 0;
    virtual void SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF) = 0;
    //	Overdraw
    virtual void overdrawBegin() = 0;
    virtual void overdrawEnd() = 0;

    //	Resources control
    virtual void DeferredLoad(BOOL E) = 0;
    virtual void ResourcesDeferredUpload() = 0;
    virtual void ResourcesGetMemoryUsage(xr::render_memory_usage& usage) const = 0;
    virtual void ResourcesDumpMemoryUsage() const = 0;

    //	Device state
    virtual DeviceState GetDeviceState() = 0;
    virtual BOOL GetForceGPU_REF() = 0;
    virtual u32 GetCacheStatPolys() = 0;
    virtual void Begin() = 0;
    virtual void Clear() = 0;
    virtual void End() = 0;
    virtual void ClearTarget() = 0;
    virtual void SetCacheXform(Fmatrix& mView, Fmatrix& mProject) = 0;
    virtual void OnAssetsChanged() = 0;
    virtual IResourceManager* GetResourceManager() const = 0;

    virtual void editor_new() = 0;
    virtual void editor_end() = 0;
    virtual void editor_render() = 0;
};

inline IRenderDeviceRender::~IRenderDeviceRender() = default;
