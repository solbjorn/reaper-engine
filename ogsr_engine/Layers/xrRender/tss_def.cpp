#include "stdafx.h"

#include "../xrRenderDX10/dx10StateUtils.h"
#include "tss_def.h"

void SimulatorStates::set_RS(u32 a, u32 b)
{
    // Search duplicates
    for (int t = 0; t < int(States.size()); t++)
    {
        State& S = States[t];
        if ((0 == S.type) && (a == S.v1))
        {
            States.erase(States.begin() + t);
            break;
        }
    }

    // Register
    State st;
    st.set_RS(a, b);
    States.push_back(st);
}

void SimulatorStates::set_TSS(u32 a, u32 b, u32 c)
{
    // Search duplicates
    for (int t = 0; t < int(States.size()); t++)
    {
        State& S = States[t];
        if ((1 == S.type) && (a == S.v1) && (b == S.v2))
        {
            States.erase(States.begin() + t);
            break;
        }
    }

    // Register
    State st;
    st.set_TSS(a, b, c);
    States.push_back(st);
}

void SimulatorStates::set_SAMP(u32 a, u32 b, u32 c)
{
    // Search duplicates
    for (int t = 0; t < int(States.size()); t++)
    {
        State& S = States[t];
        if ((2 == S.type) && (a == S.v1) && (b == S.v2))
        {
            States.erase(States.begin() + t);
            break;
        }
    }

    // Register
    State st;
    st.set_SAMP(a, b, c);
    States.push_back(st);
}

BOOL SimulatorStates::equal(SimulatorStates& S)
{
    const auto sz = States.size();
    return sz == S.States.size() && std::memcmp(States.data(), S.States.data(), sz * sizeof(State)) == 0;
}

void SimulatorStates::clear() { States.clear(); }

void SimulatorStates::UpdateState(dx10State& state) const
{
    for (u32 it = 0; it < States.size(); it++)
    {
        const State& S = States[it];
        if (S.type == 0)
        {
            switch (S.v1)
            {
            case D3DRS_STENCILREF: state.UpdateStencilRef(S.v2); break;
            case D3DRS_ALPHAREF: state.UpdateAlphaRef(S.v2); break;
            }
        }
    }
}

void SimulatorStates::UpdateDesc(D3D_RASTERIZER_DESC& desc) const
{
    for (auto& S : States)
    {
        if (S.type != 0)
            continue;

        switch (S.v1)
        {
        case D3DRS_FILLMODE:
            if (S.v2 == D3DFILL_SOLID)
            {
                desc.FillMode = D3D_FILL_SOLID;
            }
            else
            {
                XR_ASSERT(S.v2 == D3DFILL_WIREFRAME);
                desc.FillMode = D3D_FILL_WIREFRAME;
            }

            break;
        case D3DRS_CULLMODE: desc.CullMode = dx10StateUtils::ConvertCullMode((D3DCULL)S.v2); break;
        case D3DRS_SCISSORTESTENABLE: desc.ScissorEnable = S.v2; break;
        //	TODO: DX10: Check how to scale unit for depth bias
        case D3DRS_DEPTHBIAS:
        //	TODO: DX10: Check slope scaled depth bias is used
        case D3DRS_SLOPESCALEDEPTHBIAS: xr::unreachable();
        }
    }
}

void SimulatorStates::UpdateDesc(D3D_DEPTH_STENCIL_DESC& desc) const
{
    for (auto& S : States)
    {
        if (S.type != 0)
            continue;

        switch (S.v1)
        {
        case D3DRS_ZENABLE: desc.DepthEnable = S.v2 ? 1 : 0; break;
        case D3DRS_ZWRITEENABLE: desc.DepthWriteMask = S.v2 ? D3D_DEPTH_WRITE_MASK_ALL : D3D_DEPTH_WRITE_MASK_ZERO; break;
        case D3DRS_ZFUNC: desc.DepthFunc = dx10StateUtils::ConvertCmpFunction((D3DCMPFUNC)S.v2); break;
        case D3DRS_STENCILENABLE: desc.StencilEnable = S.v2 ? 1 : 0; break;
        case D3DRS_STENCILMASK: desc.StencilReadMask = (UINT8)S.v2; break;
        case D3DRS_STENCILWRITEMASK: desc.StencilWriteMask = (UINT8)S.v2; break;
        case D3DRS_STENCILFAIL: desc.FrontFace.StencilFailOp = dx10StateUtils::ConvertStencilOp((D3DSTENCILOP)S.v2); break;
        case D3DRS_STENCILZFAIL: desc.FrontFace.StencilDepthFailOp = dx10StateUtils::ConvertStencilOp((D3DSTENCILOP)S.v2); break;
        case D3DRS_STENCILPASS: desc.FrontFace.StencilPassOp = dx10StateUtils::ConvertStencilOp((D3DSTENCILOP)S.v2); break;
        case D3DRS_STENCILFUNC: desc.FrontFace.StencilFunc = dx10StateUtils::ConvertCmpFunction((D3DCMPFUNC)S.v2); break;
        case D3DRS_CCW_STENCILFAIL: desc.BackFace.StencilFailOp = dx10StateUtils::ConvertStencilOp((D3DSTENCILOP)S.v2); break;
        case D3DRS_CCW_STENCILZFAIL: desc.BackFace.StencilDepthFailOp = dx10StateUtils::ConvertStencilOp((D3DSTENCILOP)S.v2); break;
        case D3DRS_CCW_STENCILPASS: desc.BackFace.StencilPassOp = dx10StateUtils::ConvertStencilOp((D3DSTENCILOP)S.v2); break;
        case D3DRS_CCW_STENCILFUNC: desc.BackFace.StencilFunc = dx10StateUtils::ConvertCmpFunction((D3DCMPFUNC)S.v2); break;
        }
    }
}

