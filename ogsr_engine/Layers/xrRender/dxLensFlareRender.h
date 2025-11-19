#ifndef dxLensFlareRender_included
#define dxLensFlareRender_included

#include "..\..\Include\xrRender\LensFlareRender.h"

class dxFlareRender : public IFlareRender
{
    RTTI_DECLARE_TYPEINFO(dxFlareRender, IFlareRender);

public:
    ~dxFlareRender() override = default;

    virtual void Copy(IFlareRender& _in);

    virtual void CreateShader(LPCSTR sh_name, LPCSTR tex_name);
    virtual void DestroyShader();

    // private:
public:
    ref_shader hShader;
};

class dxLensFlareRender : public ILensFlareRender
{
    RTTI_DECLARE_TYPEINFO(dxLensFlareRender, ILensFlareRender);

public:
    ~dxLensFlareRender() override = default;

    virtual void Copy(ILensFlareRender& _in);

    virtual void Render(CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient);

    virtual void OnDeviceCreate();
    virtual void OnDeviceDestroy();

private:
    ref_geom hGeom;
};

#endif //	LensFlareRender_included
