#include "stdafx.h"

#include "dx10ConstantBuffer.h"
#include "dx10BufferUtils.h"

namespace xxh
{
#include <xxhash.h>
}

dx10ConstantBuffer::~dx10ConstantBuffer()
{
    RImplementation.Resources->_DeleteConstantBuffer(this);

    _RELEASE(m_pBuffer);
    xr_free(m_pBufferData);
}

dx10ConstantBuffer::dx10ConstantBuffer(ID3DShaderReflectionConstantBuffer* pTable) : m_bChanged(true)
{
    D3D_SHADER_BUFFER_DESC Desc{};

    CHK_DX(pTable->GetDesc(&Desc));

    m_strBufferName._set(Desc.Name);
    m_eBufferType = Desc.Type;
    m_uiBufferSize = Desc.Size;

    //	Fill member list with variable descriptions
    m_MembersList.resize(Desc.Variables);
    m_MembersNames.resize(Desc.Variables);
    for (u32 i = 0; i < Desc.Variables; ++i)
    {
        ID3DShaderReflectionVariable* pVar;
        ID3DShaderReflectionType* pType;

        D3D_SHADER_VARIABLE_DESC var_desc{};

        pVar = pTable->GetVariableByIndex(i);
        VERIFY(pVar);
        pType = pVar->GetType();
        VERIFY(pType);
        pType->GetDesc(&m_MembersList[i]);
        //	Buffers with the same layout can contain totally different members
        CHK_DX(pVar->GetDesc(&var_desc));
        m_MembersNames[i]._set(var_desc.Name);
    }

    m_uiMembersXXH = xxh::XXH3_64bits(&m_MembersList[0], Desc.Variables * sizeof(m_MembersList[0]));

    R_CHK(dx10BufferUtils::CreateConstantBuffer(&m_pBuffer, Desc.Size));
    VERIFY(m_pBuffer);
    m_pBufferData = xr_malloc(Desc.Size);
    VERIFY(m_pBufferData);

    if (m_pBuffer)
        m_pBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, xr_strlen(Desc.Name), Desc.Name);
}

bool dx10ConstantBuffer::Similar(dx10ConstantBuffer& _in)
{
    if (m_strBufferName != _in.m_strBufferName)
        return false;

    if (m_eBufferType != _in.m_eBufferType)
        return false;

    if (m_uiMembersXXH != _in.m_uiMembersXXH)
        return false;

    if (m_MembersList.size() != _in.m_MembersList.size())
        return false;

    if (memcmp(&m_MembersList[0], &_in.m_MembersList[0], m_MembersList.size() * sizeof(m_MembersList[0])))
        return false;

    VERIFY(m_MembersNames.size() == _in.m_MembersNames.size());

    int iMemberNum = m_MembersNames.size();
    for (int i = 0; i < iMemberNum; ++i)
    {
        if (m_MembersNames[i].c_str() != _in.m_MembersNames[i].c_str())
            return false;
    }

    return true;
}

void dx10ConstantBuffer::Flush(ctx_id_t context_id)
{
    if (!m_bChanged)
        return;

    auto* pContext = HW.get_context(context_id);
    D3D11_MAPPED_SUBRESOURCE pSubRes;
    void* pData;

    CHK_DX(pContext->Map(m_pBuffer, 0, D3D_MAP_WRITE_DISCARD, 0, &pSubRes));
    pData = pSubRes.pData;

    VERIFY(pData);
    VERIFY(m_pBufferData);

    xr_memcpy(pData, m_pBufferData, m_uiBufferSize);
    pContext->Unmap(m_pBuffer, 0);

    m_bChanged = false;
}
