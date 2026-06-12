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
    FTreeVisual();
    ~FTreeVisual() override;

    void Render(CBackend& cmd_list, f32, bool) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
};

class FTreeVisual_ST : public FTreeVisual
{
    RTTI_DECLARE_TYPEINFO(FTreeVisual_ST, FTreeVisual);

public:
    typedef FTreeVisual inherited;

    FTreeVisual_ST();
    ~FTreeVisual_ST() override;

    void Render(CBackend& cmd_list, f32 LOD, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
};

class FTreeVisual_PM : public FTreeVisual
{
    RTTI_DECLARE_TYPEINFO(FTreeVisual_PM, FTreeVisual);

public:
    typedef FTreeVisual inherited;

private:
    FSlideWindowItem* pSWI{};
    u32 last_lod{};

public:
    FTreeVisual_PM();
    ~FTreeVisual_PM() override;

    void Render(CBackend& cmd_list, f32 LOD, bool use_fast_geo) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    void Load(gsl::czstring N, IReader* data, u32 dwFlags) override;
    void Copy(dxRender_Visual* pFrom) override;
    void Release() override;
};

constexpr inline f32 FTreeVisual_tile{16.0f};
constexpr inline f32 FTreeVisual_quant{32768.0f / FTreeVisual_tile};
