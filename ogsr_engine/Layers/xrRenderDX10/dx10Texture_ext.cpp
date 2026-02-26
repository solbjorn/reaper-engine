#include "stdafx.h"

#include "stream_reader.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <DirectXTex.h>

XR_DIAG_POP();

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wunused-parameter");

#include <ImfEnvmapAttribute.h>

XR_DIAG_POP();

#include <ImfIO.h>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wunused-parameter");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <ImfMultiPartInputFile.h>
#include <ImfRgbaFile.h>

XR_DIAG_POP();

#include <ImfTiledRgbaFile.h>
#include <ImfVersion.h>

#include <SFML/Graphics/Image.hpp>
#include <SFML/System/InputStream.hpp>

#define BASISU_SUPPORT_SSE 1

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <basisu_enc.h>

XR_DIAG_POP();

#undef BASISU_SUPPORT_SSE

namespace xr
{
namespace
{
enum class format : s32
{
    scanline = 0,
    tiled,
};

class istream final : public Imf::IStream, public sf::InputStream
{
private:
    const std::unique_ptr<CStreamReader> file;

public:
    explicit istream(gsl::czstring path) : Imf::IStream{path}, file{absl::WrapUnique(FS.rs_open(path))} { R_ASSERT(file); }
    ~istream() override = default;

    // Imf::IStream

    [[nodiscard]] bool read(char c[], s32 n) override
    {
        file->r(c, n);
        return !file->eof();
    }

    [[nodiscard]] u64 tellg() override { return gsl::narrow_cast<u64>(file->tell()); }
    void seekg(u64 pos) override { file->seek(gsl::narrow_cast<gsl::index>(pos)); }
    [[nodiscard]] s64 size() override { return file->length(); }

    [[nodiscard]] s64 read(void* buf, size_t sz, size_t offset) override { return Imf::IStream::read(buf, sz, offset); }

    // sf::InputStream

    [[nodiscard]] std::optional<size_t> read(void* data, size_t size) override
    {
        const auto real = std::min(gsl::narrow_cast<gsl::index>(size), file->elapsed());
        file->r(data, real);
        return gsl::narrow_cast<size_t>(real);
    }

    [[nodiscard]] std::optional<size_t> seek(size_t position) override
    {
        const auto real = std::min(gsl::narrow_cast<gsl::index>(position), file->length());
        file->seek(real);
        return gsl::narrow_cast<size_t>(real);
    }

    [[nodiscard]] std::optional<size_t> tell() override { return gsl::narrow_cast<size_t>(file->tell()); }
    [[nodiscard]] std::optional<size_t> getSize() override { return gsl::narrow_cast<size_t>(file->length()); }
};

[[nodiscard]] bool generate_mips(const DirectX::ScratchImage& texture, std::span<u8> mem, size_t num)
{
    auto& tsrc = texture.GetImages()[0];
    basisu::image src;
    bool ret;

    src.grant_ownership(reinterpret_cast<basisu::color_rgba*>(tsrc.pixels), tsrc.width, tsrc.height);

    for (size_t i{1}, off{0}; i < num; ++i)
    {
        auto& tdst = *texture.SetNonOwningImagePixels(i, 0, 0, &mem[off]);
        basisu::image dst;

        dst.grant_ownership(reinterpret_cast<basisu::color_rgba*>(tdst.pixels), tdst.width, tdst.height);
        ret = basisu::image_resample(src, dst, false, "kaiser");
        dst.get_pixels().assume_ownership();

        if (!ret)
            break;

        off += tdst.slicePitch;
    }

    src.get_pixels().assume_ownership();

    return ret;
}
} // namespace
} // namespace xr

