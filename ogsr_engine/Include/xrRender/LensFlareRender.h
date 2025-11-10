#ifndef LensFlareRender_included
#define LensFlareRender_included

class XR_NOVTABLE IFlareRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IFlareRender);

public:
    virtual ~IFlareRender() = 0;

    virtual void Copy(IFlareRender& _in) = 0;

    virtual void CreateShader(LPCSTR sh_name, LPCSTR tex_name) = 0;
    virtual void DestroyShader() = 0;
};

inline IFlareRender::~IFlareRender() = default;

class CLensFlare;

class XR_NOVTABLE ILensFlareRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ILensFlareRender);

public:
    virtual ~ILensFlareRender() = 0;

    virtual void Copy(ILensFlareRender& _in) = 0;

    virtual void Render(CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient) = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;
};

inline ILensFlareRender::~ILensFlareRender() = default;

#endif //	LensFlareRender_included
