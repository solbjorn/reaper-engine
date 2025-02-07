// Blender_Vertex_aref.cpp: implementation of the CBlender_Detail_Still class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Blender_Detail_still.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Detail_Still::CBlender_Detail_Still()
{
    description.CLS = B_DETAIL;
    description.version = 0;
}

CBlender_Detail_Still::~CBlender_Detail_Still() {}

void CBlender_Detail_Still::Save(IWriter& fs)
{
    IBlenderXr::Save(fs);
    xrP_BOOL oBlend;
    xrPWRITE_PROP(fs, "Alpha-blend", xrPID_BOOL, oBlend);
}

void CBlender_Detail_Still::Load(IReader& fs, u16 version)
{
    IBlenderXr::Load(fs, version);
    xrP_BOOL oBlend;
    xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
}

void CBlender_Detail_Still::SaveIni(CInifile* ini_file, LPCSTR section) { IBlenderXr::SaveIni(ini_file, section); }

void CBlender_Detail_Still::LoadIni(CInifile* ini_file, LPCSTR section) { IBlenderXr::LoadIni(ini_file, section); }

//////////////////////////////////////////////////////////////////////////
// R3
//////////////////////////////////////////////////////////////////////////
#include "uber_deffer.h"
void CBlender_Detail_Still::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case SE_R2_NORMAL_HQ: // deffer wave
        uber_deffer(C, false, "detail_w", "base", true, 0, true);
        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);
        C.r_CullMode(D3DCULL_NONE);
        C.r_End();
        break;
    case SE_R2_NORMAL_LQ: // deffer still
        uber_deffer(C, false, "detail_s", "base", true, 0, true);
        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);
        C.r_CullMode(D3DCULL_NONE);
        C.r_End();
        break;
    }
}
