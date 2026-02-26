#include "stdafx.h"

#include "stream_reader.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <DirectXTex.h>

XR_DIAG_POP();

namespace ktx
{
#define KHRONOS_STATIC

#include <ktx.h>

// From lib/src/ktxint.h, at least until they implement DirectX loader
extern "C" {
namespace
{
constexpr ktx::GLint _KTX_R16_FORMATS_NORM{0x1};
constexpr ktx::GLint _KTX_R16_FORMATS_SNORM{0x2};
constexpr ktx::GLint _KTX_ALL_R16_FORMATS{_KTX_R16_FORMATS_NORM | _KTX_R16_FORMATS_SNORM};
} // namespace

ktx::KTX_error_code _ktxUnpackETC(const ktx::GLubyte* srcETC, const ktx::GLenum srcFormat, ktx::ktx_uint32_t active_width, ktx::ktx_uint32_t active_height, ktx::GLubyte** dstImage,
                                  ktx::GLenum* format, ktx::GLenum* internalFormat, ktx::GLenum* type, ktx::GLint R16Formats, ktx::GLboolean supportsSRGB);

ktx::ktx_bool_t ktxTexture_isActiveStream(ktx::ktxTexture* This);
ktx::ktx_bool_t ktxTexture2_IsHDR(ktx::ktxTexture2* This);
}

#undef KHRONOS_STATIC
} // namespace ktx

namespace gl
{
#include <GL/gl.h>
#include <GL/glext.h>
} // namespace gl

namespace vk
{
#include <vulkan/vulkan_core.h>
}

namespace xr
{
namespace detail
{
class opaque_buffer;
}
} // namespace xr

template <>
struct std::default_delete<xr::detail::opaque_buffer>
{
    constexpr void operator()(xr::detail::opaque_buffer* ptr) const noexcept { std::free(ptr); }
};

