#pragma once

class CBlender_accum_spot : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_spot, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: accumulate spot light"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_spot();
    ~CBlender_accum_spot() override;
};

class CBlender_accum_spot_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_spot_msaa, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: accumulate spot light msaa"; }

    void Compile(CBlender_Compile& C) override;

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
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: accumulate spot light msaa"; }

    void Compile(CBlender_Compile& C) override;

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
