//---------------------------------------------------------------------------
#include "stdafx.h"

#include "du_box.h"

//---------------------------------------------------------------------------
const std::array<Fvector, DU_BOX_NUMVERTEX2> XR_ALIGNED_DEFAULT du_box_vertices2{
    Fvector{-0.5f, -0.5f, -0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, -0.5f}, Fvector{+0.5f, -0.5f, -0.5f},
    Fvector{-0.5f, -0.5f, -0.5f}, Fvector{+0.5f, -0.5f, -0.5f}, Fvector{+0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, +0.5f}, Fvector{+0.5f, +0.5f, +0.5f},
    Fvector{+0.5f, -0.5f, +0.5f}, Fvector{+0.5f, -0.5f, -0.5f}, Fvector{+0.5f, +0.5f, +0.5f}, Fvector{-0.5f, +0.5f, +0.5f}, Fvector{-0.5f, -0.5f, +0.5f},
    Fvector{+0.5f, +0.5f, +0.5f}, Fvector{-0.5f, -0.5f, +0.5f}, Fvector{+0.5f, -0.5f, +0.5f}, Fvector{-0.5f, +0.5f, +0.5f}, Fvector{-0.5f, +0.5f, -0.5f},
    Fvector{-0.5f, -0.5f, +0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{-0.5f, -0.5f, -0.5f}, Fvector{-0.5f, -0.5f, +0.5f}, Fvector{+0.5f, -0.5f, -0.5f},
    Fvector{-0.5f, -0.5f, +0.5f}, Fvector{-0.5f, -0.5f, -0.5f}, Fvector{+0.5f, -0.5f, -0.5f}, Fvector{+0.5f, -0.5f, +0.5f}, Fvector{-0.5f, -0.5f, +0.5f},
    Fvector{-0.5f, +0.5f, -0.5f}, Fvector{-0.5f, +0.5f, +0.5f}, Fvector{+0.5f, +0.5f, +0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, +0.5f},
    Fvector{+0.5f, +0.5f, -0.5f}};

const std::array<Fvector, DU_BOX_NUMVERTEX> XR_ALIGNED_DEFAULT du_box_vertices{
    Fvector{-0.5f, -0.5f, -0.5f}, Fvector{-0.5f, +0.5f, -0.5f}, Fvector{+0.5f, +0.5f, -0.5f}, Fvector{+0.5f, -0.5f, -0.5f},
    Fvector{-0.5f, +0.5f, +0.5f}, Fvector{-0.5f, -0.5f, +0.5f}, Fvector{+0.5f, +0.5f, +0.5f}, Fvector{+0.5f, -0.5f, +0.5f},
};

const std::array<u16, DU_BOX_NUMFACES * 3> XR_ALIGNED_DEFAULT du_box_faces{
    0, 1, 2, 2, 3, 0, 3, 2, 6, 6, 7, 3, 6, 4, 5, 6, 5, 7, 4, 1, 5, 1, 0, 5, 3, 5, 0, 3, 7, 5, // bottom
    1, 4, 6, 1, 6, 2 // top
};

const std::array<u16, DU_BOX_NUMLINES * 2> XR_ALIGNED_DEFAULT du_box_lines{0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 7, 6, 7, 6, 4, 0, 5, 1, 4, 2, 6, 3, 7};
