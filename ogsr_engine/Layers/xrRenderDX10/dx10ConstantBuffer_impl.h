#ifndef dx10ConstantBuffer_impl_included
#define dx10ConstantBuffer_impl_included

template <typename T>
inline T& dx10ConstantBuffer::Access(std::size_t offset)
{
    //	TODO: DX10: Implement code which will check if set actually changes code.
    m_bChanged = true;

    // Buffer boundaries are checked at call sites
    return *reinterpret_cast<T*>(m_pBufferData + offset);
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fmatrix& A)
{
    Fmatrix trans;
    trans.transpose(A);
    auto& it = Access<Fmatrix>(L.index);

    switch (L.cls)
    {
    case RC_2x4:
        XR_ASSERT(C->type == RC_float && std::size_t{L.index} + 2 * lineSize <= m_uiBufferSize, "", C->type, L.index, m_uiBufferSize);
        it.vm[0].set(trans.vm[0]);
        it.vm[1].set(trans.vm[1]);
        break;
    case RC_3x4:
        XR_ASSERT(C->type == RC_float && std::size_t{L.index} + 3 * lineSize <= m_uiBufferSize, "", C->type, L.index, m_uiBufferSize);
        it.vm[0].set(trans.vm[0]);
        it.vm[1].set(trans.vm[1]);
        it.vm[2].set(trans.vm[2]);
        break;
    case RC_4x4:
        XR_ASSERT(C->type == RC_float && std::size_t{L.index} + 4 * lineSize <= m_uiBufferSize, "", C->type, L.index, m_uiBufferSize);
        it = trans;
        break;
    default: xr::unreachable();
    }
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fvector4& A)
{
    XR_ASSERT(C->type == RC_float && std::size_t{L.index} + lineSize <= m_uiBufferSize, "", C->type, L.index, m_uiBufferSize);
    auto& it = Access<Fvector4>(L.index);

    switch (L.cls)
    {
    case RC_1x2: *reinterpret_cast<DirectX::XMVECTOR*>(&it) = DirectX::XMLoadFloat2A(reinterpret_cast<const DirectX::XMFLOAT2A*>(&A)); break;
    case RC_1x3: *reinterpret_cast<DirectX::XMVECTOR*>(&it) = DirectX::XMLoadFloat3A(reinterpret_cast<const DirectX::XMFLOAT3A*>(&A)); break;
    case RC_1x4: it = A; break;
    default: xr::unreachable();
    }
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, float A)
{
    XR_ASSERT(C->type == RC_float && L.cls == RC_1x1 && std::size_t{L.index} + sizeof(f32) <= m_uiBufferSize, "", C->type, L.cls, L.index, m_uiBufferSize);
    Access<f32>(L.index) = A;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, int A)
{
    XR_ASSERT(C->type == RC_int && L.cls == RC_1x1 && std::size_t{L.index} + sizeof(s32) <= m_uiBufferSize, "", C->type, L.cls, L.index, m_uiBufferSize);
    Access<s32>(L.index) = A;
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fmatrix& A)
{
    Fmatrix trans;
    trans.transpose(A);

    switch (L.cls)
    {
    case RC_2x4: {
        const auto off = std::size_t{L.index} + 2 * lineSize * e;
        XR_ASSERT(C->type == RC_float && off + 2 * lineSize <= m_uiBufferSize, C->type, off, m_uiBufferSize);
        auto& it = Access<Fmatrix>(off);

        it.vm[0].set(trans.vm[0]);
        it.vm[1].set(trans.vm[1]);
        break;
    }
    case RC_3x4: {
        const auto off = std::size_t{L.index} + 3 * lineSize * e;
        XR_ASSERT(C->type == RC_float && off + 3 * lineSize <= m_uiBufferSize, C->type, off, m_uiBufferSize);
        auto& it = Access<Fmatrix>(off);

        it.vm[0].set(trans.vm[0]);
        it.vm[1].set(trans.vm[1]);
        it.vm[2].set(trans.vm[2]);
        break;
    }
    case RC_4x4: {
        const auto off = std::size_t{L.index} + 4 * lineSize * e;
        XR_ASSERT(C->type == RC_float && off + 4 * lineSize <= m_uiBufferSize, C->type, off, m_uiBufferSize);

        Access<Fmatrix>(off) = trans;
        break;
    }
    default: xr::unreachable();
    }
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fvector4& A)
{
    const auto off = std::size_t{L.index} + lineSize * e;
    XR_ASSERT(C->type == RC_float && off + lineSize <= m_uiBufferSize, "", C->type, off, m_uiBufferSize);
    auto& it = Access<Fvector4>(off);

    switch (L.cls)
    {
    case RC_1x2: *reinterpret_cast<DirectX::XMVECTOR*>(&it) = DirectX::XMLoadFloat2A(reinterpret_cast<const DirectX::XMFLOAT2A*>(&A)); break;
    case RC_1x3: *reinterpret_cast<DirectX::XMVECTOR*>(&it) = DirectX::XMLoadFloat3A(reinterpret_cast<const DirectX::XMFLOAT3A*>(&A)); break;
    case RC_1x4: it = A; break;
    default: xr::unreachable();
    }
}

IC void* dx10ConstantBuffer::AccessDirect(R_constant_load& L, size_t DataSize)
{
    XR_ASSERT(std::size_t{L.index} + DataSize <= m_uiBufferSize, "", L.index, DataSize);
    m_bChanged = true;

    return m_pBufferData + L.index;
}

#endif //	dx10ConstantBuffer_impl_included
