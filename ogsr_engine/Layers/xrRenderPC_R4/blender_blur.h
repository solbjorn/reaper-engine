#pragma once

class CBlender_blur : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_blur, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "Blur generation"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_blur();
    ~CBlender_blur() override;
};

// SSS
class CBlender_ssfx_ssr : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_ssr, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_ssr"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_ssr();
    ~CBlender_ssfx_ssr() override;
};

class CBlender_ssfx_volumetric_blur : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_volumetric_blur, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_volumetric_blur"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_volumetric_blur();
    ~CBlender_ssfx_volumetric_blur() override;
};

class CBlender_ssfx_ao : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_ao, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_ao"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_ao();
    ~CBlender_ssfx_ao() override;
};

class CBlender_ssfx_sss : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_sss, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_sss"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_sss();
    ~CBlender_ssfx_sss() override;
};

class CBlender_ssfx_sss_ext : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_sss_ext, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_sss_ext"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_sss_ext();
    ~CBlender_ssfx_sss_ext() override;
};

class CBlender_ssfx_rain : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_rain, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_rain"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_rain();
    ~CBlender_ssfx_rain() override;
};

class CBlender_ssfx_water_blur : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_water_blur, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "ssfx_water"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_water_blur();
    ~CBlender_ssfx_water_blur() override;
};
