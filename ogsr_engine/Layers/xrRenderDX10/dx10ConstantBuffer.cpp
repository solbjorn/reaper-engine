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

dx10ConstantBuffer::dx10ConstantBuffer(ID3DShaderReflectionConstantBuffer* pTable)
{
    D3D_SHADER_BUFFER_DESC Desc{};
    XR_ASSERT(xr::hr(pTable->GetDesc(&Desc)));

    m_strBufferName._set(Desc.Name);
    m_eBufferType = Desc.Type;
    m_uiBufferSize = Desc.Size;

    //	Fill member list with variable descriptions
    m_MembersList.resize(Desc.Variables);
    m_MembersNames.resize(Desc.Variables);

    for (auto [id, desc, name] : std::views::zip(std::views::iota(0u, Desc.Variables), m_MembersList, m_MembersNames))
    {
        auto pVar = XR_ASSERT_VAL(pTable->GetVariableByIndex(id) != nullptr);
        XR_ASSERT_VAL(pVar->GetType() != nullptr)->GetDesc(&desc);
        // Exclude pointers from hashing
        desc.Name = nullptr;

        //	Buffers with the same layout can contain totally different members
        D3D_SHADER_VARIABLE_DESC var_desc{};
        XR_ASSERT(xr::hr(pVar->GetDesc(&var_desc)));
        name._set(var_desc.Name);
    }

    m_uiMembersXXH = xxh::XXH3_64bits(m_MembersList.data(), xr::size_bytes(m_MembersList));

    XR_ASSERT(xr::hr(dx10BufferUtils::CreateConstantBuffer(&m_pBuffer, Desc.Size)));
    m_pBufferData = xr_alloc<std::byte>(Desc.Size);

    m_pBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, xr_strlen(Desc.Name), Desc.Name);
}

bool dx10ConstantBuffer::Similar(const dx10ConstantBuffer& _in)
{
    if (m_strBufferName != _in.m_strBufferName)
        return false;

    if (m_eBufferType != _in.m_eBufferType)
        return false;

    // Implies: list.size() == in.list.size()
    // Implies: std::memcmp(list.data(), in.list.data(), xr::size_bytes(list)) == 0
    if (m_uiMembersXXH != _in.m_uiMembersXXH)
        return false;

    return m_MembersNames == _in.m_MembersNames;
}

void dx10ConstantBuffer::Flush(ctx_id_t context_id)
{
    if (!m_bChanged)
        return;

    auto pContext = HW.get_context(context_id);
    D3D11_MAPPED_SUBRESOURCE pSubRes{};

    XR_ASSERT(xr::hr(pContext->Map(m_pBuffer, 0, D3D_MAP_WRITE_DISCARD, 0, &pSubRes)));
    xr_memcpy(pSubRes.pData, m_pBufferData, m_uiBufferSize);
    pContext->Unmap(m_pBuffer, 0);

    m_bChanged = false;
}
