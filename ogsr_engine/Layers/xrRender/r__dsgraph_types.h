#pragma once

class dxRender_Visual;

namespace R_dsgraph
{
// Elementary types
struct _NormalItem
{
    dxRender_Visual* pVisual;
};

struct _MatrixItem
{
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    Fmatrix Matrix; // matrix (copy)
    Fmatrix PrevMatrix;

    constexpr inline _MatrixItem() = default;
    constexpr inline _MatrixItem(const _MatrixItem& i) { xr_memcpy128(this, &i, sizeof(i)); }
    constexpr inline _MatrixItem(IRenderable* o, dxRender_Visual* v, Fmatrix& m) : pObject(o), pVisual(v), Matrix(m), PrevMatrix(Fidentity) {}
    constexpr inline _MatrixItem(IRenderable* o, dxRender_Visual* v, Fmatrix& m, Fmatrix& p) : pObject(o), pVisual(v), Matrix(m), PrevMatrix(p) {}

    constexpr inline _MatrixItem& operator=(const _MatrixItem& i)
    {
        xr_memcpy128(this, &i, sizeof(i));
        return *this;
    }
};

struct _MatrixItemS
{
    float ssa;
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    ShaderElement* se;
    Fmatrix Matrix; // matrix (copy)
    Fmatrix PrevMatrix;

    constexpr inline _MatrixItemS() = default;
    constexpr inline _MatrixItemS(const _MatrixItemS& i) { xr_memcpy128(this, &i, sizeof(i)); }
    constexpr inline _MatrixItemS(float s, dxRender_Visual* v, ShaderElement* e) : ssa(s), pObject(nullptr), pVisual(v), Matrix(Fidentity), PrevMatrix(Fidentity), se(e) {}
    constexpr inline _MatrixItemS(float s, IRenderable* o, dxRender_Visual* v, Fmatrix& m, ShaderElement* e)
        : ssa(s), pObject(o), pVisual(v), Matrix(m), PrevMatrix(Fidentity), se(e)
    {}
    constexpr inline _MatrixItemS(float s, IRenderable* o, dxRender_Visual* v, Fmatrix& m, Fmatrix& p, ShaderElement* e)
        : ssa(s), pObject(o), pVisual(v), Matrix(m), PrevMatrix(p), se(e)
    {}

    constexpr inline _MatrixItemS& operator=(const _MatrixItemS& i)
    {
        xr_memcpy128(this, &i, sizeof(i));
        return *this;
    }
};

struct alignas(16) _LodItem
{
    float ssa;
    dxRender_Visual* pVisual;

    constexpr inline _LodItem() = default;
    constexpr inline _LodItem(const _LodItem& i) { xr_memcpy16(this, &i); }
    constexpr inline _LodItem(float s, dxRender_Visual* v) : ssa(s), pVisual(v) {}

    constexpr inline _LodItem& operator=(const _LodItem& i)
    {
        xr_memcpy16(this, &i);
        return *this;
    }
};

// NORMAL, key is SSA
using mapNormalDirect = xr_multimap<float, _NormalItem, std::greater<float>>;

struct mapNormalItems : public mapNormalDirect
{
    float ssa;
};

using mapNormal_T = xr_unordered_map<SPass*, mapNormalItems>;
using mapNormalPasses_T = mapNormal_T[SHADER_PASSES_MAX];

// MATRIX, key is SSA
using mapMatrixDirect = xr_multimap<float, _MatrixItem, std::greater<float>>;

struct mapMatrixItems : public mapMatrixDirect
{
    float ssa;
};

using mapMatrix_T = xr_unordered_map<SPass*, mapMatrixItems>;
using mapMatrixPasses_T = mapMatrix_T[SHADER_PASSES_MAX];

// Top level, key is square distance to camera
using map_item = xr_multimap<float, _MatrixItemS>;
using map_sorted = xr_multimap<float, _MatrixItemS, std::greater<float>>;
using map_lod = xr_multimap<float, _LodItem>;
}; // namespace R_dsgraph
