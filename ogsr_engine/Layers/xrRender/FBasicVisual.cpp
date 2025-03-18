// dxRender_Visual.cpp: implementation of the dxRender_Visual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../../xr_3da/render.h"

#include "fbasicvisual.h"
#include "../../xr_3da/fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IRender_Mesh::~IRender_Mesh()
{
    _RELEASE(p_rm_Vertices);
    _RELEASE(p_rm_Indices);
}

dxRender_Visual::dxRender_Visual()
{
    Type = 0;
    shader = 0;
    vis.clear();
}

dxRender_Visual::~dxRender_Visual() {}

void dxRender_Visual::Release() {}

constexpr const char* vis_shaders_replace = "vis_shaders_replace";
constexpr const char* vis_shaders_override = "vis_shaders_override";

static bool replaceShadersLine(const char* N, char* fnS, u32 fnS_size, LPCSTR item)
{
    if (!pSettings->line_exist(vis_shaders_replace, item))
        return false;

    LPCSTR overrides = pSettings->r_string(vis_shaders_replace, item);
    u32 cnt = _GetItemCount(overrides);
    ASSERT_FMT(cnt % 2 == 0, "[%s]: %s: wrong format cnt = %u: %s = %s", __FUNCTION__, vis_shaders_replace, cnt, item, overrides);

    for (u32 i = 0; i < cnt; i += 2)
    {
        string256 s1, s2;
        _GetItem(overrides, i, s1);
        _GetItem(overrides, i + 1, s2);
        if (xr_strcmp(s1, fnS) == 0)
        {
            xr_strcpy(fnS, fnS_size, s2);
            // Msg("~~[%s][%s] replaced [%s] by [%s]", __FUNCTION__, N, s1, s2);
            break;
        }
    }

    return true;
}

static bool replaceShaders(const char* N, char* fnS, u32 fnS_size)
{
    if (!pSettings->section_exist(vis_shaders_replace))
        return false;

    if (replaceShadersLine(N, fnS, fnS_size, N))
        return true;

    std::string s{N};

    if (strchr(N, ':'))
    {
        s.erase(s.find(":"));
        if (replaceShadersLine(N, fnS, fnS_size, s.c_str()))
            return true;
    }

    while (xr_string_utils::SplitFilename(s))
    {
        if (replaceShadersLine(N, fnS, fnS_size, s.c_str()))
            return true;
    }

    return false;
}

static bool overrideShadersLine(const char* N, char* fnS, u32 fnS_size, const char* item)
{
    if (!pSettings->line_exist(vis_shaders_override, item))
        return false;

    LPCSTR overrides = pSettings->r_string(vis_shaders_override, item);
    u32 cnt = _GetItemCount(overrides);
    ASSERT_FMT(cnt % 2 == 0, "[%s]: %s: wrong format cnt = %u: %s = %s", __FUNCTION__, vis_shaders_override, cnt, item, overrides);

    for (u32 i = 0; i < cnt; i += 2)
    {
        string256 s1, s2;
        _GetItem(overrides, i, s1);
        _GetItem(overrides, i + 1, s2);
        if (xr_strcmp(s1, fnS) == 0)
        {
            xr_strcpy(fnS, fnS_size, s2);
            // Msg("~~[%s][%s] overridden [%s] by [%s]", __FUNCTION__, N, s1, s2);
            break;
        }
    }

    return true;
}

static bool overrideShaders(const char* fnT, char* fnS, u32 fnS_size)
{
    if (!pSettings->section_exist(vis_shaders_override))
        return false;

    if (overrideShadersLine(fnT, fnS, fnS_size, fnT))
        return true;

    std::string s{fnT};

    if (strchr(fnT, ':'))
    {
        s.erase(s.find(":"));
        if (overrideShadersLine(fnT, fnS, fnS_size, s.c_str()))
            return true;
    }

    while (xr_string_utils::SplitFilename(s))
    {
        if (overrideShadersLine(fnT, fnS, fnS_size, s.c_str()))
            return true;
    }

    return false;
}

void dxRender_Visual::Load(const char* N, IReader* data, u32)
{
    IsHudVisual = RImplementation.hud_loading;
    dbg_name = N;

    // header
    VERIFY(data);
    ogf_header hdr;
    if (data->r_chunk_safe(OGF_HEADER, &hdr, sizeof(hdr)))
    {
        R_ASSERT2(hdr.format_version == xrOGF_FormatVersion, "Invalid visual version");
        Type = hdr.type;
        if (hdr.shader_id)
            shader = RImplementation.getShader(hdr.shader_id);
        vis.box.set(hdr.bb.min, hdr.bb.max);
        vis.sphere.set(hdr.bs.c, hdr.bs.r);
    }
    else
    {
        FATAL("Invalid visual");
    }

    // Shader
    if (data->find_chunk(OGF_TEXTURE))
    {
        string256 fnT, fnS;
        data->r_stringZ(fnT, sizeof(fnT));
        data->r_stringZ(fnS, sizeof(fnS));
        if (replaceShaders(N, fnS, sizeof fnS))
        {
            // Msg("~~[%s] replaced shaders for [%s]: %s", __FUNCTION__, N, fnS);
        }
        if (overrideShaders(fnT, fnS, sizeof(fnS)))
        {
            // Msg("~~[%s] overridden shaders for [%s]: %s", __FUNCTION__, N, fnS);
        }
        shader.create(fnS, fnT);
    }
}

#define PCOPY(a) a = pFrom->a
void dxRender_Visual::Copy(dxRender_Visual* pFrom)
{
    PCOPY(Type);
    PCOPY(shader);
    PCOPY(vis);
    PCOPY(dbg_name);
    PCOPY(IsHudVisual);
}
