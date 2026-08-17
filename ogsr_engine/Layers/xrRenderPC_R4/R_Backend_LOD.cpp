#include "stdafx.h"

#include "R_Backend_LOD.h"

R_LOD::R_LOD() { unmap(); }

void R_LOD::set_LOD(CBackend& cmd_list, f32 LOD)
{
    if (c_LOD != nullptr)
    {
        const auto factor = clampr(ceil(LOD * LOD * LOD * LOD * LOD * 8.0f), 1.0f, 7.0f);
        cmd_list.set_c(c_LOD, factor);
    }
}
