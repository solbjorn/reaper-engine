// BlenderDefault.cpp: implementation of the CBlender_BmmD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "blender_BmmD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_BmmD::CBlender_BmmD()
{
    description.CLS = B_BmmD;
    xr_strcpy(oT2_Name, "$null");

    description.version = 3;
    xr_strcpy(oR_Name, "detail\\detail_grnd_grass"); //"$null");
    xr_strcpy(oG_Name, "detail\\detail_grnd_asphalt"); //"$null");
    xr_strcpy(oB_Name, "detail\\detail_grnd_earth"); //"$null");
    xr_strcpy(oA_Name, "detail\\detail_grnd_yantar"); //"$null");
}

CBlender_BmmD::~CBlender_BmmD() {}

void CBlender_BmmD::Save(IWriter& fs)
{
    IBlenderXr::Save(fs);
    xrPWRITE_MARKER(fs, "Detail map");
    xrPWRITE_PROP(fs, "Name", xrPID_TEXTURE, oT2_Name);
    string64 oT2_xform; // xform for secondary texture
    xrPWRITE_PROP(fs, "Transform", xrPID_MATRIX, oT2_xform);
    xrPWRITE_PROP(fs, "R2-R", xrPID_TEXTURE, oR_Name);
    xrPWRITE_PROP(fs, "R2-G", xrPID_TEXTURE, oG_Name);
    xrPWRITE_PROP(fs, "R2-B", xrPID_TEXTURE, oB_Name);
    xrPWRITE_PROP(fs, "R2-A", xrPID_TEXTURE, oA_Name);
}

void CBlender_BmmD::Load(IReader& fs, u16 version)
{
    IBlenderXr::Load(fs, version);
    string64 oT2_xform; // xform for secondary texture
    if (version < 3)
    {
        xrPREAD_MARKER(fs);
        xrPREAD_PROP(fs, xrPID_TEXTURE, oT2_Name);
        xrPREAD_PROP(fs, xrPID_MATRIX, oT2_xform);
    }
    else
    {
        xrPREAD_MARKER(fs);
        xrPREAD_PROP(fs, xrPID_TEXTURE, oT2_Name);
        xrPREAD_PROP(fs, xrPID_MATRIX, oT2_xform);
        xrPREAD_PROP(fs, xrPID_TEXTURE, oR_Name);
        xrPREAD_PROP(fs, xrPID_TEXTURE, oG_Name);
        xrPREAD_PROP(fs, xrPID_TEXTURE, oB_Name);
        xrPREAD_PROP(fs, xrPID_TEXTURE, oA_Name);
    }
}

void CBlender_BmmD::SaveIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::SaveIni(ini_file, section);

    ini_file->w_string(section, "detail_name", oT2_Name);
    ini_file->w_string(section, "r2_r", oR_Name);
    ini_file->w_string(section, "r2_g", oG_Name);
    ini_file->w_string(section, "r2_b", oB_Name);
    ini_file->w_string(section, "r2_a", oA_Name);
}

void CBlender_BmmD::LoadIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::LoadIni(ini_file, section);

    strcpy_s(oT2_Name, ini_file->r_string(section, "detail_name"));
    strcpy_s(oR_Name, ini_file->r_string(section, "r2_r"));
    strcpy_s(oG_Name, ini_file->r_string(section, "r2_g"));
    strcpy_s(oB_Name, ini_file->r_string(section, "r2_b"));
    strcpy_s(oA_Name, ini_file->r_string(section, "r2_a"));
}

//////////////////////////////////////////////////////////////////////////
// R3
//////////////////////////////////////////////////////////////////////////
#include "uber_deffer.h"

