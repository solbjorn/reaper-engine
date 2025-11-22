// TextureManager.cpp: implementation of the CResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ResourceManager.h"

#include "blenders\blender.h"
#include "blenders\blender_recorder.h"
#include "tss.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Winconsistent-missing-destructor-override");
XR_DIAG_IGNORE("-Wunused-template");

#include <oneapi/tbb/parallel_for_each.h>

XR_DIAG_POP();

IBlender* CResourceManager::_GetBlender(LPCSTR Name)
{
    R_ASSERT(Name && Name[0]);

    auto I = m_blenders.find(Name);
    if (I == m_blenders.end())
        FATAL("!![%s] DX10: Shader [%s] not found in library.", __FUNCTION__, Name);

    return I->second;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CResourceManager::_ParseList(sh_list& dest, LPCSTR names)
{
    if (nullptr == names || 0 == names[0])
        names = "$null";

    dest.clear();
    const char* P = (const char*)names;
    svector<char, 128> N;

    while (*P)
    {
        if (*P == ',')
        {
            // flush
            N.push_back(0);
            _strlwr(N.data());

            fix_texture_name(N.data());
            dest.emplace_back(N.data());

            N.clear();
        }
        else
        {
            N.push_back(*P);
        }

        P++;
    }

    if (!N.empty())
    {
        // flush
        N.push_back(0);
        _strlwr(N.data());

        fix_texture_name(N.data());
        dest.emplace_back(N.data());
    }
}

ShaderElement* CResourceManager::_CreateElement(ShaderElement&& S)
{
    if (S.passes.empty())
        return nullptr;

    // Search equal in shaders array
    for (ShaderElement* elem : v_elements)
        if (S.equal(*elem))
            return elem;

    // Create _new_ entry
    ShaderElement* N = v_elements.emplace_back(xr_new<ShaderElement>());
    N->clone(std::move(S));
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return N;
}

void CResourceManager::_DeleteElement(const ShaderElement* S)
{
    if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_elements, S))
        return;
    Msg("! ERROR: Failed to find compiled 'shader-element'");
}

Shader* CResourceManager::Create(IBlender* B, const char* s_shader, const char* s_textures) { return _cpp_Create(B, s_shader, s_textures); }

Shader* CResourceManager::Create(const char* s_shader, const char* s_textures)
{
    //	TODO: DX10: When all shaders are ready switch to common path
    if (_lua_HasShader(s_shader))
        return _lua_Create(s_shader, s_textures);

    Shader* pShader = _cpp_Create(s_shader, s_textures);
    if (pShader != nullptr)
        return pShader;

    FATAL("Can't find %s", s_shader);
}

Shader* CResourceManager::_cpp_Create(const char* s_shader, const char* s_textures)
{
    //	TODO: DX10: When all shaders are ready switch to common path
    IBlender* pBlender = _GetBlender(s_shader ? s_shader : "null");
    if (!pBlender)
        return nullptr;
    return _cpp_Create(pBlender, s_shader, s_textures);
}

Shader* CResourceManager::_cpp_Create(IBlender* B, const char* s_shader, const char* s_textures)
{
    CBlender_Compile C;
    Shader S;

    // Access to template
    C.BT = B;
    C.bDetail = FALSE;
    C.HudElement = false;

    // Parse names
    _ParseList(C.L_textures, s_textures);

    if (RImplementation.hud_loading && RImplementation.o.ssfx_hud_raindrops)
        C.HudElement = true;

    // Compile element	(LOD0 - HQ)
    {
        C.iElement = SE_R1_NORMAL_HQ;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[SE_R1_NORMAL_HQ]._set(_CreateElement(std::move(E)));
    }

    // Compile element	(LOD1)
    {
        C.iElement = SE_R1_NORMAL_LQ;
        C.bDetail = m_textures_description.GetDetailTexture(C.L_textures[0], C.detail_texture, C.detail_scaler);
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[SE_R1_NORMAL_LQ]._set(_CreateElement(std::move(E)));
    }

    // Compile element
    {
        C.iElement = SE_R1_LPOINT;
        C.bDetail = FALSE;
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[SE_R1_LPOINT]._set(_CreateElement(std::move(E)));
    }

    // Compile element
    {
        C.iElement = SE_R1_LSPOT;
        C.bDetail = FALSE;
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[SE_R1_LSPOT]._set(_CreateElement(std::move(E)));
    }

    // Compile element
    {
        C.iElement = SE_R1_LMODELS;
        C.bDetail = TRUE; //.$$$ HACK :)
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[SE_R1_LMODELS]._set(_CreateElement(std::move(E)));
    }

    // Compile element
    {
        C.iElement = 5;
        C.bDetail = FALSE;
        ShaderElement E;
        C._cpp_Compile(&E);
        S.E[5]._set(_CreateElement(std::move(E)));
    }

    // Hacky way to remove from the HUD mask transparent stuff. ( Let's try something better later... )
    if (RImplementation.hud_loading)
    {
        if (strstr(s_shader, "lens"))
            S.E[0]->passes[0]->ps->dwFlags |= xr_resource_flagged::RF_HUD_DISABLED;
    }

    // Search equal in shaders array
    for (auto sh : v_shaders)
    {
        if (S.equal(sh))
            return sh;
    }

    // Create _new_ entry
    Shader* N = v_shaders.emplace_back(xr_new<Shader>());
    N->clone(std::move(S));
    N->dwFlags |= xr_resource_flagged::RF_REGISTERED;

    return N;
}

