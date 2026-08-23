#include "stdafx.h"

#include "../xrRender/ResourceManager.h"

#include "../../xr_3da/xrTheora_Surface.h"

#include "StateManager/dx10ShaderResourceStateCache.h"

namespace skb
{
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <skribidi/skb_image_atlas.h>

XR_DIAG_POP();
} // namespace skb

void resptrcode_texture::create(LPCSTR _name) { _set(RImplementation.Resources->_CreateTexture(_name)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture() : bind{CallMe::fromMethod<&CTexture::apply_load>(this)} {}

CTexture::~CTexture()
{
    Unload();

    // release external reference
    RImplementation.Resources->_DeleteTexture(this);
}

void CTexture::surface_set(::ID3D11Resource* surf)
{
    if (pSurface == surf)
        return;

    if (surf)
        surf->AddRef();
    _RELEASE(pSurface);

    pSurface = surf;

    if (pSurface != nullptr)
    {
        ::D3D11_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);

        if (type == ::D3D11_RESOURCE_DIMENSION::D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        {
            ::D3D11_TEXTURE2D_DESC desc{};

            auto T = static_cast<::ID3D11Texture2D*>(pSurface);
            T->GetDesc(&desc);

            desc_Width = desc.Width;
            desc_Height = desc.Height;

            ::D3D11_SHADER_RESOURCE_VIEW_DESC ViewDesc{};

            if (desc.MiscFlags & ::D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE)
            {
                ViewDesc.ViewDimension = ::D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBE;
                ViewDesc.TextureCube.MipLevels = desc.MipLevels;
            }
            else
            {
                const bool isArray = desc.ArraySize > 1;
                if (desc.SampleDesc.Count <= 1)
                {
                    ViewDesc.ViewDimension =
                        isArray ? ::D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DARRAY : ::D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
                    if (isArray)
                    {
                        ViewDesc.Texture2DArray.MipLevels = desc.MipLevels;
                        ViewDesc.Texture2DArray.ArraySize = desc.ArraySize;
                    }
                    else
                    {
                        ViewDesc.Texture2D.MipLevels = desc.MipLevels;
                    }
                }
                else
                {
                    ViewDesc.ViewDimension =
                        isArray ? ::D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : ::D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DMS;
                    if (isArray)
                        ViewDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
                }
            }

            switch (desc.Format)
            {
            case ::DXGI_FORMAT::DXGI_FORMAT_R32G8X24_TYPELESS: ViewDesc.Format = ::DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; break;
            case ::DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS: ViewDesc.Format = ::DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
            case ::DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS: ViewDesc.Format = ::DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT; break;
            case ::DXGI_FORMAT::DXGI_FORMAT_R16_TYPELESS: ViewDesc.Format = ::DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT; break;
            default: break;
            }

            _RELEASE(srv_all);

            XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &srv_all)));
            srv_per_slice.resize(desc.ArraySize);

            for (u32 id = 0; id < desc.ArraySize; ++id)
            {
                _RELEASE(srv_per_slice[id]);

                if (desc.SampleDesc.Count <= 1)
                {
                    ViewDesc.Texture2DArray.ArraySize = 1;
                    ViewDesc.Texture2DArray.FirstArraySlice = id;
                }
                else
                {
                    ViewDesc.Texture2DMSArray.ArraySize = 1;
                    ViewDesc.Texture2DMSArray.FirstArraySlice = id;
                }

                XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &srv_per_slice[id])));
            }

            set_slice(-1);
        }
        else
        {
            _RELEASE(m_pSRView);
            XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView)));
        }
    }
}

void CTexture::PostLoad()
{
    meta.visit(std::__variant_detail::__all_overloads{[this](const normal&) { bind = CallMe::fromMethod<&CTexture::Apply>(this); },
                                                      [this](const font&) { bind = CallMe::fromMethod<&CTexture::apply_font>(this); },
                                                      [this](const seq&) { bind = CallMe::fromMethod<&CTexture::apply_seq>(this); },
                                                      [this](const theora&) { bind = CallMe::fromMethod<&CTexture::apply_theora>(this); }});
}

void CTexture::apply_load(CBackend& cmd_list, u32 dwStage)
{
    if (!bLoaded)
        Load();
    else
        PostLoad();

    bind(cmd_list, dwStage);
}

