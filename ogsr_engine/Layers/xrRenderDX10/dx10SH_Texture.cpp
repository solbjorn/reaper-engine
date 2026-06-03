#include "stdafx.h"

#include "../xrRender/ResourceManager.h"

#include "../../xr_3da/tntQAVI.h"
#include "../../xr_3da/xrTheora_Surface.h"

#include "StateManager/dx10ShaderResourceStateCache.h"

namespace skb
{
XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wc++98-compat-extra-semi");

#include <skb_image_atlas.h>

XR_DIAG_POP();
} // namespace skb

void resptrcode_texture::create(LPCSTR _name) { _set(RImplementation.Resources->_CreateTexture(_name)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
    seqMSPF = 0;
    flags.memUsage = 0;
    flags.bLoaded = false;
    flags.seqCycles = 0;
    m_material = 1.0f;
    bind = CallMe::fromMethod<&CTexture::apply_load>(this);
}

CTexture::~CTexture()
{
    Unload();

    // release external reference
    RImplementation.Resources->_DeleteTexture(this);
}

void CTexture::surface_set(ID3DBaseTexture* surf)
{
    if (pSurface == surf)
        return;

    if (surf)
        surf->AddRef();
    _RELEASE(pSurface);

    pSurface = surf;

    if (pSurface)
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);

        if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
        {
            D3D_TEXTURE2D_DESC desc{};

            ID3DTexture2D* T = (ID3DTexture2D*)pSurface;
            T->GetDesc(&desc);

            desc_Width = desc.Width;
            desc_Height = desc.Height;

            D3D_SHADER_RESOURCE_VIEW_DESC ViewDesc{};

            if (desc.MiscFlags & D3D_RESOURCE_MISC_TEXTURECUBE)
            {
                ViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
                ViewDesc.TextureCube.MipLevels = desc.MipLevels;
            }
            else
            {
                const bool isArray = desc.ArraySize > 1;
                if (desc.SampleDesc.Count <= 1)
                {
                    ViewDesc.ViewDimension = isArray ? D3D_SRV_DIMENSION_TEXTURE2DARRAY : D3D_SRV_DIMENSION_TEXTURE2D;
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
                    ViewDesc.ViewDimension = isArray ? D3D_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D_SRV_DIMENSION_TEXTURE2DMS;
                    if (isArray)
                        ViewDesc.Texture2DMSArray.ArraySize = desc.ArraySize;
                }
            }

            switch (desc.Format)
            {
            case DXGI_FORMAT_R32G8X24_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; break;
            case DXGI_FORMAT_R24G8_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
            case DXGI_FORMAT_R32_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
            case DXGI_FORMAT_R16_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R16_FLOAT; break;
            default: break;
            }

            _RELEASE(srv_all);
            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &srv_all));

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

                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &srv_per_slice[id]));
            }

            set_slice(-1);
        }
        else
        {
            _RELEASE(m_pSRView);
            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, NULL, &m_pSRView));
        }
    }
}

ID3DBaseTexture* CTexture::surface_get() const { return pSurface; }

void CTexture::PostLoad()
{
    if (atlas != nullptr)
        bind = CallMe::fromMethod<&CTexture::apply_font>(this);
    else if (pTheora != nullptr)
        bind = CallMe::fromMethod<&CTexture::apply_theora>(this);
    else if (pAVI)
        bind = CallMe::fromMethod<&CTexture::apply_avi>(this);
    else if (!m_seqSRView.empty())
        bind = CallMe::fromMethod<&CTexture::apply_seq>(this);
    else
        bind = CallMe::fromMethod<&CTexture::apply_normal>(this);
}

void CTexture::apply_load(CBackend& cmd_list, u32 dwStage)
{
    if (!flags.bLoaded)
        Load();
    else
        PostLoad();
    bind(cmd_list, dwStage);
}

