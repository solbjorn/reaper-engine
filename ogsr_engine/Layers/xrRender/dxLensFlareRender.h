#ifndef dxLensFlareRender_included
#define dxLensFlareRender_included

#include "..\..\Include\xrRender\LensFlareRender.h"

class dxFlareRender : public IFlareRender
{
    RTTI_DECLARE_TYPEINFO(dxFlareRender, IFlareRender);

public:
    ~dxFlareRender() override = default;

    void Copy(IFlareRender& _in) override;

    void CreateShader(gsl::czstring sh_name, gsl::czstring tex_name) override;
    void DestroyShader() override;

    // private:
public:
    ref_shader hShader;
};

class dxLensFlareRender : public ILensFlareRender
{
    RTTI_DECLARE_TYPEINFO(dxLensFlareRender, ILensFlareRender);

public:
    ~dxLensFlareRender() override = default;

    void Copy(ILensFlareRender& _in) override;

    void Render(CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient) override;

    void OnDeviceCreate() override;
    void OnDeviceDestroy() override;

private:
    ref_geom hGeom;
};

#endif //	LensFlareRender_included
