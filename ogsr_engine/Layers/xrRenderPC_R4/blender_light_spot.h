#pragma once

class CBlender_accum_spot : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_spot, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate spot light"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_accum_spot();
    ~CBlender_accum_spot() override;
};

class CBlender_accum_spot_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_spot_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate spot light msaa"; }

    virtual void Compile(CBlender_Compile& C);

    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    CBlender_accum_spot_msaa();
    ~CBlender_accum_spot_msaa() override;

    LPCSTR Name;
    LPCSTR Definition;
};

class CBlender_accum_volumetric_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_volumetric_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: accumulate spot light msaa"; }

    virtual void Compile(CBlender_Compile& C);

    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    CBlender_accum_volumetric_msaa();
    ~CBlender_accum_volumetric_msaa() override;

    LPCSTR Name;
    LPCSTR Definition;
};
