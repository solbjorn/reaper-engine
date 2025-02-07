#include "stdafx.h"
#include "../../xr_3da/fmesh.h"
#include "flod.h"

static constexpr D3DVERTEXELEMENT9 dwDecl[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // pos-0
                                               {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1}, // pos-1
                                               {0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0}, // nrm-0
                                               {0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1}, // nrm-1
                                               {0, 48, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0}, // factors
                                               {0, 52, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, // uv
                                               {0, 60, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1}, // uv
                                               {0, 68, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2}, // rgbh-0
                                               {0, 72, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3}, // rgbh-1
                                               D3DDECL_END()};

void FLOD::Load(LPCSTR N, IReader* data, u32 dwFlags)
{
    inherited::Load(N, data, dwFlags);

    // LOD-def
    R_ASSERT(data->find_chunk(OGF_LODDEF2));
    for (int f = 0; f < 8; f++)
    {
        data->r(facets[f].v, sizeof(facets[f].v));
        _vertex* v = facets[f].v;

        Fvector N, T;
        N.set(0, 0, 0);
        T.mknormal(v[0].v, v[1].v, v[2].v);
        N.add(T);
        T.mknormal(v[1].v, v[2].v, v[3].v);
        N.add(T);
        T.mknormal(v[2].v, v[3].v, v[0].v);
        N.add(T);
        T.mknormal(v[3].v, v[0].v, v[1].v);
        N.add(T);
        N.div(4.f);
        facets[f].N.normalize(N);
        facets[f].N.invert();
    }

    // VS
    geom.create(dwDecl, RCache.Vertex.Buffer(), RCache.QuadIB);

    // lod correction
    Fvector3 S;
    vis.box.getradius(S);
    float r = vis.sphere.R;
    std::sort(&S.x, &S.x + 3);
    float a = S.y;
    float Sf = 4.f * (0.5f * (r * r * asin(a / r) + a * _sqrt(r * r - a * a)));
    float Ss = M_PI * r * r;
    lod_factor = Sf / Ss;
}

void FLOD::Copy(dxRender_Visual* pFrom)
{
    inherited::Copy(pFrom);

    FLOD* F = (FLOD*)pFrom;
    geom = F->geom;
    lod_factor = F->lod_factor;
    CopyMemory(facets, F->facets, sizeof(facets));
}

void FLOD::Render(float LOD) {}
