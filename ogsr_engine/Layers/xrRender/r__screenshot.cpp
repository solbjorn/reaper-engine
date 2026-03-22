#include "stdafx.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <DirectXTex.h>

XR_DIAG_POP();

#include <ImfIO.h>

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wunused-parameter");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <ImfRgbaFile.h>

XR_DIAG_POP();

#include <SFML/Graphics/Image.hpp>

#define BASISU_SUPPORT_SSE 1

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wdeprecated-anon-enum-enum-conversion");
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wextra-semi-stmt");
XR_DIAG_IGNORE("-Wfloat-equal");
XR_DIAG_IGNORE("-Wzero-as-null-pointer-constant");

#include <basisu_enc.h>

XR_DIAG_POP();

#undef BASISU_SUPPORT_SSE

namespace png
{
#include <png.h>
}

namespace xr
{
namespace
{
enum class format : s32
{
    none = 0,
    dds,
    exr,
    png,
    qoi,
    jpg,
    tga,
    bmp,
};

constexpr std::array<std::tuple<xr::format, std::string_view, std::string_view>, 7> ssfmt{{
    {xr::format::dds, "dds", "-ss_dds"},
    {xr::format::exr, "exr", "-ss_exr"},
    {xr::format::png, "png", "-ss_png"},
    {xr::format::qoi, "qoi", "-ss_qoi"},
    {xr::format::jpg, "jpg", "-ss_jpg"},
    {xr::format::tga, "tga", "-ss_tga"},
    {xr::format::bmp, "bmp", "-ss_bmp"},
}};

constexpr size_t GAMESAVE_SIZE{1024};

class ostream final : public Imf::OStream
{
private:
    IWriter* file;

public:
    explicit ostream(gsl::czstring path) : Imf::OStream{path}, file{FS.w_open(path)} { R_ASSERT(file != nullptr); }
    ~ostream() override { FS.w_close(file); }

    void write(const char c[], s32 n) override { file->w(c, n); }
    [[nodiscard]] u64 tellp() override { return gsl::narrow_cast<u64>(file->tell()); }
    void seekp(u64 pos) override { file->seek(gsl::narrow_cast<gsl::index>(pos)); }
};

class png_writer final
{
private:
    png::png_structp st;
    png::png_infop info;

    [[nodiscard]] static IWriter* writer(const png::png_structp st) { return static_cast<IWriter*>(png::png_get_io_ptr(st)); }

    static void write_error(png::png_structp, png::png_const_charp msg) { FATAL("PNG write error: %s", msg); }
    static void write_warning(png::png_structp, png::png_const_charp) {}

    static void write(png::png_structp st, png::png_bytep data, png::png_size_t len) { writer(st)->w(data, gsl::narrow_cast<gsl::index>(len)); }
    static void flush(png::png_structp st) { std::ignore = writer(st)->flush(); }

public:
    png_writer()
    {
        st = png::png_create_write_struct(PNG_LIBPNG_VER_STRING, this, &png_writer::write_error, &png_writer::write_warning);
        R_ASSERT(st != nullptr);

        info = png::png_create_info_struct(st);
        R_ASSERT(info != nullptr);
    }

    ~png_writer() { png::png_destroy_write_struct(&st, &info); }