void CTexture::Apply(CBackend& cmd_list, u32 dwStage) const
{
    if (dwStage < rstVertex) //	Pixel shader stage resources
    {
        cmd_list.SRVSManager.SetPSResource(dwStage, m_pSRView);
    }
    else if (dwStage < rstGeometry) //	Vertex shader stage resources
    {
        cmd_list.SRVSManager.SetVSResource(dwStage - rstVertex, m_pSRView);
    }
    else if (dwStage < rstHull) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetGSResource(dwStage - rstGeometry, m_pSRView);
    }
    else if (dwStage < rstDomain) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetHSResource(dwStage - rstHull, m_pSRView);
    }
    else if (dwStage < rstCompute) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetDSResource(dwStage - rstDomain, m_pSRView);
    }
    else if (dwStage < rstInvalid) //	Geometry shader stage resources
    {
        cmd_list.SRVSManager.SetCSResource(dwStage - rstCompute, m_pSRView);
    }
    else
        VERIFY("Invalid stage");
}

void CTexture::apply_font(CBackend& cmd_list, u32 dwStage) const
{
    const auto bounds = skb::skb_image_atlas_get_and_reset_texture_dirty_bounds(atlas, texture_idx);
    if (skb::skb_rect2i_is_empty(bounds))
    {
    apply:
        Apply(cmd_list, dwStage);
        return;
    }

    auto tex = static_cast<ID3DTexture2D*>(staging);
    auto& context = *cmd_list.context();

    D3D_MAPPED_TEXTURE2D data;
    R_CHK(context.Map(tex, 0, D3D11_MAP_WRITE, 0, &data));

    auto image = skb::skb_image_atlas_get_texture(atlas, texture_idx);
    R_ASSERT(data.RowPitch == gsl::narrow_cast<u32>(image->stride_bytes));
    const auto bpp = image->bpp;

    for (decltype(bounds.height) y{0}; y < bounds.height; ++y)
    {
        const auto off = (bounds.y + y) * data.RowPitch + bounds.x * bpp;
        std::memcpy(&static_cast<std::byte*>(data.pData)[off], &image->buffer[off], bounds.width * bpp);
    }

    context.Unmap(tex, 0);

    const D3D11_BOX box{gsl::narrow_cast<u32>(bounds.x),
                        gsl::narrow_cast<u32>(bounds.y),
                        0,
                        gsl::narrow_cast<u32>(bounds.x + bounds.width),
                        gsl::narrow_cast<u32>(bounds.y + bounds.height),
                        1};
    context.CopySubresourceRegion(pSurface, 0, bounds.x, bounds.y, 0, staging, 0, &box);

    goto apply;
}

void CTexture::apply_theora(CBackend& cmd_list, u32 dwStage)
{
    if (pTheora->Update(m_play_time != 0xFFFFFFFF ? m_play_time : Device.dwTimeContinual))
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);
        R_ASSERT(D3D_RESOURCE_DIMENSION_TEXTURE2D == type);
        ID3DTexture2D* T2D = (ID3DTexture2D*)pSurface;
        D3D_MAPPED_TEXTURE2D mapData;

        auto* pContext = cmd_list.context();
        u32 _w = pTheora->Width(false);

        R_CHK(pContext->Map(T2D, 0, D3D_MAP_WRITE_DISCARD, 0, &mapData));
        R_ASSERT(mapData.RowPitch == _w * 4);
        int _pos = 0;
        pTheora->DecompressFrame((u32*)mapData.pData, _w - pTheora->Width(true), _pos);
        VERIFY(u32(_pos) == pTheora->Height(true) * _w);
        pContext->Unmap(T2D, 0);
    }

    Apply(cmd_list, dwStage);
}

void CTexture::apply_avi(CBackend& cmd_list, u32 dwStage) const
{
    if (pAVI->NeedUpdate())
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);
        R_ASSERT(D3D_RESOURCE_DIMENSION_TEXTURE2D == type);
        ID3DTexture2D* T2D = (ID3DTexture2D*)pSurface;
        D3D_MAPPED_TEXTURE2D mapData;

        auto* pContext = cmd_list.context();

        R_CHK(pContext->Map(T2D, 0, D3D_MAP_WRITE_DISCARD, 0, &mapData));
        R_ASSERT(mapData.RowPitch == pAVI->m_dwWidth * 4);
        BYTE* ptr;
        pAVI->GetFrame(&ptr);
        std::memcpy(mapData.pData, ptr, pAVI->m_dwWidth * pAVI->m_dwHeight * 4);
        pContext->Unmap(T2D, 0);
    }

    Apply(cmd_list, dwStage);
}

