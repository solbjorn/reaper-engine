#pragma once

class CBlender_gasmask_drops : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_gasmask_drops, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "Gasmask_drops"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_gasmask_drops();
    ~CBlender_gasmask_drops() override;
};
