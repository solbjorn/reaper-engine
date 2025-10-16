#ifndef dx10R_Backend_Runtime_included
#define dx10R_Backend_Runtime_included

#include "StateManager/dx10ShaderResourceStateCache.h"

ICF ID3D11DeviceContext1* CBackend::context() const { return HW.get_context(context_id); }

IC void CBackend::set_xform(u32, const Fmatrix&)
{
    stat.xforms++;
    //	TODO: DX10: Implement CBackend::set_xform
    // VERIFY(!"Implement CBackend::set_xform");
}

IC void CBackend::set_RT(ID3DRenderTargetView* RT, u32 ID)
{
    if (RT != pRT[ID])
    {
        PGO(Msg("PGO:setRT"));
        stat.target_rt++;
        pRT[ID] = RT;

        //	Reset all RT's here to allow RT to be bounded as input
        if (!m_bChangedRTorZB)
            context()->OMSetRenderTargets(0, nullptr, nullptr);

        m_bChangedRTorZB = true;
    }
}

IC void CBackend::set_ZB(ID3DDepthStencilView* ZB)
{
    if (ZB != pZB)
    {
        PGO(Msg("PGO:setZB"));
        stat.target_zb++;
        pZB = ZB;

        //	Reset all RT's here to allow RT to be bounded as input
        if (!m_bChangedRTorZB)
            context()->OMSetRenderTargets(0, nullptr, nullptr);

        m_bChangedRTorZB = true;
    }
}

IC void CBackend::ClearRT(ID3DRenderTargetView* rt, const Fcolor& color) { context()->ClearRenderTargetView(rt, reinterpret_cast<const float*>(&color)); }

IC void CBackend::ClearZB(ID3DDepthStencilView* zb, float depth) { context()->ClearDepthStencilView(zb, D3D_CLEAR_DEPTH, depth, 0); }

IC void CBackend::ClearZB(ID3DDepthStencilView* zb, float depth, u32 stencil) { context()->ClearDepthStencilView(zb, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, depth, stencil); }

ICF void CBackend::set_Format(SDeclaration* _decl)
{
    if (decl != _decl)
    {
        PGO(Msg("PGO:v_format:%x", _decl));
#ifdef DEBUG
        stat.decl++;
#endif
        decl = _decl;
    }
}

ICF void CBackend::set_PS(ID3DPixelShader* _ps, [[maybe_unused]] LPCSTR _n)
{
    if (ps != _ps)
    {
        PGO(Msg("PGO:Pshader:%x", _ps));
        stat.ps++;
        ps = _ps;
        context()->PSSetShader(ps, nullptr, 0);

#ifdef DEBUG
        ps_name = _n;
#endif
    }
}

ICF void CBackend::set_GS(ID3DGeometryShader* _gs, [[maybe_unused]] LPCSTR _n)
{
    if (gs != _gs)
    {
        PGO(Msg("PGO:Gshader:%x", _ps));
        //	TODO: DX10: Get statistics for G Shader change
        // stat.gs			++;
        gs = _gs;
        context()->GSSetShader(gs, nullptr, 0);

#ifdef DEBUG
        gs_name = _n;
#endif
    }
}

ICF void CBackend::set_HS(ID3D11HullShader* _hs, [[maybe_unused]] LPCSTR _n)
{
    if (hs != _hs)
    {
        PGO(Msg("PGO:Hshader:%x", _ps));
        //	TODO: DX10: Get statistics for H Shader change
        // stat.hs			++;
        hs = _hs;
        context()->HSSetShader(hs, nullptr, 0);

#ifdef DEBUG
        hs_name = _n;
#endif
    }
}

ICF void CBackend::set_DS(ID3D11DomainShader* _ds, [[maybe_unused]] LPCSTR _n)
{
    if (ds != _ds)
    {
        PGO(Msg("PGO:Dshader:%x", _ps));
        //	TODO: DX10: Get statistics for D Shader change
        // stat.ds			++;
        ds = _ds;
        context()->DSSetShader(ds, nullptr, 0);

#ifdef DEBUG
        ds_name = _n;
#endif
    }
}

ICF void CBackend::set_CS(ID3D11ComputeShader* _cs, [[maybe_unused]] LPCSTR _n)
{
    if (cs != _cs)
    {
        PGO(Msg("PGO:Cshader:%x", _ps));
        //	TODO: DX10: Get statistics for D Shader change
        // stat.cs			++;
        cs = _cs;
        context()->CSSetShader(cs, nullptr, 0);

#ifdef DEBUG
        cs_name = _n;
#endif
    }
}

ICF bool CBackend::is_TessEnabled() { return ds || hs; }