void CTexture::Apply(CBackend& cmd_list, u32 dwStage) const
{
    if (dwStage < rstVertex)
        // Pixel shader stage resources
        cmd_list.SRVSManager.SetPSResource(dwStage, m_pSRView);
    else if (dwStage < rstGeometry)
        // Vertex shader stage resources
        cmd_list.SRVSManager.SetVSResource(dwStage - rstVertex, m_pSRView);
    else if (dwStage < rstHull)
        // Geometry shader stage resources
        cmd_list.SRVSManager.SetGSResource(dwStage - rstGeometry, m_pSRView);
    else if (dwStage < rstDomain)
        // Hull shader stage resources
        cmd_list.SRVSManager.SetHSResource(dwStage - rstHull, m_pSRView);
    else if (dwStage < rstCompute)
        // Domain shader stage resources
        cmd_list.SRVSManager.SetDSResource(dwStage - rstDomain, m_pSRView);
    else if (dwStage < rstInvalid)
        // Compute shader stage resources
        cmd_list.SRVSManager.SetCSResource(dwStage - rstCompute, m_pSRView);
    else
        XR_PANIC("invalid texture stage", dwStage);
}

void CTexture::apply_font(CBackend& cmd_list, u32 dwStage) const
{
    auto& font = *XR_ASSERT_VAL(std::get_if<CTexture::font>(&meta) != nullptr);
    const auto bounds = skb::skb_image_atlas_get_and_reset_texture_dirty_bounds(font.atlas, font.texture_idx);

    if (skb::skb_rect2i_is_empty(bounds))
    {
    apply:
        Apply(cmd_list, dwStage);
        return;
    }

    Microsoft::WRL::ComPtr<::ID3D11Texture2D> tex;
    XR_ASSERT(xr::hr(font.staging.As(&tex)));
    auto& context = *cmd_list.context();

    ::D3D11_MAPPED_SUBRESOURCE data;
    XR_ASSERT(xr::hr(context.Map(tex.Get(), 0, ::D3D11_MAP::D3D11_MAP_WRITE, 0, &data)));

    auto image = skb::skb_image_atlas_get_texture(font.atlas, font.texture_idx);
    const auto stride = image->stride_bytes;
    const auto bpp = image->bpp;

    for (auto y : std::views::indices(bounds.height))
    {
        const auto doff = bounds.x * bpp + (bounds.y + y) * data.RowPitch;
        const auto soff = bounds.x * bpp + (bounds.y + y) * stride;

        std::memcpy(static_cast<std::byte*>(data.pData) + doff, image->buffer + soff, bounds.width * bpp);
    }

    context.Unmap(tex.Get(), 0);

    const ::D3D11_BOX box{gsl::narrow_cast<u32>(bounds.x),
                          gsl::narrow_cast<u32>(bounds.y),
                          0,
                          gsl::narrow_cast<u32>(bounds.x + bounds.width),
                          gsl::narrow_cast<u32>(bounds.y + bounds.height),
                          1};
    context.CopySubresourceRegion(pSurface, 0, bounds.x, bounds.y, 0, font.staging.Get(), 0, &box);

    goto apply;
}

void CTexture::apply_seq(CBackend& cmd_list, u32 dwStage)
{
    auto& seq = *XR_ASSERT_VAL(std::get_if<CTexture::seq>(&meta) != nullptr);
    const std::size_t frame = Device.dwTimeContinual / seq.seqMSPF;
    const auto frame_data = seq.m_seqSRView.size();

    std::size_t frame_id;
    if (seq.seqCycles)
    {
        frame_id = frame % (frame_data * 2);
        if (frame_id >= frame_data)
            frame_id = (frame_data - 1) - (frame_id % frame_data);
    }
    else
    {
        frame_id = frame % frame_data;
    }

    pSurface = seq.seqDATA[frame_id].Get();
    m_pSRView = seq.m_seqSRView[frame_id].Get();

    Apply(cmd_list, dwStage);
}

