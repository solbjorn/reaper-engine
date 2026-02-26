#include "stdafx.h"

#include "TextureDescrManager.h"

#include "ETextureParams.h"

// eye-params
float r__dtex_range = 50;

class cl_dt_scaler : public R_constant_setup
{
    RTTI_DECLARE_TYPEINFO(cl_dt_scaler, R_constant_setup);

public:
    f32 scale;

    explicit cl_dt_scaler(f32 s) : scale{s} {}

    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, scale, scale, scale, 1.0f / r__dtex_range); }
};

tmc::task<void> CTextureDescrMngr::LoadLTX(gsl::czstring initial)
{
    FS_FileSet flist;
    std::ignore = FS.file_list(flist, initial, FS_ListFiles | FS_RootOnly, "*textures*.ltx");

    Msg("Count of *textures*.ltx files in [%s]: [%zu]", initial, flist.size());

    if (flist.empty())
        co_return;

    for (const auto& file : flist)
    {
        string_path fn;
        std::ignore = FS.update_path(fn, initial, file.name.c_str());

        CInifile ini{fn};
        tmc::mutex lock;

        if (ini.section_exist("association"))
        {
            const auto& Sect = ini.r_section("association");

            m_texture_details.reserve(m_texture_details.size() + Sect.Data.size());
            m_detail_scalers.reserve(m_detail_scalers.size() + Sect.Data.size());

            co_await tmc::spawn_many(Sect.Data | std::views::transform([this, &lock](const auto& item) -> tmc::task<void> {
                                         return [](auto& m_texture_details, auto& m_detail_scalers, auto& lock, const auto& item) -> tmc::task<void> {
                                             co_await lock;
                                             texture_desc& desc = m_texture_details[item.first];
                                             cl_dt_scaler*& dts = m_detail_scalers[item.first];
                                             co_await lock.co_unlock();

                                             if (desc.m_assoc)
                                                 xr_delete(desc.m_assoc);
                                             desc.m_assoc = xr_new<texture_assoc>();

                                             string_path T;
                                             f32 s;
                                             R_ASSERT(sscanf(item.second.c_str(), "%[^,],%f", T, &s) == 2);
                                             desc.m_assoc->detail_name._set(T);

                                             if (dts)
                                                 dts->scale = s;
                                             else
                                                 dts = xr_new<cl_dt_scaler>(s);

                                             if (strstr(item.second.c_str(), "usage[diffuse_or_bump]"))
                                             {
                                                 desc.m_assoc->usage.set(texture_assoc::flDiffuseDetail);
                                                 desc.m_assoc->usage.set(texture_assoc::flBumpDetail);
                                             }
                                             else if (strstr(item.second.c_str(), "usage[bump]"))
                                             {
                                                 desc.m_assoc->usage.set(texture_assoc::flBumpDetail);
                                             }
                                             else if (strstr(item.second.c_str(), "usage[diffuse]"))
                                             {
                                                 desc.m_assoc->usage.set(texture_assoc::flDiffuseDetail);
                                             }
                                         }(m_texture_details, m_detail_scalers, lock, item);
                                     }))
                .with_priority(xr::tmc_priority_any);
        }

        if (ini.section_exist("specification"))
        {
            const auto& Sect = ini.r_section("specification");
            m_texture_details.reserve(m_texture_details.size() + Sect.Data.size());

            co_await tmc::spawn_many(Sect.Data | std::views::transform([this, &lock](const auto& item) -> tmc::task<void> {
                                         return [](auto& m_texture_details, auto& lock, const auto& item) -> tmc::task<void> {
                                             co_await lock;
                                             texture_desc& desc = m_texture_details[item.first];
                                             co_await lock.co_unlock();

                                             if (desc.m_spec)
                                                 xr_delete(desc.m_spec);
                                             desc.m_spec = xr_new<texture_spec>();

                                             string_path bmode, bparallax;
                                             bmode[0] = '\0';
                                             bparallax[0] = '\0';
                                             R_ASSERT(sscanf(item.second.c_str(), "bump_mode[%[^]]], material[%f], parallax[%[^]]", bmode, &desc.m_spec->m_material, bparallax) >=
                                                      2);

                                             if ((bmode[0] == 'u') && (bmode[1] == 's') && (bmode[2] == 'e') && (bmode[3] == ':')) // bump-map specified
                                                 desc.m_spec->m_bump_name._set(bmode + 4);

                                             desc.m_spec->m_use_steep_parallax = (bparallax[0] == 'y') && (bparallax[1] == 'e') && (bparallax[2] == 's');
                                         }(m_texture_details, lock, item);
                                     }))
                .with_priority(xr::tmc_priority_any);
        }
    }
}