ICF void CBackend::set_VS(ID3DVertexShader* _vs, [[maybe_unused]] LPCSTR _n)
{
    if (vs != _vs)
    {
        PGO(Msg("PGO:Vshader:%x", _vs));
        stat.vs++;
        vs = _vs;

        context()->VSSetShader(vs, nullptr, 0);

#ifdef DEBUG
        vs_name = _n;
#endif
    }
}

ICF void CBackend::set_Vertices(ID3DVertexBuffer* _vb, u32 _vb_stride)
{
    if ((vb != _vb) || (vb_stride != _vb_stride))
    {
        PGO(Msg("PGO:VB:%x,%d", _vb, _vb_stride));
#ifdef DEBUG
        stat.vb++;
#endif
        vb = _vb;
        vb_stride = _vb_stride;

        u32 iOffset = 0;
        context()->IASetVertexBuffers(0, 1, &vb, &_vb_stride, &iOffset);
    }
}

ICF void CBackend::set_Indices(ID3DIndexBuffer* _ib)
{
    if (ib != _ib)
    {
        PGO(Msg("PGO:IB:%x", _ib));
#ifdef DEBUG
        stat.ib++;
#endif
        ib = _ib;
        context()->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);
    }
}

IC D3D_PRIMITIVE_TOPOLOGY TranslateTopology(D3DPRIMITIVETYPE T)
{
    static constexpr D3D_PRIMITIVE_TOPOLOGY translateTable[] = {
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, //	None
        D3D_PRIMITIVE_TOPOLOGY_POINTLIST, //	D3DPT_POINTLIST = 1,
        D3D_PRIMITIVE_TOPOLOGY_LINELIST, //	D3DPT_LINELIST = 2,
        D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, //	D3DPT_LINESTRIP = 3,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, //	D3DPT_TRIANGLELIST = 4,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, //	D3DPT_TRIANGLESTRIP = 5,
        D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, //	D3DPT_TRIANGLEFAN = 6,
    };

    VERIFY(T < sizeof(translateTable) / sizeof(translateTable[0]));
    VERIFY(T >= 0);

    D3D_PRIMITIVE_TOPOLOGY result = translateTable[T];

    VERIFY(result != D3D_PRIMITIVE_TOPOLOGY_UNDEFINED);

    return result;
}

IC u32 GetIndexCount(D3DPRIMITIVETYPE T, u32 iPrimitiveCount)
{
    switch (T)
    {
    case D3DPT_POINTLIST: return iPrimitiveCount;
    case D3DPT_LINELIST: return iPrimitiveCount * 2;
    case D3DPT_LINESTRIP: return iPrimitiveCount + 1;
    case D3DPT_TRIANGLELIST: return iPrimitiveCount * 3;
    case D3DPT_TRIANGLESTRIP: return iPrimitiveCount + 2;
    default: NODEFAULT;
#ifdef DEBUG
        return 0;
#endif // #ifdef DEBUG
    }
}

IC void CBackend::ApplyPrimitieTopology(D3D_PRIMITIVE_TOPOLOGY Topology)
{
    if (m_PrimitiveTopology != Topology)
    {
        m_PrimitiveTopology = Topology;
        context()->IASetPrimitiveTopology(m_PrimitiveTopology);
    }
}

