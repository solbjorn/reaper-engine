// Frustum.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "xrCDB.h"

#include "fixedvector.h"

enum EFC_Visible : u32
{
    fcvNone = 0,
    fcvPartial,
    fcvFully,
};

#define FRUSTUM_MAXPLANES 12
#define FRUSTUM_P_LEFT (1 << 0)
#define FRUSTUM_P_RIGHT (1 << 1)
#define FRUSTUM_P_TOP (1 << 2)
#define FRUSTUM_P_BOTTOM (1 << 3)
#define FRUSTUM_P_NEAR (1 << 4)
#define FRUSTUM_P_FAR (1 << 5)

#define FRUSTUM_P_LRTB (FRUSTUM_P_LEFT | FRUSTUM_P_RIGHT | FRUSTUM_P_TOP | FRUSTUM_P_BOTTOM)
#define FRUSTUM_P_ALL (FRUSTUM_P_LRTB | FRUSTUM_P_NEAR | FRUSTUM_P_FAR)
#define FRUSTUM_SAFE (FRUSTUM_MAXPLANES * 4)

using sPoly = svector<Fvector, FRUSTUM_SAFE>;

#define mx 0
#define my 1
#define mz 2
#define Mx 3
#define My 4
#define Mz 5
constexpr inline u32 frustum_aabb_remap[8][6]{{Mx, My, Mz, mx, my, mz}, {Mx, My, mz, mx, my, Mz}, {Mx, my, Mz, mx, My, mz}, {Mx, my, mz, mx, My, Mz},
                                              {mx, My, Mz, Mx, my, mz}, {mx, My, mz, Mx, my, Mz}, {mx, my, Mz, Mx, My, mz}, {mx, my, mz, Mx, My, Mz}};
#undef mx
#undef my
#undef mz
#undef Mx
#undef My
#undef Mz

class CFrustum
{
public:
    struct fplane : public Fplane
    {
        u32 aabb_overlap_id; // [0..7]

        void cache();
    };

    std::array<fplane, FRUSTUM_MAXPLANES> planes;
    gsl::index p_count{};

    [[nodiscard]] constexpr EFC_Visible AABB_OverlapPlane(const fplane& P, const float* mM) const
    {
        // calc extreme pts (neg,pos) along normal axis (pos in dir of norm, etc.)
        const u32* id = frustum_aabb_remap[P.aabb_overlap_id];

        Fvector Neg;
        Neg.set(mM[id[3]], mM[id[4]], mM[id[5]]);
        if (P.classify(Neg) > 0)
            return fcvNone;

        Fvector Pos;
        Pos.set(mM[id[0]], mM[id[1]], mM[id[2]]);
        if (P.classify(Pos) <= 0)
            return fcvFully;

        return fcvPartial;
    }

    constexpr void _clear() { p_count = 0; }
    void _add(const Fplane& P);
    void _add(const Fvector& P1, const Fvector& P2, const Fvector& P3);

private:
    [[nodiscard]] constexpr auto plane_span() { return std::span{planes.data(), gsl::narrow_cast<size_t>(p_count)}; }
    [[nodiscard]] constexpr auto plane_span() const { return std::span{planes.data(), gsl::narrow_cast<size_t>(p_count)}; }

    void SimplifyPoly_AABB(sPoly& P, const Fplane& plane);

public:
    void CreateFromPoints(std::span<const Fvector> p, const Fvector& vBase);
    void CreateFromMatrix(const Fmatrix& M, u32 mask);
    void CreateFromPortal(sPoly& P, const Fvector& vBase, const Fmatrix& mFullXFORM);
    void CreateFromPlanes(std::span<const Fplane> p);

    [[nodiscard]] sPoly* ClipPoly(sPoly& src, sPoly& dest) const;
    [[nodiscard]] constexpr u32 getMask() const { return (1 << p_count) - 1; }

    [[nodiscard]] EFC_Visible testSphere(const Fvector& c, float r, u32& test_mask) const;
    [[nodiscard]] bool testSphere_dirty(const Fvector& c, float r) const;
    [[nodiscard]] EFC_Visible testAABB(const float* mM, u32& test_mask) const;
    [[nodiscard]] EFC_Visible testSAABB(const Fvector& c, float r, const float* mM, u32& test_mask) const;
};
