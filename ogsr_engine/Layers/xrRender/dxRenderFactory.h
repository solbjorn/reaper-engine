#ifndef dxRenderFactory_included
#define dxRenderFactory_included

#include "../../Include/xrRender/RenderFactory.h"

class dxRenderFactory : public IRenderFactory
{
    RTTI_DECLARE_TYPEINFO(dxRenderFactory, IRenderFactory);

public:
    ~dxRenderFactory() override = default;

    [[nodiscard]] IUISequenceVideoItem* CreateUISequenceVideoItem() override;
    void DestroyUISequenceVideoItem(IUISequenceVideoItem* pObject) override;
    [[nodiscard]] IUIShader* CreateUIShader() override;
    void DestroyUIShader(IUIShader* pObject) override;
    [[nodiscard]] IStatGraphRender* CreateStatGraphRender() override;
    void DestroyStatGraphRender(IStatGraphRender* pObject) override;
    [[nodiscard]] IConsoleRender* CreateConsoleRender() override;
    void DestroyConsoleRender(IConsoleRender* pObject) override;
    [[nodiscard]] IRenderDeviceRender* CreateRenderDeviceRender() override;
    void DestroyRenderDeviceRender(IRenderDeviceRender* pObject) override;

#ifdef DEBUG
    [[nodiscard]] IObjectSpaceRender* CreateObjectSpaceRender() override;
    void DestroyObjectSpaceRender(IObjectSpaceRender* pObject) override;
#endif // DEBUG

    [[nodiscard]] IWallMarkArray* CreateWallMarkArray() override;
    void DestroyWallMarkArray(IWallMarkArray* pObject) override;
    [[nodiscard]] IStatsRender* CreateStatsRender() override;
    void DestroyStatsRender(IStatsRender* pObject) override;

    [[nodiscard]] IFlareRender* CreateFlareRender() override;
    void DestroyFlareRender(IFlareRender* pObject) override;
    [[nodiscard]] IThunderboltRender* CreateThunderboltRender() override;
    void DestroyThunderboltRender(IThunderboltRender* pObject) override;
    [[nodiscard]] IThunderboltDescRender* CreateThunderboltDescRender() override;
    void DestroyThunderboltDescRender(IThunderboltDescRender* pObject) override;
    [[nodiscard]] IRainRender* CreateRainRender() override;
    void DestroyRainRender(IRainRender* pObject) override;
    [[nodiscard]] ILensFlareRender* CreateLensFlareRender() override;
    void DestroyLensFlareRender(ILensFlareRender* pObject) override;
    [[nodiscard]] IEnvironmentRender* CreateEnvironmentRender() override;
    void DestroyEnvironmentRender(IEnvironmentRender* pObject) override;
    [[nodiscard]] IEnvDescriptorRender* CreateEnvDescriptorRender() override;
    void DestroyEnvDescriptorRender(IEnvDescriptorRender* pObject) override;
    [[nodiscard]] IFontRender* CreateFontRender() override;
    void DestroyFontRender(IFontRender* pObject) override;
};

extern dxRenderFactory RenderFactoryImpl;

#endif //	RenderFactory_included