IC void CBackend::Compute(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
    stat.calls++;

    SRVSManager.Apply(context_id);
    StateManager.Apply();
    //	State manager may alter constants
    constants.flush();
    context()->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

IC void CBackend::Render(D3DPRIMITIVETYPE T, u32 baseV, u32, u32 countV, u32 startI, u32 PC)
{
    D3D_PRIMITIVE_TOPOLOGY Topology = TranslateTopology(T);
    u32 iIndexCount = GetIndexCount(T, PC);

    //!!! HACK !!!
    if (hs || ds)
    {
        R_ASSERT(Topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Topology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    }

    stat.calls++;
    stat.verts += countV;
    stat.polys += PC;

    ApplyPrimitieTopology(Topology);

    SRVSManager.Apply(context_id);
    ApplyRTandZB();
    ApplyVertexLayout();
    StateManager.Apply();
    //	State manager may alter constants
    constants.flush();
    context()->DrawIndexed(iIndexCount, startI, baseV);

    PGO(Msg("PGO:DIP:%dv/%df", countV, PC));
}

IC void CBackend::Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC)
{
    //	TODO: DX10: Remove triangle fan usage from the engine
    if (T == D3DPT_TRIANGLEFAN)
        return;

    D3D_PRIMITIVE_TOPOLOGY Topology = TranslateTopology(T);
    u32 iVertexCount = GetIndexCount(T, PC);

    stat.calls++;
    stat.verts += 3 * PC;
    stat.polys += PC;

    ApplyPrimitieTopology(Topology);
    SRVSManager.Apply(context_id);
    ApplyRTandZB();
    ApplyVertexLayout();
    StateManager.Apply();
    //	State manager may alter constants
    constants.flush();
    context()->Draw(iVertexCount, startV);

    PGO(Msg("PGO:DIP:%dv/%df", 3 * PC, PC));
}

IC void CBackend::set_Geometry(SGeometry* _geom)
{
    set_Format(&*_geom->dcl);

    set_Vertices(_geom->vb, _geom->vb_stride);
    set_Indices(_geom->ib);
}

IC void CBackend::set_Scissor(Irect* R)
{
    auto* pContext = context();

    if (R)
    {
        StateManager.EnableScissoring();
        RECT* clip = (RECT*)R;
        pContext->RSSetScissorRects(1, clip);
    }
    else
    {
        StateManager.EnableScissoring(FALSE);
        pContext->RSSetScissorRects(0, nullptr);
    }
}

IC void CBackend::SetViewport(const D3D_VIEWPORT& viewport) const { context()->RSSetViewports(1, &viewport); }

IC void CBackend::set_viewport_size(float w, float h) const
{
    const D3D11_VIEWPORT viewport = {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = w,
        .Height = h,
        .MinDepth = 0.f,
        .MaxDepth = 1.f,
    };

    SetViewport(viewport);
}

IC void CBackend::set_Stencil(u32 _enable, u32 _func, u32 _ref, u32 _mask, u32 _writemask, u32 _fail, u32 _pass, u32 _zfail)
{
    StateManager.SetStencil(_enable, _func, _ref, _mask, _writemask, _fail, _pass, _zfail);
}

IC void CBackend::set_Z(u32 _enable) { StateManager.SetDepthEnable(_enable); }

IC void CBackend::set_ZFunc(u32 _func) { StateManager.SetDepthFunc(_func); }

IC void CBackend::set_AlphaRef(u32)
{
    //	TODO: DX10: Implement rasterizer state update to support alpha ref
    VERIFY(!"Not implemented.");
}

IC void CBackend::set_ColorWriteEnable(u32 _mask) { StateManager.SetColorWriteEnable(_mask); }

ICF void CBackend::set_CullMode(u32 _mode) { StateManager.SetCullMode(_mode); }

ICF void CBackend::set_FillMode(u32 _mode) { StateManager.SetFillMode(_mode); }

IC void CBackend::ApplyVertexLayout()
{
    VERIFY(vs);
    VERIFY(decl);
    VERIFY(m_pInputSignature);

    xr_map<ID3DBlob*, ID3DInputLayout*>::iterator it;

    it = decl->vs_to_layout.find(m_pInputSignature);

    if (it == decl->vs_to_layout.end())
    {
        ID3DInputLayout* pLayout;

        CHK_DX(HW.pDevice->CreateInputLayout(&decl->dx10_dcl_code[0], decl->dx10_dcl_code.size() - 1, m_pInputSignature->GetBufferPointer(), m_pInputSignature->GetBufferSize(),
                                             &pLayout));

        it = decl->vs_to_layout.emplace(m_pInputSignature, pLayout).first;
    }

    if (m_pInputLayout != it->second)
    {
        m_pInputLayout = it->second;
        context()->IASetInputLayout(m_pInputLayout);
    }
}

ICF void CBackend::set_VS(ref_vs& _vs)
{
    m_pInputSignature = _vs->signature->signature;
    set_VS(_vs->vs, _vs->cName.c_str());
}

ICF void CBackend::set_VS(SVS* _vs)
{
    m_pInputSignature = _vs->signature->signature;
    set_VS(_vs->vs, _vs->cName.c_str());
}

IC bool CBackend::CBuffersNeedUpdate(ref_cbuffer buf1[MaxCBuffers], ref_cbuffer buf2[MaxCBuffers], u32& uiMin, u32& uiMax)
{
    bool bRes = false;
    int i = 0;
    while ((i < MaxCBuffers) && (buf1[i] == buf2[i]))
        ++i;

    uiMin = i;

    for (; i < MaxCBuffers; ++i)
    {
        if (buf1[i] != buf2[i])
        {
            bRes = true;
            uiMax = i;
        }
    }

    return bRes;
}

IC void CBackend::set_Constants(R_constant_table* C)
{
    // caching
    if (ctable == C)
        return;

    ctable = C;
    xforms.unmap();
    hemi.unmap();
    tree.unmap();
    LOD.unmap();
    StateManager.UnmapConstants();

    if (!C)
        return;

    PGO(Msg("PGO:c-table"));

    //	Setup constant tables
    {
        ref_cbuffer aPixelConstants[MaxCBuffers];
        ref_cbuffer aVertexConstants[MaxCBuffers];
        ref_cbuffer aGeometryConstants[MaxCBuffers];
        ref_cbuffer aHullConstants[MaxCBuffers];
        ref_cbuffer aDomainConstants[MaxCBuffers];
        ref_cbuffer aComputeConstants[MaxCBuffers];

        for (int i = 0; i < MaxCBuffers; ++i)
        {
            aPixelConstants[i] = m_aPixelConstants[i];
            aVertexConstants[i] = m_aVertexConstants[i];
            aGeometryConstants[i] = m_aGeometryConstants[i];

            aHullConstants[i] = m_aHullConstants[i];
            aDomainConstants[i] = m_aDomainConstants[i];
            aComputeConstants[i] = m_aComputeConstants[i];

            m_aPixelConstants[i] = nullptr;
            m_aVertexConstants[i] = nullptr;
            m_aGeometryConstants[i] = nullptr;

            m_aHullConstants[i] = nullptr;
            m_aDomainConstants[i] = nullptr;
            m_aComputeConstants[i] = nullptr;
        }

        R_constant_table::cb_table::iterator it = C->m_CBTable[context_id].begin();
        R_constant_table::cb_table::iterator end = C->m_CBTable[context_id].end();
        for (; it != end; ++it)
        {
            u32 uiBufferIndex = it->first;

            if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferPixelShader)
            {
                VERIFY((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aPixelConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferVertexShader)
            {
                VERIFY((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aVertexConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferGeometryShader)
            {
                VERIFY((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aGeometryConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferHullShader)
            {
                VERIFY((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aHullConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferDomainShader)
            {
                VERIFY((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aDomainConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else if ((uiBufferIndex & CB_BufferTypeMask) == CB_BufferComputeShader)
            {
                VERIFY((uiBufferIndex & CB_BufferIndexMask) < MaxCBuffers);
                m_aComputeConstants[uiBufferIndex & CB_BufferIndexMask] = it->second;
            }
            else
                VERIFY("Invalid enumeration");
        }

        ID3DBuffer* tempBuffer[MaxCBuffers];
        auto* pContext = context();

        u32 uiMin;
        u32 uiMax;

        if (CBuffersNeedUpdate(m_aPixelConstants, aPixelConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aPixelConstants[i])
                    tempBuffer[i] = m_aPixelConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }

            pContext->PSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
        }

        if (CBuffersNeedUpdate(m_aVertexConstants, aVertexConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aVertexConstants[i])
                    tempBuffer[i] = m_aVertexConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            pContext->VSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
        }

        if (CBuffersNeedUpdate(m_aGeometryConstants, aGeometryConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aGeometryConstants[i])
                    tempBuffer[i] = m_aGeometryConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            pContext->GSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
        }

        if (CBuffersNeedUpdate(m_aHullConstants, aHullConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aHullConstants[i])
                    tempBuffer[i] = m_aHullConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            pContext->HSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
        }

        if (CBuffersNeedUpdate(m_aDomainConstants, aDomainConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aDomainConstants[i])
                    tempBuffer[i] = m_aDomainConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            pContext->DSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
        }

        if (CBuffersNeedUpdate(m_aComputeConstants, aComputeConstants, uiMin, uiMax))
        {
            ++uiMax;

            for (u32 i = uiMin; i < uiMax; ++i)
            {
                if (m_aComputeConstants[i])
                    tempBuffer[i] = m_aComputeConstants[i]->GetBuffer();
                else
                    tempBuffer[i] = nullptr;
            }
            pContext->CSSetConstantBuffers(uiMin, uiMax - uiMin, &tempBuffer[uiMin]);
        }
    }

    // process constant-loaders
    R_constant_table::c_table::iterator it = C->table.begin();
    R_constant_table::c_table::iterator end = C->table.end();
    for (; it != end; it++)
    {
        R_constant* Cs = &**it;
        VERIFY(Cs);
        if (Cs && Cs->handler)
            Cs->handler->setup(*this, Cs);
    }
}

ICF void CBackend::ApplyRTandZB()
{
    if (m_bChangedRTorZB)
    {
        m_bChangedRTorZB = false;
        context()->OMSetRenderTargets(sizeof(pRT) / sizeof(pRT[0]), pRT, pZB);
    }
}

IC void CBackend::get_ConstantDirect(const char* n, size_t DataSize, void** pVData, void** pGData, void** pPData)
{
    ref_constant C = get_c(n);

    if (C)
    {
        constants.access_direct(&*C, DataSize, pVData, pGData, pPData);
    }
    else
    {
        if (pVData)
            *pVData = nullptr;
        if (pGData)
            *pGData = nullptr;
        if (pPData)
            *pPData = nullptr;
    }
}

#endif //	dx10R_Backend_Runtime_included