void CTexture::apply_seq(CBackend& cmd_list, u32 dwStage)
{
    // SEQ
    u32 frame = Device.dwTimeContinual / seqMSPF; // Device.dwTimeGlobal
    u32 frame_data = m_seqSRView.size();
    if (flags.seqCycles)
    {
        u32 frame_id = frame % (frame_data * 2);
        if (frame_id >= frame_data)
            frame_id = (frame_data - 1) - (frame_id % frame_data);
        pSurface = seqDATA[frame_id];
        m_pSRView = m_seqSRView[frame_id];
    }
    else
    {
        u32 frame_id = frame % frame_data;
        pSurface = seqDATA[frame_id];
        m_pSRView = m_seqSRView[frame_id];
    }

    Apply(cmd_list, dwStage);
}

void CTexture::apply_normal(CBackend& cmd_list, u32 dwStage) const { Apply(cmd_list, dwStage); }

void CTexture::set_slice(gsl::index slice)
{
    m_pSRView = (slice < 0) ? srv_all : srv_per_slice[slice];
    curr_slice = slice;
}

void CTexture::Preload() { Preload(cName.c_str()); }

void CTexture::Preload(const char* Name)
{
    m_bumpmap = RImplementation.Resources->m_textures_description.GetBumpName(shared_str{Name});
    m_material = RImplementation.Resources->m_textures_description.GetMaterial(shared_str{Name});
}

void CTexture::Load() { Load(cName.c_str()); }

void CTexture::Load(const char* Name)
{
    if (flags.bLoaded)
        return;

    flags.bLoaded = true;
    flags.memUsage = 0;

    const std::string_view name{Name};

    if (std::is_eq(xr::strcasecmp(name, "$null")))
        return;

    if (name.starts_with("$user$"))
        return;

    Preload(Name);
    string_path fn;

    if (name.starts_with("$font$"))
    {
        const auto params = xr::font_atlas_get(name);
        atlas = &params.first;
        texture_idx = params.second;
        auto image = skb::skb_image_atlas_get_texture(atlas, texture_idx);

        D3D_TEXTURE2D_DESC desc{};
        desc.Width = image->width;
        desc.Height = image->height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = image->bpp == 4 ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D_BIND_SHADER_RESOURCE;

        const D3D_SUBRESOURCE_DATA data{image->buffer, gsl::narrow_cast<u32>(image->stride_bytes), 0};
        ID3DTexture2D* tex;

        R_CHK(HW.pDevice->CreateTexture2D(&desc, &data, &tex));
        pSurface = tex;

        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        R_CHK(HW.pDevice->CreateTexture2D(&desc, &data, &tex));
        staging = tex;

        CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
        flags.memUsage = data.SysMemPitch * desc.Height * 2;
    }
    else if (FS.exist(fn, "$game_textures$", Name, ".ogm"))
    {
        // OGM
        pTheora = xr_new<CTheoraSurface>();
        m_play_time = 0xFFFFFFFF;

        if (!pTheora->Load(fn))
        {
            xr_delete(pTheora);
            FATAL("Can't open video stream");
        }
        else
        {
            flags.memUsage = pTheora->Width(true) * pTheora->Height(true) * 4;
            pTheora->Play(TRUE, Device.dwTimeContinual);

            // Now create texture
            ID3DTexture2D* pTexture{};

            D3D_TEXTURE2D_DESC desc{};
            desc.Width = pTheora->Width(false);
            desc.Height = pTheora->Height(false);
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D_USAGE_DYNAMIC;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
            HRESULT hrr = HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

            if (FAILED(hrr))
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pTheora);
                pSurface = nullptr;
                m_pSRView = nullptr;
            }
            else
            {
                pSurface = pTexture;
                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
            }
        }
    }
    else if (FS.exist(fn, "$game_textures$", Name, ".avi"))
    {
        // AVI
        pAVI = xr_new<CAviPlayerCustom>();

        if (!pAVI->Load(fn))
        {
            xr_delete(pAVI);
            FATAL("Can't open video stream");
        }
        else
        {
            flags.memUsage = pAVI->m_dwWidth * pAVI->m_dwHeight * 4;

            ID3DTexture2D* pTexture{};

            D3D_TEXTURE2D_DESC desc{};
            desc.Width = pAVI->m_dwWidth;
            desc.Height = pAVI->m_dwHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D_USAGE_DYNAMIC;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
            HRESULT hrr = HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

            if (FAILED(hrr))
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pAVI);
                pSurface = nullptr;
                m_pSRView = nullptr;
            }
            else
            {
                pSurface = pTexture;
                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
            }
        }
    }
    else if (FS.exist(fn, "$game_textures$", Name, ".seq"))
    {
        // Sequence
        string256 buffer;
        IReader* _fs = FS.r_open(fn);

        flags.seqCycles = FALSE;
        _fs->r_string(buffer, sizeof(buffer));
        if (std::is_eq(xr::strcasecmp(buffer, "cycled")))
        {
            flags.seqCycles = TRUE;
            _fs->r_string(buffer, sizeof(buffer));
        }

        const auto res = scn::scan_int<u32>(buffer);
        R_ASSERT(res, res.error().msg());
        seqMSPF = 1000 / std::max(res->value(), 1u);

        while (!_fs->eof())
        {
            _fs->r_string(buffer, sizeof(buffer));
            std::ignore = _Trim(buffer);

            if (buffer[0])
            {
                // Load another texture
                u32 mem = 0;
                pSurface = RImplementation.texture_load(buffer, mem);

                if (pSurface)
                {
                    seqDATA.push_back(pSurface);
                    m_seqSRView.push_back(nullptr);

                    HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_seqSRView.back());
                    flags.memUsage += mem;
                }
            }
        }

        FS.r_close(_fs);

        if (pSurface)
        {
            D3D_RESOURCE_DIMENSION type;
            pSurface->GetType(&type);

            if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
            {
                D3D_TEXTURE2D_DESC desc{};

                ID3DTexture2D* T = (ID3DTexture2D*)pSurface;
                T->GetDesc(&desc);

                desc_Width = desc.Width;
                desc_Height = desc.Height;
            }
        }

        pSurface = nullptr;
    }
    else
    {
        // Normal texture
        u32 mem = 0;
        pSurface = RImplementation.texture_load(Name, mem);

        // Calc memory usage and preload into vid-mem
        if (pSurface)
        {
            flags.memUsage = mem;
            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, nullptr, &m_pSRView));
        }
    }

    if (pSurface)
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);

        if (D3D_RESOURCE_DIMENSION_TEXTURE2D == type)
        {
            D3D_TEXTURE2D_DESC desc{};

            ID3DTexture2D* T = (ID3DTexture2D*)pSurface;
            T->GetDesc(&desc);

            desc_Width = desc.Width;
            desc_Height = desc.Height;
        }
    }

    if (pSurface)
    {
        pSurface->SetPrivateData(WKPDID_D3DDebugObjectName, cName.size(), cName.c_str());
    }

    PostLoad();
}