void CResourceManager::Delete(const Shader* S)
{
    if (0 == (S->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;
    if (reclaim(v_shaders, S))
        return;
    Msg("! ERROR: Failed to find complete shader");
}

void CResourceManager::DeferredUpload()
{
    if (!Device.b_is_Ready)
        return;

    Msg("CResourceManager::DeferredUpload [MT] -> START, size = [%zu]", m_textures.size());
    Msg("CResourceManager::DeferredUpload VRAM usage before:");

    u32 m_base = 0;
    u32 c_base = 0;
    u32 m_lmaps = 0;
    u32 c_lmaps = 0;

    _GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
    Msg("textures loaded size %f MB (%f bytes)", (float)(m_base + m_lmaps) / 1024 / 1024, (float)(m_base + m_lmaps));

    HW.DumpVideoMemoryUsage();

    // Теперь многопоточная загрузка текстур даёт очень существенный прирост скорости, проверено.
    oneapi::tbb::parallel_for_each(m_textures, [&](auto m_tex) { m_tex.second->Load(); });

    Msg("CResourceManager::DeferredUpload VRAM usage after:");

    _GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
    Msg("textures loaded size %f MB (%f bytes)", (float)(m_base + m_lmaps) / 1024 / 1024, (float)(m_base + m_lmaps));

    HW.DumpVideoMemoryUsage();

    Msg("CResourceManager::DeferredUpload -> END");
}

void CResourceManager::_GetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
    m_base = c_base = m_lmaps = c_lmaps = 0;

    map_Texture::iterator I = m_textures.begin();
    map_Texture::iterator E = m_textures.end();
    for (; I != E; I++)
    {
        u32 m = I->second->flags.memUsage;
        if (strstr(I->first, "lmap"))
        {
            c_lmaps++;
            m_lmaps += m;
        }
        else
        {
            c_base++;
            m_base += m;
        }
    }
}

void CResourceManager::_DumpMemoryUsage()
{
    xr_multimap<u32, std::pair<u32, shared_str>> mtex;

    // sort
    {
        map_Texture::iterator I = m_textures.begin();
        map_Texture::iterator E = m_textures.end();
        for (; I != E; I++)
        {
            u32 m = I->second->flags.memUsage;
            shared_str n = I->second->cName;
            mtex.emplace(m, std::make_pair(I->second->ref_count.load(), n));
        }
    }

    // dump
    {
        xr_multimap<u32, std::pair<u32, shared_str>>::iterator I = mtex.begin();
        xr_multimap<u32, std::pair<u32, shared_str>>::iterator E = mtex.end();
        for (; I != E; I++)
            Msg("* %4.1f : [%4u] %s", float(I->first) / 1024.f, I->second.first, I->second.second.c_str());
    }
}

xr_vector<ITexture*> CResourceManager::FindTexture(const char* Name) const
{
    R_ASSERT(Name != nullptr && xr_strlen(Name) > 0);

    string_path filename;
    strcpy_s(filename, Name);
    fix_texture_name(filename);

    char* ch = strchr(filename, '*');

    xr_vector<ITexture*> res;

    if (!ch) // no wildcard?
    {
        auto I = m_textures.find(filename);
        if (I != m_textures.end())
            res.emplace_back(smart_cast<ITexture*>(I->second));
    }
    else
    {
        // alpet: test for wildcard matching
        ch[0] = 0; // remove *

        for (const auto& [name, tex] : m_textures)
            if (strstr(name, filename))
                res.emplace_back(smart_cast<ITexture*>(tex));
    }

    return res;
}
