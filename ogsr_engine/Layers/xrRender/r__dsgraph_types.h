#pragma once

class dxRender_Visual;

namespace R_dsgraph
{
// Elementary types
struct _NormalItem
{
    dxRender_Visual* pVisual;

    constexpr _NormalItem() = default;
    constexpr explicit _NormalItem(dxRender_Visual* v) : pVisual{v} {}
};

struct XR_TRIVIAL _MatrixItem
{
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    Fmatrix Matrix; // matrix (copy)
    Fmatrix PrevMatrix;

    constexpr _MatrixItem() = default;
    constexpr explicit _MatrixItem(IRenderable* o, dxRender_Visual* v, Fmatrix& m) : pObject{o}, pVisual{v}, Matrix{m}, PrevMatrix{Fidentity} {}
    constexpr explicit _MatrixItem(IRenderable* o, dxRender_Visual* v, Fmatrix& m, Fmatrix& p) : pObject{o}, pVisual{v}, Matrix{m}, PrevMatrix{p} {}

    constexpr _MatrixItem(const _MatrixItem& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _MatrixItem(_MatrixItem&&) = default;
#else
    constexpr _MatrixItem(_MatrixItem&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr _MatrixItem& operator=(const _MatrixItem& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _MatrixItem& operator=(_MatrixItem&&) = default;
#else
    constexpr _MatrixItem& operator=(_MatrixItem&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(_MatrixItem);

struct XR_TRIVIAL _MatrixItemS
{
    float ssa;
    IRenderable* pObject;
    dxRender_Visual* pVisual;
    ShaderElement* se;
    Fmatrix Matrix; // matrix (copy)
    Fmatrix PrevMatrix;

    constexpr _MatrixItemS() = default;
    constexpr explicit _MatrixItemS(float s, dxRender_Visual* v, ShaderElement* e) : ssa{s}, pObject{nullptr}, pVisual{v}, se{e}, Matrix{Fidentity}, PrevMatrix{Fidentity} {}
    constexpr explicit _MatrixItemS(float s, IRenderable* o, dxRender_Visual* v, Fmatrix& m, ShaderElement* e)
        : ssa{s}, pObject{o}, pVisual{v}, se{e}, Matrix{m}, PrevMatrix{Fidentity}
    {}
    constexpr explicit _MatrixItemS(float s, IRenderable* o, dxRender_Visual* v, Fmatrix& m, Fmatrix& p, ShaderElement* e)
        : ssa{s}, pObject{o}, pVisual{v}, se{e}, Matrix{m}, PrevMatrix{p}
    {}

    constexpr _MatrixItemS(const _MatrixItemS& that) { xr_memcpy128(this, &that, sizeof(that)); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _MatrixItemS(_MatrixItemS&&) = default;
#else
    constexpr _MatrixItemS(_MatrixItemS&& that) { xr_memcpy128(this, &that, sizeof(that)); }
#endif

    constexpr _MatrixItemS& operator=(const _MatrixItemS& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _MatrixItemS& operator=(_MatrixItemS&&) = default;
#else
    constexpr _MatrixItemS& operator=(_MatrixItemS&& that)
    {
        xr_memcpy128(this, &that, sizeof(that));
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(_MatrixItemS);

struct XR_TRIVIAL alignas(16) _LodItem
{
    float ssa;
    dxRender_Visual* pVisual;

    constexpr _LodItem() = default;
    constexpr explicit _LodItem(float s, dxRender_Visual* v) : ssa{s}, pVisual{v} {}

    constexpr _LodItem(const _LodItem& that) { xr_memcpy16(this, &that); }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _LodItem(_LodItem&&) = default;
#else
    constexpr _LodItem(_LodItem&& that) { xr_memcpy16(this, &that); }
#endif

    constexpr _LodItem& operator=(const _LodItem& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }

#ifdef XR_TRIVIAL_BROKEN
    constexpr _LodItem& operator=(_LodItem&&) = default;
#else
    constexpr _LodItem& operator=(_LodItem&& that)
    {
        xr_memcpy16(this, &that);
        return *this;
    }
#endif
};
XR_TRIVIAL_ASSERT(_LodItem);

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
} // namespace R_dsgraph