void SimulatorStates::UpdateDesc(D3D_BLEND_DESC& desc) const
{
    for (auto& S : States)
    {
        if (S.type != 0)
            continue;

        switch (S.v1)
        {
        case XRDX10RS_ALPHATOCOVERAGE: desc.AlphaToCoverageEnable = S.v2 ? 1 : 0; break;
        case D3DRS_SRCBLEND:
            for (auto& rt : desc.RenderTarget)
                rt.SrcBlend = dx10StateUtils::ConvertBlendArg((D3DBLEND)S.v2);

            break;
        case D3DRS_DESTBLEND:
            for (auto& rt : desc.RenderTarget)
                rt.DestBlend = dx10StateUtils::ConvertBlendArg((D3DBLEND)S.v2);

            break;
        case D3DRS_BLENDOP:
            for (auto& rt : desc.RenderTarget)
                rt.BlendOp = dx10StateUtils::ConvertBlendOp((D3DBLENDOP)S.v2);

            break;
        case D3DRS_SRCBLENDALPHA:
            for (auto& rt : desc.RenderTarget)
                rt.SrcBlendAlpha = dx10StateUtils::ConvertBlendArg((D3DBLEND)S.v2);

            break;
        case D3DRS_DESTBLENDALPHA:
            for (auto& rt : desc.RenderTarget)
                rt.DestBlendAlpha = dx10StateUtils::ConvertBlendArg((D3DBLEND)S.v2);

            break;
        case D3DRS_BLENDOPALPHA:
            for (auto& rt : desc.RenderTarget)
                rt.BlendOpAlpha = dx10StateUtils::ConvertBlendOp((D3DBLENDOP)S.v2);

            break;
        case D3DRS_ALPHABLENDENABLE:
            for (auto& rt : desc.RenderTarget)
                rt.BlendEnable = S.v2 ? 1 : 0;

            break;
        case D3DRS_COLORWRITEENABLE: desc.RenderTarget[0].RenderTargetWriteMask = (u8)S.v2; break;
        case D3DRS_COLORWRITEENABLE1: desc.RenderTarget[1].RenderTargetWriteMask = (u8)S.v2; break;
        case D3DRS_COLORWRITEENABLE2: desc.RenderTarget[2].RenderTargetWriteMask = (u8)S.v2; break;
        case D3DRS_COLORWRITEENABLE3: desc.RenderTarget[3].RenderTargetWriteMask = (u8)S.v2; break;
        }
    }
}

