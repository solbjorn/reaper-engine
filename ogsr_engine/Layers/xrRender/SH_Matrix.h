#ifndef SH_MATRIX_H
#define SH_MATRIX_H
#pragma once

#include "../../xr_3da/WaveForm.h"

class IReader;
class IWriter;

class ECORE_API CMatrix : public xr_resource_named
{
public:
    enum
    {
        modeProgrammable = 0,
        modeTCM,
        modeS_refl,
        modeC_refl,
        modeDetail
    };
    enum
    {
        tcmScale = (1 << 0),
        tcmRotate = (1 << 1),
        tcmScroll = (1 << 2),
        tcmFORCE32 = u32(-1)
    };

public:
    Fmatrix xform;

    u32 dwFrame;
    u32 dwMode;
    union
    {
        u32 tcm; // mask for tc-modifiers
        Flags32 tcm_flags;
    };
    WaveForm scaleU, scaleV;
    WaveForm rotate;
    WaveForm scrollU, scrollV;

    CMatrix() { Memory.mem_fill(this, 0, sizeof(CMatrix)); }

    constexpr inline void tc_trans(Fmatrix& T, float u, float v)
    {
        T.mm.r[0] = DirectX::g_XMIdentityR0.v;
        T.mm.r[1] = DirectX::g_XMIdentityR1.v;
        T.vm[2].set(u, v, 1, 0);
        T.mm.r[3] = DirectX::g_XMIdentityR3.v;
    }
    void Calculate();

    IC BOOL Similar(CMatrix& M) // comare by modes and params
    {
        if (dwMode != M.dwMode)
            return FALSE;
        if (tcm != M.tcm)
            return FALSE;
        if (!scaleU.Similar(M.scaleU))
            return FALSE;
        if (!scaleV.Similar(M.scaleV))
            return FALSE;
        if (!rotate.Similar(M.rotate))
            return FALSE;
        if (!scrollU.Similar(M.scrollU))
            return FALSE;
        if (!scrollV.Similar(M.scrollV))
            return FALSE;
        return TRUE;
    }

    void Load(IReader* fs);
    void Save(IWriter* fs);
};

typedef resptr_core<CMatrix, resptr_base<CMatrix>> ref_matrix;

#endif