ID3DBaseTexture* CRender::texture_load_exr(const string_path& path, u32& size)
{
    xr::istream is{path};

    const auto ver = Imf::MultiPartInputFile{is}.version();
    is.seekg(0);

    if (Imf::isMultiPart(ver))
    {
        Msg("! Unsupported multi-part EXR texture: [%s]", path);
        return nullptr;
    }

    if (Imf::isNonImage(ver))
    {
        Msg("! Unsupported deep EXR texture: [%s]", path);
        return nullptr;
    }

    DirectX::ScratchImage texture;
    DirectX::TexMetadata meta{};

    const auto fmt = Imf::isTiled(ver) ? xr::format::tiled : xr::format::scanline;
    switch (fmt)
    {
    case xr::format::scanline: {
        Imf::RgbaInputFile rgba{is};

        const auto dw = rgba.dataWindow();
        const auto width = dw.max.x - dw.min.x + 1;
        auto height = dw.max.y - dw.min.y + 1;

        if (width < 1 || height < 1)
        {
            Msg("! Failed to load scanline EXR texture: [%s]", path);
            return nullptr;
        }

        size_t arr{1};
        u32 misc{};

        if (const auto envmap = rgba.header().findTypedAttribute<Imf::EnvmapAttribute>("envmap"); envmap != nullptr)
        {
            if (envmap->value() == Imf::ENVMAP_CUBE && width == height / 6)
            {
                height = width;
                arr = 6;
                misc = DirectX::TEX_MISC_TEXTURECUBE;
            }
            else
            {
                Msg("! Unsupported scanline EXR texture envmap mode: [%s]", path);
                return nullptr;
            }
        }

        meta.width = width;
        meta.height = height;
        meta.depth = 1;
        meta.arraySize = arr;
        meta.mipLevels = 1;
        meta.miscFlags = misc;
        meta.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

        if (const auto hr = texture.Initialize(meta); FAILED(hr))
        {
            Msg("! Failed to initialize scanline EXR texture data: [%s], error: [%ld]", path, hr);
            return nullptr;
        }

        rgba.setFrameBuffer(reinterpret_cast<Imf::Rgba*>(texture.GetPixels()) - dw.min.x - dw.min.y * width, 1, width);
        rgba.readPixels(dw.min.y, dw.max.y);

        break;
    }
    case xr::format::tiled: {
        Imf::TiledRgbaInputFile rgba{is};

        const auto dw = rgba.dataWindow();
        auto width = dw.max.x - dw.min.x + 1;
        auto height = dw.max.y - dw.min.y + 1;

        if (width < 1 || height < 1)
        {
            Msg("! Failed to load tiled EXR texture: [%s]", path);
            return nullptr;
        }

        const auto& header = rgba.header();
        size_t mip;

        switch (header.tileDescription().mode)
        {
        case Imf::ONE_LEVEL:
        case Imf::MIPMAP_LEVELS: mip = rgba.numLevels(); break;
        default: Msg("! Unsupported tiled EXR texture tile mode: [%s]", path); return nullptr;
        }

        size_t arr{1};
        u32 misc{};

        if (const auto envmap = header.findTypedAttribute<Imf::EnvmapAttribute>("envmap"); envmap != nullptr)
        {
            if (envmap->value() == Imf::ENVMAP_CUBE && width == height / 6 && mip == 1)
            {
                height = width;
                arr = 6;
                misc = DirectX::TEX_MISC_TEXTURECUBE;
            }
            else
            {
                Msg("! Unsupported tiled EXR texture envmap mode: [%s]", path);
                return nullptr;
            }
        }

        meta.width = width;
        meta.height = height;
        meta.depth = 1;
        meta.arraySize = arr;
        meta.mipLevels = mip;
        meta.miscFlags = misc;
        meta.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

        if (const auto hr = texture.Initialize(meta); FAILED(hr))
        {
            Msg("! Failed to initialize tiled EXR texture data: [%s], error: [%ld]", path, hr);
            return nullptr;
        }

        for (size_t i{0}; i < mip; ++i, width >>= 1)
        {
            auto pixels = reinterpret_cast<Imf::Rgba*>(texture.GetImages()[i].pixels);

            rgba.setFrameBuffer(pixels - dw.min.x - dw.min.y * width, 1, width);
            rgba.readTiles(0, rgba.numXTiles(i) - 1, 0, rgba.numYTiles(i) - 1, i);
        }

        break;
    }
    }

    meta = texture.GetMetadata();
    ID3DBaseTexture* pTexture2D;

    if (const auto hr = DirectX::CreateTextureEx(HW.pDevice, texture.GetImages(), texture.GetImageCount(), meta, D3D_USAGE_IMMUTABLE, D3D_BIND_SHADER_RESOURCE, 0, meta.miscFlags,
                                                 DirectX::CREATETEX_DEFAULT, &pTexture2D);
        FAILED(hr))
    {
        Msg("! Failed to create EXR texture: [%s], error: [%ld]", path, hr);
        return nullptr;
    }

    size = texture.GetImages()[0].slicePitch * meta.arraySize;

    return pTexture2D;
}

ID3DBaseTexture* CRender::texture_load_sf(const string_path& path, u32& size, bool mip)
{
    xr::istream is{path};
    sf::Image image;

    if (!image.loadFromStream(is))
    {
        Msg("! Failed to load SFML texture: [%s]", path);
        return nullptr;
    }

    const auto [width, height] = image.getSize();

    DirectX::TexMetadata meta{};
    meta.width = width;
    meta.height = height;
    meta.depth = 1;
    meta.arraySize = 1;
    meta.mipLevels = mip ? 0 : 1;
    meta.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

    DirectX::ScratchImage texture;

    if (const auto hr = texture.Initialize(meta, DirectX::CP_FLAGS_NONE, false); FAILED(hr))
    {
        Msg("! Failed to initialize SFML texture data: [%s], error: [%ld]", path, hr);
        return nullptr;
    }

    const auto base = texture.SetNonOwningImagePixels(0, 0, 0, const_cast<u8*>(image.getPixelsPtr()))->slicePitch;
    meta = texture.GetMetadata();
    xr_vector<u8> mem;

    if (meta.mipLevels > 1)
    {
        mem.resize(base / 2);

        if (!xr::generate_mips(texture, mem, meta.mipLevels))
            Msg("! Failed to generate mipmaps for SFML texture: [%s]", path);
    }

    ID3DBaseTexture* pTexture2D;

    if (const auto hr = DirectX::CreateTextureEx(HW.pDevice, texture.GetImages(), texture.GetImageCount(), meta, D3D_USAGE_IMMUTABLE, D3D_BIND_SHADER_RESOURCE, 0, meta.miscFlags,
                                                 DirectX::CREATETEX_DEFAULT, &pTexture2D);
        FAILED(hr))
    {
        Msg("! Failed to create SFML texture: [%s], error: [%ld]", path, hr);
        return nullptr;
    }

    size = base;

    return pTexture2D;
}
