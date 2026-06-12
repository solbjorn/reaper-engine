#pragma once

class CBlender_accum_point : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_point, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: accumulate point light"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_point();
    ~CBlender_accum_point() override;
};

class CBlender_accum_point_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_point_msaa, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: accumulate point light msaa"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_point_msaa();
    ~CBlender_accum_point_msaa() override;

    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    LPCSTR Name;
    LPCSTR Definition;
};