void CTexture::apply_theora(CBackend& cmd_list, u32 dwStage) const
{
    auto& theora = *XR_ASSERT_VAL(std::get_if<CTexture::theora>(&meta) != nullptr);

    if (!theora.pTheora->Update(theora.m_play_time != std::numeric_limits<u32>::max() ? theora.m_play_time : Device.dwTimeContinual))
    {
    apply:
        Apply(cmd_list, dwStage);
        return;
    }

    Microsoft::WRL::ComPtr<::ID3D11Texture2D> tex;
    XR_ASSERT(xr::hr(theora.staging.As(&tex)));
    auto& context = *cmd_list.context();

    ::D3D11_MAPPED_SUBRESOURCE mapData;
    XR_ASSERT(xr::hr(context.Map(tex.Get(), 0, ::D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mapData)));

    const auto height = theora.pTheora->Height();
    theora.pTheora->DecompressFrame(std::span{static_cast<std::byte*>(mapData.pData), (height + (height / 2)) * mapData.RowPitch}, mapData.RowPitch);

    context.Unmap(tex.Get(), 0);
    context.CopyResource(theora.yuv.Get(), theora.staging.Get());

    Microsoft::WRL::ComPtr<::ID3D11VideoContext> vcontext;
    XR_ASSERT(xr::hr(context.QueryInterface(IID_PPV_ARGS(&vcontext))));

    ::D3D11_VIDEO_PROCESSOR_COLOR_SPACE colorspace{};
    colorspace.RGB_Range = 1;
    colorspace.Nominal_Range = ::D3D11_VIDEO_PROCESSOR_NOMINAL_RANGE::D3D11_VIDEO_PROCESSOR_NOMINAL_RANGE_16_235;

    vcontext->VideoProcessorSetStreamColorSpace(theora.processor.Get(), 0, &colorspace);

    colorspace.RGB_Range = 0;
    colorspace.Nominal_Range = ::D3D11_VIDEO_PROCESSOR_NOMINAL_RANGE::D3D11_VIDEO_PROCESSOR_NOMINAL_RANGE_0_255;

    vcontext->VideoProcessorSetOutputColorSpace(theora.processor.Get(), &colorspace);

    ::D3D11_VIDEO_PROCESSOR_STREAM stream{};
    stream.Enable = true;
    stream.pInputSurface = theora.input.Get();

    XR_ASSERT(xr::hr(vcontext->VideoProcessorBlt(theora.processor.Get(), theora.output.Get(), 0, 1, &stream)));

    goto apply;
}

void CTexture::set_slice(gsl::index slice)
{
    m_pSRView = (slice < 0) ? srv_all : srv_per_slice[slice];
    curr_slice = slice;
}

void CTexture::Preload(const char* Name)
{
    auto& desc = RImplementation.Resources->m_textures_description;
    const shared_str name{Name};

    m_bumpmap = desc.GetBumpName(name);
    m_material = desc.GetMaterial(name);
}

void CTexture::Preload() { Preload(cName.c_str()); }

void CTexture::load_normal(gsl::czstring name)
{
    u32 mem{0};
    pSurface = RImplementation.texture_load(name, mem);

    // Calc memory usage and preload into vid-mem
    if (pSurface != nullptr)
    {
        XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView)));
        memUsage = mem;
    }
}

void CTexture::load_font(std::string_view name)
{
    auto& font = meta.emplace<CTexture::font>();
    const auto params = xr::font_atlas_get(name);

    font.atlas = &params.first;
    font.texture_idx = params.second;
    auto image = skb::skb_image_atlas_get_texture(font.atlas, font.texture_idx);

    ::D3D11_TEXTURE2D_DESC desc{};
    desc.Width = image->width;
    desc.Height = image->height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = image->bpp == 4 ? ::DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM : ::DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = ::D3D11_USAGE::D3D11_USAGE_DEFAULT;
    desc.BindFlags = ::D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

    const ::D3D11_SUBRESOURCE_DATA data{image->buffer, gsl::narrow_cast<u32>(image->stride_bytes), 0};

    ::ID3D11Texture2D* tex;
    XR_ASSERT(xr::hr(HW.pDevice->CreateTexture2D(&desc, &data, &tex)));
    pSurface = tex;

    desc.Usage = ::D3D11_USAGE::D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = ::D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

    Microsoft::WRL::ComPtr<::ID3D11Texture2D> staging;
    XR_ASSERT(xr::hr(HW.pDevice->CreateTexture2D(&desc, &data, &staging)));
    XR_ASSERT(xr::hr(staging.As(&font.staging)));

    XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView)));
    memUsage = data.SysMemPitch * desc.Height * 2;
}

