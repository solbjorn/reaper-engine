#pragma once

class CBlender_accum_direct_mask : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_direct_mask, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: mask direct light"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_direct_mask();
    ~CBlender_accum_direct_mask() override;
};

class CBlender_accum_direct_mask_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_accum_direct_mask_msaa, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: mask direct light msaa"; }

    virtual void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    void Compile(CBlender_Compile& C) override;

    CBlender_accum_direct_mask_msaa();
    ~CBlender_accum_direct_mask_msaa() override;

    LPCSTR Name{};
    LPCSTR Definition{};
};
