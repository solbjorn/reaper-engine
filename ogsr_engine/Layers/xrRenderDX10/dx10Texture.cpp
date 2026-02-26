#include "stdafx.h"

#include "stream_reader.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <DirectXTex.h>

XR_DIAG_POP();

namespace xr
{
namespace
{
enum class format : s32
{
    none = 0,
    dds,
    exr,
    ktx,
    sf
};

class istream final : public DirectX::InputStream
{
private:
    const std::unique_ptr<CStreamReader> file;

public:
    explicit istream(gsl::czstring path) : file{absl::WrapUnique(FS.rs_open(path))} { R_ASSERT(file); }
    ~istream() override = default;

    [[nodiscard]] bool Read(void* data, size_t size) override
    {
        const auto real = std::min(gsl::narrow_cast<gsl::index>(size), file->elapsed());
        file->r(data, real);
        return gsl::narrow_cast<size_t>(real) == size;
    }

    [[nodiscard]] bool Seek(size_t position) override
    {
        const auto real = std::min(gsl::narrow_cast<gsl::index>(position), file->length());
        file->seek(real);
        return gsl::narrow_cast<size_t>(real) == position;
    }

    [[nodiscard]] size_t Size() override { return gsl::narrow_cast<size_t>(file->length()); }
};

[[nodiscard]] xr::format find_texture(string_path& fn, std::span<const std::string_view> places, std::string_view fname)
{
    static constexpr std::array<std::pair<std::string_view, format>, 4> formats{
        {{".dds", xr::format::dds}, {".exr", xr::format::exr}, {".ktx", xr::format::ktx}, {".ktx2", xr::format::ktx}}};

    for (auto& ext : xr::fsgame::formats::texture)
    {
        for (auto& place : places)
        {
            if (!FS.exist(fn, place.data(), fname.data(), ext.data()))
                continue;

            if (const auto it = std::ranges::find(formats, ext, &decltype(formats)::value_type::first); it != formats.end())
                return it->second;

            return xr::format::sf;
        }
    }

    return xr::format::none;
}
} // namespace

bool texture_exists(string_path& fn, std::span<const std::string_view> places, std::string_view fname)
{
    return xr::find_texture(fn, places, fname) != xr::format::none;
}
} // namespace xr

void fix_texture_name(gsl::zstring fn)
{
    gsl::zstring _ext = strext(fn);
    if (_ext == nullptr)
        return;

    const std::string_view fext{_ext};

    for (auto& ext : xr::fsgame::formats::texture)
    {
        if (std::is_neq(xr::strcasecmp(fext, ext)))
            continue;

        *_ext = '\0';
        return;
    }

    for (const auto& ext : std::array<std::string_view, 4>{".seq", ".ogm", ".avi", ".thm"})
    {
        if (std::is_neq(xr::strcasecmp(fext, ext)))
            continue;

        *_ext = '\0';
        return;
    }
}

ID3DBaseTexture* CRender::texture_load(LPCSTR fRName, u32& ret_msize)
{
    // validation
    R_ASSERT(fRName && fRName[0]);

    // make file name
    string_path fname, fn;
    xr_strcpy(fname, fRName);
    fix_texture_name(fname);

    const std::string_view fview{fname};
    const bool bump = fview.contains("_bump");
    xr::format fmt;

    if (bump)
    {
        if (fmt = xr::find_texture(fn, std::array{xr::fsgame::level, xr::fsgame::game_textures}, fview); fmt == xr::format::none)
        {
            Msg("! Fallback to default bump map: [%s]", fname);

            if (fview.contains("_bump#"))
                fmt = xr::find_texture(fn, std::array{xr::fsgame::game_textures}, "ed\\ed_dummy_bump#");
            else
                fmt = xr::find_texture(fn, std::array{xr::fsgame::game_textures}, "ed\\ed_dummy_bump");

            R_ASSERT(fmt != xr::format::none);
        }
    }
    else if (fmt = xr::find_texture(fn, std::array{xr::fsgame::level, xr::fsgame::game_textures, xr::fsgame::game_saves}, fview); fmt == xr::format::none)
    {
        Msg("! Can't find texture [%s]", fname);

        fmt = xr::find_texture(fn, std::array{xr::fsgame::game_textures}, "ed\\ed_not_existing_texture");
        R_ASSERT(fmt != xr::format::none);
    }

    switch (fmt)
    {
    case xr::format::dds: return texture_load_dds(fn, ret_msize);
    case xr::format::exr: return texture_load_exr(fn, ret_msize);
    case xr::format::ktx: return texture_load_ktx(fn, ret_msize);
    case xr::format::sf: return texture_load_sf(fn, ret_msize, bump || Resources->m_textures_description.contains(fview));
    default: NODEFAULT;
    }
}

ID3DBaseTexture* CRender::texture_load_dds(const string_path& path, u32& size)
{
    DirectX::DDS_FLAGS dds_flags{DirectX::DDS_FLAGS_PERMISSIVE};
    bool allowFallback{true};
    xr::istream is{path};

    do
    {
        std::ignore = is.Seek(0);

        DirectX::ScratchImage texture;
        DirectX::TexMetadata meta;

        if (const auto hr = DirectX::LoadFromDDSStream(is, dds_flags, &meta, texture); FAILED(hr))
        {
            Msg("! Failed to load DDS texture: [%s], error: [%ld]", path, hr);
            return nullptr;
        }

        ID3DBaseTexture* pTexture2D;

        const auto hr = DirectX::CreateTextureEx(HW.pDevice, texture.GetImages(), texture.GetImageCount(), meta, D3D_USAGE_IMMUTABLE, D3D_BIND_SHADER_RESOURCE, 0, meta.miscFlags,
                                                 DirectX::CREATETEX_DEFAULT, &pTexture2D);
        if (SUCCEEDED(hr))
        {
            // Получилось. Считаем сколько весит текстура и сваливаем.
            size = texture.GetImages()[0].slicePitch * meta.arraySize * meta.depth;
            return pTexture2D;
        }

        if (!allowFallback)
        {
            Msg("! Failed to create DDS texture: [%s], error: [%ld]", path, hr);
            return nullptr; // Уже была вторая попытка, прекращаем.
        }

        // Помянем, не получилось загрузить текстуру...
        // Давай заново, с конвертацией текстур. Может помочь.
        dds_flags |= DirectX::DDS_FLAGS::DDS_FLAGS_NO_16BPP | DirectX::DDS_FLAGS_FORCE_RGB;
        allowFallback = false;
    } while (true);
}
