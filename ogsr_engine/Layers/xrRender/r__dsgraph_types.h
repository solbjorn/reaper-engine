#pragma once

#include "../../xrCore/fixedmap.h"

class dxRender_Visual;

namespace R_dsgraph
{
// Elementary types
struct _NormalItem
{
    float ssa;
    dxRender_Visual* pVisual;
};

struct _MatrixItem
{
    float ssa;
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    Fmatrix Matrix; // matrix (copy)
    Fmatrix PrevMatrix;

    constexpr inline _MatrixItem() = default;
    constexpr inline _MatrixItem(const _MatrixItem& i) { xr_memcpy128(this, &i, sizeof(i)); }
    constexpr inline _MatrixItem(float s, IRenderable* o, dxRender_Visual* v, Fmatrix& m) : ssa(s), pObject(o), pVisual(v), Matrix(m), PrevMatrix(Fidentity) {}
    constexpr inline _MatrixItem(float s, IRenderable* o, dxRender_Visual* v, Fmatrix& m, Fmatrix& p) : ssa(s), pObject(o), pVisual(v), Matrix(m), PrevMatrix(p) {}

    constexpr inline _MatrixItem& operator=(const _MatrixItem& i)
    {
        xr_memcpy128(this, &i, sizeof(i));
        return *this;
    }
};

struct _MatrixItemS
{
    // Хак для использования списков инициализации
    // Не используем наследование
    // _MatrixItem begin
    float ssa;
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    Fmatrix Matrix; // matrix (copy)
    Fmatrix PrevMatrix;
    // _MatrixItem end
    ShaderElement* se;

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

struct _LodItem
{
    float ssa;
    dxRender_Visual* pVisual;
};

// NORMAL
using mapNormalDirect = xr_vector<_NormalItem>;

struct mapNormalItems : public mapNormalDirect
{
    float ssa;
};

using mapNormal_T = FixedMAP<SPass*, mapNormalItems>;
using mapNormalPasses_T = mapNormal_T[SHADER_PASSES_MAX];

// MATRIX
using mapMatrixDirect = xr_vector<_MatrixItem>;

struct mapMatrixItems : public mapMatrixDirect
{
    float ssa;
};

using mapMatrix_T = FixedMAP<SPass*, mapMatrixItems>;
using mapMatrixPasses_T = mapMatrix_T[SHADER_PASSES_MAX];

// Top level
typedef FixedMAP<float, _MatrixItemS> mapSorted_T;
typedef mapSorted_T::TNode mapSorted_Node;

typedef FixedMAP<float, _MatrixItemS> mapHUD_T;
typedef mapHUD_T::TNode mapHUD_Node;

typedef FixedMAP<float, _MatrixItemS> HUDMask_T;
typedef HUDMask_T::TNode HUDMask_Node;

typedef FixedMAP<float, _LodItem> mapLOD_T;
typedef mapLOD_T::TNode mapLOD_Node;

typedef FixedMAP<float, _MatrixItemS> mapLandscape_T;
typedef mapLandscape_T::TNode mapLandscape_Node;

typedef FixedMAP<float, _MatrixItemS> mapWater_T;
typedef mapWater_T::TNode mapWater_Node;
}; // namespace R_dsgraph
