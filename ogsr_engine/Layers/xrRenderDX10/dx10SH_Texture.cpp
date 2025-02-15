#include "stdafx.h"

#include "../xrRender/ResourceManager.h"

#include "../../xr_3da/render.h"

#include "../../xr_3da/tntQAVI.h"
#include "../../xr_3da/xrTheora_Surface.h"

#include "../xrRender/dxRenderDeviceRender.h"

#include "StateManager/dx10ShaderResourceStateCache.h"

#define PRIORITY_HIGH 12
#define PRIORITY_NORMAL 8
#define PRIORITY_LOW 4

void resptrcode_texture::create(LPCSTR _name) { _set(DEV->_CreateTexture(_name)); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
    pSurface = NULL;
    m_pSRView = NULL;
    pAVI = NULL;
    pTheora = NULL;
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
    DEV->_DeleteTexture(this);
}

void CTexture::surface_set(ID3DBaseTexture* surf)
{
    if (pSurface == surf)
        return;

    if (surf)
        surf->AddRef();
    _RELEASE(pSurface);
    _RELEASE(m_pSRView);

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
                ViewDesc.TextureCube.MostDetailedMip = 0;
                ViewDesc.TextureCube.MipLevels = desc.MipLevels;
            }
            else
            {
                if (desc.SampleDesc.Count <= 1)
                {
                    ViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
                    ViewDesc.Texture2D.MostDetailedMip = 0;
                    ViewDesc.Texture2D.MipLevels = desc.MipLevels;
                }
                else
                {
                    ViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
                    ViewDesc.Texture2D.MostDetailedMip = 0;
                    ViewDesc.Texture2D.MipLevels = desc.MipLevels;
                }
            }

            ViewDesc.Format = DXGI_FORMAT_UNKNOWN;

            switch (desc.Format)
            {
            case DXGI_FORMAT_R24G8_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
            case DXGI_FORMAT_R32_TYPELESS: ViewDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
            }

            // this would be supported by DX10.1 but is not needed for stalker
            // if( ViewDesc.Format != DXGI_FORMAT_R24_UNORM_X8_TYPELESS )
            if ((desc.SampleDesc.Count <= 1) || (ViewDesc.Format != DXGI_FORMAT_R24_UNORM_X8_TYPELESS))
                CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, &ViewDesc, &m_pSRView));
            else
                m_pSRView = 0;
        }
        else
            CHK_DX(HW.pDevice->CreateShaderResourceView(pSurface, NULL, &m_pSRView));
    }
}

ID3DBaseTexture* CTexture::surface_get() const { return pSurface; }

void CTexture::PostLoad()
{
    if (pTheora)
        bind = CallMe::fromMethod<&CTexture::apply_theora>(this);
    else if (pAVI)
        bind = CallMe::fromMethod<&CTexture::apply_avi>(this);
    else if (!m_seqSRView.empty())
        bind = CallMe::fromMethod<&CTexture::apply_seq>(this);
    else
        bind = CallMe::fromMethod<&CTexture::apply_normal>(this);
}

void CTexture::apply_load(u32 dwStage)
{
    if (!flags.bLoaded)
        Load();
    else
        PostLoad();
    bind(dwStage);
};

void CTexture::Apply(u32 dwStage) const
{
    if (dwStage < rstVertex) //	Pixel shader stage resources
    {
        SRVSManager.SetPSResource(dwStage, m_pSRView);
    }
    else if (dwStage < rstGeometry) //	Vertex shader stage resources
    {
        SRVSManager.SetVSResource(dwStage - rstVertex, m_pSRView);
    }
    else if (dwStage < rstHull) //	Geometry shader stage resources
    {
        SRVSManager.SetGSResource(dwStage - rstGeometry, m_pSRView);
    }
    else if (dwStage < rstDomain) //	Geometry shader stage resources
    {
        SRVSManager.SetHSResource(dwStage - rstHull, m_pSRView);
    }
    else if (dwStage < rstCompute) //	Geometry shader stage resources
    {
        SRVSManager.SetDSResource(dwStage - rstDomain, m_pSRView);
    }
    else if (dwStage < rstInvalid) //	Geometry shader stage resources
    {
        SRVSManager.SetCSResource(dwStage - rstCompute, m_pSRView);
    }
    else
        VERIFY("Invalid stage");
}

