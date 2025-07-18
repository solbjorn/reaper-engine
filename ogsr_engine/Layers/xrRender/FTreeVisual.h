#pragma once

#include "FBasicVisual.h"

struct FSlideWindowItem;

class FTreeVisual : public dxRender_Visual, public IRender_Mesh
{
    RTTI_DECLARE_TYPEINFO(FTreeVisual, dxRender_Visual, IRender_Mesh);

private:
    struct _5color
    {
        Fvector rgb; // - all static lighting
        float hemi; // - hemisphere
        float sun; // - sun
    };

protected:
    _5color c_scale;
    _5color c_bias;
    Fmatrix xform;

public:
    virtual void Render(CBackend& cmd_list, float LOD, bool) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    virtual void Load(LPCSTR N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

    FTreeVisual(void);
    virtual ~FTreeVisual(void);
};

class FTreeVisual_ST : public FTreeVisual
{
    RTTI_DECLARE_TYPEINFO(FTreeVisual_ST, FTreeVisual);

public:
    typedef FTreeVisual inherited;

    FTreeVisual_ST(void);
    virtual ~FTreeVisual_ST(void);

    virtual void Render(CBackend& cmd_list, float LOD, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    virtual void Load(LPCSTR N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

private:
    FTreeVisual_ST(const FTreeVisual_ST& other);
    void operator=(const FTreeVisual_ST& other) = delete;
};

class FTreeVisual_PM : public FTreeVisual
{
    RTTI_DECLARE_TYPEINFO(FTreeVisual_PM, FTreeVisual);

public:
    typedef FTreeVisual inherited;

private:
    FSlideWindowItem* pSWI;
    u32 last_lod;

public:
    FTreeVisual_PM(void);
    virtual ~FTreeVisual_PM(void);

    virtual void Render(CBackend& cmd_list, float LOD, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    virtual void Load(LPCSTR N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
    virtual void Release();

private:
    FTreeVisual_PM(const FTreeVisual_PM& other);
    void operator=(const FTreeVisual_PM& other) = delete;
};

constexpr float FTreeVisual_tile = 16.f;
constexpr float FTreeVisual_quant = 32768.f / FTreeVisual_tile;