void CTexture::load_seq(gsl::czstring fn)
{
    auto& seq = meta.emplace<CTexture::seq>();
    const auto _fs = absl::WrapUnique(FS.r_open(fn));

    string256 buffer;
    _fs->r_string(buffer, sizeof(buffer));

    if (std::is_eq(xr::strcasecmp(buffer, "cycled")))
    {
        seq.seqCycles = true;
        _fs->r_string(buffer, sizeof(buffer));
    }

    const std::string_view val{buffer};
    const auto res = scn::scan_int<u32>(val);
    XR_ASSERT(res, res.error().msg(), fn, val);

    const auto fps = res->value();
    XR_ASSERT(fps > 0 && fps <= 1000, "", fn, val, fps);
    seq.seqMSPF = 1000 / fps;

    while (!_fs->eof())
    {
        _fs->r_string(buffer, sizeof(buffer));
        std::ignore = _Trim(buffer);

        if (buffer[0] == '\0')
            continue;

        // Load another texture
        u32 mem{0};
        pSurface = RImplementation.texture_load(buffer, mem);

        if (pSurface != nullptr)
        {
            seq.seqDATA.emplace_back().Attach(pSurface);
            XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &seq.m_seqSRView.emplace_back())));

            memUsage += mem;
        }
    }

    XR_ASSERT(!seq.seqDATA.empty(), "", fn);
    pSurface = seq.seqDATA.back().Get();

    ::D3D11_RESOURCE_DIMENSION type;
    pSurface->GetType(&type);

    if (type == ::D3D11_RESOURCE_DIMENSION::D3D11_RESOURCE_DIMENSION_TEXTURE2D)
    {
        ::D3D11_TEXTURE2D_DESC desc{};
        static_cast<::ID3D11Texture2D*>(pSurface)->GetDesc(&desc);

        desc_Width = desc.Width;
        desc_Height = desc.Height;
    }

    pSurface = nullptr;
}

void CTexture::load_theora(gsl::czstring fn)
{
    auto& theora = meta.emplace<CTexture::theora>();

    theora.pTheora = std::make_unique<CTheoraSurface>();
    XR_ASSERT(theora.pTheora->Load(fn), "can't open video stream", fn);
    theora.pTheora->Play(true, Device.dwTimeContinual);

    const auto width = theora.pTheora->Width();
    const auto height = theora.pTheora->Height();

    ::D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = ::DXGI_FORMAT::DXGI_FORMAT_NV12;
    desc.SampleDesc.Count = 1;
    desc.Usage = ::D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = ::D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

    Microsoft::WRL::ComPtr<::ID3D11Texture2D> tex;
    XR_ASSERT(xr::hr(HW.pDevice->CreateTexture2D(&desc, nullptr, &tex)), "invalid video stream", fn);
    XR_ASSERT(xr::hr(tex.As(&theora.staging)));

    desc.Usage = ::D3D11_USAGE::D3D11_USAGE_DEFAULT;
    desc.BindFlags = ::D3D11_BIND_FLAG::D3D11_BIND_DECODER;
    desc.CPUAccessFlags = 0;

    XR_ASSERT(xr::hr(HW.pDevice->CreateTexture2D(&desc, nullptr, tex.ReleaseAndGetAddressOf())));
    XR_ASSERT(xr::hr(tex.As(&theora.yuv)));

    desc.Format = ::DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BindFlags = ::D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | ::D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;

    ::ID3D11Texture2D* pTexture;
    XR_ASSERT(xr::hr(HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture)));
    pSurface = pTexture;

    Microsoft::WRL::ComPtr<::ID3D11VideoDevice> device;
    XR_ASSERT(xr::hr(HW.pDevice.As(&device)));

    ::D3D11_VIDEO_PROCESSOR_CONTENT_DESC vdesc{};
    vdesc.InputFrameFormat = ::D3D11_VIDEO_FRAME_FORMAT::D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
    vdesc.InputWidth = width;
    vdesc.InputHeight = height;
    vdesc.OutputWidth = width;
    vdesc.OutputHeight = height;

    Microsoft::WRL::ComPtr<::ID3D11VideoProcessorEnumerator> penum;
    XR_ASSERT(xr::hr(device->CreateVideoProcessorEnumerator(&vdesc, &penum)));
    XR_ASSERT(xr::hr(device->CreateVideoProcessor(penum.Get(), 0, &theora.processor)));

    ::D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC indesc{};
    indesc.ViewDimension = ::D3D11_VPIV_DIMENSION::D3D11_VPIV_DIMENSION_TEXTURE2D;
    XR_ASSERT(xr::hr(device->CreateVideoProcessorInputView(theora.yuv.Get(), penum.Get(), &indesc, &theora.input)));

    ::D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC outdesc{};
    outdesc.ViewDimension = ::D3D11_VPOV_DIMENSION::D3D11_VPOV_DIMENSION_TEXTURE2D;
    XR_ASSERT(xr::hr(device->CreateVideoProcessorOutputView(pSurface, penum.Get(), &outdesc, &theora.output)));

    XR_ASSERT(xr::hr(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView)));
    // Y + UV, 2 textures
    memUsage = (width * height + ((width / 2) * (height / 2)) * 2) * 2;
    // RGBA
    memUsage += width * height * sizeof(u32);
}