namespace xr
{
namespace
{
// https://github.com/KhronosGroup/KTX-Specification/blob/main/formats.json
constexpr std::array<std::tuple<DXGI_FORMAT, u32, u32>, 72> assoc{{
    {DXGI_FORMAT_B5G6R5_UNORM, GL_RGB565, vk::VK_FORMAT_R5G6B5_UNORM_PACK16},
    {DXGI_FORMAT_B5G5R5A1_UNORM, GL_RGB5_A1, vk::VK_FORMAT_A1R5G5B5_UNORM_PACK16},
    {DXGI_FORMAT_R8_UNORM, GL_R8, vk::VK_FORMAT_R8_UNORM},
    {DXGI_FORMAT_R8_SNORM, GL_R8_SNORM, vk::VK_FORMAT_R8_SNORM},
    {DXGI_FORMAT_R8_UINT, GL_R8UI, vk::VK_FORMAT_R8_UINT},
    {DXGI_FORMAT_R8_SINT, GL_R8I, vk::VK_FORMAT_R8_SINT},
    {DXGI_FORMAT_R8G8_UNORM, GL_RG8, vk::VK_FORMAT_R8G8_UNORM},
    {DXGI_FORMAT_R8G8_SNORM, GL_RG8_SNORM, vk::VK_FORMAT_R8G8_SNORM},
    {DXGI_FORMAT_R8G8_UINT, GL_RG8UI, vk::VK_FORMAT_R8G8_UINT},
    {DXGI_FORMAT_R8G8_SINT, GL_RG8I, vk::VK_FORMAT_R8G8_SINT},
    {DXGI_FORMAT_R8G8B8A8_UNORM, GL_RGBA8, vk::VK_FORMAT_R8G8B8A8_UNORM},
    {DXGI_FORMAT_R8G8B8A8_SNORM, GL_RGBA8_SNORM, vk::VK_FORMAT_R8G8B8A8_SNORM},
    {DXGI_FORMAT_R8G8B8A8_UINT, GL_RGBA8UI, vk::VK_FORMAT_R8G8B8A8_UINT},
    {DXGI_FORMAT_R8G8B8A8_SINT, GL_RGBA8I, vk::VK_FORMAT_R8G8B8A8_SINT},
    {DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, GL_SRGB8_ALPHA8, vk::VK_FORMAT_R8G8B8A8_SRGB},
    {DXGI_FORMAT_B8G8R8A8_UNORM, GL_RGBA8, vk::VK_FORMAT_B8G8R8A8_UNORM},
    {DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, GL_SRGB8_ALPHA8, vk::VK_FORMAT_B8G8R8A8_SRGB},
    {DXGI_FORMAT_R8G8B8A8_UNORM, GL_RGBA8, vk::VK_FORMAT_A8B8G8R8_UNORM_PACK32},
    {DXGI_FORMAT_R8G8B8A8_SNORM, GL_RGBA8_SNORM, vk::VK_FORMAT_A8B8G8R8_SNORM_PACK32},
    {DXGI_FORMAT_R8G8B8A8_UINT, GL_RGBA8UI, vk::VK_FORMAT_A8B8G8R8_UINT_PACK32},
    {DXGI_FORMAT_R8G8B8A8_SINT, GL_RGBA8I, vk::VK_FORMAT_A8B8G8R8_SINT_PACK32},
    {DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, GL_SRGB8_ALPHA8, vk::VK_FORMAT_A8B8G8R8_SRGB_PACK32},
    {DXGI_FORMAT_R10G10B10A2_UNORM, GL_RGB10_A2, vk::VK_FORMAT_A2B10G10R10_UNORM_PACK32},
    {DXGI_FORMAT_R10G10B10A2_UINT, GL_RGB10_A2UI, vk::VK_FORMAT_A2B10G10R10_UINT_PACK32},
    {DXGI_FORMAT_R16_UNORM, GL_R16, vk::VK_FORMAT_R16_UNORM},
    {DXGI_FORMAT_R16_SNORM, GL_R16_SNORM, vk::VK_FORMAT_R16_SNORM},
    {DXGI_FORMAT_R16_UINT, GL_R16UI, vk::VK_FORMAT_R16_UINT},
    {DXGI_FORMAT_R16_SINT, GL_R16I, vk::VK_FORMAT_R16_SINT},
    {DXGI_FORMAT_R16_FLOAT, GL_R16F, vk::VK_FORMAT_R16_SFLOAT},
    {DXGI_FORMAT_R16G16_UNORM, GL_RG16, vk::VK_FORMAT_R16G16_UNORM},
    {DXGI_FORMAT_R16G16_SNORM, GL_RG16_SNORM, vk::VK_FORMAT_R16G16_SNORM},
    {DXGI_FORMAT_R16G16_UINT, GL_RG16UI, vk::VK_FORMAT_R16G16_UINT},
    {DXGI_FORMAT_R16G16_SINT, GL_RG16I, vk::VK_FORMAT_R16G16_SINT},
    {DXGI_FORMAT_R16G16_FLOAT, GL_RG16F, vk::VK_FORMAT_R16G16_SFLOAT},
    {DXGI_FORMAT_R16G16B16A16_UNORM, GL_RGBA16, vk::VK_FORMAT_R16G16B16A16_UNORM},
    {DXGI_FORMAT_R16G16B16A16_SNORM, GL_RGBA16_SNORM, vk::VK_FORMAT_R16G16B16A16_SNORM},
    {DXGI_FORMAT_R16G16B16A16_UINT, GL_RGBA16UI, vk::VK_FORMAT_R16G16B16A16_UINT},
    {DXGI_FORMAT_R16G16B16A16_SINT, GL_RGBA16I, vk::VK_FORMAT_R16G16B16A16_SINT},
    {DXGI_FORMAT_R16G16B16A16_FLOAT, GL_RGBA16F, vk::VK_FORMAT_R16G16B16A16_SFLOAT},
    {DXGI_FORMAT_R32_UINT, GL_R32UI, vk::VK_FORMAT_R32_UINT},
    {DXGI_FORMAT_R32_SINT, GL_R32I, vk::VK_FORMAT_R32_SINT},
    {DXGI_FORMAT_R32_FLOAT, GL_R32F, vk::VK_FORMAT_R32_SFLOAT},
    {DXGI_FORMAT_R32G32_UINT, GL_RG32UI, vk::VK_FORMAT_R32G32_UINT},
    {DXGI_FORMAT_R32G32_SINT, GL_RG32I, vk::VK_FORMAT_R32G32_SINT},
    {DXGI_FORMAT_R32G32_FLOAT, GL_RG32F, vk::VK_FORMAT_R32G32_SFLOAT},
    {DXGI_FORMAT_R32G32B32_UINT, GL_RGB32UI, vk::VK_FORMAT_R32G32B32_UINT},
    {DXGI_FORMAT_R32G32B32_SINT, GL_RGB32I, vk::VK_FORMAT_R32G32B32_SINT},
    {DXGI_FORMAT_R32G32B32_FLOAT, GL_RGB32F, vk::VK_FORMAT_R32G32B32_SFLOAT},
    {DXGI_FORMAT_R32G32B32A32_UINT, GL_RGBA32UI, vk::VK_FORMAT_R32G32B32A32_UINT},
    {DXGI_FORMAT_R32G32B32A32_SINT, GL_RGBA32I, vk::VK_FORMAT_R32G32B32A32_SINT},
    {DXGI_FORMAT_R32G32B32A32_FLOAT, GL_RGBA32F, vk::VK_FORMAT_R32G32B32A32_SFLOAT},
    {DXGI_FORMAT_R11G11B10_FLOAT, GL_R11F_G11F_B10F, vk::VK_FORMAT_B10G11R11_UFLOAT_PACK32},
    {DXGI_FORMAT_R9G9B9E5_SHAREDEXP, GL_RGB9_E5, vk::VK_FORMAT_E5B9G9R9_UFLOAT_PACK32},
    {DXGI_FORMAT_D16_UNORM, GL_DEPTH_COMPONENT16, vk::VK_FORMAT_D16_UNORM},
    {DXGI_FORMAT_D32_FLOAT, GL_DEPTH_COMPONENT32F, vk::VK_FORMAT_D32_SFLOAT},
    {DXGI_FORMAT_D32_FLOAT_S8X24_UINT, GL_DEPTH32F_STENCIL8, vk::VK_FORMAT_D32_SFLOAT_S8_UINT},
    {DXGI_FORMAT_BC1_UNORM, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, vk::VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
    {DXGI_FORMAT_BC1_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, vk::VK_FORMAT_BC1_RGBA_SRGB_BLOCK},
    {DXGI_FORMAT_BC2_UNORM, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, vk::VK_FORMAT_BC2_UNORM_BLOCK},
    {DXGI_FORMAT_BC2_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, vk::VK_FORMAT_BC2_SRGB_BLOCK},
    {DXGI_FORMAT_BC3_UNORM, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, vk::VK_FORMAT_BC3_UNORM_BLOCK},
    {DXGI_FORMAT_BC3_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, vk::VK_FORMAT_BC3_SRGB_BLOCK},
    {DXGI_FORMAT_BC4_UNORM, GL_COMPRESSED_RED_RGTC1, vk::VK_FORMAT_BC4_UNORM_BLOCK},
    {DXGI_FORMAT_BC4_SNORM, GL_COMPRESSED_SIGNED_RED_RGTC1, vk::VK_FORMAT_BC4_SNORM_BLOCK},
    {DXGI_FORMAT_BC5_UNORM, GL_COMPRESSED_RG_RGTC2, vk::VK_FORMAT_BC5_UNORM_BLOCK},
    {DXGI_FORMAT_BC5_SNORM, GL_COMPRESSED_SIGNED_RG_RGTC2, vk::VK_FORMAT_BC5_SNORM_BLOCK},
    {DXGI_FORMAT_BC6H_UF16, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, vk::VK_FORMAT_BC6H_UFLOAT_BLOCK},
    {DXGI_FORMAT_BC6H_SF16, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, vk::VK_FORMAT_BC6H_SFLOAT_BLOCK},
    {DXGI_FORMAT_BC7_UNORM, GL_COMPRESSED_RGBA_BPTC_UNORM, vk::VK_FORMAT_BC7_UNORM_BLOCK},
    {DXGI_FORMAT_BC7_UNORM_SRGB, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, vk::VK_FORMAT_BC7_SRGB_BLOCK},
    {DXGI_FORMAT_B4G4R4A4_UNORM, GL_RGBA4, vk::VK_FORMAT_A4R4G4B4_UNORM_PACK16},
    {DXGI_FORMAT_A8_UNORM, GL_ALPHA8_EXT, vk::VK_FORMAT_A8_UNORM_KHR},
}};
xr_unordered_map<u32, DXGI_FORMAT> ktx1dxgi;
xr_unordered_map<u32, DXGI_FORMAT> ktx2dxgi;

// Formats that are supported after decoding them using ETC
constexpr std::array<std::tuple<u32, u32, DXGI_FORMAT>, 8> etc{{
    {GL_COMPRESSED_R11_EAC, vk::VK_FORMAT_EAC_R11_UNORM_BLOCK, DXGI_FORMAT_R16_UNORM},
    {GL_COMPRESSED_SIGNED_R11_EAC, vk::VK_FORMAT_EAC_R11_SNORM_BLOCK, DXGI_FORMAT_R16_SNORM},
    {GL_COMPRESSED_RG11_EAC, vk::VK_FORMAT_EAC_R11G11_UNORM_BLOCK, DXGI_FORMAT_R16G16_UNORM},
    {GL_COMPRESSED_SIGNED_RG11_EAC, vk::VK_FORMAT_EAC_R11G11_SNORM_BLOCK, DXGI_FORMAT_R16G16_SNORM},
    {GL_COMPRESSED_RGBA8_ETC2_EAC, vk::VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, DXGI_FORMAT_R8G8B8A8_UNORM},
    {GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, vk::VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, DXGI_FORMAT_R8G8B8A8_UNORM},
    {GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, vk::VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB},
    {GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, vk::VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB},
}};

class alignas(2 * sizeof(gsl::index)) ktx_texture final
{
private:
    ktx::ktxTexture* tex;
    ktx::ktxStream is;

    const std::unique_ptr<CStreamReader> file;
    gsl::czstring path;

    [[nodiscard]] static constexpr CStreamReader& reader(const ktx::ktxStream* str) { return *static_cast<CStreamReader*>(str->data.custom_ptr.address); }

    [[nodiscard]] static ktx::ktx_error_code_e read(ktx::ktxStream* str, void* dst, const ktx::ktx_size_t count)
    {
        reader(str).r(dst, gsl::narrow_cast<gsl::index>(count));
        return ktx::KTX_SUCCESS;
    }

    [[nodiscard]] static ktx::ktx_error_code_e skip(ktx::ktxStream* str, const ktx::ktx_size_t count)
    {
        reader(str).advance(gsl::narrow_cast<gsl::index>(count));
        return ktx::KTX_SUCCESS;
    }

    [[nodiscard]] static ktx::ktx_error_code_e write(ktx::ktxStream*, const void*, const ktx::ktx_size_t, const ktx::ktx_size_t) { FATAL("Can't write to a read-only stream"); }

    [[nodiscard]] static ktx::ktx_error_code_e getpos(ktx::ktxStream* str, ktx::ktx_off_t* const offset)
    {
        *offset = gsl::narrow_cast<ktx::ktx_off_t>(reader(str).tell());
        return ktx::KTX_SUCCESS;
    }

    [[nodiscard]] static ktx::ktx_error_code_e setpos(ktx::ktxStream* str, const ktx::ktx_off_t offset)
    {
        reader(str).seek(gsl::narrow_cast<gsl::index>(offset));
        return ktx::KTX_SUCCESS;
    }

    [[nodiscard]] static ktx::ktx_error_code_e getsize(ktx::ktxStream* str, ktx::ktx_size_t* const size)
    {
        *size = gsl::narrow_cast<ktx::ktx_off_t>(reader(str).length());
        return ktx::KTX_SUCCESS;
    }

    static void destruct(ktx::ktxStream* str) { str->data.custom_ptr.address = nullptr; }

    [[nodiscard]] static ktx::ktx_error_code_e callback(int miplevel, int face, int width, int height, int, ktx::ktx_uint64_t, void* pixels, void* userdata);

public:
    explicit ktx_texture(gsl::czstring path);

    ~ktx_texture()
    {
        if (tex != nullptr)
            ktxTexture_Destroy(tex);
    }

    [[nodiscard]] constexpr ktx::ktxTexture* get() const { return tex; }
    [[nodiscard]] constexpr explicit operator bool() const { return tex != nullptr; }
    [[nodiscard]] constexpr ktx::ktxTexture* operator->() const { return tex; }

    [[nodiscard]] bool iterate(const DirectX::ScratchImage& texture, xr_vector<std::unique_ptr<xr::detail::opaque_buffer>>& packed, u32 etc) const;
};

ktx_texture::ktx_texture(gsl::czstring path) : file{absl::WrapUnique(FS.rs_open(path))}, path{path}
{
    R_ASSERT(file);

    is.read = &ktx_texture::read;
    is.skip = &ktx_texture::skip;
    is.write = &ktx_texture::write;
    is.getpos = &ktx_texture::getpos;
    is.setpos = &ktx_texture::setpos;
    is.getsize = &ktx_texture::getsize;
    is.destruct = &ktx_texture::destruct;

    is.type = ktx::eStreamTypeCustom;
    is.data.custom_ptr.address = file.get();
    is.data.custom_ptr.allocatorAddress = nullptr;
    is.data.custom_ptr.size = 0;
    is.readpos = 0;
    is.closeOnDestruct = false;

    if (const auto res = ktx::ktxTexture_CreateFromStream(&is, ktx::KTX_TEXTURE_CREATE_NO_FLAGS, &tex); res != ktx::KTX_SUCCESS)
    {
        Msg("! Failed to load KTX texture: [%s], error: [-%d]", path, res);
        tex = nullptr;
    }
}

bool ktx_texture::iterate(const DirectX::ScratchImage& texture, xr_vector<std::unique_ptr<xr::detail::opaque_buffer>>& packed, u32 etc) const
{
    if (ktx::ktxTexture_isActiveStream(tex))
    {
        if (const auto res = ktxTexture_LoadImageData(tex, nullptr, 0); res != ktx::KTX_SUCCESS)
        {
            Msg("! Failed to load KTX texture: [%s], error: [-%d]", path, res);
            return false;
        }
    }

    const std::tuple<const DirectX::ScratchImage&, xr_vector<std::unique_ptr<xr::detail::opaque_buffer>>&, u32> data{texture, packed, etc};
    void* userdata = const_cast<void*>(static_cast<const void*>(&data));

    if (const auto res = ktx::ktxTexture_IterateLevelFaces(tex, &xr::ktx_texture::callback, userdata); res != ktx::KTX_SUCCESS)
    {
        Msg("! Failed to iterate KTX texture subimages: [%s], error: [-%d]", path, res);
        return false;
    }

    return true;
}

ktx::ktx_error_code_e ktx_texture::callback(int miplevel, int face, int width, int height, int, ktx::ktx_uint64_t, void* pixels, void* userdata)
{
    auto& data = *static_cast<const std::tuple<const DirectX::ScratchImage&, xr_vector<std::unique_ptr<xr::detail::opaque_buffer>>&, u32>*>(userdata);
    const auto etc = std::get<2>(data);

    if (etc != 0)
    {
        ktx::GLenum fmt, internal, type;
        ktx::GLubyte* unpacked;

        if (const auto res = ktx::_ktxUnpackETC(static_cast<const ktx::GLubyte*>(pixels), etc, width, height, &unpacked, &fmt, &internal, &type, ktx::_KTX_ALL_R16_FORMATS, true);
            res != ktx::KTX_SUCCESS)
            return res;

        pixels = unpacked;
        std::get<1>(data).emplace_back(static_cast<xr::detail::opaque_buffer*>(pixels));
    }

    auto& texture = std::get<0>(data);
    auto& meta = texture.GetMetadata();

    if (meta.arraySize > 1 && !meta.IsCubemap())
    {
        for (size_t layer{0}, off{0}; layer < meta.arraySize; ++layer)
            off += texture.SetNonOwningImagePixels(miplevel, layer, 0, static_cast<u8*>(pixels) + off)->slicePitch;
    }
    else
    {
        texture.SetNonOwningImagePixels(miplevel, face, 0, static_cast<u8*>(pixels));
    }

    return ktx::KTX_SUCCESS;
}
} // namespace
} // namespace xr

void CRender::texture_init_ktx()
{
    for (auto& tup : xr::assoc)
    {
        xr::ktx1dxgi.try_emplace(std::get<1>(tup), std::get<0>(tup));
        xr::ktx2dxgi.try_emplace(std::get<2>(tup), std::get<0>(tup));
    }
}

ID3DBaseTexture* CRender::texture_load_ktx(const string_path& path, u32& size)
{
    xr::ktx_texture tex{path};

    if (!tex)
        return nullptr;

    if (tex->numDimensions != 2)
    {
        Msg("! Unsupported non-2D KTX texture: [%s]", path);
        return nullptr;
    }

    if (tex->isCubemap && tex->isArray)
    {
        Msg("! Unsupported cubemap array KTX texture: [%s]", path);
        return nullptr;
    }

    DirectX::TEX_ALPHA_MODE alpha{};
    DXGI_FORMAT dxgi;
    u32 etc{};

    switch (tex->classId)
    {
    case ktx::ktxTexture1_c: {
        const auto fmt = reinterpret_cast<const ktx::ktxTexture1*>(tex.get())->glInternalformat;

        if (const auto item = xr::ktx1dxgi.find(fmt); item == xr::ktx1dxgi.end())
        {
            if (const auto item2 = std::ranges::find_if(xr::etc, [fmt](auto& tup) { return std::get<0>(tup) == fmt; }); item2 == xr::etc.end())
            {
                Msg("! Unsupported KTX1 texture format: [%s], glInternalformat: [0x%x]", path, fmt);
                return nullptr;
            }
            else
            {
                etc = fmt;
                dxgi = std::get<2>(*item2);
            }
        }
        else
        {
            dxgi = item->second;
        }

        break;
    }
    case ktx::ktxTexture2_c: {
        auto tex2 = reinterpret_cast<ktx::ktxTexture2*>(tex.get());

        if (tex2->isVideo)
        {
            Msg("! Unsupported video KTX2 texture: [%s]", path);
            return nullptr;
        }

        if (ktx::ktxTexture2_IsTranscodable(tex2))
        {
            if (const auto res = ktx::ktxTexture2_TranscodeBasis(tex2, ktx::ktxTexture2_IsHDR(tex2) ? ktx::KTX_TTF_BC6HU : ktx::KTX_TTF_BC7_RGBA, ktx::KTX_TF_HIGH_QUALITY);
                res != ktx::KTX_SUCCESS)
            {
                Msg("! Failed to transcode BasisLZ/ETC1S/UASTC-encoded KTX2 texture: [%s], error: [-%d]", path, res);
                return nullptr;
            }
        }
        else if (ktx::ktxTexture2_GetColorModel_e(tex2) == ktx::KHR_DF_MODEL_ASTC)
        {
            if (const auto res = ktx::ktxTexture2_DecodeAstc(tex2); res != ktx::KTX_SUCCESS)
            {
                Msg("! Failed to decode ASTC-encoded KTX2 texture: [%s], error: [-%d]", path, res);
                return nullptr;
            }
        }

        const auto fmt = tex2->vkFormat;

        if (const auto item = xr::ktx2dxgi.find(fmt); item == xr::ktx2dxgi.end())
        {
            if (const auto item2 = std::ranges::find_if(xr::etc, [fmt](auto& tup) { return std::get<1>(tup) == fmt; }); item2 == xr::etc.end())
            {
                Msg("! Unsupported KTX2 texture format: [%s], vkFormat: [0x%x]", path, fmt);
                return nullptr;
            }
            else
            {
                etc = std::get<0>(*item2);
                dxgi = std::get<2>(*item2);
            }
        }
        else
        {
            dxgi = item->second;
        }

        alpha = ktx::ktxTexture2_GetPremultipliedAlpha(tex2) ? DirectX::TEX_ALPHA_MODE_PREMULTIPLIED : DirectX::TEX_ALPHA_MODE_STRAIGHT;
        break;
    }
    default: Msg("Invalid KTX texture version: [%s], classId: [0x%x]", path, gsl::narrow_cast<u32>(tex->classId)); return nullptr;
    }

    DirectX::TexMetadata meta{};
    meta.width = tex->baseWidth;
    meta.height = tex->baseHeight;
    meta.depth = 1;
    meta.arraySize = tex->isArray ? tex->numLayers : tex->numFaces;
    meta.mipLevels = tex->numLevels;
    meta.miscFlags = !!tex->isCubemap * DirectX::TEX_MISC_TEXTURECUBE;
    meta.SetAlphaMode(alpha);
    meta.format = dxgi;
    meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;

    DirectX::ScratchImage texture;

    if (const auto hr = texture.Initialize(meta, DirectX::CP_FLAGS_NONE, false); FAILED(hr))
    {
        Msg("! Failed to initialize KTX texture data: [%s], error: [%ld]", path, hr);
        return nullptr;
    }

    xr_vector<std::unique_ptr<xr::detail::opaque_buffer>> packed;

    if (!tex.iterate(texture, packed, etc))
        return nullptr;

    meta = texture.GetMetadata();
    ID3DBaseTexture* pTexture2D;

    if (const auto hr = DirectX::CreateTextureEx(HW.pDevice, texture.GetImages(), texture.GetImageCount(), meta, D3D_USAGE_IMMUTABLE, D3D_BIND_SHADER_RESOURCE, 0, meta.miscFlags,
                                                 DirectX::CREATETEX_DEFAULT, &pTexture2D);
        FAILED(hr))
    {
        Msg("! Failed to create KTX texture: [%s], error: [%ld]", path, hr);
        return nullptr;
    }

    size = texture.GetImages()[0].slicePitch * meta.arraySize;

    return pTexture2D;
}
