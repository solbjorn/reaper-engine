#ifndef R_BACKEND_RUNTIMEH
#define R_BACKEND_RUNTIMEH

#include "SH_Texture.h"
#include "SH_RT.h"

#include "../xrRenderDX10/dx10R_Backend_Runtime.h"
#include "../xrRenderDX10/StateManager/dx10State.h"

inline void R_xforms::set_c_w(CBackend& cmd_list, R_constant* C)
{
    c_w = C;
    cmd_list.set_c(C, m_w);
}

inline void R_xforms::set_c_invw(CBackend& cmd_list, R_constant* C)
{
    c_invw = C;
    apply_invw(cmd_list);
}

inline void R_xforms::set_c_v(CBackend& cmd_list, R_constant* C)
{
    c_v = C;
    cmd_list.set_c(C, m_v);
}

inline void R_xforms::set_c_p(CBackend& cmd_list, R_constant* C)
{
    c_p = C;
    cmd_list.set_c(C, m_p);
}

inline void R_xforms::set_c_wv(CBackend& cmd_list, R_constant* C)
{
    c_wv = C;
    cmd_list.set_c(C, m_wv);
}

inline void R_xforms::set_c_vp(CBackend& cmd_list, R_constant* C)
{
    c_vp = C;
    cmd_list.set_c(C, m_vp);
}

inline void R_xforms::set_c_wvp(CBackend& cmd_list, R_constant* C)
{
    c_wvp = C;
    cmd_list.set_c(C, m_wvp);
}

inline void CBackend::set_xform_world(const Fmatrix& M) { xforms.set_W(*this, M); }
inline void CBackend::set_xform_view(const Fmatrix& M) { xforms.set_V(*this, M); }
inline void CBackend::set_xform_project(const Fmatrix& M) { xforms.set_P(*this, M); }

IC const Fmatrix& CBackend::get_xform_world() { return xforms.get_W(); }
IC const Fmatrix& CBackend::get_xform_view() { return xforms.get_V(); }
IC const Fmatrix& CBackend::get_xform_project() { return xforms.get_P(); }

IC ID3DRenderTargetView* CBackend::get_RT(u32 ID) { return pRT[ID]; }
IC ID3DDepthStencilView* CBackend::get_ZB() { return pZB; }

ICF void CBackend::set_States(ID3DState* _state)
{
#ifdef DEBUG
    stat.states++;
#endif

    state = _state;
    state->Apply(*this);
}

IC void CBackend::set_Pass(SPass* P)
{
    XR_TRACY_ZONE_SCOPED();

    set_States(P->state);

    set_PS(P->ps);
    set_VS(P->vs);
    set_GS(P->gs);
    set_HS(P->hs);
    set_DS(P->ds);
    set_CS(P->cs);

    set_Constants(P->constants);
    set_Textures(P->T);
}

ICF void CBackend::set_Element(ShaderElement* S, u32 pass) { set_Pass(S->passes[pass]); }
ICF void CBackend::set_Shader(Shader* S, u32 pass) { set_Element(S->E[0], pass); }

#endif
