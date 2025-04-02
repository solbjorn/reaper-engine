#pragma once

IC HRESULT CreateQuery(ID3DQuery** ppQuery, D3D_QUERY Type)
{
    D3D_QUERY_DESC desc{};
    desc.Query = Type;

    return HW.pDevice->CreateQuery(&desc, ppQuery);
}

IC HRESULT GetData(ID3DQuery* pQuery, void* pData, u32 DataSize)
{
    //	Use D3Dxx_ASYNC_GETDATA_DONOTFLUSH for prevent flushing
    return HW.get_imm_context()->GetData(pQuery, pData, DataSize, 0);
}

IC HRESULT BeginQuery(ID3DQuery* pQuery, ctx_id_t context_id = R__IMM_CTX_ID)
{
    HW.get_context(context_id)->Begin(pQuery);
    return S_OK;
}

IC HRESULT EndQuery(ID3DQuery* pQuery, ctx_id_t context_id = R__IMM_CTX_ID)
{
    HW.get_context(context_id)->End(pQuery);
    return S_OK;
}

IC HRESULT ReleaseQuery(ID3DQuery* pQuery)
{
    _RELEASE(pQuery);
    return S_OK;
}
