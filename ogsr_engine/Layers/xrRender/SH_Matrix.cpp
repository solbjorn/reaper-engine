#include "stdafx.h"

void CMatrix::Calculate()
{
    if (dwFrame == RDEVICE.dwFrame)
        return;
    dwFrame = RDEVICE.dwFrame;

    // Switch on mode
    switch (dwMode)
    {
    case modeProgrammable:
    case modeDetail: return;
    case modeTCM: {
        Fmatrix T;
        float sU = 1, sV = 1, t = RDEVICE.fTimeGlobal;
        tc_trans(xform, .5f, .5f);
        if (tcm & tcmRotate)
        {
            T.rotateZ(rotate.Calculate(t) * t);
            xform.mulA_43(T);
        }
        if (tcm & tcmScale)
        {
            sU = scaleU.Calculate(t);
            sV = scaleV.Calculate(t);
            T.scale(sU, sV, 1);
            xform.mulA_43(T);
        }
        if (tcm & tcmScroll)
        {
            float u = scrollU.Calculate(t) * t;
            float v = scrollV.Calculate(t) * t;
            u *= sU;
            v *= sV;
            tc_trans(T, u, v);
            xform.mulA_43(T);
        }
        tc_trans(T, -0.5f, -0.5f);
        xform.mulB_43(T);
    }
        return;
    case modeS_refl: {
        float Ux = .5f * RDEVICE.mView._11, Uy = .5f * RDEVICE.mView._21, Uz = .5f * RDEVICE.mView._31, Uw = .5f;
        float Vx = -.5f * RDEVICE.mView._12, Vy = -.5f * RDEVICE.mView._22, Vz = -.5f * RDEVICE.mView._32, Vw = .5f;

        xform.vm[0].set(Ux, Vx, 0, 0);
        xform.vm[1].set(Uy, Vy, 0, 0);
        xform.vm[2].set(Uz, Vz, 0, 0);
        xform.vm[3].set(Uw, Vw, 0, 0);
    }
        return;
    case modeC_refl: {
        Fmatrix M = RDEVICE.mView;
        M._41 = 0.f;
        M._42 = 0.f;
        M._43 = 0.f;
        xform.invert(M);
    }
        return;
    default: return;
    }
}

void CMatrix::Load(IReader* fs)
{
    dwMode = fs->r_u32();
    tcm = fs->r_u32();
    fs->r(&scaleU, sizeof(WaveForm));
    fs->r(&scaleV, sizeof(WaveForm));
    fs->r(&rotate, sizeof(WaveForm));
    fs->r(&scrollU, sizeof(WaveForm));
    fs->r(&scrollV, sizeof(WaveForm));
}

void CMatrix::Save(IWriter* fs)
{
    fs->w_u32(dwMode);
    fs->w_u32(tcm);
    fs->w(&scaleU, sizeof(WaveForm));
    fs->w(&scaleV, sizeof(WaveForm));
    fs->w(&rotate, sizeof(WaveForm));
    fs->w(&scrollU, sizeof(WaveForm));
    fs->w(&scrollV, sizeof(WaveForm));
}