tmc::task<void> CTextureDescrMngr::LoadTHM(gsl::czstring initial)
{
    FS_FileSet flist;
    std::ignore = FS.file_list(flist, initial, FS_ListFiles, "*.thm");

    Msg("Count of .thm files in [%s]: [%zu]", initial, flist.size());

    if (flist.empty())
        co_return;

    tmc::mutex lock;

    m_texture_details.reserve(m_texture_details.size() + flist.size());
    m_detail_scalers.reserve(m_detail_scalers.size() + flist.size());

    co_await tmc::spawn_many(flist | std::views::transform([this, initial, &lock](const auto& it) -> tmc::task<void> {
                                 return [](auto& m_texture_details, auto& m_detail_scalers, gsl::czstring initial, auto& lock, const auto& it) -> tmc::task<void> {
                                     string_path fn;
                                     std::ignore = FS.update_path(fn, initial, it.name.c_str());
                                     IReader* F = FS.r_open(fn);

                                     xr_strcpy(fn, it.name.c_str());
                                     fix_texture_name(fn);
                                     R_ASSERT(F->find_chunk_thm(THM_CHUNK_TYPE, fn));

                                     std::ignore = F->r_u32();
                                     STextureParams tp;
                                     tp.Load(*F, fn);
                                     FS.r_close(F);

                                     if (
#ifdef USE_SHOC_THM_FORMAT
                                         STextureParams::ttImage == tp.fmt || STextureParams::ttTerrain == tp.fmt || STextureParams::ttNormalMap == tp.fmt
#else
                                         STextureParams::ttImage == tp.type || STextureParams::ttTerrain == tp.type || STextureParams::ttNormalMap == tp.type
#endif
                                     )
                                     {
                                         co_await lock;
                                         texture_desc& desc = m_texture_details[fn];
                                         cl_dt_scaler*& dts = m_detail_scalers[fn];
                                         co_await lock.co_unlock();

                                         if (tp.detail_name.size() && tp.flags.is_any(STextureParams::flDiffuseDetail | STextureParams::flBumpDetail))
                                         {
                                             if (desc.m_assoc)
                                                 xr_delete(desc.m_assoc);

                                             desc.m_assoc = xr_new<texture_assoc>();
                                             desc.m_assoc->detail_name = tp.detail_name;

                                             if (dts)
                                                 dts->scale = tp.detail_scale;
                                             else
                                                 dts = xr_new<cl_dt_scaler>(tp.detail_scale);

                                             if (tp.flags.is(STextureParams::flDiffuseDetail))
                                                 desc.m_assoc->usage.set(texture_assoc::flDiffuseDetail);

                                             if (tp.flags.is(STextureParams::flBumpDetail))
                                                 desc.m_assoc->usage.set(texture_assoc::flBumpDetail);
                                         }
                                         if (desc.m_spec)
                                             xr_delete(desc.m_spec);

                                         desc.m_spec = xr_new<texture_spec>();
                                         desc.m_spec->m_material = (float)(tp.material) + (tp.material < 4 ? tp.material_weight : 0);
                                         desc.m_spec->m_use_steep_parallax = false;

                                         if (tp.bump_mode == STextureParams::tbmUse)
                                         {
                                             desc.m_spec->m_bump_name = tp.bump_name;
                                         }
                                         else if (tp.bump_mode == STextureParams::tbmUseParallax)
                                         {
                                             desc.m_spec->m_bump_name = tp.bump_name;
                                             desc.m_spec->m_use_steep_parallax = true;
                                         }
                                     }
                                 }(m_texture_details, m_detail_scalers, initial, lock, it);
                             }))
        .with_priority(xr::tmc_priority_any);
}

tmc::task<void> CTextureDescrMngr::Load()
{
#ifdef DEBUG
    CTimer TT;
    TT.Start();
#endif // #ifdef DEBUG

#ifdef USE_TEXTURES_LTX
    co_await LoadLTX("$game_textures$");
#endif
    co_await LoadTHM("$game_textures$");
#ifdef USE_TEXTURES_LTX
    co_await LoadLTX("$level$");
#endif
    co_await LoadTHM("$level$");

    m_texture_details.rehash(0);
    m_detail_scalers.rehash(0);

#ifdef DEBUG
    Msg("load time=%d ms", TT.GetElapsed_ms());
#endif // #ifdef DEBUG
}

void CTextureDescrMngr::UnLoad()
{
    for (auto& it : m_texture_details)
    {
        xr_delete(it.second.m_assoc);
        xr_delete(it.second.m_spec);
    }
    m_texture_details.clear();
}

CTextureDescrMngr::~CTextureDescrMngr()
{
    for (auto& it : m_detail_scalers)
        xr_delete(it.second);

    m_detail_scalers.clear();
}

shared_str CTextureDescrMngr::GetBumpName(const shared_str& tex_name) const
{
    auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_spec)
        {
            return I->second.m_spec->m_bump_name;
        }
    }

    return shared_str{""};
}

BOOL CTextureDescrMngr::UseSteepParallax(const shared_str& tex_name) const
{
    auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_spec)
        {
            return I->second.m_spec->m_use_steep_parallax;
        }
    }
    return FALSE;
}

float CTextureDescrMngr::GetMaterial(const shared_str& tex_name) const
{
    auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_spec)
        {
            return I->second.m_spec->m_material;
        }
    }
    return 1.0f;
}

void CTextureDescrMngr::GetTextureUsage(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const
{
    auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_assoc)
        {
            const auto& usage = I->second.m_assoc->usage;
            bDiffuse = usage.test(texture_assoc::flDiffuseDetail);
            bBump = usage.test(texture_assoc::flBumpDetail);
        }
    }
}

BOOL CTextureDescrMngr::GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup*& CS) const
{
    auto I = m_texture_details.find(tex_name);
    if (I != m_texture_details.end())
    {
        if (I->second.m_assoc)
        {
            texture_assoc* TA = I->second.m_assoc;
            res = TA->detail_name.c_str();
            auto It2 = m_detail_scalers.find(tex_name);
            CS = It2 == m_detail_scalers.end() ? nullptr : It2->second; // TA->cs;

            return TRUE;
        }
    }

    return FALSE;
}
