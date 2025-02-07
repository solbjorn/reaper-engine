#pragma once

class CBlender_blur : public IBlender
{
public:
    virtual LPCSTR getComment() { return "Blur generation"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_blur();
    virtual ~CBlender_blur();
};

// SSS
class CBlender_ssfx_ssr : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_ssr"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_ssr();
    virtual ~CBlender_ssfx_ssr();
};

class CBlender_ssfx_volumetric_blur : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_volumetric_blur"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_volumetric_blur();
    virtual ~CBlender_ssfx_volumetric_blur();
};

class CBlender_ssfx_ao : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_ao"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_ao();
    virtual ~CBlender_ssfx_ao();
};

class CBlender_ssfx_sss : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_sss"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_sss();
    virtual ~CBlender_ssfx_sss();
};

class CBlender_ssfx_sss_ext : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_sss_ext"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_sss_ext();
    virtual ~CBlender_ssfx_sss_ext();
};

class CBlender_ssfx_rain : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_rain"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_rain();
    virtual ~CBlender_ssfx_rain();
};

class CBlender_ssfx_water_blur : public IBlender
{
public:
    virtual LPCSTR getComment() { return "ssfx_water"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_water_blur();
    virtual ~CBlender_ssfx_water_blur();
};
