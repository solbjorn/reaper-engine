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
enum class texfmt : s32
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
    explicit istream(gsl::czstring path) : file{absl::WrapUnique(FS.rs_open(path))} { XR_ASSERT(file, "", path); }
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

[[nodiscard]] xr::texfmt find_texture(string_path& fn, std::span<const std::string_view> places, std::string_view fname)
{
    static constexpr std::array<std::pair<std::string_view, xr::texfmt>, 4> formats{
        {{".dds", xr::texfmt::dds}, {".exr", xr::texfmt::exr}, {".ktx", xr::texfmt::ktx}, {".ktx2", xr::texfmt::ktx}}};

    for (auto& ext : xr::fsgame::formats::texture)
    {
        for (auto& place : places)
        {
            if (!FS.exist(fn, place.data(), fname.data(), ext.data()))
                continue;

            if (const auto it = std::ranges::find(formats, ext, &decltype(formats)::value_type::first); it != formats.end())
                return it->second;

            return xr::texfmt::sf;
        }
    }

    return xr::texfmt::none;
}
} // namespace

bool texture_exists(string_path& fn, std::span<const std::string_view> places, std::string_view fname)
{
    return xr::find_texture(fn, places, fname) != xr::texfmt::none;
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

    for (const auto& ext : std::array<std::string_view, 3>{".seq", ".ogm", ".thm"})
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
    XR_ASSERT(fRName != nullptr && fRName[0] != '\0');

    // make file name
    string_path fname, fn;
    xr_strcpy(fname, fRName);
    fix_texture_name(fname);

    const std::string_view fview{fname};
    const bool bump = fview.contains("_bump");
    xr::texfmt fmt;

    if (bump)
    {
        if (fmt = xr::find_texture(fn, std::array{xr::fsgame::level, xr::fsgame::game_textures}, fview); fmt == xr::texfmt::none)
        {
            XR_LOG_ERROR("Fallback to default bump map: [{}]", fview);

            if (fview.contains("_bump#"))
                fmt = xr::find_texture(fn, std::array{xr::fsgame::game_textures}, "ed\\ed_dummy_bump#");
            else
                fmt = xr::find_texture(fn, std::array{xr::fsgame::game_textures}, "ed\\ed_dummy_bump");

            XR_ASSERT(fmt != xr::texfmt::none, "", fview);
        }
    }
    else if (fmt = xr::find_texture(fn, std::array{xr::fsgame::level, xr::fsgame::game_textures, xr::fsgame::game_saves}, fview); fmt == xr::texfmt::none)
    {
        XR_LOG_ERROR("Can't find texture [{}]", fview);
        fmt = XR_ASSERT_VAL(xr::find_texture(fn, std::array{xr::fsgame::game_textures}, "ed\\ed_not_existing_texture") != xr::texfmt::none, "", fview);
    }

    ID3DBaseTexture* ret;

    switch (fmt)
    {
    case xr::texfmt::dds: ret = texture_load_dds(fn, ret_msize); break;
    case xr::texfmt::exr: ret = texture_load_exr(fn, ret_msize); break;
    case xr::texfmt::ktx: ret = texture_load_ktx(fn, ret_msize); break;
    case xr::texfmt::sf: ret = texture_load_sf(fn, ret_msize, bump || Resources->m_textures_description.contains(fview)); break;
    default: xr::unreachable();
    }

    return XR_ASSERT_VAL(ret != nullptr, "", fn);
}

ID3DBaseTexture* CRender::texture_load_dds(const string_path& path, u32& size)
{
    DirectX::DDS_FLAGS dds_flags{DirectX::DDS_FLAGS::DDS_FLAGS_NONE};
    gsl::index allowFallback{2};
    xr::istream is{path};

    do
    {
        std::ignore = is.Seek(0);

        DirectX::ScratchImage texture;
        DirectX::TexMetadata meta;
        xr::hresult hr{E_FAIL};

        if (hr = xr::hr(DirectX::LoadFromDDSStream(is, dds_flags, &meta, texture)); !hr)
        {
            XR_LOG_ERROR("Failed to load DDS texture: [{}], error: {}", path, hr);

        fallback:
            switch (allowFallback)
            {
            case 2: dds_flags |= DirectX::DDS_FLAGS::DDS_FLAGS_PERMISSIVE; break;
            case 1: dds_flags |= DirectX::DDS_FLAGS::DDS_FLAGS_FORCE_RGB | DirectX::DDS_FLAGS::DDS_FLAGS_NO_16BPP; break;
            case 0: return nullptr;
            default: xr::unreachable();
            }

            --allowFallback;
            continue;
        }

        ID3DBaseTexture* pTexture2D;

        if (hr = xr::hr(DirectX::CreateTextureEx(HW.pDevice.Get(), texture.GetImages(), texture.GetImageCount(), meta, ::D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
                                                 ::D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, 0, meta.miscFlags, DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
                                                 &pTexture2D));
            !hr)
        {
            XR_LOG_ERROR("Failed to create DDS texture: [{}], error: {}", path, hr);
            goto fallback;
        }

        size = texture.GetImages()[0].slicePitch * meta.arraySize * meta.depth;

        return pTexture2D;
    } while (true);
}