void CTexture::Unload()
{
#ifdef DEBUG
    string_path msg_buff;
    xr_sprintf(msg_buff, sizeof(msg_buff), "* Unloading texture [%s] pSurface RefCount=", cName.c_str());
    _SHOW_REF(msg_buff, pSurface);
#endif // DEBUG

    flags.bLoaded = false;

    if (!seqDATA.empty())
    {
        for (u32 I = 0; I < seqDATA.size(); I++)
        {
            _RELEASE(seqDATA[I]);
            _RELEASE(m_seqSRView[I]);
        }
        seqDATA.clear();
        m_seqSRView.clear();

        pSurface = nullptr;
    }

    _RELEASE(pSurface);
    _RELEASE(staging);
    _RELEASE(srv_all);

    for (auto& srv : srv_per_slice)
        _RELEASE(srv);

    xr_delete(pAVI);
    xr_delete(pTheora);

    bind = CallMe::fromMethod<&CTexture::apply_load>(this);
}

void CTexture::video_Play(BOOL looped, u32 _time)
{
    if (pTheora)
        pTheora->Play(looped, (_time != 0xFFFFFFFF) ? (m_play_time = _time) : Device.dwTimeContinual);
}

void CTexture::video_Pause(BOOL state) const
{
    if (pTheora)
        pTheora->Pause(state);
}

void CTexture::video_Stop() const
{
    if (pTheora)
        pTheora->Stop();
}

BOOL CTexture::video_IsPlaying() const { return (pTheora) ? pTheora->IsPlaying() : FALSE; }