    void write(IWriter* file, const DirectX::ScratchImage& cap);
};

void png_writer::write(IWriter* file, const DirectX::ScratchImage& cap)
{
    R_ASSERT(file != nullptr);

    png::png_set_write_fn(st, file, &png_writer::write, &png_writer::flush);
    png::png_set_compression_level(st, 6);
    png::png_set_filter(st, 0, PNG_NO_FILTERS);

    auto& image = cap.GetImages()[0];

    png::png_set_IHDR(st, info, image.width, image.height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png::png_write_info(st, info);

    xr_vector<png::png_bytep> rows(image.height);

    for (auto [i, row] : xr::views_enumerate(rows))
        row = &image.pixels[image.rowPitch * i];

    png::png_write_image(st, rows.data());
    png::png_write_end(st, info);

    FS.w_close(file);
}

void grant_ownership(basisu::imagef& image, basisu::vec4F* pixels, u32 w, u32 h)
{
    image.clear();
    image.get_pixels().grant_ownership(pixels, w * h, w * h);

    // basisu::image has ::grant_ownership(), but basisu::imagef does not. Why...
    struct fake_imagef
    {
        u32 width;
        u32 height;
        u32 pitch;
        basisu::vec4F_vec pixels;
    }* fake_image = reinterpret_cast<fake_imagef*>(&image);

    fake_image->width = w;
    fake_image->height = h;
    fake_image->pitch = w;
}

void save_dds(IWriter* file, DirectX::ScratchImage& cap, bool resample)
{
    R_ASSERT(file != nullptr);

    DirectX::ScratchImage* tgt = &cap;
    DirectX::ScratchImage res;

    if (resample)
    {
        R_CHK(res.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, xr::GAMESAVE_SIZE, xr::GAMESAVE_SIZE, 1, 1));

        auto& image = cap.GetImages()[0];
        auto& thumb = res.GetImages()[0];
        basisu::image src, dst;

        src.grant_ownership(reinterpret_cast<basisu::color_rgba*>(image.pixels), image.width, image.height);
        dst.grant_ownership(reinterpret_cast<basisu::color_rgba*>(thumb.pixels), thumb.width, thumb.height);

        R_ASSERT(basisu::image_resample(src, dst, false, "kaiser"));

        dst.get_pixels().assume_ownership();
        src.get_pixels().assume_ownership();

        cap.Release();
        tgt = &res;
    }

    DirectX::ScratchImage comp;
    R_CHK(DirectX::Compress(HW.pDevice, tgt->GetImages()[0], DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_ALPHA_WEIGHT_DEFAULT, comp));
    tgt->Release();

    DirectX::Blob blob;
    R_CHK(DirectX::SaveToDDSMemory(comp.GetImages()[0], DirectX::DDS_FLAGS_NONE, blob));

    file->w(blob.GetBufferPointer(), blob.GetBufferSize());
    FS.w_close(file);
}

void save_exr(gsl::czstring path, DirectX::ScratchImage& cap, bool resample)
{
    DirectX::ScratchImage* tgt = &cap;
    DirectX::ScratchImage res;

    if (resample)
    {
        DirectX::ScratchImage full;
        R_CHK(DirectX::Convert(cap.GetImages()[0], DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, full));
        cap.Release();

        DirectX::ScratchImage crop;
        R_CHK(crop.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, xr::GAMESAVE_SIZE, xr::GAMESAVE_SIZE, 1, 1));

        auto& image = full.GetImages()[0];
        auto& thumb = crop.GetImages()[0];
        basisu::imagef src, dst;

        xr::grant_ownership(src, reinterpret_cast<basisu::vec4F*>(image.pixels), image.width, image.height);
        xr::grant_ownership(dst, reinterpret_cast<basisu::vec4F*>(thumb.pixels), thumb.width, thumb.height);

        R_ASSERT(basisu::image_resample(src, dst, "kaiser"));

        dst.get_pixels().assume_ownership();
        src.get_pixels().assume_ownership();

        full.Release();
        R_CHK(DirectX::Convert(thumb, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, res));

        crop.Release();
        tgt = &res;
    }
    else if (cap.GetMetadata().format == DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        R_CHK(DirectX::Convert(cap.GetImages()[0], DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, res));

        cap.Release();
        tgt = &res;
    }

    auto& image = tgt->GetImages()[0];
    xr::ostream os{path};
    Imf::RgbaOutputFile rgba{os, Imf::Header{gsl::narrow_cast<s32>(image.width), gsl::narrow_cast<s32>(image.height)}, Imf::WRITE_RGBA};

    rgba.setFrameBuffer(reinterpret_cast<const Imf::Rgba*>(image.pixels), 1, image.width);
    rgba.writePixels(image.height);
}

void save_sf(IWriter* file, DirectX::ScratchImage& cap, std::string_view ext)
{
    R_ASSERT(file != nullptr);

    auto& image = cap.GetImages()[0];
    const sf::Image out{{gsl::narrow_cast<u32>(image.width), gsl::narrow_cast<u32>(image.height)}, image.pixels};
    cap.Release();

    const auto blob = out.saveToMemory(ext);
    R_ASSERT(blob);

    file->w(blob->data(), blob->size());
    FS.w_close(file);
}
} // namespace
} // namespace xr

