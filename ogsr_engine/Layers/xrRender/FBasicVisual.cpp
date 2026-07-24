// dxRender_Visual.cpp: implementation of the dxRender_Visual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../../xr_3da/Render.h"

#include "FBasicVisual.h"
#include "../../xr_3da/fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IRender_Mesh::~IRender_Mesh()
{
    _RELEASE(p_rm_Vertices);
    _RELEASE(p_rm_Indices);
}

dxRender_Visual::dxRender_Visual() { vis.clear(); }
dxRender_Visual::~dxRender_Visual() = default;

void dxRender_Visual::Release() {}

namespace xr
{
namespace
{
constexpr gsl::czstring vis_shaders_replace{"vis_shaders_replace"};
constexpr gsl::czstring vis_shaders_override{"vis_shaders_override"};

[[nodiscard]] bool replaceShadersLine(gsl::zstring fnS, std::size_t fnS_size, gsl::czstring item)
{
    if (!pSettings->line_exist(xr::vis_shaders_replace, item))
        return false;

    LPCSTR overrides = pSettings->r_string(xr::vis_shaders_replace, item);
    u32 cnt = _GetItemCount(overrides);
    XR_ASSERT(xr::is_aligned(cnt, 2u), "wrong format for shaders replacement", xr::vis_shaders_replace, cnt, item, overrides);

    for (u32 i = 0; i < cnt; i += 2)
    {
        string256 s1, s2;
        std::ignore = _GetItem(overrides, i, s1);
        std::ignore = _GetItem(overrides, i + 1, s2);
        if (xr_strcmp(s1, fnS) == 0)
        {
            xr_strcpy(fnS, fnS_size, s2);
            break;
        }
    }

    return true;
}

void replaceShaders(gsl::czstring N, gsl::zstring fnS, std::size_t fnS_size)
{
    if (!pSettings->section_exist(xr::vis_shaders_replace))
        return;

    if (xr::replaceShadersLine(fnS, fnS_size, N))
        return;

    xr_string s{N};

    if (strchr(N, ':'))
    {
        s.erase(s.find(":"));

        if (xr::replaceShadersLine(fnS, fnS_size, s.c_str()))
            return;
    }

    while (xr_string_utils::SplitFilename(s))
    {
        if (xr::replaceShadersLine(fnS, fnS_size, s.c_str()))
            return;
    }
}

[[nodiscard]] bool overrideShadersLine(gsl::zstring fnS, std::size_t fnS_size, gsl::czstring item)
{
    if (!pSettings->line_exist(xr::vis_shaders_override, item))
        return false;

    LPCSTR overrides = pSettings->r_string(xr::vis_shaders_override, item);
    u32 cnt = _GetItemCount(overrides);
    XR_ASSERT(xr::is_aligned(cnt, 2u), "wrong format for shaders override", xr::vis_shaders_override, cnt, item, overrides);

    for (u32 i = 0; i < cnt; i += 2)
    {
        string256 s1, s2;
        std::ignore = _GetItem(overrides, i, s1);
        std::ignore = _GetItem(overrides, i + 1, s2);
        if (xr_strcmp(s1, fnS) == 0)
        {
            xr_strcpy(fnS, fnS_size, s2);
            break;
        }
    }

    return true;
}
} // namespace

void override_shaders(gsl::czstring fnT, gsl::zstring fnS, std::size_t fnS_size)
{
    if (!pSettings->section_exist(xr::vis_shaders_override))
        return;

    if (xr::overrideShadersLine(fnS, fnS_size, fnT))
        return;

    xr_string s{fnT};

    if (strchr(fnT, ':'))
    {
        s.erase(s.find(":"));

        if (xr::overrideShadersLine(fnS, fnS_size, s.c_str()))
            return;
    }

    while (xr_string_utils::SplitFilename(s))
    {
        if (xr::overrideShadersLine(fnS, fnS_size, s.c_str()))
            return;
    }
}
} // namespace xr

void dxRender_Visual::Load(const char* N, IReader* data, u32)
{
    IsHudVisual = RImplementation.hud_loading;
    dbg_name._set(N);

    // header
    XR_ASSERT(data != nullptr);
    ogf_header hdr;

    XR_ASSERT(data->r_chunk_safe(OGF_HEADER, &hdr, sizeof(hdr)), "invalid visual", N);
    XR_ASSERT(hdr.format_version == xrOGF_FormatVersion, "invalid visual version", N);

    Type = hdr.type;

    if (hdr.shader_id != 0)
        shader = RImplementation.getShader(hdr.shader_id);

    vis.box.set(hdr.bb.min, hdr.bb.max);
    vis.sphere.set(hdr.bs.c, hdr.bs.r);

    // Shader
    if (data->find_chunk(OGF_TEXTURE))
    {
        string256 fnT, fnS;
        data->r_stringZ(fnT, sizeof(fnT));
        data->r_stringZ(fnS, sizeof(fnS));

        xr::replaceShaders(N, fnS, sizeof(fnS));
        xr::override_shaders(fnT, fnS, sizeof(fnS));

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
    PCOPY(simplified_fast_geom);
}
