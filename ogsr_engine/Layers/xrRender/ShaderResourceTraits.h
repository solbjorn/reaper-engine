#pragma once

#include "ResourceManager.h"

template <typename T>
struct ShaderTypeTraits;

template <>
struct ShaderTypeTraits<SHS> final
{
    using MapType = CResourceManager::map_HS;
    using DXIface = ID3D11HullShader;

    [[nodiscard]] static constexpr const char* GetShaderExt() { return ".hs"; }
    [[nodiscard]] static constexpr const char* GetCompilationTarget() { return "hs_5_0"; }

    [[nodiscard]] static DXIface* CreateHWShader(const DWORD* buffer, size_t size)
    {
        DXIface* hs{};
        XR_ASSERT(xr::hr(HW.pDevice->CreateHullShader(buffer, size, nullptr, &hs)));
        return hs;
    }

    [[nodiscard]] static constexpr u32 GetShaderDest() { return RC_dest_hull; }
};

template <>
struct ShaderTypeTraits<SDS> final
{
    using MapType = CResourceManager::map_DS;
    using DXIface = ID3D11DomainShader;

    [[nodiscard]] static constexpr const char* GetShaderExt() { return ".ds"; }
    [[nodiscard]] static constexpr const char* GetCompilationTarget() { return "ds_5_0"; }

    [[nodiscard]] static DXIface* CreateHWShader(const DWORD* buffer, size_t size)
    {
        DXIface* hs{};
        XR_ASSERT(xr::hr(HW.pDevice->CreateDomainShader(buffer, size, nullptr, &hs)));
        return hs;
    }

    [[nodiscard]] static constexpr u32 GetShaderDest() { return RC_dest_domain; }
};

template <>
struct ShaderTypeTraits<SCS> final
{
    using MapType = CResourceManager::map_CS;
    using DXIface = ID3D11ComputeShader;

    [[nodiscard]] static constexpr const char* GetShaderExt() { return ".cs"; }
    [[nodiscard]] static constexpr const char* GetCompilationTarget() { return "cs_5_0"; }

    [[nodiscard]] static DXIface* CreateHWShader(const DWORD* buffer, size_t size)
    {
        DXIface* cs{};
        XR_ASSERT(xr::hr(HW.pDevice->CreateComputeShader(buffer, size, nullptr, &cs)));
        return cs;
    }

    [[nodiscard]] static constexpr u32 GetShaderDest() { return RC_dest_compute; }
};

template <>
inline CResourceManager::map_DS& CResourceManager::GetShaderMap()
{
    return m_ds;
}

template <>
inline CResourceManager::map_HS& CResourceManager::GetShaderMap()
{
    return m_hs;
}

template <>
inline CResourceManager::map_CS& CResourceManager::GetShaderMap()
{
    return m_cs;
}

constexpr inline auto D3DCOMPILE_FLAGS_DEFAULT{D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR};
constexpr inline auto D3DCOMPILE_FLAGS_DEBUG{D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG};

template <typename T>
inline T* CResourceManager::CreateShader(const char* name)
{
    auto& sh_map = GetShaderMap<typename ShaderTypeTraits<T>::MapType>();
    if (const auto I = sh_map.find(name); I != sh_map.end())
        return I->second;

    T* sh = xr_new<T>();
    sh->dwFlags |= xr_resource_flagged::RF_REGISTERED;
    sh_map.emplace(sh->set_name(name), sh);

    if (std::is_eq(xr::strcasecmp(name, "null")))
    {
        sh->sh = nullptr;
        return sh;
    }

    string_path shName;
    const char* pchr = strchr(name, '(');
    ptrdiff_t strSize = pchr ? pchr - name : xr_strlen(name);
    strncpy_s(shName, name, strSize);
    shName[strSize] = 0;

    // Open file
    string_path cname;
    strconcat(sizeof(cname), cname, RImplementation.getShaderPath(), shName, ShaderTypeTraits<T>::GetShaderExt());
    std::ignore = FS.update_path(cname, "$game_shaders$", cname);

    // duplicate and zero-terminate
    const auto file = XR_ASSERT_VAL(absl::WrapUnique(FS.r_open(cname)), "", cname);
    file->skip_bom(cname);

    // Select target
    LPCSTR c_target = ShaderTypeTraits<T>::GetCompilationTarget();
    LPCSTR c_entry = "main";

    DWORD Flags{D3DCOMPILE_FLAGS_DEFAULT};
    if (strstr(Core.Params, "-shadersdbg"))
        Flags |= D3DCOMPILE_FLAGS_DEBUG;

    // Compile
    XR_ASSERT(xr::hr(RImplementation.shader_compile(name, (const DWORD*)file->pointer(), file->elapsed(), c_entry, c_target, Flags, (void*&)sh)),
              "failed to compile shader", cname, c_target);

    return sh;
}

template <typename T>
inline void CResourceManager::DestroyShader(const T* sh)
{
    if (!(sh->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto& sh_map = GetShaderMap<typename ShaderTypeTraits<T>::MapType>();
    if (const auto I = sh_map.find(sh->cName); I == sh_map.end())
        Msg("! ERROR: Failed to find compiled shader '{}'", sh->cName);
    else
        sh_map.erase(I);
}