void CBlender_BmmD::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);
    // codepath is the same, only the shaders differ
    // ***only pixel shaders differ***
    C.SH->flags.isLandscape = FALSE;
    string256 mask;
    strconcat(sizeof(mask), mask, C.L_textures[0].c_str(), "_mask");
    bool z_prepass = ps_r2_ls_flags.test(R2FLAG_TERRAIN_PREPASS);
    switch (C.iElement)
    {
    case SE_R2_NORMAL_HQ: // deffer
        if (z_prepass)
        {
            C.SH->flags.bLandscape = TRUE;
            C.r_Pass("shadow_direct_base", "shadow_direct_base", FALSE, TRUE, TRUE);
            C.r_ColorWriteEnable(false, false, false, false);
            C.r_End();
        }

        if (RImplementation.o.ssfx_terrain)
        {
            C.SH->flags.isLandscape = TRUE;
            uber_deffer(C, true, "terrain", "terrain_high", false, oT2_Name[0] ? oT2_Name : 0, true, z_prepass);
        }
        else
        {
            uber_deffer(C, true, "impl", "impl", false, oT2_Name[0] ? oT2_Name : 0, true, z_prepass);
        }

        if (z_prepass)
            C.RS.SetRS(D3DRS_ZFUNC, D3DCMP_EQUAL);

        C.r_dx10Texture("s_mask", mask);
        C.r_dx10Texture("s_lmap", C.L_textures[1]);

        C.r_dx10Texture("s_dt_r", oR_Name);
        C.r_dx10Texture("s_dt_g", oG_Name);
        C.r_dx10Texture("s_dt_b", oB_Name);
        C.r_dx10Texture("s_dt_a", oA_Name);

        C.r_dx10Texture("s_dn_r", strconcat(sizeof(mask), mask, oR_Name, "_bump"));
        C.r_dx10Texture("s_dn_g", strconcat(sizeof(mask), mask, oG_Name, "_bump"));
        C.r_dx10Texture("s_dn_b", strconcat(sizeof(mask), mask, oB_Name, "_bump"));
        C.r_dx10Texture("s_dn_a", strconcat(sizeof(mask), mask, oA_Name, "_bump"));

        if (RImplementation.o.ssfx_terrain)
        {
            C.r_dx10Texture("s_dnE_r", strconcat(sizeof(mask), mask, oR_Name, "_bump#"));
            C.r_dx10Texture("s_dnE_g", strconcat(sizeof(mask), mask, oG_Name, "_bump#"));
            C.r_dx10Texture("s_dnE_b", strconcat(sizeof(mask), mask, oB_Name, "_bump#"));
            C.r_dx10Texture("s_dnE_a", strconcat(sizeof(mask), mask, oA_Name, "_bump#"));
        }

        C.r_dx10Texture("s_puddles_normal", "fx\\water_normal");
        C.r_dx10Texture("s_puddles_perlin", "fx\\puddles_perlin");
        C.r_dx10Texture("s_puddles_mask", strconcat(sizeof(mask), mask, C.L_textures[0].c_str(), "_puddles_mask"));
        C.r_dx10Texture("s_rainsplash", "fx\\water_sbumpvolume");

        C.r_dx10Sampler("smp_base");
        C.r_dx10Sampler("smp_linear");

        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);

        C.r_End();
        break;
    case SE_R2_NORMAL_LQ: // deffer
        if (z_prepass)
        {
            C.SH->flags.bLandscape = TRUE;
            C.r_Pass("shadow_direct_base", "shadow_direct_base", FALSE, TRUE, TRUE);
            C.r_ColorWriteEnable(false, false, false, false);
            C.r_End();
        }

        if (RImplementation.o.ssfx_terrain)
        {
            C.SH->flags.isLandscape = TRUE;
            uber_deffer(C, false, "base", "terrain_mid", false, oT2_Name[0] ? oT2_Name : 0, true, z_prepass);
        }
        else
        {
            // Vanilla
            uber_deffer(C, false, "base", "impl", false, oT2_Name[0] ? oT2_Name : 0, true, z_prepass);
        }

        if (z_prepass)
            C.RS.SetRS(D3DRS_ZFUNC, D3DCMP_EQUAL);

        C.r_dx10Texture("s_mask", mask);

        if (RImplementation.o.ssfx_terrain)
        {
            LPSTR LodTexture = strconcat(sizeof(mask), mask, C.L_textures[0].c_str(), "_lod_textures");
            string_path fn;
            if (FS.exist(fn, "$game_textures$", LodTexture, ".dds"))
            {
                C.r_dx10Texture("s_lod_texture", LodTexture);
            }
            else
            {
                C.r_dx10Texture("s_lod_texture", "terrain\\default_lod_textures");
            }
        }

        C.r_dx10Sampler("smp_base");
        C.r_dx10Sampler("smp_linear");

        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);

        C.r_End();
        break;
    case 3: // SSFX Low quality terrain
        if (z_prepass)
        {
            C.SH->flags.bLandscape = TRUE;
            C.r_Pass("shadow_direct_base", "shadow_direct_base", FALSE, TRUE, TRUE);
            C.r_ColorWriteEnable(false, false, false, false);
            C.r_End();
        }

        C.SH->flags.isLandscape = TRUE;

        uber_deffer(C, false, "base", "terrain_low", false, oT2_Name[0] ? oT2_Name : 0, true, z_prepass);
        if (z_prepass)
            C.RS.SetRS(D3DRS_ZFUNC, D3DCMP_EQUAL);

        C.r_dx10Sampler("smp_linear");

        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);

        C.r_End();
        break;

    case SE_R2_SHADOW: // smap
        if (RImplementation.o.ssfx_terrain)
        {
            C.r_Pass("shadow_direct_terrain", "dumb", FALSE, TRUE, TRUE, FALSE);
        }
        else
        {
            // Vanilla
            C.r_Pass("shadow_direct_base", "dumb", FALSE, TRUE, TRUE, FALSE);
        }

        C.r_dx10Texture("s_base", C.L_textures[0]);
        C.r_dx10Sampler("smp_base");
        C.r_dx10Sampler("smp_linear");
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
    }
}
