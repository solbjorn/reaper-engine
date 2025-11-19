#pragma once

class CBlender_accum_direct : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_direct, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate direct light"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_accum_direct();
    ~CBlender_accum_direct() override;
};

class CBlender_accum_direct_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_direct_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate direct light"; }

    virtual void Compile(CBlender_Compile& C);
    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    LPCSTR Name{};
    LPCSTR Definition{};

    CBlender_accum_direct_msaa();
    ~CBlender_accum_direct_msaa() override;
};

class CBlender_accum_direct_volumetric_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_direct_volumetric_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate direct light"; }

    virtual void Compile(CBlender_Compile& C);
    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    CBlender_accum_direct_volumetric_msaa();
    ~CBlender_accum_direct_volumetric_msaa() override;

    LPCSTR Name{};
    LPCSTR Definition{};
};

class CBlender_accum_direct_volumetric_sun_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_direct_volumetric_sun_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate direct light"; }

    virtual void Compile(CBlender_Compile& C);
    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    CBlender_accum_direct_volumetric_sun_msaa();
    ~CBlender_accum_direct_volumetric_sun_msaa() override;

    LPCSTR Name{};
    LPCSTR Definition{};
};
