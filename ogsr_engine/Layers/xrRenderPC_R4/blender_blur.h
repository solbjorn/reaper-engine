#pragma once

class CBlender_blur : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_blur, IBlender);

public:
    virtual LPCSTR getComment() { return "Blur generation"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_blur();
    ~CBlender_blur() override;
};

// SSS
class CBlender_ssfx_ssr : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_ssr, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_ssr"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_ssr();
    ~CBlender_ssfx_ssr() override;
};

class CBlender_ssfx_volumetric_blur : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_volumetric_blur, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_volumetric_blur"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_volumetric_blur();
    ~CBlender_ssfx_volumetric_blur() override;
};

class CBlender_ssfx_ao : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_ao, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_ao"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_ao();
    ~CBlender_ssfx_ao() override;
};

class CBlender_ssfx_sss : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_sss, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_sss"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_sss();
    ~CBlender_ssfx_sss() override;
};

class CBlender_ssfx_sss_ext : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_sss_ext, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_sss_ext"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_sss_ext();
    ~CBlender_ssfx_sss_ext() override;
};

class CBlender_ssfx_rain : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_rain, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_rain"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_rain();
    ~CBlender_ssfx_rain() override;
};

class CBlender_ssfx_water_blur : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_water_blur, IBlender);

public:
    virtual LPCSTR getComment() { return "ssfx_water"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_water_blur();
    ~CBlender_ssfx_water_blur() override;
};
