#include "stdafx.h"

#include "R_Backend_hemi.h"

R_hemi::R_hemi() { unmap(); }

void R_hemi::unmap()
{
    c_pos_faces = nullptr;
    c_neg_faces = nullptr;
    c_material = nullptr;
}

void R_hemi::set_pos_faces(CBackend& cmd_list, f32 posx, f32 posy, f32 posz)
{
    if (c_pos_faces != nullptr)
        cmd_list.set_c(c_pos_faces, posx, posy, posz, 0.0f);
}

void R_hemi::set_neg_faces(CBackend& cmd_list, f32 negx, f32 negy, f32 negz)
{
    if (c_neg_faces != nullptr)
        cmd_list.set_c(c_neg_faces, negx, negy, negz, 0.0f);
}

void R_hemi::set_material(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w)
{
    if (c_material != nullptr)
        cmd_list.set_c(c_material, x, y, z, w);
}
