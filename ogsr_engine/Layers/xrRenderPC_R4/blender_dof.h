#pragma once

class CBlender_dof : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_dof, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "DoF"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_dof();
    ~CBlender_dof() override;
};
