#include "stdafx.h"

template <bool b_first>
class alignas(16) walker
{
public:
    u32 mask;
    Fvector center;
    Fvector size;
    Fbox box;
    ISpatial_DB* space;

    constexpr inline walker() = default;
    constexpr inline walker(const walker<b_first>& w) { xr_memcpy128(this, &w, sizeof(w)); }
    constexpr inline walker<b_first>& operator=(const walker<b_first>& w)
    {
        xr_memcpy128(this, &w, sizeof(w));
        return *this;
    }

    walker(ISpatial_DB* _space, u32 _mask, const Fvector& _center, const Fvector& _size)
    {
        mask = _mask;
        center = _center;
        size = _size;
        box.setb(center, size);
        space = _space;
    }

    void walk(ISpatial_NODE* N, Fvector& n_C, float n_R)
    {
        // box
        float n_vR = 2 * n_R;
        Fbox BB;
        BB.set(n_C.x - n_vR, n_C.y - n_vR, n_C.z - n_vR, n_C.x + n_vR, n_C.y + n_vR, n_C.z + n_vR);
        if (!BB.intersect(box))
            return;

        // test items
        for (auto& it : N->items)
        {
            ISpatial* S = it;
            if (0 == (S->spatial.type & mask))
                continue;

            Fvector& sC = S->spatial.sphere.P;
            float sR = S->spatial.sphere.R;
            Fbox sB;
            sB.set(sC.x - sR, sC.y - sR, sC.z - sR, sC.x + sR, sC.y + sR, sC.z + sR);
            if (!sB.intersect(box))
                continue;

            space->q_result->push_back(S);
            if constexpr (b_first)
                return;
        }

        // recurse
        float c_R = n_R / 2;
        for (u32 octant = 0; octant < 8; octant++)
        {
            if (0 == N->children[octant])
                continue;
            Fvector c_C;
            c_C.mad(n_C, c_spatial_offset[octant], c_R);
            walk(N->children[octant], c_C, c_R);
            if constexpr (b_first)
            {
                if (!space->q_result->empty())
                    return;
            }
        }
    }
};

void ISpatial_DB::q_box(xr_vector<ISpatial*>& R, u32 _o, u32 _mask, const Fvector& _center, const Fvector& _size)
{
    cs.Enter();
    q_result = &R;
    q_result->clear();
    if (_o & O_ONLYFIRST)
    {
        walker<true> W(this, _mask, _center, _size);
        W.walk(m_root, m_center, m_bounds);
    }
    else
    {
        walker<false> W(this, _mask, _center, _size);
        W.walk(m_root, m_center, m_bounds);
    }
    cs.Leave();
}

void ISpatial_DB::q_sphere(xr_vector<ISpatial*>& R, u32 _o, u32 _mask, const Fvector& _center, const float _radius)
{
    Fvector _size = {_radius, _radius, _radius};
    q_box(R, _o, _mask, _center, _size);
}
