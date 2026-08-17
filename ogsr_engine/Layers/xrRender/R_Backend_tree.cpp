#include "stdafx.h"

#include "R_Backend_tree.h"

R_tree::R_tree() { unmap(); }

void R_tree::unmap()
{
    c_m_xform_v = nullptr;
    c_m_xform = nullptr;
    c_consts = nullptr;
    c_wave = nullptr;
    c_wind = nullptr;
    c_c_scale = nullptr;
    c_c_bias = nullptr;
    c_c_sun = nullptr;
}

void R_tree::set_m_xform_v(CBackend& cmd_list, const Fmatrix& mat)
{
    if (c_m_xform_v != nullptr)
        cmd_list.set_c(c_m_xform_v, mat);
}

void R_tree::set_m_xform(CBackend& cmd_list, const Fmatrix& mat)
{
    if (c_m_xform != nullptr)
        cmd_list.set_c(c_m_xform, mat);
}

void R_tree::set_consts(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w)
{
    if (c_consts != nullptr)
        cmd_list.set_c(c_consts, x, y, z, w);
}

void R_tree::set_wave(CBackend& cmd_list, const Fvector4& vec)
{
    if (c_wave != nullptr)
        cmd_list.set_c(c_wave, vec);
}

void R_tree::set_wind(CBackend& cmd_list, const Fvector4& vec)
{
    if (c_wind != nullptr)
        cmd_list.set_c(c_wind, vec);
}

void R_tree::set_c_scale(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w)
{
    if (c_c_scale != nullptr)
        cmd_list.set_c(c_c_scale, x, y, z, w);
}

void R_tree::set_c_bias(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w)
{
    if (c_c_bias != nullptr)
        cmd_list.set_c(c_c_bias, x, y, z, w);
}

void R_tree::set_c_sun(CBackend& cmd_list, f32 x, f32 y, f32 z, f32 w)
{
    if (c_c_sun != nullptr)
        cmd_list.set_c(c_c_sun, x, y, z, w);
}
