#pragma once

class CBlender_bloom_build : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_bloom_build, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: combine to bloom target"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_bloom_build();
    ~CBlender_bloom_build() override;
};

class CBlender_bloom_build_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_bloom_build_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: combine to bloom target msaa"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_bloom_build_msaa();
    ~CBlender_bloom_build_msaa() override;
};

class CBlender_postprocess_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_postprocess_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: combine to bloom target msaa"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_postprocess_msaa();
    ~CBlender_postprocess_msaa() override;
};

class CBlender_ssfx_bloom_build : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_build, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_build();
    ~CBlender_ssfx_bloom_build() override;
};

class CBlender_ssfx_bloom_lens : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_lens, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom Lens"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_lens();
    ~CBlender_ssfx_bloom_lens() override;
};

class CBlender_ssfx_bloom_downsample : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_downsample, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom Downsample"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_downsample();
    ~CBlender_ssfx_bloom_downsample() override;
};

class CBlender_ssfx_bloom_upsample : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_upsample, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom Upsample"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_upsample();
    ~CBlender_ssfx_bloom_upsample() override;
};
