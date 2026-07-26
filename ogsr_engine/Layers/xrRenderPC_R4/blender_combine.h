#pragma once

class CBlender_combine final : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_combine, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: combiner"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_combine();
    ~CBlender_combine() override;
};

class CBlender_combine_msaa final : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_combine_msaa, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: combiner"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_combine_msaa();
    ~CBlender_combine_msaa() override;

    void SetDefine(LPCSTR Name, LPCSTR Definition)
    {
        this->Name = Name;
        this->Definition = Definition;
    }

    LPCSTR Name;
    LPCSTR Definition;
};