void CTexture::Load(const char* Name)
{
    if (bLoaded)
        return;

    bLoaded = true;
    memUsage = 0;

    const std::string_view name{Name};

    if (name.starts_with("$user$") || std::is_eq(xr::strcasecmp(name, "$null")))
        return;

    Preload(Name);

    string_path fn;
    if (name.starts_with("$font$"))
        load_font(name);
    else if (FS.exist(fn, "$game_textures$", Name, ".seq") != nullptr)
        load_seq(fn);
    else if (FS.exist(fn, "$game_textures$", Name, ".ogm") != nullptr)
        load_theora(fn);
    else
        load_normal(Name);

    if (pSurface != nullptr)
    {
        ::D3D11_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);

        if (type == ::D3D11_RESOURCE_DIMENSION::D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        {
            ::D3D11_TEXTURE2D_DESC desc{};
            static_cast<::ID3D11Texture2D*>(pSurface)->GetDesc(&desc);

            desc_Width = desc.Width;
            desc_Height = desc.Height;
        }

        pSurface->SetPrivateData(::WKPDID_D3DDebugObjectName, cName.size(), cName.c_str());
    }

    PostLoad();
}

void CTexture::Load() { Load(cName.c_str()); }

void CTexture::Unload()
{
#ifdef DEBUG
    string_path msg_buff;
    xr_sprintf(msg_buff, sizeof(msg_buff), "* Unloading texture [%s] pSurface RefCount=", cName.c_str());
    _SHOW_REF(msg_buff, pSurface);
#endif // DEBUG

    bLoaded = false;

    if (std::holds_alternative<seq>(meta))
        pSurface = nullptr;

    meta.emplace<normal>();

    _RELEASE(pSurface);
    _RELEASE(srv_all);

    for (auto& srv : srv_per_slice)
        _RELEASE(srv);

    bind = CallMe::fromMethod<&CTexture::apply_load>(this);
}

void CTexture::video_Play(bool looped, u32 _time)
{
    const auto theora = std::get_if<CTexture::theora>(&meta);
    if (theora == nullptr)
        return;

    theora->m_play_time = _time;
    theora->pTheora->Play(looped, _time != std::numeric_limits<u32>::max() ? _time : Device.dwTimeContinual);
}

void CTexture::video_Pause(bool state)
{
    if (const auto theora = std::get_if<CTexture::theora>(&meta); theora != nullptr)
        theora->pTheora->Pause(state);
}

void CTexture::video_Stop()
{
    if (const auto theora = std::get_if<CTexture::theora>(&meta); theora != nullptr)
        theora->pTheora->Stop();
}

bool CTexture::video_IsPlaying() const
{
    if (const auto theora = std::get_if<CTexture::theora>(&meta); theora != nullptr)
        return theora->pTheora->IsPlaying();

    return false;
}
