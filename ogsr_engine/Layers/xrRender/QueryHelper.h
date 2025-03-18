#pragma once

IC HRESULT CreateQuery(ID3DQuery** ppQuery, D3DQUERYTYPE Type)
{
    D3D_QUERY_DESC desc{};
    desc.Query = D3D_QUERY_OCCLUSION;

    R_ASSERT(Type == D3DQUERYTYPE_OCCLUSION);

    return HW.pDevice->CreateQuery(&desc, ppQuery);
}

IC HRESULT GetData(ID3DQuery* pQuery, void* pData, u32 DataSize)
{
    //	Use D3Dxx_ASYNC_GETDATA_DONOTFLUSH for prevent flushing
    return HW.get_imm_context()->GetData(pQuery, pData, DataSize, 0);
}

IC HRESULT BeginQuery(ID3DQuery* pQuery)
{
    HW.get_imm_context()->Begin(pQuery);
    return S_OK;
}

IC HRESULT EndQuery(ID3DQuery* pQuery)
{
    HW.get_imm_context()->End(pQuery);
    return S_OK;
}
