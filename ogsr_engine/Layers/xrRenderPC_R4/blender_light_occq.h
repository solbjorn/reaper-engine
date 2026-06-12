#pragma once

class CBlender_light_occq : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_light_occq, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: occlusion testing"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_light_occq();
    ~CBlender_light_occq() override;
};