void CTexture::apply_theora(u32 dwStage)
{
    if (pTheora->Update(m_play_time != 0xFFFFFFFF ? m_play_time : Device.dwTimeContinual))
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);
        R_ASSERT(D3D_RESOURCE_DIMENSION_TEXTURE2D == type);
        ID3DTexture2D* T2D = (ID3DTexture2D*)pSurface;
        D3D_MAPPED_TEXTURE2D mapData;

        u32 _w = pTheora->Width(false);

        // R_CHK(T2D->LockRect(0,&R,&rect,0));

        R_CHK(HW.pContext->Map(T2D, 0, D3D_MAP_WRITE_DISCARD, 0, &mapData));
        R_ASSERT(mapData.RowPitch == int(_w * 4));
        int _pos = 0;
        pTheora->DecompressFrame((u32*)mapData.pData, _w - pTheora->Width(true), _pos);
        VERIFY(u32(_pos) == pTheora->Height(true) * _w);
        // R_CHK				(T2D->UnlockRect(0));
        HW.pContext->Unmap(T2D, 0);
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
}

void CTexture::apply_avi(u32 dwStage) const
{
    if (pAVI->NeedUpdate())
    {
        D3D_RESOURCE_DIMENSION type;
        pSurface->GetType(&type);
        R_ASSERT(D3D_RESOURCE_DIMENSION_TEXTURE2D == type);
        ID3DTexture2D* T2D = (ID3DTexture2D*)pSurface;
        D3D_MAPPED_TEXTURE2D mapData;

        // R_CHK	(T2D->LockRect(0,&R,NULL,0));

        R_CHK(HW.pContext->Map(T2D, 0, D3D_MAP_WRITE_DISCARD, 0, &mapData));
        R_ASSERT(mapData.RowPitch == int(pAVI->m_dwWidth * 4));
        BYTE* ptr;
        pAVI->GetFrame(&ptr);
        CopyMemory(mapData.pData, ptr, pAVI->m_dwWidth * pAVI->m_dwHeight * 4);
        // R_CHK	(T2D->UnlockRect(0));
        HW.pContext->Unmap(T2D, 0);
    }
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
}

void CTexture::apply_seq(u32 dwStage)
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
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
}

void CTexture::apply_normal(u32 dwStage) const
{
    // CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
    Apply(dwStage);
};

void CTexture::Preload() { Preload(cName.c_str()); }

void CTexture::Preload(const char* Name)
{
    m_bumpmap = DEV->m_textures_description.GetBumpName(Name);
    m_material = DEV->m_textures_description.GetMaterial(Name);
}

void CTexture::Load() { Load(cName.c_str()); }

void CTexture::Load(const char* Name)
{
    if (flags.bLoaded)
        return;

    flags.bLoaded = true;
    flags.memUsage = 0;

    if (!stricmp(Name, "$null"))
        return;

    // we need to check only the beginning of the string,
    // so let's use strncmp instead of strstr.
    if (!strncmp(Name, "$user$", sizeof("$user$") - 1))
        return;

    Preload(Name);

    // Check for OGM
    string_path fn;
    if (FS.exist(fn, "$game_textures$", Name, ".ogm"))
    {
        // AVI
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
            ID3DTexture2D* pTexture = 0;

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
            desc.MiscFlags = 0;
            HRESULT hrr = HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

            if (FAILED(hrr))
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pTheora);
                pSurface = 0;
                m_pSRView = 0;
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

            ID3DTexture2D* pTexture = 0;

            D3D_TEXTURE2D_DESC desc{};
            desc.Width = pAVI->m_dwWidth;
            desc.Height = pAVI->m_dwHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D_USAGE_DYNAMIC;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;
            HRESULT hrr = HW.pDevice->CreateTexture2D(&desc, nullptr, &pTexture);

            if (FAILED(hrr))
            {
                FATAL("Invalid video stream");
                R_CHK(hrr);
                xr_delete(pAVI);
                pSurface = 0;
                m_pSRView = 0;
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
        if (0 == stricmp(buffer, "cycled"))
        {
            flags.seqCycles = TRUE;
            _fs->r_string(buffer, sizeof(buffer));
        }
        u32 fps = atoi(buffer);
        seqMSPF = 1000 / fps;

        while (!_fs->eof())
        {
            _fs->r_string(buffer, sizeof(buffer));
            _Trim(buffer);

            if (buffer[0])
            {
                // Load another texture
                u32 mem = 0;
                pSurface = ::RImplementation.texture_load(buffer, mem);

                if (pSurface)
                {
                    seqDATA.push_back(pSurface);

                    m_seqSRView.push_back(0);
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

        pSurface = 0;
    }
    else
    {
        // Normal texture
        u32 mem = 0;
        pSurface = ::RImplementation.texture_load(Name, mem);

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

        pSurface = 0;
        m_pSRView = 0;
    }

#ifdef DEBUG
    _SHOW_REF(msg_buff, pSurface);
#endif // DEBUG
    _RELEASE(pSurface);
    _RELEASE(m_pSRView);

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
