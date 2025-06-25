#ifndef LensFlareRender_included
#define LensFlareRender_included

class IFlareRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IFlareRender);

public:
    virtual ~IFlareRender() { ; }
    virtual void Copy(IFlareRender& _in) = 0;

    virtual void CreateShader(LPCSTR sh_name, LPCSTR tex_name) = 0;
    virtual void DestroyShader() = 0;
};

class CLensFlare;

class ILensFlareRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(ILensFlareRender);

public:
    virtual ~ILensFlareRender() { ; }
    virtual void Copy(ILensFlareRender& _in) = 0;

    virtual void Render(CLensFlare& owner, BOOL bSun, BOOL bFlares, BOOL bGradient) = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;
};

#endif //	LensFlareRender_included
