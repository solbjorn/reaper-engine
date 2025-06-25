#pragma once

class CBlender_bloom_build : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_bloom_build, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: combine to bloom target"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_bloom_build();
    virtual ~CBlender_bloom_build();
};

class CBlender_bloom_build_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_bloom_build_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: combine to bloom target msaa"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_bloom_build_msaa();
    virtual ~CBlender_bloom_build_msaa();
};

class CBlender_postprocess_msaa : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_postprocess_msaa, IBlender);

public:
    virtual LPCSTR getComment() { return "INTERNAL: combine to bloom target msaa"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_postprocess_msaa();
    virtual ~CBlender_postprocess_msaa();
};

class CBlender_ssfx_bloom_build : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_build, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_build();
    virtual ~CBlender_ssfx_bloom_build();
};

class CBlender_ssfx_bloom_lens : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_lens, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom Lens"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_lens();
    virtual ~CBlender_ssfx_bloom_lens();
};

class CBlender_ssfx_bloom_downsample : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_downsample, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom Downsample"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_downsample();
    virtual ~CBlender_ssfx_bloom_downsample();
};

class CBlender_ssfx_bloom_upsample : public IBlender
{
    RTTI_DECLARE_TYPEINFO(CBlender_ssfx_bloom_upsample, IBlender);

public:
    virtual LPCSTR getComment() { return "SSFX Bloom Upsample"; }

    virtual void Compile(CBlender_Compile& C);

    CBlender_ssfx_bloom_upsample();
    virtual ~CBlender_ssfx_bloom_upsample();
};
