#pragma once

IC HRESULT CreateQuery(ID3DQuery** ppQuery, D3DQUERYTYPE Type)
{
    D3D_QUERY_DESC desc{};
    desc.Query = D3D_QUERY_OCCLUSION;

    R_ASSERT(Type == D3DQUERYTYPE_OCCLUSION);

    return HW.pDevice->CreateQuery(&desc, ppQuery);
}

IC HRESULT GetData(ID3DQuery* pQuery, void* pData, UINT DataSize)
{
    //	Use D3Dxx_ASYNC_GETDATA_DONOTFLUSH for prevent flushing
    return HW.pContext->GetData(pQuery, pData, DataSize, 0);
}

IC HRESULT BeginQuery(ID3DQuery* pQuery)
{
    HW.pContext->Begin(pQuery);
    return S_OK;
}

IC HRESULT EndQuery(ID3DQuery* pQuery)
{
    HW.pContext->End(pQuery);
    return S_OK;
}
