#include "stdafx.h"

#include "../xrRender/ResourceManager.h"
#include "dx10TextureUtils.h"

CRT::~CRT()
{
    destroy();

    // release external reference
    RImplementation.Resources->_DeleteRT(this);
}

void CRT::create(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount, u32 slices_num, Flags32 flags)
{
    if (pSurface != nullptr)
        return;

    XR_ASSERT(HW.pDevice != nullptr && Name != nullptr && Name[0] != '\0');
    XR_ASSERT(w > 0 && h > 0, "", Name, w, h);

    _order = CPU::QPC(); // Device.GetTimerGlobal()->GetElapsed_clk();

    dwWidth = w;
    dwHeight = h;
    fmt = f;
    sampleCount = SampleCount;
    n_slices = slices_num;

    // Check width-and-height of render target surface
    if (w > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION || h > D3D_REQ_TEXTURE2D_U_OR_V_DIMENSION)
        return;

    // Select usage
    u32 usage = D3DUSAGE_RENDERTARGET;
    if (D3DFMT_D24X8 == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (D3DFMT_D24S8 == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (D3DFMT_D15S1 == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (D3DFMT_D16 == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (D3DFMT_D16_LOCKABLE == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if (D3DFMT_D32F_LOCKABLE == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;
    else if ((D3DFORMAT)MAKEFOURCC('D', 'F', '2', '4') == fmt)
        usage = D3DUSAGE_DEPTHSTENCIL;

    DXGI_FORMAT dx10FMT;
    if (fmt == D3DFMT_D24S8)
    {
        dx10FMT = DXGI_FORMAT_R24G8_TYPELESS;
        usage = D3DUSAGE_DEPTHSTENCIL;
    }
    else
    {
        dx10FMT = dx10TextureUtils::ConvertTextureFormat(fmt);
    }

    const bool useAsDepth = usage == D3DUSAGE_DEPTHSTENCIL;

    if (flags.test(CreateBase))
    {
        dwFlags |= CreateBase;

        if (!useAsDepth)
        {
            const std::string_view name{Name};
            const auto pos = XR_ASSERT_VAL(name.rfind('_') != std::string_view::npos, "", name);

            const auto res = scn::scan_int<u32>(name.substr(pos + 1));
            XR_ASSERT(res, res.error().msg(), name, pos);

            XR_ASSERT(xr::hr(HW.m_pSwapChain->GetBuffer(res->value(), IID_PPV_ARGS(&pSurface))));
        }
    }

    // Try to create texture/surface

    // Create the render target texture
    D3D_TEXTURE2D_DESC desc{};
    if (pSurface)
        pSurface->GetDesc(&desc);
    else
    {
        desc.Width = dwWidth;
        desc.Height = dwHeight;
        desc.MipLevels = 1;
        desc.ArraySize = n_slices;
        desc.Format = dx10FMT;
        desc.SampleDesc.Count = SampleCount;
        desc.Usage = D3D_USAGE_DEFAULT;
        desc.BindFlags = D3D_BIND_SHADER_RESOURCE | (useAsDepth ? D3D_BIND_DEPTH_STENCIL : D3D_BIND_RENDER_TARGET);

        if (SampleCount > 1)
            desc.SampleDesc.Quality = u32(D3D_STANDARD_MULTISAMPLE_PATTERN);

        if (!useAsDepth && SampleCount == 1 && flags.test(CreateUAV))
        {
            dwFlags |= CreateUAV;
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }

        XR_ASSERT(xr::hr(HW.pDevice->CreateTexture2D(&desc, nullptr, &pSurface)));
    }

    if (pSurface)
    {
        HW.stats_manager.increment_stats_rtarget(pSurface);
        pSurface->SetPrivateData(WKPDID_D3DDebugObjectName, cName.size(), cName.c_str());
    }

    if (useAsDepth)
    {
        D3D_DEPTH_STENCIL_VIEW_DESC ViewDesc{};

        if (SampleCount <= 1)
        {
            ViewDesc.ViewDimension = n_slices > 1 ? D3D_DSV_DIMENSION_TEXTURE2DARRAY : D3D_DSV_DIMENSION_TEXTURE2D;
            if (n_slices > 1)
                ViewDesc.Texture2DArray.ArraySize = n_slices;
        }
        else
        {
            ViewDesc.ViewDimension = n_slices > 1 ? D3D_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D_DSV_DIMENSION_TEXTURE2DMS;
            if (n_slices > 1)
                ViewDesc.Texture2DMSArray.ArraySize = n_slices;
        }

        switch (desc.Format)
        {
        case DXGI_FORMAT_R16_TYPELESS: ViewDesc.Format = DXGI_FORMAT_D16_UNORM; break;
        case DXGI_FORMAT_R24G8_TYPELESS: ViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
        case DXGI_FORMAT_R32_TYPELESS: ViewDesc.Format = DXGI_FORMAT_D32_FLOAT; break;
        case DXGI_FORMAT_R32G8X24_TYPELESS: ViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT; break;
        default: ViewDesc.Format = desc.Format;
        }

        XR_ASSERT(xr::hr(HW.pDevice->CreateDepthStencilView(pSurface, &ViewDesc, &dsv_all)));

        char name[128];
        xr_sprintf(name, "%s:all", Name);
        dsv_all->SetPrivateData(WKPDID_D3DDebugObjectName, xr_strlen(name), name);

        dsv_per_slice.resize(n_slices);
        for (u32 idx = 0; idx < n_slices; idx++)
        {
            if (SampleCount <= 1)
            {
                ViewDesc.Texture2DArray.ArraySize = 1;
                ViewDesc.Texture2DArray.FirstArraySlice = idx;
            }
            else
            {
                ViewDesc.Texture2DMSArray.ArraySize = 1;
                ViewDesc.Texture2DMSArray.FirstArraySlice = idx;
            }

            XR_ASSERT(xr::hr(HW.pDevice->CreateDepthStencilView(pSurface, &ViewDesc, &dsv_per_slice[idx])));

            xr_sprintf(name, "%s:s%d", Name, idx);
            dsv_per_slice[idx]->SetPrivateData(WKPDID_D3DDebugObjectName, xr_strlen(name), name);
        }

        for (ctx_id_t id = 0; id < R__NUM_CONTEXTS; id++)
            set_slice_write(id, -1);
    }
    else
    {
        XR_ASSERT(xr::hr(HW.pDevice->CreateRenderTargetView(pSurface, nullptr, &pRT)));
    }

    if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
        UAVDesc.Format = dx10FMT;
        UAVDesc.ViewDimension = n_slices > 1 ? D3D_UAV_DIMENSION_TEXTURE2DARRAY : D3D_UAV_DIMENSION_TEXTURE2D;
        UAVDesc.Buffer.NumElements = dwWidth * dwHeight;

        if (n_slices > 1)
            UAVDesc.Texture2DArray.ArraySize = n_slices;

        XR_ASSERT(xr::hr(HW.pDevice->CreateUnorderedAccessView(pSurface, &UAVDesc, &pUAView)));
    }

    pTexture._set(RImplementation.Resources->_CreateTexture(Name));
    pTexture->surface_set(pSurface);
}

void CRT::destroy()
{
    if (pTexture._get())
    {
        pTexture->surface_set(nullptr);
        pTexture.destroy();
        pTexture._set(nullptr);
    }

    _RELEASE(pRT);

    for (auto& dsv : dsv_per_slice)
        _RELEASE(dsv);

    _RELEASE(dsv_all);

    HW.stats_manager.decrement_stats_rtarget(pSurface);

    _RELEASE(pSurface);
    _RELEASE(pUAView);
}

void CRT::set_slice_read(gsl::index slice)
{
    XR_ASSERT(slice >= -1 && slice < n_slices, "", slice, n_slices);
    pTexture->set_slice(slice);
}

void CRT::set_slice_write(ctx_id_t context_id, gsl::index slice)
{
    XR_ASSERT(slice >= -1 && slice < n_slices, "", slice, n_slices);
    pZRT[context_id] = slice < 0 ? dsv_all : dsv_per_slice[slice];
}

void CRT::reset_begin() { destroy(); }
void CRT::reset_end() { create(cName.c_str(), dwWidth, dwHeight, fmt, sampleCount, n_slices, {dwFlags}); }

void resptrcode_crt::create(LPCSTR Name, u32 w, u32 h, D3DFORMAT f, u32 SampleCount, u32 slices_num, Flags32 flags)
{
    _set(RImplementation.Resources->_CreateRT(Name, w, h, f, SampleCount, slices_num, flags));
}