void SimulatorStates::UpdateDesc(D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT], bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT],
                                 int iBaseSamplerIndex) const
{
    constexpr int MipfilterLinear = 0x01;
    constexpr int MagfilterLinear = 0x04;
    constexpr int MinfilterLinear = 0x10;
    constexpr int AllfilterLinear = 0x15;
    constexpr int FilterAnisotropic = 0x40;
    constexpr int FilterComparison = 0x80;

    for (auto& S : States)
    {
        if (S.type != 2)
            continue;

        int iSamplerIndex = int(S.v1);
        iSamplerIndex -= iBaseSamplerIndex;

        if ((iSamplerIndex >= D3D_COMMONSHADER_SAMPLER_SLOT_COUNT) || iSamplerIndex < 0)
            continue;

        SamplerUsed[iSamplerIndex] = true;
        D3D_SAMPLER_DESC& desc = descArray[iSamplerIndex];

        switch (S.v2)
        {
        // D3D_FILTER Filter;
        /* D3DTEXTUREFILTER filter to use for magnification */
        case D3DSAMP_MAGFILTER:
            switch (S.v3)
            {
            case D3DTEXF_NONE:
            case D3DTEXF_POINT: desc.Filter = (D3D_FILTER)(desc.Filter & (~MagfilterLinear)); break;
            case D3DTEXF_LINEAR: desc.Filter = (D3D_FILTER)(desc.Filter | MagfilterLinear); break;
            default: xr::unreachable();
            }

            break;
        /* D3DTEXTUREFILTER filter to use for minification */
        case D3DSAMP_MINFILTER:
            switch (S.v3)
            {
            case D3DTEXF_NONE:
            case D3DTEXF_POINT: desc.Filter = (D3D_FILTER)(desc.Filter & (~MinfilterLinear)); break;
            case D3DTEXF_LINEAR: desc.Filter = (D3D_FILTER)(desc.Filter | MinfilterLinear); break;
            default: xr::unreachable();
            }

            break;
        /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
        case D3DSAMP_MIPFILTER:
            switch (S.v3)
            {
            case D3DTEXF_NONE:
            case D3DTEXF_POINT: desc.Filter = (D3D_FILTER)(desc.Filter & (~MipfilterLinear)); break;
            case D3DTEXF_LINEAR: desc.Filter = (D3D_FILTER)(desc.Filter | MipfilterLinear); break;
            default: xr::unreachable();
            }

            break;
        case XRDX10SAMP_ANISOTROPICFILTER:
            if (S.v3)
                desc.Filter = (D3D_FILTER)(desc.Filter | FilterAnisotropic);
            else
                desc.Filter = (D3D_FILTER)(desc.Filter & (~FilterAnisotropic));

            break;
        case XRDX10SAMP_COMPARISONFILTER:
            if (S.v3)
                desc.Filter = (D3D_FILTER)(desc.Filter | FilterComparison);
            else
                desc.Filter = (D3D_FILTER)(desc.Filter & (~FilterComparison));

            break;
        // D3Dxx_TEXTURE_ADDRESS_MODE AddressU;
        /* D3DTEXTUREADDRESS for U coordinate */
        case D3DSAMP_ADDRESSU: desc.AddressU = dx10StateUtils::ConvertTextureAddressMode(D3DTEXTUREADDRESS(S.v3)); break;
        /* D3DTEXTUREADDRESS for V coordinate */
        case D3DSAMP_ADDRESSV: desc.AddressV = dx10StateUtils::ConvertTextureAddressMode(D3DTEXTUREADDRESS(S.v3)); break;
        /* D3DTEXTUREADDRESS for W coordinate */
        case D3DSAMP_ADDRESSW: desc.AddressW = dx10StateUtils::ConvertTextureAddressMode(D3DTEXTUREADDRESS(S.v3)); break;
        //	FLOAT MipLODBias
        case D3DSAMP_MIPMAPLODBIAS: desc.MipLODBias = *((const float*)(&(S.v3))); break;
        //	UINT MaxAnisotropy;
        case D3DSAMP_MAXANISOTROPY: desc.MaxAnisotropy = S.v3; break;
        //	D3Dxx_COMPARISON_FUNC ComparisonFunc;
        case XRDX10SAMP_COMPARISONFUNC: desc.ComparisonFunc = (D3D_COMPARISON_FUNC)S.v3; break;
        //	FLOAT BorderColor[4];
        case D3DSAMP_BORDERCOLOR: {
            desc.BorderColor[0] = ((S.v3 >> 16) & 0xff) / 255.0f;
            desc.BorderColor[1] = ((S.v3 >> 8) & 0xff) / 255.0f;
            desc.BorderColor[2] = ((S.v3) & 0xff) / 255.0f;
            desc.BorderColor[3] = ((S.v3 >> 24) & 0xff) / 255.0f;
            break;
        }
        //	FLOAT MinLOD;
        case XRDX10SAMP_MINLOD: desc.MinLOD = (FLOAT)S.v3; break;
        //	FLOAT MaxLOD;
        case D3DSAMP_MAXMIPLEVEL: desc.MaxLOD = (FLOAT)S.v3; break;
        }
    }

    //	Validate data
    for (auto& desc : std::span{descArray, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT})
    {
        if (desc.Filter & FilterAnisotropic)
            desc.Filter = (D3D_FILTER)(desc.Filter | AllfilterLinear);

        XR_ASSERT(desc.MinLOD <= desc.MaxLOD);
    }
}
