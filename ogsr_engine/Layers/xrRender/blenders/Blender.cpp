// Blender.cpp: implementation of the IBlender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define _USE_32BIT_TIME_T
#include <time.h>

#include "Blender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IBlender::IBlender()
{
    oPriority.min = 0;
    oPriority.max = 3;
    oPriority.value = 1;
    xr_strcpy(oT_Name, "$base0");
}

void IBlenderXr::WriteInteger(CInifile* ini_file, LPCSTR section, LPCSTR line, const xrP_INTEGER v) { ini_file->w_ivector3(section, line, Ivector3{v.value, v.min, v.max}); }

void IBlenderXr::WriteBool(CInifile* ini_file, LPCSTR section, LPCSTR line, const xrP_BOOL v) { ini_file->w_bool(section, line, v.value); }

void IBlenderXr::WriteToken(CInifile* ini_file, LPCSTR section, LPCSTR line, const xrP_TOKEN v) { ini_file->w_u32(section, line, v.IDselected); }

void IBlenderXr::ReadInteger(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_INTEGER& v)
{
    const Ivector3 r = ini_file->r_ivector3(section, line);
    v.value = r.x;
    v.min = r.y;
    v.max = r.z;
}

void IBlenderXr::ReadBool(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_BOOL& v) { v.value = ini_file->r_bool(section, line); }

void IBlenderXr::ReadToken(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_TOKEN& v) { v.IDselected = ini_file->r_u32(section, line); }

void IBlender::Compile(CBlender_Compile& C) { C.SetParams(oPriority.value, oStrictSorting.value ? true : false); }

void IBlenderXr::Save(IWriter& fs)
{
    string32 empty{};

    fs.w(&description, offsetof(CBlender_DESC, cName) + sizeof(description.cName));
    // write empty PC name
    fs.w(&empty, sizeof(empty));
    // time, version, 2-byte pad
    fs.w(&description.cTime, 8);

    xrPWRITE_MARKER(fs, "General");
    xrPWRITE_PROP(fs, "Priority", xrPID_INTEGER, oPriority);
    xrPWRITE_PROP(fs, "Strict sorting", xrPID_BOOL, oStrictSorting);
    xrPWRITE_MARKER(fs, "Base Texture");
    xrPWRITE_PROP(fs, "Name", xrPID_TEXTURE, oT_Name);
    string64 oT_xform{};
    xrPWRITE_PROP(fs, "Transform", xrPID_MATRIX, oT_xform);
}

void IBlenderXr::Load(IReader& fs, u16)
{
    fs.r(&description, offsetof(CBlender_DESC, cName) + sizeof(description.cName));
    // ignore PC name
    fs.advance(sizeof(string32));

    fs.r(&description.cTime, sizeof(description.cTime));
    // ignore version and 2-byte pad
    fs.advance(sizeof(u32));

    // Properties
    xrPREAD_MARKER(fs);
    xrPREAD_PROP(fs, xrPID_INTEGER, oPriority);
    xrPREAD_PROP(fs, xrPID_BOOL, oStrictSorting);
    xrPREAD_MARKER(fs);
    xrPREAD_PROP(fs, xrPID_TEXTURE, oT_Name);
    string64 oT_xform;
    xrPREAD_PROP(fs, xrPID_MATRIX, oT_xform);
}

void IBlenderXr::SaveIni(CInifile* ini_file, LPCSTR section)
{
    ini_file->w_clsid(section, "class", description.CLS);
    ini_file->w_u16(section, "version", description.version);

    WriteInteger(ini_file, section, "priority", oPriority);
    WriteBool(ini_file, section, "strict_sorting", oStrictSorting);

    ini_file->w_string(section, "base_name", oT_Name);
}

void IBlenderXr::LoadIni(CInifile* ini_file, LPCSTR section)
{
    description.CLS = ini_file->r_clsid(section, "class");

    ReadInteger(ini_file, section, "priority", oPriority);
    ReadBool(ini_file, section, "strict_sorting", oStrictSorting);

    strcpy_s(oT_Name, ini_file->r_string(section, "base_name"));
}
