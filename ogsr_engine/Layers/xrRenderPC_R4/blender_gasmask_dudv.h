#pragma once

class CBlender_gasmask_dudv : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_gasmask_dudv, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "Gasmask_dudv"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_gasmask_dudv();
    ~CBlender_gasmask_dudv() override;
};
