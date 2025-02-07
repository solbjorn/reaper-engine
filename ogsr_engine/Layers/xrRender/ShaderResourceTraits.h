#pragma once

#include "ResourceManager.h"

template <typename T>
struct ShaderTypeTraits;

template <>
struct ShaderTypeTraits<SHS>
{
    typedef CResourceManager::map_HS MapType;
    typedef ID3D11HullShader DXIface;

    static inline const char* GetShaderExt() { return ".hs"; }
    static inline const char* GetCompilationTarget() { return "hs_5_0"; }
    static inline DXIface* CreateHWShader(DWORD const* buffer, size_t size)
    {
        DXIface* hs = 0;
        R_CHK(HW.pDevice->CreateHullShader(buffer, size, NULL, &hs));
        return hs;
    }

    static inline u32 GetShaderDest() { return RC_dest_hull; }
};

template <>
struct ShaderTypeTraits<SDS>
{
    typedef CResourceManager::map_DS MapType;
    typedef ID3D11DomainShader DXIface;

    static inline const char* GetShaderExt() { return ".ds"; }
    static inline const char* GetCompilationTarget() { return "ds_5_0"; }
    static inline DXIface* CreateHWShader(DWORD const* buffer, size_t size)
    {
        DXIface* hs = 0;
        R_CHK(HW.pDevice->CreateDomainShader(buffer, size, NULL, &hs));
        return hs;
    }

    static inline u32 GetShaderDest() { return RC_dest_domain; }
};

template <>
struct ShaderTypeTraits<SCS>
{
    typedef CResourceManager::map_CS MapType;
    typedef ID3D11ComputeShader DXIface;

    static inline const char* GetShaderExt() { return ".cs"; }
    static inline const char* GetCompilationTarget() { return "cs_5_0"; }
    static inline DXIface* CreateHWShader(DWORD const* buffer, size_t size)
    {
        DXIface* cs = 0;
        R_CHK(HW.pDevice->CreateComputeShader(buffer, size, NULL, &cs));
        return cs;
    }

    static inline u32 GetShaderDest() { return RC_dest_compute; }
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

#define D3DCOMPILE_FLAGS_DEFAULT (D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR)
#define D3DCOMPILE_FLAGS_DEBUG (D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG)

template <typename T>
inline T* CResourceManager::CreateShader(const char* name)
{
    auto& sh_map = GetShaderMap<typename ShaderTypeTraits<T>::MapType>();
    auto I = sh_map.find(name);

    if (I != sh_map.end())
        return I->second;
    else
    {
        T* sh = xr_new<T>();

        sh->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        sh_map.emplace(sh->set_name(name), sh);
        if (0 == stricmp(name, "null"))
        {
            sh->sh = NULL;
            return sh;
        }

        string_path shName;
        const char* pchr = strchr(name, '(');
        ptrdiff_t strSize = pchr ? pchr - name : xr_strlen(name);
        strncpy_s(shName, name, strSize);
        shName[strSize] = 0;

        // Open file
        string_path cname;
        strconcat(sizeof(cname), cname, ::Render->getShaderPath(), /*name*/ shName, ShaderTypeTraits<T>::GetShaderExt());
        FS.update_path(cname, "$game_shaders$", cname);

        // duplicate and zero-terminate
        IReader* file = FS.r_open(cname);
        R_ASSERT2(file, cname);

        file->skip_bom(cname);

        // Select target
        LPCSTR c_target = ShaderTypeTraits<T>::GetCompilationTarget();
        LPCSTR c_entry = "main";

        DWORD Flags{D3DCOMPILE_FLAGS_DEFAULT};
        if (strstr(Core.Params, "-shadersdbg"))
            Flags |= D3DCOMPILE_FLAGS_DEBUG;

        // Compile
        HRESULT const _hr = ::Render->shader_compile(name, (DWORD const*)file->pointer(), file->elapsed(), c_entry, c_target, Flags, (void*&)sh);

        FS.r_close(file);

        ASSERT_FMT(!FAILED(_hr), "Can't compile shader [%s]", name);

        return sh;
    }
}

template <typename T>
inline void CResourceManager::DestroyShader(const T* sh)
{
    if (0 == (sh->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return;

    auto& sh_map = GetShaderMap<typename ShaderTypeTraits<T>::MapType>();

    LPSTR N = LPSTR(*sh->cName);
    auto I = sh_map.find(N);

    if (I != sh_map.end())
    {
        sh_map.erase(I);
        return;
    }

    Msg("! ERROR: Failed to find compiled geometry shader '%s'", *sh->cName);
}
