#pragma once

#include "fhierrarhyvisual.h"

class FLOD : public FHierrarhyVisual
{
    RTTI_DECLARE_TYPEINFO(FLOD, FHierrarhyVisual);

private:
    typedef FHierrarhyVisual inherited;

public:
    struct _vertex
    {
        Fvector v;
        float tx, ty;
        u32 c_rgb_hemi; // rgb,hemi
        u8 c_sun;
    };
    static_assert(sizeof(_vertex) == 28);
    struct _face
    {
        _vertex v[4];
        Fvector N;
    };
    static_assert(sizeof(_face) == 124);
    struct _hw
    {
        Fvector p0;
        Fvector p1;
        Fvector n0;
        Fvector n1;
        u32 sun_af;
        float t0x, t0y;
        float t1x, t1y;
        u32 rgbh0;
        u32 rgbh1;
    };
    static_assert(sizeof(_hw) == 76);

    ref_geom geom;
    _face facets[8];
    float lod_factor;

public:
    ~FLOD() override = default;

    virtual void Render(CBackend&, float, bool) override; // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
    virtual void Load(LPCSTR N, IReader* data, u32 dwFlags);
    virtual void Copy(dxRender_Visual* pFrom);
};
