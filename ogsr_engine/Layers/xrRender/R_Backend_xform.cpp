#include "stdafx.h"

#include "R_Backend_xform.h"

void R_xforms::set_W(CBackend& cmd_list, const Fmatrix& m)
{
    m_w.set(m);
    m_wv.mul_43(m_v, m_w);
    m_wvp.mul(m_p, m_wv);

    if (c_w != nullptr)
        cmd_list.set_c(c_w, m_w);
    if (c_wv != nullptr)
        cmd_list.set_c(c_wv, m_wv);
    if (c_wvp != nullptr)
        cmd_list.set_c(c_wvp, m_wvp);

    m_bInvWValid = false;

    if (c_invw != nullptr)
        apply_invw(cmd_list);

    cmd_list.stat.xforms++;
}

void R_xforms::set_V(CBackend& cmd_list, const Fmatrix& m)
{
    m_v.set(m);
    m_invv.invert(m_v);
    m_wv.mul_43(m_v, m_w);
    m_vp.mul(m_p, m_v);
    m_wvp.mul(m_p, m_wv);

    if (c_v != nullptr)
        cmd_list.set_c(c_v, m_v);
    if (c_vp != nullptr)
        cmd_list.set_c(c_vp, m_vp);
    if (c_wv != nullptr)
        cmd_list.set_c(c_wv, m_wv);
    if (c_wvp != nullptr)
        cmd_list.set_c(c_wvp, m_wvp);

    cmd_list.stat.xforms++;
}

void R_xforms::set_P(CBackend& cmd_list, const Fmatrix& m)
{
    m_p.set(m);
    m_vp.mul(m_p, m_v);
    m_wvp.mul(m_p, m_wv);

    if (c_p != nullptr)
        cmd_list.set_c(c_p, m_p);
    if (c_vp != nullptr)
        cmd_list.set_c(c_vp, m_vp);
    if (c_wvp != nullptr)
        cmd_list.set_c(c_wvp, m_wvp);

    cmd_list.stat.xforms++;
}

void R_xforms::apply_invw(CBackend& cmd_list)
{
    if (!m_bInvWValid)
    {
        std::ignore = m_invw.invert_b(m_w);
        m_bInvWValid = true;
    }

    cmd_list.set_c(XR_ASSERT_VAL(c_invw != nullptr), m_invw);
}

void R_xforms::unmap()
{
    c_w = nullptr;
    c_invw = nullptr;
    c_v = nullptr;
    c_p = nullptr;
    c_wv = nullptr;
    c_vp = nullptr;
    c_wvp = nullptr;
}

R_xforms::R_xforms()
{
    unmap();

    m_w.identity();
    m_invw.identity();
    m_v.identity();
    m_p.identity();
    m_wv.identity();
    m_vp.identity();
    m_wvp.identity();

    m_bInvWValid = true;
}
