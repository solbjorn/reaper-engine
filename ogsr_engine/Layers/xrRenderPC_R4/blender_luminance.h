#pragma once

class CBlender_luminance : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_luminance, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: luminance estimate"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_luminance();
    ~CBlender_luminance() override;
};
