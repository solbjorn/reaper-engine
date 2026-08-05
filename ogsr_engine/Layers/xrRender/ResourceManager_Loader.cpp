#include "stdafx.h"

#include "ResourceManager.h"

#include "blenders/Blender.h"

bool bShadersXrExport{false};

void CResourceManager::OnDeviceDestroy(BOOL)
{
    if (Device.b_is_Ready)
        return;

    m_textures_description.UnLoad();

    // Release blenders
    for (auto& b : m_blenders)
    {
        char* key = const_cast<char*>(b.first);
        xr_free(key);
        IBlenderXr::Destroy(b.second);
    }

    m_blenders.clear();

    // scripting
    LS_Unload();
}

tmc::task<void> CResourceManager::OnDeviceCreate()
{
    if (!Device.b_is_Ready)
        co_return;

    // scripting
    LS_Load();

    string_path fname;
    if (FS.exist(fname, _game_data_, "shaders.ltx"))
    {
        Msg("Loading shader file: [{}]", fname);
        LoadShaderLtxFile(fname);
    }
    else if (FS.exist(fname, _game_data_, "shaders_cop.xr") || FS.exist(fname, _game_data_, "shaders.xr"))
    {
        Msg("Loading shader file: [{}]", fname);
        LoadShaderFile(fname);
    }

    co_await m_textures_description.Load();
}

void CResourceManager::LoadShaderFile(LPCSTR fname)
{
    // Check if file is compressed already
    const auto F = XR_ASSERT_VAL(absl::WrapUnique(FS.r_open(fname)), "", fname);

    std::array<std::byte, 8> id;
    F->r(&id[0], std::ssize(id));
    XR_ASSERT(std::memcmp(id.data(), "shENGINE", id.size()) != 0, "unsupported blender library, probably compressed", fname);

    string_path ini_path;
    strcpy_s(ini_path, fname);

    if (const auto ext = strext(ini_path); ext != nullptr)
        *ext = '\0';

    strcat_s(ini_path, ".ltx");

    CInifile ini(ini_path, 0, 1, bShadersXrExport);

    // Load blenders
    if (IReader* fs = F->open_chunk(2))
    {
        IReader* chunk;
        int chunk_id = 0;

        while ((chunk = fs->open_chunk(chunk_id)) != nullptr)
        {
            CBlender_DESC desc;

            chunk->r(&desc, offsetof(CBlender_DESC, cName) + sizeof(desc.cName));
            // ignore PC name
            chunk->advance(sizeof(string32));
            // time, version, 2-byte pad
            chunk->r(&desc.cTime, 8);

            IBlenderXr* B = IBlenderXr::Create(desc.CLS);
            if (!B)
            {
                Msg("! Renderer doesn't support blender '{}'", desc.cName);
            }
            else
            {
                if (B->getDescription().version < desc.version)
                    Msg("! Version conflict in shader '{}'", desc.cName);

                chunk->seek(0);

                B->Load(*chunk, desc.version);

                // для конвертации в ltx
                if (bShadersXrExport)
                {
                    if (ini.section_exist(desc.cName))
                    {
                        Msg("~~Found existing section [{}] in [{}]. Replacing!", desc.cName, ini_path);
                        ini.remove_section(desc.cName);
                    }

                    B->SaveIni(&ini, desc.cName);
                }

                XR_ASSERT(m_blenders.insert_or_assign(xr_strdup(desc.cName), B).second, "duplicate shader name", desc.cName);
            }

            chunk->close();
            chunk_id += 1;
        }

        fs->close();
    }
}

void CResourceManager::LoadShaderLtxFile(LPCSTR fname)
{
    string_path ini_path;
    strcpy_s(ini_path, fname);

    CInifile ini(ini_path);

    for (const auto& it : ini.sections())
    {
        auto& name = it.first;

        const CLASS_ID cls = ini.r_clsid(name, "class");
        const u16 version = ini.r_u16(name, "version");

        IBlenderXr* B = IBlenderXr::Create(cls);
        if (!B)
        {
            Msg("! Renderer doesn't support blender '{}'", name);
        }
        else
        {
            if (B->getDescription().version < version)
                Msg("! Version conflict in shader '{}'", name);

            B->LoadIni(&ini, name.c_str());
            XR_ASSERT(m_blenders.insert_or_assign(xr_strdup(name.c_str()), B).second, "duplicate shader name", name);
        }
    }
}
