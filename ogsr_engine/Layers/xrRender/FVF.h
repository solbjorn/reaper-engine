#ifndef _FVF_H_
#define _FVF_H_
#pragma once

//-----------------------------------------------------------------------------
#pragma pack(push, 4)
namespace FVF
{
struct alignas(16) L
{
    Fvector p;
    u32 color;
    IC void set(const L& src) { *this = src; };
    IC void set(float x, float y, float z, u32 C)
    {
        p.set(x, y, z);
        color = C;
    }
    IC void set(const Fvector& _p, u32 C)
    {
        p.set(_p);
        color = C;
    }
};
static_assert(sizeof(struct L) == 16);
const u32 F_L = D3DFVF_XYZ | D3DFVF_DIFFUSE;

struct V
{
    Fvector p;
    float tx, ty;
    IC void set(const V& src) { *this = src; };
    IC void set(float x, float y, float z, float u, float v)
    {
        p.set(x, y, z);
        tx = u;
        ty = v;
    }
    IC void set(const Fvector& _p, float u, float v)
    {
        p.set(_p);
        tx = u;
        ty = v;
    }
};
static_assert(sizeof(struct V) == 20);
const u32 F_V = D3DFVF_XYZ | D3DFVF_TEX1;

struct alignas(8) LIT
{
    Fvector p;
    u32 color;
    Fvector2 t;
    IC void set(const LIT& src) { *this = src; };
    IC void set(float x, float y, float z, u32 C, float u, float v)
    {
        p.set(x, y, z);
        color = C;
        t.set(u, v);
    }
    IC void set(const Fvector& _p, u32 C, float u, float v)
    {
        p.set(_p);
        color = C;
        t.set(u, v);
    }
};
static_assert(sizeof(struct LIT) == 24);
const u32 F_LIT = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

struct TL0uv
{
    float px, py, pz, pw;
    u32 color;
    IC void set(const TL0uv& src) { *this = src; };
    IC void set(float x, float y, u32 c) { set(x, y, .0001f, .9999f, c); };
    IC void set(int x, int y, u32 c) { set(float(x), float(y), .0001f, .9999f, c); };
    IC void set(float x, float y, float z, float w, u32 c)
    {
        px = x;
        py = y;
        pz = z;
        pw = w;
        color = c;
    };
    IC void transform(const Fvector& v, const Fmatrix& matSet)
    {
        // Transform it through the matrix set. Takes in mean projection.
        // Finally, scale the vertices to screen coords.
        // Note 1: device coords range from -1 to +1 in the viewport.
        // Note 2: the p.z-coordinate will be used in the z-buffer.
        pw = matSet._14 * v.x + matSet._24 * v.y + matSet._34 * v.z + matSet._44;
        px = (matSet._11 * v.x + matSet._21 * v.y + matSet._31 * v.z + matSet._41) / pw;
        py = -(matSet._12 * v.x + matSet._22 * v.y + matSet._32 * v.z + matSet._42) / pw;
        pz = (matSet._13 * v.x + matSet._23 * v.y + matSet._33 * v.z + matSet._43) / pw;
    };
};
static_assert(sizeof(struct TL0uv) == 20);
const u32 F_TL0uv = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

struct TL
{
    float px, py, pz, pw;
    u32 color;
    float uvx, uvy;
    IC void set(const TL& src) { *this = src; };
    IC void set(float x, float y, u32 c, Fvector2& t) { set(x, y, .0001f, .9999f, c, t.x, t.y); };
    IC void set(float x, float y, u32 c, float u, float v) { set(x, y, .0001f, .9999f, c, u, v); };
    IC void set(int x, int y, u32 c, float u, float v) { set(float(x), float(y), .0001f, .9999f, c, u, v); };
    IC void set(float x, float y, float z, float w, u32 c) { set(x, y, z, w, c, uvx, uvy); };
    IC void set(float x, float y, float z, float w, u32 c, float u, float v)
    {
        px = x;
        py = y;
        pz = z;
        pw = w;
        color = c;
        uvx = u;
        uvy = v;
    };
    IC void transform(const Fvector& v, const Fmatrix& matSet)
    {
        // Transform it through the matrix set. Takes in mean projection.
        // Finally, scale the vertices to screen coords.
        // Note 1: device coords range from -1 to +1 in the viewport.
        // Note 2: the p.z-coordinate will be used in the z-buffer.
        pw = matSet._14 * v.x + matSet._24 * v.y + matSet._34 * v.z + matSet._44;
        px = (matSet._11 * v.x + matSet._21 * v.y + matSet._31 * v.z + matSet._41) / pw;
        py = -(matSet._12 * v.x + matSet._22 * v.y + matSet._32 * v.z + matSet._42) / pw;
        pz = (matSet._13 * v.x + matSet._23 * v.y + matSet._33 * v.z + matSet._43) / pw;
    };
};
static_assert(sizeof(struct TL) == 28);
const u32 F_TL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

struct TL2uv
{
    float px, py, pz, pw;
    u32 color;
    float uv0x, uv0y, uv1x, uv1y;
    IC void set(const TL2uv& src) { *this = src; };
    IC void set(float x, float y, u32 c, Fvector2& t0, Fvector2& t1) { set(x, y, .0001f, .9999f, c, t0.x, t0.y, t1.x, t1.y); };
    IC void set(float x, float y, float z, float w, u32 c, Fvector2& t0, Fvector2& t1) { set(x, y, z, w, c, t0.x, t0.y, t1.x, t1.y); };
    IC void set(float x, float y, u32 c, float u, float v, float u2, float v2) { set(x, y, .0001f, .9999f, c, u, v, u2, v2); };
    IC void set(int x, int y, u32 c, float u, float v, float u2, float v2) { set(float(x), float(y), .0001f, .9999f, c, u, v, u2, v2); };
    IC void set(float x, float y, float z, float w, u32 c, float u, float v, float u2, float v2)
    {
        px = x;
        py = y;
        pz = z;
        pw = w;
        color = c;
        uv0x = u;
        uv0y = v;
        uv1x = u2;
        uv1y = v2;
    };
    IC void transform(const Fvector& v, const Fmatrix& matSet)
    {
        // Transform it through the matrix set. Takes in mean projection.
        // Finally, scale the vertices to screen coords.
        // Note 1: device coords range from -1 to +1 in the viewport.
        // Note 2: the p.z-coordinate will be used in the z-buffer.
        pw = matSet._14 * v.x + matSet._24 * v.y + matSet._34 * v.z + matSet._44;
        px = (matSet._11 * v.x + matSet._21 * v.y + matSet._31 * v.z + matSet._41) / pw;
        py = -(matSet._12 * v.x + matSet._22 * v.y + matSet._32 * v.z + matSet._42) / pw;
        pz = (matSet._13 * v.x + matSet._23 * v.y + matSet._33 * v.z + matSet._43) / pw;
    };
};
static_assert(sizeof(struct TL2uv) == 36);
const u32 F_TL2uv = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;

struct TL4uv
{
    float px, py, pz, pw;
    u32 color;
    float uv0x, uv0y, uv1x, uv1y, uv2x, uv2y, uv3x, uv3y;
    IC void set(const TL4uv& src) { *this = src; };
    IC void set(float x, float y, u32 c, Fvector2& t0, Fvector2& t1) { set(x, y, .0001f, .9999f, c, t0.x, t0.y, t1.x, t1.y); };
    IC void set(float x, float y, float z, float w, u32 c, Fvector2& t0, Fvector2& t1) { set(x, y, z, w, c, t0.x, t0.y, t1.x, t1.y); };
    IC void set(float x, float y, u32 c, float u, float v, float u2, float v2) { set(x, y, .0001f, .9999f, c, u, v, u2, v2); };
    IC void set(int x, int y, u32 c, float u, float v, float u2, float v2) { set(float(x), float(y), .0001f, .9999f, c, u, v, u2, v2); };
    IC void set(float x, float y, float z, float w, u32 c, float u, float v, float u2, float v2)
    {
        px = x;
        py = y;
        pz = z;
        pw = w;
        color = c;
        uv0x = u;
        uv0y = v;
        uv1x = u2;
        uv1y = v2;
    };
};
static_assert(sizeof(struct TL4uv) == 52);
const u32 F_TL4uv = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX4;
}; // namespace FVF
#pragma pack(pop)

//-----------------------------------------------------------------------------
#endif
