#pragma once

class CBlender_bloom_build : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_bloom_build, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: combine to bloom target"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_bloom_build();
    ~CBlender_bloom_build() override;
};

class CBlender_bloom_build_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_bloom_build_msaa, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: combine to bloom target msaa"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_bloom_build_msaa();
    ~CBlender_bloom_build_msaa() override;
};

class CBlender_postprocess_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_postprocess_msaa, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "INTERNAL: combine to bloom target msaa"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_postprocess_msaa();
    ~CBlender_postprocess_msaa() override;
};

class CBlender_ssfx_bloom_build : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_build, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "SSFX Bloom"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_bloom_build();
    ~CBlender_ssfx_bloom_build() override;
};

class CBlender_ssfx_bloom_lens : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_lens, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "SSFX Bloom Lens"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_bloom_lens();
    ~CBlender_ssfx_bloom_lens() override;
};

class CBlender_ssfx_bloom_downsample : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_downsample, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "SSFX Bloom Downsample"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_bloom_downsample();
    ~CBlender_ssfx_bloom_downsample() override;
};

class CBlender_ssfx_bloom_upsample : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_upsample, IBlender);

public:
    [[nodiscard]] gsl::czstring getComment() override { return "SSFX Bloom Upsample"; }

    void Compile(CBlender_Compile& C) override;

    CBlender_ssfx_bloom_upsample();
    ~CBlender_ssfx_bloom_upsample() override;
};
