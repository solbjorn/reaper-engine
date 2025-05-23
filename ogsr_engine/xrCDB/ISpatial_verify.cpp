#include "stdafx.h"

class alignas(8) walker
{
public:
    u32 o_count{};
    u32 n_count{};

public:
    void walk(ISpatial_NODE* N, Fvector& n_C, float n_R)
    {
        // test items
        n_count += 1;
        o_count += N->items.size();

        // recurse
        float c_R = n_R / 2;
        for (u32 octant = 0; octant < 8; octant++)
        {
            if (0 == N->children[octant])
                continue;
            Fvector c_C;
            c_C.mad(n_C, c_spatial_offset[octant], c_R);
            walk(N->children[octant], c_C, c_R);
        }
    }
};

BOOL ISpatial_DB::verify()
{
    walker W;
    W.walk(m_root, m_center, m_bounds);
    BOOL bResult = (W.o_count == stat_objects) && (W.n_count == stat_nodes);
    VERIFY(bResult);
    return bResult;
}