void CRender::Screenshot(ScreenshotMode mode, LPCSTR name)
{
    ID3DResource* pSrcTexture;
    Target->get_base_rt()->GetResource(&pSrcTexture);

    DirectX::ScratchImage SImage;
    R_CHK(DirectX::CaptureTexture(HW.pDevice, HW.get_imm_context(), pSrcTexture, SImage));
    _RELEASE(pSrcTexture);

    xr::format fmt;

    switch (SImage.GetMetadata().format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM: fmt = xr::format::none; break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT: fmt = xr::format::exr; break;
    default: FATAL("Unsupported render target format for screenshot: [0x%x]", gsl::narrow_cast<u32>(SImage.GetMetadata().format));
    }

    std::string_view ext;
    string_path buf;

    switch (mode)
    {
    case IRender_interface::SM_NORMAL:
        if (fmt == xr::format::none)
        {
            const std::string_view params{Core.Params};

            for (auto& entry : xr::ssfmt)
            {
                if (params.contains(std::get<2>(entry)))
                {
                    fmt = std::get<0>(entry);
                    ext = std::get<1>(entry);
                }
            }

            if (fmt == xr::format::none)
                fmt = xr::format::png;
        }

        if (ext.empty())
            ext = std::get<1>(*std::ranges::find_if(xr::ssfmt, [fmt](const auto& entry) { return std::get<0>(entry) == fmt; }));

        string64 t_stemp;
        xr_sprintf(buf, sizeof(buf), "ss_%s_%s_(%s).%s", Core.UserName, timestamp(t_stemp), (g_pGameLevel) ? g_pGameLevel->name().c_str() : "mainmenu", ext.data());

        switch (fmt)
        {
        case xr::format::none: NODEFAULT;
        case xr::format::dds: xr::save_dds(FS.w_open("$screenshots$", buf), SImage, false); break;
        case xr::format::exr: {
            string_path path;
            std::ignore = FS.update_path(path, "$screenshots$", buf);

            xr::save_exr(path, SImage, false);
            break;
        }
        case xr::format::png: xr::png_writer{}.write(FS.w_open("$screenshots$", buf), SImage); break;
        default: xr::save_sf(FS.w_open("$screenshots$", buf), SImage, ext); break;
        }

        break;
    case IRender_interface::SM_FOR_GAMESAVE:
        if (fmt == xr::format::none)
            fmt = xr::format::dds;

        ext = std::get<1>(*std::ranges::find_if(xr::ssfmt, [fmt](const auto& entry) { return std::get<0>(entry) == fmt; }));

        xr_strcpy(buf, name);
        fix_texture_name(buf);

        xr_strcat(buf, ".");
        xr_strcat(buf, ext.data());

        switch (fmt)
        {
        case xr::format::dds: xr::save_dds(FS.w_open(buf), SImage, true); break;
        case xr::format::exr: xr::save_exr(buf, SImage, true); break;
        default: NODEFAULT;
        }

        break;
    case IRender_interface::SM_FOR_LEVELMAP:
    case IRender_interface::SM_FOR_CUBEMAP:
        if (fmt == xr::format::none)
            fmt = xr::format::tga;

        ext = std::get<1>(*std::ranges::find_if(xr::ssfmt, [fmt](const auto& entry) { return std::get<0>(entry) == fmt; }));

        VERIFY(name != nullptr);
        xr_strconcat(buf, name, ".", ext.data());

        switch (fmt)
        {
        case xr::format::exr: {
            string_path path;
            std::ignore = FS.update_path(path, "$screenshots$", buf);

            xr::save_exr(path, SImage, false);
            break;
        }
        case xr::format::tga: xr::save_sf(FS.w_open("$screenshots$", buf), SImage, ext); break;
        default: NODEFAULT;
        }

        break;
    }
}
